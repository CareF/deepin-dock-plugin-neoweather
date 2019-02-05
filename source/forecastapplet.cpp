#include "forecastapplet.h"
#include <QApplication> // provide qApp

#define DATEFORMAT "MM/dd ddd"

ForecastApplet::ForecastApplet(const WeatherClient *wcli,
                             QString thm, QWidget *parent)
    : QWidget(parent), client(wcli), themeName(thm)
{
    setFixedWidth(300);

    WImgNow = new QLabel;
    WImgNow->setPixmap(loadWIcon("na", 80));
    WImgNow->setAlignment(Qt::AlignCenter);
    defaultLayout.addWidget(WImgNow, 0, 0);
    tempNow = new QLabel("25.0°C");
    tempNow->setAlignment(Qt::AlignCenter);
    tempNow->setStyleSheet("font-size:20px;");
    defaultLayout.addWidget(tempNow, 0, 1);
    cityNow = new QLabel("City");
    cityNow->setAlignment(Qt::AlignCenter);
    cityNow->setStyleSheet("font-size:20px;");
    defaultLayout.addWidget(cityNow, 0, 2);

    const QDate today = QDate::currentDate();
    for (int i=0; i<MAXDAYS; i++) {
        fcstLabels[i].WImg = new QLabel;
        fcstLabels[i].WImg->setPixmap(loadWIcon());
        fcstLabels[i].WImg->setAlignment(Qt::AlignCenter);
        defaultLayout.addWidget(fcstLabels[i].WImg, i+1, 0);
        fcstLabels[i].Temp = new QLabel("Clear 25.0°C");
        fcstLabels[i].Temp->setAlignment(Qt::AlignCenter);
        fcstLabels[i].Temp->setStyleSheet("font-size:12px;");
        defaultLayout.addWidget(fcstLabels[i].Temp, i+1, 1);
        fcstLabels[i].Date = new QLabel(
                    today.addDays(i+1).toString(DATEFORMAT));
        fcstLabels[i].Date->setStyleSheet("font-size:12px;");
        fcstLabels[i].Date->setAlignment(Qt::AlignCenter);
        defaultLayout.addWidget(fcstLabels[i].Date, i+1, 2);
    }

    setLayout(&defaultLayout);
    connect(client, &WeatherClient::forecastReady,
            this, &ForecastApplet::reloadForecast);
    connect(client, &WeatherClient::changed,
            this, &ForecastApplet::reloadForecast);
    connect(client, &OpenWeatherClient::error,
            this, &ForecastApplet::updateError);
}

ForecastApplet::~ForecastApplet() {
    // Not necessary becasue they go with defaultLayout
//    delete WImgNow;
//    delete tempNow;
//    delete cityNow;
//    for (auto & f : fcstLabels) {
//        delete f.WImg;
//        delete f.Temp;
//        delete f.Date;
//    }
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

void ForecastApplet::reloadForecast() {
    auto forcasts = client->getForecast();
    WImgNow->setPixmap(loadWIcon(forcasts[0].icon, 80));
    tempNow->setText(QString::number(forcasts[0].temp, 'f', 1)+
                     client->tempUnit());
    cityNow->setText(client->cityName());

    int n = 0;
    for (auto iter = forcasts.begin() + 1;
         iter != forcasts.end() && n < MAXDAYS;
         iter++) {
        if (iter->dateTime.time() == QTime(12, 0, 0)) {
            // TODO: Do a post processing for daily statistics
            fcstLabels[n].Date->setText(iter->dateTime.toString(DATEFORMAT));
            fcstLabels[n].WImg->setPixmap(loadWIcon(iter->icon));
            fcstLabels[n].Temp->setText(
                        QString("%1, %2%3").arg(iter->description).arg(
                            QString::number(qRound(iter->temp))).arg(
                            client->tempUnit()));
            n++;
        }
    }
    for (; n < MAXDAYS; n++) {
        fcstLabels[n].Date->setText(tr("N/A"));
        fcstLabels[n].WImg->setPixmap(loadWIcon("na"));
        fcstLabels[n].Temp->setText(tr("N/A"));
    }
}

void ForecastApplet::updateError(OpenWeatherClient::ErrorCode e) {
    WImgNow->setPixmap(loadWIcon("na", 80));
    tempNow->setText(tr("Error: %1").arg(e));
    for (auto & f: fcstLabels) {
        f.Date->setText(tr("N/A"));
        f.WImg->setPixmap(loadWIcon("na"));
        f.Temp->setText(tr("N/A"));
    }
    return;
}
