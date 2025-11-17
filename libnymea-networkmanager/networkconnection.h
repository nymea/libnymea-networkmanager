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

#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H

#include <QUuid>
#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

typedef QMap<QString, QVariantMap> ConnectionSettings;

class NetworkConnection : public QObject
{
    Q_OBJECT
public:
    explicit NetworkConnection(const QDBusObjectPath &objectPath, QObject *parent = nullptr);

    void deleteConnection();

    static void registerTypes();

    QDBusObjectPath objectPath() const;
    ConnectionSettings connectionSettings() const;

    QString id() const;
    QString name() const;
    QString type() const;
    QUuid uuid() const;
    QString interfaceName() const;
    bool autoconnect() const;
    QDateTime timeStamp() const;

private:
    QDBusObjectPath m_objectPath;
    QDBusInterface *m_connectionInterface = nullptr;

    ConnectionSettings m_connectionSettings;
};

Q_DECLARE_METATYPE(ConnectionSettings)
QDebug operator<<(QDebug debug, NetworkConnection *networkConnection);

#endif // NETWORKCONNECTION_H
