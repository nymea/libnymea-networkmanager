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

#ifndef WIREDNETWORKDEVICE_H
#define WIREDNETWORKDEVICE_H

#include <QObject>
#include <QDBusObjectPath>

#include "networkdevice.h"

class WiredNetworkDevice : public NetworkDevice
{
    Q_OBJECT
public:
    explicit WiredNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent = nullptr);

    QString macAddress() const;
    int bitRate() const;
    bool pluggedIn() const;

signals:
    void pluggedInChanged(bool pluggedIn);

private slots:
    void onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void processProperties(const QVariantMap &properties);

private:
    QDBusInterface *m_wiredInterface = nullptr;

    QString m_macAddress;
    int m_bitRate = 0;
    bool m_pluggedIn = false;

};

QDebug operator<<(QDebug debug, WiredNetworkDevice *networkDevice);

#endif // WIREDNETWORKDEVICE_H
