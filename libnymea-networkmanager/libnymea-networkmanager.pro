TARGET = nymea-networkmanager
TEMPLATE = lib

QT += dbus network

QMAKE_CXXFLAGS *= -Werror -std=c++11 -g
QMAKE_LFLAGS *= -std=c++11

HEADERS += \
    networkmanager.h \
    networkconnection.h \
    networkdevice.h \
    networksettings.h \
    wirednetworkdevice.h \
    wirelessaccesspoint.h \
    wirelessnetworkdevice.h \
    networkmanagerutils.h \

SOURCES += \
    networkmanager.cpp \
    networkconnection.cpp \
    networkdevice.cpp \
    networksettings.cpp \
    wirednetworkdevice.cpp \
    wirelessaccesspoint.cpp \
    wirelessnetworkdevice.cpp \
    networkmanagerutils.cpp \


target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# install header file with relative subdirectory
for(header, HEADERS) {
    path = /usr/include/libnymea-networkmanager/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}

# Create pkgconfig file
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = libnymea-networkmanager
QMAKE_PKGCONFIG_DESCRIPTION = nymea networkmanager development library
QMAKE_PKGCONFIG_PREFIX = $$[QT_INSTALL_PREFIX]
QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_PREFIX]/include/libnymea-networkmanager/
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION_STRING
QMAKE_PKGCONFIG_FILE = libnymea-networkmanager
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
