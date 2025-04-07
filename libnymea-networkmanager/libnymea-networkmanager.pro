TARGET = nymea-networkmanager
TEMPLATE = lib

QT += dbus network

greaterThan(QT_MAJOR_VERSION, 5) {
    message("Building using Qt6 support")
    CONFIG *= c++17
    QMAKE_LFLAGS *= -std=c++17
    QMAKE_CXXFLAGS *= -std=c++17
} else {
    message("Building using Qt5 support")
    CONFIG *= c++11
    QMAKE_LFLAGS *= -std=c++11
    QMAKE_CXXFLAGS *= -std=c++11
    DEFINES += QT_DISABLE_DEPRECATED_UP_TO=0x050F00
}

QMAKE_CXXFLAGS *= -Werror -g

DEFINES += VERSION_STRING=\\\"$${VERSION_STRING}\\\"

HEADERS += \
    networkmanager.h \
    networkconnection.h \
    networkdevice.h \
    networksettings.h \
    wirednetworkdevice.h \
    wirelessaccesspoint.h \
    wirelessnetworkdevice.h \
    networkmanagerutils.h

SOURCES += \
    networkmanager.cpp \
    networkconnection.cpp \
    networkdevice.cpp \
    networksettings.cpp \
    wirednetworkdevice.cpp \
    wirelessaccesspoint.cpp \
    wirelessnetworkdevice.cpp \
    networkmanagerutils.cpp

equals(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 7) {
    message(Building with Bluetooth LE server functionality. Qt $${QT_VERSION}.)

    QT += bluetooth

    HEADERS += \
        bluetooth/bluetoothserver.h \
        bluetooth/bluetoothuuids.h \
        bluetooth/networkservice.h \
        bluetooth/wirelessservice.h \

    SOURCES += \
        bluetooth/bluetoothserver.cpp \
        bluetooth/networkservice.cpp \
        bluetooth/wirelessservice.cpp \
} else {
    message(Bluetooth LE server functionality not supported with Qt $${QT_VERSION}.)
}

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

# install header file with relative subdirectory
for(header, HEADERS) {
    path = $$[QT_INSTALL_PREFIX]/include/nymea-networkmanager/$${dirname(header)}
    eval(headers_$${path}.files += $${header})
    eval(headers_$${path}.path = $${path})
    eval(INSTALLS *= headers_$${path})
}

# Create pkgconfig file
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_NAME = nymea-networkmanager
QMAKE_PKGCONFIG_DESCRIPTION = nymea networkmanager development library
QMAKE_PKGCONFIG_PREFIX = $$[QT_INSTALL_PREFIX]
QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_PREFIX]/include/nymea-networkmanager/
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION_STRING
QMAKE_PKGCONFIG_FILE = nymea-networkmanager
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
