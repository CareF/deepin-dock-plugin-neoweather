#ifndef FORECASTWIDGET_H
#define FORECASTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "weatherclient.h"

#define MAXDAYS 5

class ForecastApplet : public QWidget
{
    Q_OBJECT

public:
    explicit ForecastApplet(const WeatherClient *wcli,
                           QString theme="hty",
                           QWidget *parent = nullptr);
    virtual ~ForecastApplet();

    void setTheme(const QString& theme) {
        themeName = theme; reloadForecast();}
    const QString& theme() const {return themeName;}
    QPixmap loadWIcon(const QString &name="na", int size=50) const;

public slots:
    void reloadForecast();
    void updateError(OpenWeatherClient::ErrorCode);

private:
    const WeatherClient *client;
    QString themeName;
    QPointer<QLabel> WImgNow, cityNow, tempNow;
    struct ForecastColumn {
        QPointer<QLabel> Date, WImg, Temp;
    } fcstLabels[MAXDAYS];
    QGridLayout defaultLayout;

};

#endif // FORECASTWIDGET_H
