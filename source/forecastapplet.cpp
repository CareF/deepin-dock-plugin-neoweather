#include "forecastapplet.h"
#include "ui_forecastapplet.h"
#include <QApplication> // provide qApp

ForecastApplet::ForecastApplet(const WeatherClient *wcli,
                               QString thm, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ForecastApplet),
    client (wcli), themeName(thm)
{
    ui->setupUi(this);
    connect(client, &WeatherClient::allReady,
            this, &ForecastApplet::reloadForecast);
    connect(client, &WeatherClient::changed,
            this, &ForecastApplet::reloadForecast);
    connect(client, &OpenWeatherClient::error,
            this, &ForecastApplet::updateError);
}

ForecastApplet::~ForecastApplet()
{
    delete ui;
}

QPixmap ForecastApplet::loadWIcon(const QString &name, int size) const {
    // TODO: cache icon pixmap
    const auto ratio = qApp->devicePixelRatio();
    const int iconSize = static_cast<int> (size * ratio);
    QPixmap iconPixmap =  QPixmap(
                QString(":/%1/%2").arg(themeName).arg(name)).scaled(
                iconSize, iconSize,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
    iconPixmap.setDevicePixelRatio(ratio);
    return iconPixmap;
}

QVector<WeatherClient::Weather>::const_iterator ForecastApplet::getDayStat(
        QVector<WeatherClient::Weather>::const_iterator iter,
        double &temp_min, double &temp_max,
        const WeatherClient::Weather **p_primary) const {
    //TODO: secondary weather?
    QDate theDay = iter->dateTime.date();
    temp_min = iter->temp_min;
    temp_max = iter->temp_max;
    *p_primary = iter;
    for(iter++; iter != client->getForecast().end() &&
        iter->dateTime.date() == theDay; iter++) {
        if (iter->temp_max > temp_max) {
            temp_max = iter->temp_max;
        }
        if (iter->temp_min < temp_min) {
            temp_min = iter->temp_min;
        }
        if (iter->weatherID / 100 < (*p_primary)->weatherID / 100 &&
                iter->weatherID % 100 > (*p_primary)->weatherID % 100) {
            // Get the most serious condition
            *p_primary = iter;
        }
    }
    return iter;
}

void ForecastApplet::reloadForecast() {
    return;
}

void ForecastApplet::updateError(OpenWeatherClient::ErrorCode) {
    return;
}
