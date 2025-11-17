// SPDX-License-Identifier: LGPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of libnymea-networkmanager.
*
* libnymea-networkmanager is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3
* of the License, or (at your option) any later version.
*
* libnymea-networkmanager is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with libnymea-networkmanager. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QHostAddress>

#include "networksettings.h"
#include "wirednetworkdevice.h"
#include "networkmanagerutils.h"
#include "wirelessnetworkdevice.h"

// Docs: https://developer.gnome.org/NetworkManager/unstable/spec.html

class NetworkManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkManagerState)
    Q_ENUMS(NetworkManagerError)
    Q_ENUMS(NetworkManagerConnectivityState)

public:
    enum NetworkManagerState {
        NetworkManagerStateUnknown = 0,
        NetworkManagerStateAsleep = 10,
        NetworkManagerStateDisconnected = 20,
        NetworkManagerStateDisconnecting = 30,
        NetworkManagerStateConnecting = 40,
        NetworkManagerStateConnectedLocal = 50,
        NetworkManagerStateConnectedSite = 60,
        NetworkManagerStateConnectedGlobal = 70
    };
    Q_ENUM(NetworkManagerState)

    enum NetworkManagerConnectivityState {
        NetworkManagerConnectivityStateUnknown = 0,
        NetworkManagerConnectivityStateNone = 1,
        NetworkManagerConnectivityStatePortal = 2,
        NetworkManagerConnectivityStateLimited = 3,
        NetworkManagerConnectivityStateFull = 4
    };
    Q_ENUM(NetworkManagerConnectivityState)

    enum NetworkManagerError {
        NetworkManagerErrorNoError,
        NetworkManagerErrorUnknownError,
        NetworkManagerErrorWirelessNotAvailable,
        NetworkManagerErrorAccessPointNotFound,
        NetworkManagerErrorNetworkInterfaceNotFound,
        NetworkManagerErrorInvalidNetworkDeviceType,
        NetworkManagerErrorWirelessNetworkingDisabled,
        NetworkManagerErrorWirelessConnectionFailed,
        NetworkManagerErrorNetworkingDisabled,
        NetworkManagerErrorNetworkManagerNotAvailable,
        NetworkManagerErrorInvalidConfiguration,
        NetworkManagerErrorUnsupportedFeature
    };
    Q_ENUM(NetworkManagerError)

    enum AuthAlgorithm {
        AuthAlgorithmOpen
    };
    Q_ENUM(AuthAlgorithm)

    enum KeyManagement {
        KeyManagementWpaPsk
    };
    Q_ENUM(KeyManagement)

    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    bool available() const;
    bool wirelessAvailable() const;

    QList<NetworkDevice *> networkDevices() const;
    QList<WirelessNetworkDevice *> wirelessNetworkDevices() const;
    QList<WiredNetworkDevice *> wiredNetworkDevices() const;

    NetworkSettings *networkSettings() const;
    NetworkDevice *getNetworkDevice(const QString &interface);

    // Properties
    QString version() const;
    NetworkManagerState state() const;
    QString stateString() const;
    NetworkManagerConnectivityState connectivityState() const;

    NetworkManagerError connectWifi(const QString &interface, const QString &ssid, const QString &password, AuthAlgorithm authAlgorithm = AuthAlgorithmOpen, KeyManagement keyManagement = KeyManagementWpaPsk, bool hidden = false);
    NetworkManagerError startAccessPoint(const QString &interface, const QString &ssid, const QString &password);
    NetworkManagerError createWiredAutoConnection(const QString &interface);
    NetworkManagerError createWiredManualConnection(const QString &interface, const QHostAddress &ip, quint8 prefix, const QHostAddress &gateway, const QHostAddress &dns);
    NetworkManagerError createSharedConnection(const QString& interface, const QHostAddress &ip, quint8 prefix);

    // Networking
    bool networkingEnabled() const;
    bool enableNetworking(bool enabled);

    // Wireless Networking
    bool wirelessEnabled() const;
    bool enableWireless(bool enabled);

    void checkConnectivity();

private:
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
    QDBusInterface *m_networkManagerInterface  = nullptr;
    NetworkSettings *m_networkSettings  = nullptr;

    QHash<QDBusObjectPath, NetworkDevice *> m_networkDevices;
    QHash<QDBusObjectPath, WirelessNetworkDevice *> m_wirelessNetworkDevices;
    QHash<QDBusObjectPath, WiredNetworkDevice *> m_wiredNetworkDevices;

    bool m_available = false;

    QString m_version;
    NetworkManagerState m_state = NetworkManagerStateUnknown;
    NetworkManagerConnectivityState m_connectivityState = NetworkManagerConnectivityStateUnknown;
    bool m_networkingEnabled = false;
    bool m_wirelessEnabled = false;

    void init();
    void deinit();

    void loadDevices();

    static QString networkManagerStateToString(const NetworkManagerState &state);
    static QString networkManagerConnectivityStateToString(const NetworkManagerConnectivityState &state);

    void setAvailable(bool available);
    void setVersion(const QString &version);
    void setNetworkingEnabled(bool enabled);
    void setWirelessEnabled(bool enabled);
    void setConnectivityState(const NetworkManagerConnectivityState &connectivityState);
    void setState(const NetworkManagerState &state);

signals:
    void availableChanged(bool available);
    void versionChanged(const QString &version);
    void networkingEnabledChanged(bool enabled);
    void wirelessEnabledChanged(bool enabled);
    void wirelessAvailableChanged(bool available);
    void stateChanged(const NetworkManagerState &state);
    void connectivityStateChanged(const NetworkManagerConnectivityState &state);

    void wirelessDeviceAdded(WirelessNetworkDevice *wirelessDevice);
    void wirelessDeviceRemoved(const QString &interface);
    void wirelessDeviceChanged(WirelessNetworkDevice *wirelessDevice);

    void wiredDeviceAdded(WiredNetworkDevice *wiredDevice);
    void wiredDeviceRemoved(const QString &interface);
    void wiredDeviceChanged(WiredNetworkDevice *wiredDevice);

private slots:
    void onServiceRegistered();
    void onServiceUnregistered();

    void onStateChanged(uint state);
    void onDeviceAdded(const QDBusObjectPath &deviceObjectPath);
    void onDeviceRemoved(const QDBusObjectPath &deviceObjectPath);
    void onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void processProperties(const QVariantMap &properties);

    void onWirelessDeviceChanged();
    void onWiredDeviceChanged();

public slots:
    void start();
    void stop();

};

#endif // NETWORKMANAGER_H
