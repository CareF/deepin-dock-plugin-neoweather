#ifndef WEATHEQRSETTINGDIALOG_H
#define WEATHERSETTINGDIALOG_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QRadioButton>
#include "dabstractdialog.h"
#include "dde-dock/pluginsiteminterface.h"
#include "weatherclient.h"
#include "weatherplugin.h"

const QStringList themeSet({"hty", "gray"});

class LimitedHightComboBox: public QComboBox {
    Q_OBJECT
public:
    LimitedHightComboBox(int h, QWidget *parent=nullptr);
    virtual void showPopup () override;

private:
    int height;
};

class WeatherSettingDialog : public DTK_WIDGET_NAMESPACE::DAbstractDialog
{
    Q_OBJECT
public:
    explicit WeatherSettingDialog(PluginProxyInterface *proxyInter,
                                  WeatherPlugin *weatherplugin,
                                  QNetworkAccessManager &net,
                                  QTextStream &logStream,
                                  QWidget *parent = nullptr);
//    virtual ~WeatherSettingDialog() override;

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
    PluginProxyInterface *m_proxyInter;
    WeatherPlugin *m_weatherPlugin;
    QPointer<CityLookup> m_cityLookupClient;
    QPointer<QComboBox> cityBox;
    QPointer<QComboBox> countryBox;
    QPointer<QComboBox> themeBox;
    QPointer<QLineEdit> timeIntvBox;
    QPointer<QRadioButton> metricButton;
    QPointer<QRadioButton> imperialButton;
    QPointer<QLineEdit> appidBox;

    void loadSettings();
};

#endif // WEATHERSETTINGDIALOG_H
