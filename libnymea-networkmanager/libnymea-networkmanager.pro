TARGET = nymea-networkmanager
TEMPLATE = lib

QT += dbus network

QMAKE_CXXFLAGS *= -Werror -std=c++11 -g
QMAKE_LFLAGS *= -std=c++11

gcc {
    COMPILER_VERSION = $$system($$QMAKE_CXX " -dumpversion")
    COMPILER_MAJOR_VERSION = $$str_member($$COMPILER_VERSION)
    greaterThan(COMPILER_MAJOR_VERSION, 7): QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

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

equals(QT_MAJOR_VERSION, 5):!lessThan(QT_MINOR_VERSION, 7):qtHaveModule(bluetooth) {
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
