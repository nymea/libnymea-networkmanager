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

#ifndef NETWORKMANAGERUTILS_H
#define NETWORKMANAGERUTILS_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dcNetworkManager)
Q_DECLARE_LOGGING_CATEGORY(dcNetworkManagerBluetoothServer)

class NetworkManagerUtils
{
    Q_GADGET

public:
    static QString networkManagerServiceString();

    static QString networkManagerPathString();
    static QString settingsPathString();

    static QString deviceInterfaceString();
    static QString wirelessInterfaceString();
    static QString wiredInterfaceString();
    static QString accessPointInterfaceString();
    static QString settingsInterfaceString();
    static QString connectionsInterfaceString();

};

#endif // NETWORKMANAGERUTILS_H
