#include "weatheritem.h"
#include "dde-dock/constants.h" // provide Dock::DisplayMode
#include <QApplication> // provide qApp
#include <QPainter>
#ifdef QT_DEBUG
#include <QDebug>
#endif

WeatherItem::WeatherItem(const WeatherClient *wcli,
                         const ForecastApplet *popups,
                         QWidget *parent):
    QWidget(parent), client(wcli), fcstApplet(popups)
{ 
    connect(client, &WeatherClient::weatherReady,
            this, &WeatherItem::refreshIcon);
    connect(client, &WeatherClient::changed,
            this, &WeatherItem::refreshIcon);
}

void WeatherItem::paintEvent(QPaintEvent *e) {
    QWidget::paintEvent(e);

    // const auto ratio = qApp->devicePixelRatio();
    const Dock::DisplayMode displayMode = qApp->property(
                PROP_DISPLAY_MODE).value<Dock::DisplayMode>();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (displayMode == Dock::Efficient) {
        painter.setPen(Qt::white);
        QString format = "%1\n%2";
        // TODO: weathernowtext to weathernowdesc for i18n?
        painter.drawText(rect(), Qt::AlignCenter,
                         format.arg(client->weatherNowText()).arg(
                             client->tempNow()));
    }
    else {
        const QRectF &rf = QRectF(rect());
        const QRectF &rfp = QRectF(m_iconPixmap.rect());
        painter.drawPixmap(rf.center() - rfp.center()
                           / m_iconPixmap.devicePixelRatioF(), m_iconPixmap);

    }
}

QSize WeatherItem::sizeHint() const {
    QFontMetrics FM(qApp->font());
    QSize size;
    const Dock::DisplayMode displayMode = qApp->property(
                PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    if (displayMode == Dock::Efficient) {
        auto s1 = FM.boundingRect(client->weatherNowDesc());
        auto s2 = FM.boundingRect(client->tempNow());
        size = s1.width() > s2.width() ?
                    s1.size() + QSize(10, s2.height()) :
                    s2.size() + QSize(10, s2.height());
    }else{
        size = m_iconPixmap.size();
    }
    return size;
}

void WeatherItem::refreshIcon() {
    const Dock::DisplayMode displayMode =
            qApp->property(PROP_DISPLAY_MODE).value<Dock::DisplayMode>();
    const int iconSize = (displayMode == Dock::Fashion ?
            static_cast<int>(std::min(width(), height())) : 24);
    QString weather;
    weather = client->weatherNowIcon();
    m_iconPixmap = fcstApplet->loadWIcon(weather, iconSize);
    update();
}

