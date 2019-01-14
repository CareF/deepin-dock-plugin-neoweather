#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QWidget>
#include <QSettings>
#include "weatherclient.h"
#include "forecastapplet.h"

class WeatherItem : public QWidget {
    Q_OBJECT

public:
    explicit WeatherItem(const WeatherClient *wcli,
                         const ForecastApplet *popups,
                         QWidget *parent = nullptr);

protected:
    QSize sizeHint() const;
    void resizeEvent(QResizeEvent *e){
        QWidget::resizeEvent(e);
        refresh();
    }
    void paintEvent(QPaintEvent *e);

public slots:
    void refresh();

private:
    const WeatherClient *client;
    const ForecastApplet *fcstApplet;
    QPixmap weather;
};

#endif // WEATHERWIDGET_H
