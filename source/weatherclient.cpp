#include "weatherclient.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QTimeZone>
#define LOGTIMEFORMAT "yyyy/MM/dd HH:mm:ss"

/* OpenWeatherClient members */

QString OpenWeatherClient::appid = "8c3a954fdf4092d93ecf8e7039dbb6ea";
OpenWeatherClient::OpenWeatherClient(QNetworkAccessManager &net,
                                     QTextStream &logStream,
                                     QObject *parent):
    QObject (parent), log(logStream), checking(false), netmgr(net)
{
        netTimer.setSingleShot(true);
        netTimer.callOnTimeout([this](){
            this->errorHandle(NetWorkTimeOut);});
}

QNetworkReply *OpenWeatherClient::netRequest(const QString type,
                                             const QString &options)
{
    QString url = QString(
                "https://api.openweathermap.org/data/2.5/%1?%2&appid=%3"
                ).arg(type).arg(options).arg(appid);
    log << "Connect: " << url << endl;
    return netmgr.get(QNetworkRequest(QUrl(url)));
}

inline void cleanNetworkReply(QPointer<QNetworkReply> reply) {
    // Discard unfinished network requirement
    if (reply != nullptr) {
        reply->disconnect();
        reply->close();
        reply->deleteLater();
    }
}
#define LOGMAX 144
QByteArray OpenWeatherClient::netResult(QPointer<QNetworkReply> reply) {
    netTimer.stop();
    QByteArray result = reply->readAll();
    cleanNetworkReply(reply);
    if (result.length() > LOGMAX) {
        log << (QString("Get reply: ...") +
                (result.constData() - LOGMAX)) << endl;
    }
    else {
        log << (QString("Get reply: ") + result) << endl;
    }
    return result;
}



/* WeatherClient members */

WeatherClient::WeatherClient(QNetworkAccessManager &net,
                             QTextStream &logStream,
                             int icityid,
                             const QString &icity,
                             const QString &icountry,
                             bool ismetric, QObject *parent) :
    OpenWeatherClient(net, logStream, parent),
    weatherReply(nullptr), forecastReply(nullptr),
    cityid(icityid), city(icity), country(icountry),
    isMetric(ismetric), lang(QLocale::system().name()),
    last(QDateTime::fromMSecsSinceEpoch(0, QTimeZone::utc()))
{
}

WeatherClient::~WeatherClient() {
    cleanNetworkReply(forecastReply);
    cleanNetworkReply(weatherReply);
}

void WeatherClient::update(int timeout) {
    if (checking) {
        emit error(AlreadyChecking);
        return;
    }
    last = QDateTime::currentDateTime();
    log << "Check update: " << last.toString(LOGTIMEFORMAT) << endl;
    checking = true;
    status = NoneDone;
    QString options = QString("%1&lang=%2&units=%3").arg(
                cityid == 0 ?
                    QString("q=%1,%2").arg(city).arg(country) :
                    QString("id=%1").arg(cityid)
                    ).arg(lang).arg(isMetric? "metric" : "imperial");

    cleanNetworkReply(weatherReply);
    weatherReply = netRequest("weather", options);
    connect(weatherReply, &QNetworkReply::finished,
            this, &WeatherClient::parseWeather);

    cleanNetworkReply(forecastReply);
    forecastReply = netRequest("forcast", options);
    connect(forecastReply, &QNetworkReply::finished,
            this, &WeatherClient::parseWeather);


    netTimer.start(timeout);
}

void WeatherClient::errorHandle(ErrorCode e){
    switch (e) {
    case AlreadyChecking:
        return;
    case NetWorkTimeOut:
        log << "Network requirement time out!" << endl;
        break;
    case NoValidJson:
        break;
    case NoJsonList:
        log << "Forcast parser cannot find \"list\" in json reply" << endl;
        break;
    }
    checking = false;
    cleanNetworkReply(forecastReply);
    cleanNetworkReply(weatherReply);
}

inline OpenWeatherClient::Weather jWeatherParser(const QJsonObject &w) {
    // See https://openweathermap.org/current for more info
    return OpenWeatherClient::Weather(
    {QDateTime::fromMSecsSinceEpoch(w.value("dt").toInt(), QTimeZone::utc()),       // date
     w.value("weather").toArray().at(0).toObject().value("main").toString(),        // weather
     w.value("weather").toArray().at(0).toObject().value("description").toString(), // description
     w.value("weather").toArray().at(0).toObject().value("icon").toString(),        // icon
     w.value("main").toObject().value("temp").toDouble(),
     w.value("main").toObject().value("temp_min").toDouble(),
     w.value("main").toObject().value("temp_max").toDouble(),
     w.value("main").toObject().value("pressure").toDouble(),
     w.value("main").toObject().value("humidity").toInt(),
     w.value("wind").toObject().value("speed").toDouble(),
     w.value("wind").toObject().value("deg").toInt(),
     w.value("clouds").toObject().value("all").toInt()});
}
void WeatherClient::parseWeather() {
    QByteArray result = netResult(forecastReply);
    QJsonParseError JPE;
    QJsonDocument JD = QJsonDocument::fromJson(result, &JPE);
    if (JPE.error != QJsonParseError::NoError) {
        log << "Weather json Parse Error" << JPE.error << endl;
        emit error(NoValidJson);
        return;
    } else {
        const QJsonObject o = JD.object();
        // TODO: sunrise and sunset?
        if (city == "") {
            city = o.value("name").toString();
            country = o.value("sys").toObject().value("country").toString();
        }
        if (cityid == 0)
            cityid = o.value("id").toInt();

        log << QString("Parsing weather for %1, %2 (id: %3)\n"
                       ).arg(city).arg(country).arg(cityid) << endl;
        wnow = jWeatherParser(o);
        log << QString("\tDate: %1\n"
                       "\tWeather: %2, %3, icon %4\n"
                       "\tTemperature: %5\n").arg(
                   wnow.date.toString(LOGTIMEFORMAT)).arg(
                   wnow.weather).arg(wnow.description).arg(
                   wnow.icon).arg(wnow.temp) << endl;
    }
    emit weatherReady();
    status &= WeatherDone;
    if (status == AllDone)
        checking = false;
}

