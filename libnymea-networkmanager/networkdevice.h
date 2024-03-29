/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright 2013 - 2020, nymea GmbH
* Contact: contact@nymea.io
*
* This file is part of nymea.
* This project including source code and documentation is protected by
* copyright law, and remains the property of nymea GmbH. All rights, including
* reproduction, publication, editing and translation, are reserved. The use of
* this project is subject to the terms of a license agreement to be concluded
* with nymea GmbH in accordance with the terms of use of nymea GmbH, available
* under https://nymea.io/license
*
* GNU Lesser General Public License Usage
* Alternatively, this project may be redistributed and/or modified under the
* terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; version 3. This project is distributed in the hope that
* it will be useful, but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this project. If not, see <https://www.gnu.org/licenses/>.
*
* For any further details and any questions please contact us under
* contact@nymea.io or see our FAQ/Licensing Information on
* https://nymea.io/license/faq
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QDebug>
#include <QObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>

#include "networkmanagerutils.h"

class NetworkDevice : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkDeviceType)
    Q_ENUMS(NetworkDeviceState)
    Q_ENUMS(NetworkDeviceStateReason)

public:
    enum NetworkDeviceState {
        NetworkDeviceStateUnknown = 0,
        NetworkDeviceStateUnmanaged = 10,
        NetworkDeviceStateUnavailable = 20,
        NetworkDeviceStateDisconnected = 30,
        NetworkDeviceStatePrepare = 40,
        NetworkDeviceStateConfig = 50,
        NetworkDeviceStateNeedAuth = 60,
        NetworkDeviceStateIpConfig = 70,
        NetworkDeviceStateIpCheck = 80,
        NetworkDeviceStateSecondaries = 90,
        NetworkDeviceStateActivated = 100,
        NetworkDeviceStateDeactivating = 110,
        NetworkDeviceStateFailed = 120
    };
    Q_ENUM(NetworkDeviceState)

    enum NetworkDeviceStateReason {
        NetworkDeviceStateReasonNone = 0,
        NetworkDeviceStateReasonUnknown = 1,
        NetworkDeviceStateReasonNowManaged = 2,
        NetworkDeviceStateReasonNowUnmanaged = 3,
        NetworkDeviceStateReasonConfigFailed = 4,
        NetworkDeviceStateReasonIpConfigUnavailable = 5,
        NetworkDeviceStateReasonIpConfigExpired = 6,
        NetworkDeviceStateReasonNoSecrets = 7,
        NetworkDeviceStateReasonSupplicantDisconnected = 8,
        NetworkDeviceStateReasonSupplicantConfigFailed = 9,
        NetworkDeviceStateReasonSupplicantFailed = 10,
        NetworkDeviceStateReasonSupplicantTimeout = 11,
        NetworkDeviceStateReasonPppStartFailed = 12,
        NetworkDeviceStateReasonPppDisconnected = 13,
        NetworkDeviceStateReasonPppFailed = 14,
        NetworkDeviceStateReasonDhcpStartFailed = 15,
        NetworkDeviceStateReasonDhcpError = 16,
        NetworkDeviceStateReasonDhcpFailed = 17,
        NetworkDeviceStateReasonSharedStartFailed = 18,
        NetworkDeviceStateReasonSharedFailed = 19,
        NetworkDeviceStateReasonAutoIpStartFailed = 20,
        NetworkDeviceStateReasonAutoIpError = 21,
        NetworkDeviceStateReasonAutoIpFailed = 22,
        NetworkDeviceStateReasonModemBusy = 23,
        NetworkDeviceStateReasonModemNoDialTone = 24,
        NetworkDeviceStateReasonModemNoCarrier = 25,
        NetworkDeviceStateReasonModemDialTimeout = 26,
        NetworkDeviceStateReasonModemDialFailed = 27,
        NetworkDeviceStateReasonModemInitFailed = 28,
        NetworkDeviceStateReasonGsmApnFailed = 29,
        NetworkDeviceStateReasonGsmRegistrationNotSearching = 30,
        NetworkDeviceStateReasonGsmRegistrationDenied = 31,
        NetworkDeviceStateReasonGsmRegistrationTimeout = 32,
        NetworkDeviceStateReasonGsmRegistrationFailed = 33,
        NetworkDeviceStateReasonGsmPinCheckFailed = 34,
        NetworkDeviceStateReasonFirmwareMissing = 35,
        NetworkDeviceStateReasonRemoved = 36,
        NetworkDeviceStateReasonSleeping = 37,
        NetworkDeviceStateReasonConnectionRemoved = 38,
        NetworkDeviceStateReasonUserRequest = 39,
        NetworkDeviceStateReasonCarrier = 40,
        NetworkDeviceStateReasonConnectionAssumed = 41,
        NetworkDeviceStateReasonSupplicantAvailable = 42,
        NetworkDeviceStateReasonModemNotFound = 43,
        NetworkDeviceStateReasonBtFailed = 44,
        NetworkDeviceStateReasonGsmSimNotInserted = 45,
        NetworkDeviceStateReasonGsmSimPinRequired = 46,
        NetworkDeviceStateReasonGsmSimPukRequired = 47,
        NetworkDeviceStateReasonGsmSimWrong = 48,
        NetworkDeviceStateReasonInfinibandMode = 49,
        NetworkDeviceStateReasonDependencyFailed = 50,
        NetworkDeviceStateReasonBR2684Failed = 51,
        NetworkDeviceStateReasonModemManagerUnavailable = 52,
        NetworkDeviceStateReasonSsidNotFound = 53,
        NetworkDeviceStateReasonSecondaryConnectionFailed = 54,
        NetworkDeviceStateReasonDcbFoecFailed = 55,
        NetworkDeviceStateReasonTeamdControlFailed = 56,
        NetworkDeviceStateReasonModemFailed = 57,
        NetworkDeviceStateReasonModemAvailable = 58,
        NetworkDeviceStateReasonSimPinIncorrect = 59,
        NetworkDeviceStateReasonNewActivision = 60,
        NetworkDeviceStateReasonParentChanged = 61,
        NetworkDeviceStateReasonParentManagedChanged = 62
    };
    Q_ENUM(NetworkDeviceStateReason)

    enum NetworkDeviceType {
        NetworkDeviceTypeUnknown = 0,
        NetworkDeviceTypeEthernet = 1,
        NetworkDeviceTypeWifi = 2,
        NetworkDeviceTypeBluetooth = 5,
        NetworkDeviceTypeOlpcMesh = 6,
        NetworkDeviceTypeWiMax = 7,
        NetworkDeviceTypeModem = 8,
        NetworkDeviceTypeInfiniBand = 9,
        NetworkDeviceTypeBond = 10,
        NetworkDeviceTypeVLan = 11,
        NetworkDeviceTypeAdsl = 12,
        NetworkDeviceTypeBridge = 13,
        NetworkDeviceTypeGeneric = 14,
        NetworkDeviceTypeTeam = 15,
        NetworkDeviceTypeTun = 16,
        NetworkDeviceTypeIpTunnel = 17,
        NetworkDeviceTypeMacVLan = 18,
        NetworkDeviceTypeVXLan = 19,
        NetworkDeviceTypeVEth = 20,
    };
    Q_ENUM(NetworkDeviceType)

    explicit NetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = nullptr);

    QDBusObjectPath objectPath() const;

    QString udi() const;
    QString interface() const;
    QString ipInterface() const;
    QString driver() const;
    QString driverVersion() const;
    QString firmwareVersion() const;
    QString physicalPortId() const;
    uint mtu() const;
    uint metered() const;
    bool autoconnect() const;
    QStringList ipv4Addresses() const;
    QStringList ipv6Addresses() const;

    NetworkDeviceState deviceState() const;
    QString deviceStateString() const;

    NetworkDeviceStateReason deviceStateReason() const;
    NetworkDeviceType deviceType() const;

    QDBusObjectPath activeConnection() const;
    QDBusObjectPath ip4Config() const;
    QList<QDBusObjectPath> availableConnections() const;

    void disconnectDevice();

    static QString deviceTypeToString(const NetworkDeviceType &deviceType);
    static QString deviceStateToString(const NetworkDeviceState &deviceState);
    static QString deviceStateReasonToString(const NetworkDeviceStateReason &deviceStateReason);

signals:
    void deviceChanged();
    void stateChanged(const NetworkDeviceState &state);

private slots:
    void onStateChanged(uint newState, uint oldState, uint reason);

private:
    QStringList readIpAddresses(const QString &property, const QString &interface);

private:
    QDBusInterface *m_networkDeviceInterface = nullptr;
    QDBusObjectPath m_objectPath;

    // Device properties
    QString m_udi;
    QString m_interface;
    QString m_ipInterface;
    QString m_driver;
    QString m_driverVersion;
    QString m_firmwareVersion;
    QString m_physicalPortId;
    QList<QString> m_ipv4Addresses;
    QList<QString> m_ipv6Addresses;
    uint m_mtu = 0;
    uint m_metered = 0;
    bool m_autoconnect = false;
    NetworkDeviceState m_deviceState = NetworkDeviceStateUnknown;
    NetworkDeviceStateReason m_deviceStateReason = NetworkDeviceStateReasonUnknown;
    NetworkDeviceType m_deviceType = NetworkDeviceTypeUnknown;

    QDBusObjectPath m_activeConnection;

    QList<QDBusObjectPath> m_availableConnections;

};

QDebug operator<<(QDebug debug, NetworkDevice *device);

#endif // NETWORKDEVICE_H
