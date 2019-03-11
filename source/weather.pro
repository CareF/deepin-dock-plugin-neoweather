QT              += widgets svg network gui
TEMPLATE         = lib
CONFIG          += plugin c++11 lrelease embed_translations link_pkgconfig
PKGCONFIG       += dtkwidget

TARGET           = $$qtLibraryTarget(neoweather)
DESTDIR          = $$_PRO_FILE_PWD_
DISTFILES       += \
    neoweather.json

HEADERS += \
    weatherplugin.h \
    weatheritem.h \
    weatherclient.h \
    weathersettingdialog.h \
    countrycode.h \
    themeset.h \
    forecastapplet.h

SOURCES += \
    weatherplugin.cpp \
    weatheritem.cpp \
    weatherclient.cpp \
    weathersettingdialog.cpp \
    countrycode.cpp \
    forecastapplet.cpp

isEmpty(PREFIX) {
    PREFIX = /usr
}

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += res.qrc

TRANSLATIONS = $$_PRO_FILE_PWD_/i18n/neoweather-zh_CN.ts

FORMS += \
    forecastapplet.ui
