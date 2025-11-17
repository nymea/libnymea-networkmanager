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

#include "networkmanagerutils.h"

Q_LOGGING_CATEGORY(dcNetworkManager, "NetworkManager")
Q_LOGGING_CATEGORY(dcNetworkManagerBluetoothServer, "NetworkManagerBluetoothServer")

QString NetworkManagerUtils::NetworkManagerUtils::networkManagerServiceString()
{
    return "org.freedesktop.NetworkManager";
}

QString NetworkManagerUtils::networkManagerPathString()
{
    return "/org/freedesktop/NetworkManager";
}

QString NetworkManagerUtils::NetworkManagerUtils::settingsPathString()
{
    return "/org/freedesktop/NetworkManager/Settings";
}

QString NetworkManagerUtils::NetworkManagerUtils::deviceInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device";
}

QString NetworkManagerUtils::NetworkManagerUtils::wirelessInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device.Wireless";
}

QString NetworkManagerUtils::NetworkManagerUtils::wiredInterfaceString()
{
    return "org.freedesktop.NetworkManager.Device.Wired";
}

QString NetworkManagerUtils::NetworkManagerUtils::accessPointInterfaceString()
{
    return "org.freedesktop.NetworkManager.AccessPoint";
}

QString NetworkManagerUtils::NetworkManagerUtils::settingsInterfaceString()
{
    return "org.freedesktop.NetworkManager.Settings";
}

QString NetworkManagerUtils::connectionsInterfaceString()
{
    return "org.freedesktop.NetworkManager.Settings.Connection";
}


