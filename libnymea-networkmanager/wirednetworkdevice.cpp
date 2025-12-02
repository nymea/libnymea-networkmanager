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
/*!
    \class WiredNetworkDevice
    \brief Represents a wired network device.
    \inmodule nymea-networkmanager
    \ingroup networkmanager


*/

#include "wirednetworkdevice.h"

#include <QDebug>

/*! Constructs a new \l{WiredNetworkDevice} with the given dbus \a objectPath and \a parent. */
WiredNetworkDevice::WiredNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent) :
    NetworkDevice(objectPath, parent)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(dcNetworkManager()) << "WiredNetworkDevice: System DBus not connected";
        return;
    }

    m_wiredInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wiredInterfaceString(), systemBus, this);
    if(!m_wiredInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "WiredNetworkDevice: Invalid wired dbus interface";
        return;
    }

    m_macAddress = m_wiredInterface->property("HwAddress").toString();
    m_bitRate = m_wiredInterface->property("Bitrate").toInt();
    m_pluggedIn = m_wiredInterface->property("Carrier").toBool();

    // Networkmanager < 1.2.0 uses custom signal instead of the standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wiredInterfaceString(), "PropertiesChanged", this, SLOT(processProperties(QVariantMap)));
    // Networkmanager >= 1.2.0 uses standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

/*! Returns the mac address of this \l{WiredNetworkDevice}. */
QString WiredNetworkDevice::macAddress() const
{
    return m_macAddress;
}

/*! Returns the current bit rate [Mb/s] of this \l{WiredNetworkDevice}. */
int WiredNetworkDevice::bitRate() const
{
    return m_bitRate;
}

/*! Returns true if this \l{WiredNetworkDevice} has a cable plugged in. */
bool WiredNetworkDevice::pluggedIn() const
{
    return m_pluggedIn;
}

void WiredNetworkDevice::onPropertiesChanged(const QString &interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interfaceName)
    Q_UNUSED(invalidatedProperties)
    //qCDebug(dcNetworkManager()) << "WiredNetworkDevice: Properties changed" << interface << changedProperties << invalidatedProperties;
    processProperties(changedProperties);
}

void WiredNetworkDevice::processProperties(const QVariantMap &properties)
{
    if (properties.contains("Carrier")) {
        m_pluggedIn = properties.value("Carrier").toBool();
        emit pluggedInChanged(m_pluggedIn);
    }

    emit deviceChanged();
}

QDebug operator<<(QDebug debug, WiredNetworkDevice *networkDevice)
{
    debug.nospace() << "WiredNetworkDevice(" << networkDevice->interface() << ", ";
    debug.nospace() << networkDevice->macAddress() <<  ", ";
    debug.nospace() << networkDevice->bitRate() <<  " [Mb/s], ";
    debug.nospace() << networkDevice->pluggedIn() <<  ", ";
    debug.nospace() << networkDevice->deviceStateString() <<  ") ";
    return debug.space();
}
