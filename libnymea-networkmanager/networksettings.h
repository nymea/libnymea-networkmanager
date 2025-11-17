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

#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QObject>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

#include "networkconnection.h"

class NetworkConnection;

typedef QList<QVariantMap> NMVariantMapList;
Q_DECLARE_METATYPE(NMVariantMapList)

typedef QList<QList<uint>> NMIntListList;
Q_DECLARE_METATYPE(NMIntListList)

typedef QList<uint> NMIntList;
Q_DECLARE_METATYPE(NMIntList)

class NetworkSettings : public QObject
{
    Q_OBJECT
public:
    explicit NetworkSettings(QObject *parent = nullptr);

    QDBusObjectPath addConnection(const ConnectionSettings &settings);
    QList<NetworkConnection *> connections() const;

private:
    QDBusInterface *m_settingsInterface = nullptr;
    QHash<QDBusObjectPath, NetworkConnection *> m_connections;

    void loadConnections();

private slots:
    void connectionAdded(const QDBusObjectPath &objectPath);
    void connectionRemoved(const QDBusObjectPath &objectPath);
    void onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties);
    void processProperties(const QVariantMap &properties);

};

#endif // NETWORKSETTINGS_H