void WeatherClient::parseForecast() {
    QByteArray result = netResult(forecastReply);
    QJsonParseError JPE;
    QJsonDocument JD = QJsonDocument::fromJson(result, &JPE);
    if (JPE.error != QJsonParseError::NoError) {
        log << "Forcast json Parse Error" << JPE.error << endl;
        emit error(NoValidJson);
        return;
    } else {
        QJsonValue obj = JD.object().value("list");
        if (obj == QJsonValue::Undefined) {
            emit error(NoJsonList);
            return;
        }

        log << QString("Parsing forcast for %1, %2 (id: %3)\n"
                       ).arg(city).arg(country).arg(cityid) << endl;

        QJsonArray list = obj.toArray();
        forecasts.clear();
        for (auto item: list) {
            forecasts.append(jWeatherParser(item.toObject()));
            const auto &o = forecasts.last();
            log << QString("\tDate: %1\n"
                           "\tWeather: %2, %3, icon %4\n"
                           "\tTemperature: %5\n").arg(
                       o.date.toString(LOGTIMEFORMAT)).arg(
                       o.weather).arg(o.description).arg(
                       o.icon).arg(o.temp) << endl;
        }
    }
    emit forecastReady();
    status &= ForecastDone;
    if (status == AllDone)
        checking = false;
}

inline void unitTransform (bool isMetric, OpenWeatherClient::Weather &item) {
    if (isMetric) {
        item.temp = (item.temp - 32)/1.8;
        item.temp_max = (item.temp_max - 32)/1.8;
        item.temp_min = (item.temp_min - 32)/1.8;
        item.wind /= 2.23693629;
    }
    else {
        item.temp = item.temp * 1.8 + 32;
        item.temp_max = item.temp_max * 1.8 + 32;
        item.temp_min = item.temp_min * 1.8 + 32;
        item.wind *= 2.23693629;
    }
}
void WeatherClient::setMetric(bool is) {
    if(is == isMetric) {
        return;
    }
    isMetric = is;
    unitTransform(isMetric, wnow);
    for (auto &item: forecasts) {
        unitTransform(isMetric, item);
    }
    emit changed();
}



/* CityLookup members */

CityLookup::~CityLookup() {
    cleanNetworkReply(netReply);
}

void CityLookup::lookForCity(const QString &city, const QString &country,
                             int timeout){
    if (country == "")
        return;
    log << QString("Look for city: %1, %2 ").arg(city).arg(country);
    log << QDateTime::currentDateTime().toString(LOGTIMEFORMAT) << endl;
    cleanNetworkReply(netReply);
    netReply = netRequest("find", QString("q=%1").arg(city) +
                          country=="" ? "" : ","+country);
    connect(netReply, &QNetworkReply::finished,
            this, &CityLookup::parseCityInfo);
    netTimer.start(timeout);
    return;
}

void CityLookup::parseCityInfo() {
    QByteArray result = netResult(netReply);
    QJsonParseError JPE;
    QJsonDocument JD = QJsonDocument::fromJson(result, &JPE);
    if (JPE.error != QJsonParseError::NoError) {
        log << "CityInfo json Parse Error" << JPE.error << endl;
        emit error(NoValidJson);
    } else {
        QJsonValue obj = JD.object().value("list");
        if (obj == QJsonValue::Undefined) {
            emit error(NoJsonList);
            return;
        }

        QJsonArray list = obj.toArray();
        QList<CityInfo> cityList;
        for (const auto item: list) {
            auto o = item.toObject();
            cityList.append(CityInfo{
                                o.value("id").toInt(), o.value("name").toString(),
                                o.value("sys").toObject().value("country").toString(),
                                o.value("coord").toObject().value("lat").toDouble(),
                                o.value("coord").toObject().value("lon").toDouble()
                            });
        }
        emit foundCity(cityList);
    }
    return;
}

void CityLookup::errorHandle(ErrorCode e){
    switch (e) {
    case AlreadyChecking:
        return;
    case NetWorkTimeOut:
        log << "Network requirement time out!" << endl;
        break;
    case NoValidJson:
        break;
    case NoJsonList:
        log << "CityInfo cannot find \"list\" in json reply" << endl;
        break;
    }
    checking = false;
    cleanNetworkReply(netReply);
}
