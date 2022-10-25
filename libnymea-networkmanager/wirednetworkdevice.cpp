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

void WiredNetworkDevice::propertiesChanged(const QString &interface_name, const QVariantMap &changed_properties, const QStringList &invalidated_properties)
{
    Q_UNUSED(interface_name)
    Q_UNUSED(invalidated_properties)
    if (changed_properties.contains("Carrier")) {
        m_pluggedIn = changed_properties.value("Carrier").toBool();
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
