#ifndef WEATHERPLUGIN_H
#define WEATHERPLUGIN_H

#include "dde-dock/pluginsiteminterface.h"
#include "weatheritem.h"
#include "forecastapplet.h"
#include "weatherclient.h"
#include <QDateTime>
#include <QFile>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QLabel>

// itemKey for the plugin
#define WEATHER_KEY "weather-key"
#define STATE_KEY "enable"

#define VERSION "v1.0"
// itemID or menuID for Context Menu.. Why can't I use enum and switch..
#define REFRESH "chk"
#define SETTINGS "set"
#define ABOUT "about"
#define SHOWLOG "log"
// keys for settings from .config/deepin/dde-dock.conf,
// set and get through m_proxyInter
#define THEME_KEY "theme"
#define CITY_KEY "city"
#define COUNTRY_KEY "country"
#define CITYID_KEY "cityid"
#define UNIT_KEY "isMetric"
#define CHK_INTERVAL_KEY "chk_intvl"

class WeatherPlugin : public QObject, PluginsItemInterface {
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface"
                      FILE "weather.json")

public:
    explicit WeatherPlugin(QObject *parent = nullptr);
    virtual ~WeatherPlugin() override;

    static QString logPath() {
        return QStandardPaths::standardLocations(
                    QStandardPaths::CacheLocation).first()
                + "/dde-dock-plugin-weather.log";}

    const QString pluginName() const override {return "Weather";}
    const QString pluginDisplayName() const override {return tr("Weather");}
    void init(PluginProxyInterface *proxyInter) override;

    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override {
        return !m_proxyInter->getValue(this, STATE_KEY, true).toBool();
    }

    int itemSortKey(const QString &itemKey) override;
    void setSortKey(const QString &itemKey, const int order) override;
    void refreshIcon(const QString &itemKey) override;

    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;
    QWidget *itemPopupApplet(const QString &itemKey) override;

    const QString itemContextMenu(const QString &itemKey) override;
    void invokedMenuItem(const QString &itemKey, const QString &menuId,
                         const bool checked) override;

signals:
    void checkUpdate();

private slots:
    void refreshTips();

private:
    QPointer<WeatherClient> m_client;
    QPointer<WeatherItem> m_items;
    QPointer<ForecastApplet> m_popups;
    QPointer<QLabel> m_tips;
    QNetworkAccessManager netmgr;
    QFile logFile;
    QTextStream log;
    QTimer m_refershTimer;

    static const QStringList themeSet;
};

#endif // WEATHERPLUGIN_H
