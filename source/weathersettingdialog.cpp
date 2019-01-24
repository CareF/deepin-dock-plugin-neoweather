#include "weathersettingdialog.h"
#include <QBoxLayout>
#include <QDialogButtonBox>

DWIDGET_USE_NAMESPACE
const QStringList WeatherSettingDialog::themeSet({"hty", "gray"});

WeatherSettingDialog::WeatherSettingDialog(PluginProxyInterface *proxyInter,
                                           QNetworkAccessManager &net,
                                           QTextStream &logStream,
                                           QWidget *parent) :
    DAbstractDialog(parent), m_proxyInter(proxyInter),
    m_cityLookupClient(new CityLookup(net, logStream, this))
{

    QVBoxLayout *vLayout = new QVBoxLayout();
    QGridLayout *itemLayout = new QGridLayout();
    vLayout->addLayout(itemLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(
                QDialogButtonBox::Ok|
                QDialogButtonBox::Cancel,
                this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    vLayout->addWidget(buttons);

    setLayout(vLayout);
}

