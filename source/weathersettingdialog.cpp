#include "weathersettingdialog.h"
#include <QBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QAbstractItemView>
#include <QApplication>
#include <QProxyStyle>
#include <QListView>
#include <QObject>
#include <QPushButton>
#include <QDesktopServices>
#include "countrycode.h"

DWIDGET_USE_NAMESPACE

/*
class NonPopupComboboxStyle : public QProxyStyle
{
  public:
    NonPopupComboboxStyle(): QProxyStyle(qApp->style()) {}
    virtual int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                          const QWidget *widget = nullptr,
                          QStyleHintReturn *returnData = nullptr) const override;
};

int NonPopupComboboxStyle::styleHint(StyleHint hint,
                                     const QStyleOption *option,
                                     const QWidget *widget,
                                     QStyleHintReturn *returnData) const {
    switch (hint) {
    case QStyle::SH_ComboBox_Popup:
        return 0;
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}
*/

LimitedHightComboBox::LimitedHightComboBox(int h, QWidget *parent):
    QComboBox(parent), height(h) {
    view()->setMaximumHeight(height);
    view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void LimitedHightComboBox::showPopup() {
    QComboBox::showPopup();
    QFrame *popup = findChild<QFrame*>();
    popup->setMaximumHeight(height);
    // Position vertically so the curently selected item lines up
    // with the combo box, inspired by QComboBox source code
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    QPoint above = mapToGlobal(style()->subControlRect(
                                   QStyle::CC_ComboBox, &opt,
                                   QStyle::SC_ComboBoxListBoxPopup,
                                   this).topLeft());
    const QRect currentItemRect = view()->visualRect(view()->currentIndex());
    int top = view()->mapToGlobal(currentItemRect.topLeft()).y();
    popup->move(popup->x(), popup->y() + above.y() - top);
}

#define OPTIONS_WIDTH 100
WeatherSettingDialog::WeatherSettingDialog(PluginProxyInterface *proxyInter,
                                           WeatherPlugin *weatherplugin,
                                           QNetworkAccessManager &net,
                                           QTextStream &logStream,
                                           QWidget *parent) :
    DAbstractDialog(parent),
    m_proxyInter(proxyInter), m_weatherPlugin(weatherplugin),
    m_cityLookupClient(new CityLookup(net, logStream, this)),
    cityBox(new QComboBox(this)), countryBox(new LimitedHightComboBox(500, this)),
    themeBox(new QComboBox(this)), timeIntvBox(new QLineEdit(this)),
    appidBox(new QLineEdit(this))
{
    QVBoxLayout *vLayout = new QVBoxLayout();

    QGroupBox *geoGroup = new QGroupBox(tr("City Information"));
    QGridLayout *geoLayout = new QGridLayout();
    geoLayout->addWidget(new QLabel(tr("City")), 0, 0);
    cityBox->setEditable(true);
    cityBox->setCurrentText(m_weatherPlugin->m_client->cityName());
    cityBox->setMinimumWidth(OPTIONS_WIDTH*1.25);
    geoLayout->addWidget(cityBox, 0, 1);
    geoLayout->addWidget(new QLabel(tr("Country")), 0, 2);
    countryBox->setModel(new CountryModel(this));
    countryBox->setMaximumWidth(OPTIONS_WIDTH*1.25);
    cityBox->setMinimumHeight(countryBox->height());
//    countryBox->view()->setMaximumHeight(200);
//    countryBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    countryBox->findChild<QFrame*>()->setMaximumHeight(200);
//    countryBox->setMaxVisibleItems(20);
//    countryBox->setStyle(new NonPopupComboboxStyle);
    QString country = m_weatherPlugin->m_client->countryName();
    countryBox->setCurrentIndex(countryBox->findData(country));
    geoLayout->addWidget(countryBox, 0, 3);
    QLabel *geoComments =  new QLabel(tr("You can also input cityid from "
                                         "<a href=\"https://openweathermap.org/city\" style=\"color: yellow\">openweathermap.org</a> "
                                         "(the number in the URL of your city page) "
                                         "to avoid unambiguous result. "));
    geoComments->setWordWrap(true);
    geoComments->setTextFormat(Qt::RichText);
    geoComments->setTextInteractionFlags(Qt::TextBrowserInteraction);
    geoComments->setOpenExternalLinks(true);
    geoLayout->addWidget(geoComments, 1, 0, 1, -1);
    geoGroup->setLayout(geoLayout);
    // TODO: help icon
    vLayout->addWidget(geoGroup);

    QGroupBox *optionGroup = new QGroupBox(tr("Options"));
    QGridLayout *settingLayout = new QGridLayout();
    settingLayout->addWidget(new QLabel(tr("Intervel")),
                             0, 0);
    timeIntvBox->setText(QString::number(m_proxyInter->getValue(
                              m_weatherPlugin, CHK_INTERVAL_KEY,
                              DEFAULT_INTERVAL).toInt()));
    timeIntvBox->setInputMask(tr("999 mi\\n"));
    timeIntvBox->setMaximumWidth(OPTIONS_WIDTH);
    settingLayout->addWidget(timeIntvBox, 0, 1);
    settingLayout->addWidget(new QLabel(tr("Units")), 1, 0);
    QHBoxLayout *unitButtons = new QHBoxLayout();
    metricButton = new QRadioButton(tr("metric"), optionGroup);
    imperialButton = new QRadioButton(tr("imperial"), optionGroup);
    unitButtons->addWidget(metricButton);
    unitButtons->addWidget(imperialButton);
    unitButtons->addStretch();
    if (m_proxyInter->getValue(m_weatherPlugin, UNIT_KEY, true).toBool()) {
        metricButton->setChecked(true);
    }
    else {
        imperialButton->setChecked(true);
    }
    settingLayout->addLayout(unitButtons, 1, 1);
    settingLayout->addWidget(new QLabel(tr("Theme")), 2, 0);
    themeBox->addItems(themeSet);
    themeBox->setCurrentText(m_proxyInter->getValue(m_weatherPlugin, THEME_KEY,
                                  themeSet[0]).toString());
    themeBox->setMaximumWidth(OPTIONS_WIDTH);
    settingLayout->addWidget(themeBox, 2, 1);

    appidBox->setPlaceholderText(tr("appid from openweathermap.org"));
    QString appid = m_proxyInter->getValue(m_weatherPlugin, APPID_KEY, "").toString();
    if (appid != "") {
        appidBox->setText(appid);
    }
    settingLayout->addWidget(new QLabel(tr("AppID")), 3, 0);
    settingLayout->addWidget(appidBox, 3, 1);
    QPushButton* appidHelp = new QPushButton("?");
    appidHelp->setMaximumWidth(appidHelp->fontMetrics().size(
                                   Qt::TextShowMnemonic, " ? ").width());
    appidHelp->setToolTip(tr("The plugin default appid is proviede by the author. "
                             "Click to learn how to get your own appid."));
    connect(appidHelp, &QPushButton::released, [=](){
        QDesktopServices::openUrl(QUrl("https://openweathermap.org/appid"));});
    settingLayout->addWidget(appidHelp, 3, 2);
    optionGroup->setLayout(settingLayout);
    vLayout->addWidget(optionGroup);

    QDialogButtonBox *buttons = new QDialogButtonBox(
                QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &WeatherSettingDialog::accept);
    vLayout->addWidget(buttons);

    setLayout(vLayout);
}

void WeatherSettingDialog::accept() {
    m_proxyInter->saveValue(m_weatherPlugin, THEME_KEY,
                            themeBox->currentText());
    // TODO: cityid
    QString cityInput = cityBox->currentText();
    bool isCityid;
    int cityid = cityInput.toInt(&isCityid);
    if (isCityid) {
        m_proxyInter->saveValue(m_weatherPlugin, CITYID_KEY, cityid);
        m_proxyInter->saveValue(m_weatherPlugin, CITY_KEY, "");
        m_proxyInter->saveValue(m_weatherPlugin, COUNTRY_KEY, "");
    }
    else {
        QString countryInput = countryBox->currentData().toString();
        if (cityInput != m_weatherPlugin->m_client->cityName() ||
                countryInput != m_weatherPlugin->m_client->countryName()) {
            m_proxyInter->saveValue(m_weatherPlugin, CITYID_KEY, 0);
            m_proxyInter->saveValue(m_weatherPlugin, CITY_KEY, cityInput);
            m_proxyInter->saveValue(m_weatherPlugin, COUNTRY_KEY, countryInput);
        }
    }
    m_proxyInter->saveValue(m_weatherPlugin, UNIT_KEY,
                            metricButton->isChecked());
    m_proxyInter->saveValue(m_weatherPlugin, APPID_KEY,
                            appidBox->text());
    m_proxyInter->saveValue(m_weatherPlugin, CHK_INTERVAL_KEY,
                            timeIntvBox->text().chopped(4).toInt());
    QDialog::accept();
}
