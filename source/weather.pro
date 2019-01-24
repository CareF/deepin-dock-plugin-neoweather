QT              += widgets svg network
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dtkwidget

TARGET           = $$qtLibraryTarget(weather)
DESTDIR          = $$_PRO_FILE_PWD_
DISTFILES       += weather.json

HEADERS += \
    weatherplugin.h \
    weatheritem.h \
    weatherclient.h \
    forecastapplet.h \
    weathersettingdialog.h

SOURCES += \
    weatherplugin.cpp \
    weatheritem.cpp \
    weatherclient.cpp \
    forecastapplet.cpp \
    weathersettingdialog.cpp

isEmpty(PREFIX) {
    PREFIX = /usr
}

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += res.qrc
