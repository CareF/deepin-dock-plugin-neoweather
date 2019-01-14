#include "weatherplugin.h"
#include <QDesktopServices>
#include <DAboutDialog>

#define MINUTE 60000 // minute in ms
#define DEFAULT_INTERVAL 30 //min

WeatherPlugin::WeatherPlugin (QObject *parent):
    QObject (parent), m_client(nullptr), m_items(nullptr),
    m_popups(nullptr), netmgr(this), logFile(logPath()),
    m_refershTimer(this)
{

}

WeatherPlugin::~WeatherPlugin () {
    delete m_popups;
    delete m_tips;
    delete m_items;
    delete m_client;
}

void WeatherPlugin::init(PluginProxyInterface *proxyInter) {
    this->m_proxyInter = proxyInter;
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qDebug() << "Cannot open log file";
    log.setDevice(&logFile);
    m_client = new WeatherClient(
                netmgr, log,
                m_proxyInter->getValue(this, CITYID_KEY, 0).toInt(),
                m_proxyInter->getValue(this, CITY_KEY, "").toString(),
                m_proxyInter->getValue(this, COUNTRY_KEY, "").toString(),
                m_proxyInter->getValue(this, UNIT_KEY, true).toBool());
    m_popups = new ForecastApplet(m_client, m_proxyInter->getValue(
                                      this, THEME_KEY, "hty").toString());
    m_items = new WeatherItem(m_client, m_popups);

    if(!pluginIsDisable()) {
        this->m_proxyInter->itemAdded(this, WEATHER_KEY);

        m_client->update();
        m_refershTimer.start(MINUTE * m_proxyInter->getValue(
                                 this, CHK_INTERVAL_KEY, DEFAULT_INTERVAL
                                 ).toInt());
        m_refershTimer.callOnTimeout(m_client, &WeatherClient::update);
    }
}

void WeatherPlugin::pluginStateSwitched() {
    m_proxyInter->saveValue(this, STATE_KEY, pluginIsDisable());
    if (pluginIsDisable()) {
        m_refershTimer.stop();
        m_proxyInter->itemRemoved(this, WEATHER_KEY);
    }
    else {
        m_refershTimer.start();
        m_proxyInter->itemAdded(this, WEATHER_KEY);
        m_client->update();
    }
}

QWidget *WeatherPlugin::itemWidget(const QString &itemKey) {
    if (itemKey == WEATHER_KEY) {
        return m_items;
    }
    return nullptr;
}

QWidget *WeatherPlugin::itemTipsWidget(const QString &itemKey) {
    if (itemKey == WEATHER_KEY) {
        return m_tips;
    }
    return nullptr;
}

QWidget *WeatherPlugin::itemPopupApplet(const QString &itemKey) {
    if (itemKey == WEATHER_KEY) {
        return m_popups;
    }
    return nullptr;
}

int WeatherPlugin::itemSortKey(const QString &itemKey) {
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(displayMode());
    return m_proxyInter->getValue(this, key,
            displayMode() == Dock::DisplayMode::Fashion ? 2 : 3).toInt();
}

void WeatherPlugin::setSortKey(const QString &itemKey, const int order) {
    const QString key = QString("pos_%1_%2").arg(itemKey).arg(displayMode());
    m_proxyInter->saveValue(this, key, order);
}

void WeatherPlugin::refreshIcon(const QString &itemKey) {
    if (itemKey == WEATHER_KEY) {
        m_items->refresh();
    }
}

const QString WeatherPlugin::itemContextMenu(const QString &itemKey) {
    if (itemKey == WEATHER_KEY) {
        QList<QVariant> items;
        items.reserve(3);

        QMap<QString, QVariant> set;
        set["itemId"] = SETTINGS;
        set["itemText"] = tr("Settings");
        set["isActive"] = true;
        items.push_back(set);

        QMap<QString, QVariant> refresh;
        refresh["itemId"] = REFRESH;
        refresh["itemText"] = tr("Refresh");
        refresh["isActive"] = true;
        items.push_back(refresh);

        QMap<QString, QVariant> log;
        log["itemId"] = SHOWLOG;
        log["itemText"] = tr("Show Log");
        log["isActive"] = true;
        items.push_back(log);

        QMap<QString, QVariant> about;
        about["itemId"] = ABOUT;
        about["itemText"] = tr("About");
        about["isActive"] = true;
        items.push_back(about);

        QMap<QString, QVariant> menu;
        menu["items"] = items;
        menu["checkableMenu"] = false;
        menu["singleCheck"] = false;

        return QJsonDocument::fromVariant(menu).toJson();
    }
    return QString();
}

void WeatherPlugin::invokedMenuItem(const QString &itemKey,
                                    const QString &menuId,
                                    const bool checked)
{
    Q_UNUSED(checked)
    if (itemKey == WEATHER_KEY) {
        if (menuId == SETTINGS) {
            // TODO
        }
        else if (menuId == REFRESH) {
            m_client->update();
        }
        else if (menuId == SHOWLOG) {
            QString surl = "file://" + logPath();
            QDesktopServices::openUrl(QUrl(surl));
        }
        else if (menuId == ABOUT) {
            DWIDGET_USE_NAMESPACE
            DAboutDialog *about = new DAboutDialog(m_items);
            about->setWindowTitle(tr("About Weather"));
            about->setProductName(tr("Deepin Dock Plugin: Weather"));
            about->setProductIcon(QIcon(":/hty/01d"));
            about->setVersion(VERSION);
            about->setDescription(tr("Author: CareF <me@mail.caref.xyz>"));
            // void setCompanyLogo(const QPixmap &companyLogo);
            about->setWebsiteName(tr("Source Code (GitHub)"));
            about->setWebsiteLink("https://github.com/CareF/deepin-dock-plugin-weather");
            // void setAcknowledgementLink(const QString &acknowledgementLink);
            about->setLicense(tr("License: GPLv3.0"));

            about->setWindowModality(Qt::NonModal);
            about->show();
        }
    }

}

const QStringList WeatherPlugin::themeSet({"hty", "gray"});
