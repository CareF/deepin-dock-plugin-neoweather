#ifndef WEATHEQRSETTINGDIALOG_H
#define WEATHERSETTINGDIALOG_H

#include <QWidget>
#include "dabstractdialog.h"
#include "dde-dock/pluginsiteminterface.h"
#include "weatherclient.h"

class WeatherSettingDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit WeatherSettingDialog(PluginProxyInterface *proxyInter,
                                  QNetworkAccessManager &net,
                                  QTextStream &logStream,
                                  QWidget *parent = nullptr);
    virtual ~WeatherSettingDialog() override;

    const static QStringList themeSet;

public slots:
    virtual void accept() override; // Also delete self lateron

signals:

protected:
    ///
    /// \brief m_proxyInter
    /// To get and save setting options.
    /// the WeatherPlugin class is responsible to reload setting
    /// when accepted SIGNAL is sent
    ///
    QPointer<PluginProxyInterface> m_proxyInter;
    QPointer<CityLookup> m_cityLookupClient;

    void loadSettings();
};

#endif // WEATHERSETTINGDIALOG_H
