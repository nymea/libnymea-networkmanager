/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 * Copyright (C) 2018-2019 Simon Stürz <simon.stuerz@nymea.io>             *
 *                                                                         *
 * This file is part of libnymea-networkmanager.                           *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
    enum Mode {
        ModeUnknown          = 0,
        ModeAdhoc            = 1,
        ModeInfrastructure   = 2,
        ModeAccessPoint      = 3
    };
    Q_ENUM(Mode)

    explicit WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = nullptr);

    // Properties
    QString macAddress() const;
    int bitRate() const;
    Mode mode() const;
    WirelessAccessPoint *activeAccessPoint();

    // Accesspoints
    QList<WirelessAccessPoint *> accessPoints();
    WirelessAccessPoint *getAccessPoint(const QString &ssid);
    WirelessAccessPoint *getAccessPoint(const QDBusObjectPath &objectPath);

    // Methods
    void scanWirelessNetworks();

private:
    QDBusInterface *m_wirelessInterface = nullptr;
    WirelessAccessPoint *m_activeAccessPoint = nullptr;

    int m_bitRate;
    QString m_macAddress;
    Mode m_mode = ModeUnknown;
    int m_lastScan = -1;
    QDBusObjectPath m_activeAccessPointObjectPath;

    QHash<QDBusObjectPath, WirelessAccessPoint *> m_accessPointsTable;

    void readAccessPoints();

    void setMacAddress(const QString &macAddress);
    void setMode(Mode mode);
    void setBitrate(int bitRate);
    void setLastScan(int lastScan);
    void setActiveAccessPoint(const QDBusObjectPath &activeAccessPointObjectPath);

private slots:
    void accessPointAdded(const QDBusObjectPath &objectPath);
    void accessPointRemoved(const QDBusObjectPath &objectPath);
    void propertiesChanged(const QVariantMap &properties);

signals:
    void bitRateChanged(int bitRate);
    void modeChanged(Mode mode);

};

QDebug operator<<(QDebug debug, WirelessNetworkDevice *device);

#endif // WIRELESSNETWORKMANAGER_H
