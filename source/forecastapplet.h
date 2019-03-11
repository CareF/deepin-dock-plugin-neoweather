#ifndef FORECASTAPPLET_H
#define FORECASTAPPLET_H

#include <QWidget>
#include <QLabel>
#include "weatherclient.h"
#include "themeset.h"

#define MAXDAYS 5

namespace Ui {
class ForecastApplet;
}

class ForecastApplet : public QWidget
{
    Q_OBJECT

public:
    explicit ForecastApplet(const WeatherClient *wcli,
                           QString theme=themeSet[0],
                           QWidget *parent = nullptr);
    virtual ~ForecastApplet();

    void setTheme(const QString& theme) {
        themeName = theme; reloadForecast();}
    const QString& theme() const {return themeName;}
    QPixmap loadWIcon(const QString &name="na", int size=40) const;

public slots:
    void reloadForecast();
    void updateError(OpenWeatherClient::ErrorCode);

private:
    Ui::ForecastApplet *ui;
    const WeatherClient *client;
    QString themeName;
    QPointer<QLabel> WImgNow, cityNow, tempNow;

    QVector<WeatherClient::Weather>::const_iterator getDayStat(
            const WeatherClient::Weather *start,
            double &temp_min, double &temp_max,
            const WeatherClient::Weather **p_primaryWeather) const;

};

#endif // FORECASTAPPLET_H
