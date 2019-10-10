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

#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QObject>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

#include "networkconnection.h"

class NetworkConnection;

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
    void propertiesChanged(const QVariantMap &properties);

};

#endif // NETWORKSETTINGS_H
