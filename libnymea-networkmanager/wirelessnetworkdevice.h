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

#ifndef WIRELESSNETWORKMANAGER_H
#define WIRELESSNETWORKMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusContext>
#include <QDBusArgument>

#include "networkdevice.h"
#include "wirelessaccesspoint.h"

class WirelessNetworkDevice : public NetworkDevice
{
    Q_OBJECT
public:
    enum WirelessMode {
        WirelessModeUnknown          = 0,
        WirelessModeAdhoc            = 1,
        WirelessModeInfrastructure   = 2,
        WirelessModeAccessPoint      = 3
    };
    Q_ENUM(WirelessMode)

    enum WirelessCapability {
        WirelessCapabilityNone = 0x0000,
        WirelessCapabilityCipherWEP40 = 0x0001,
        WirelessCapabilityCipherWEP104 = 0x0002,
        WirelessCapabilityCipherTKIP = 0x0004,
        WirelessCapabilityCipherCCMP = 0x0008,
        WirelessCapabilityWPA = 0x0010,
        WirelessCapabilityRSN = 0x0020,
        WirelessCapabilityAP = 0x0040,
        WirelessCapabilityAdHoc = 0x0080,
        WirelessCapabilityFreqValid = 0x0100,
        WirelessCapability2Ghz = 0x0200,
        WirelessCapability5Ghz = 0x0400,
    };
    Q_ENUM(WirelessCapability)

    Q_DECLARE_FLAGS(WirelessCapabilities, WirelessCapability)
    Q_FLAG(WirelessCapabilities)

    explicit WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = nullptr);

    // Properties
    QString macAddress() const;
    int bitRate() const;
    WirelessCapabilities wirelessCapabilities() const;
    WirelessMode wirelessMode() const;
    WirelessAccessPoint *activeAccessPoint();

    // Accesspoints
    QList<WirelessAccessPoint *> accessPoints();
    WirelessAccessPoint *getAccessPoint(const QString &ssid);
    WirelessAccessPoint *getAccessPoint(const QDBusObjectPath &objectPath);

    // Methods
    void scanWirelessNetworks();

signals:
    void bitRateChanged(int bitRate);
    void wirelessCapabilitiesChanged(WirelessCapabilities wirelessCapabilities);
    void wirelessModeChanged(WirelessMode mode);
    void lastScanChanged(int lastScan);

private slots:
    void accessPointAdded(const QDBusObjectPath &objectPath);
    void accessPointRemoved(const QDBusObjectPath &objectPath);
    void processProperties(const QVariantMap &properties);
    void onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);

private:
    QDBusInterface *m_wirelessInterface = nullptr;
    WirelessAccessPoint *m_activeAccessPoint = nullptr;

    int m_bitRate;
    QString m_macAddress;
    WirelessCapabilities m_wirelessCapabilities = WirelessCapabilityNone;
    WirelessMode m_wirelessMode = WirelessModeUnknown;
    int m_lastScan = -1;
    QDBusObjectPath m_activeAccessPointObjectPath;

    QHash<QDBusObjectPath, WirelessAccessPoint *> m_accessPointsTable;

    void readAccessPoints();

    void setActiveAccessPoint(const QDBusObjectPath &activeAccessPointObjectPath);
};

QDebug operator<<(QDebug debug, WirelessNetworkDevice *device);

#endif // WIRELESSNETWORKMANAGER_H
