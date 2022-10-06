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
    \class WirelessNetworkDevice
    \brief Represents a wireless network device.
    \inmodule nymea-networkmanager
    \ingroup networkmanager


*/

/*! \enum WirelessNetworkDevice::Mode
    \value ModeUnknown
    \value ModeAdhoc
    \value ModeInfrastructure
    \value ModeAccessPoint

*/

/*! \fn void WirelessNetworkDevice::bitRateChanged(int bitRate);
    This signal will be emitted when the \a bitRate of this \l{WirelessNetworkDevice} has changed.
*/

/*! \fn void WirelessNetworkDevice::modeChanged(Mode mode);
    This signal will be emitted when the current \a mode of this \l{WirelessNetworkDevice} has changed.

    \sa WirelessNetworkDevice::Mode
*/


#include "networkmanagerutils.h"
#include "wirelessnetworkdevice.h"

#include <QUuid>
#include <QDebug>
#include <QMetaEnum>

/*! Constructs a new \l{WirelessNetworkDevice} with the given dbus \a objectPath and \a parent. */
WirelessNetworkDevice::WirelessNetworkDevice(const QDBusObjectPath &objectPath, QObject *parent) :
    NetworkDevice(objectPath, parent),
    m_activeAccessPoint(nullptr)
{
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!systemBus.isConnected()) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: System DBus not connected";
        return;
    }

    m_wirelessInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), systemBus, this);
    if (!m_wirelessInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "WirelessNetworkDevice: Invalid wireless dbus interface";
        return;
    }

    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "AccessPointAdded", this, SLOT(accessPointAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "AccessPointRemoved", this, SLOT(accessPointRemoved(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::wirelessInterfaceString(), "PropertiesChanged", this, SLOT(propertiesChanged(QVariantMap)));

    readAccessPoints();

    m_macAddress = m_wirelessInterface->property("HwAddress").toString();
    m_wirelessMode = static_cast<WirelessMode>(m_wirelessInterface->property("Mode").toUInt());
    m_bitRate = m_wirelessInterface->property("Bitrate").toInt() / 1000;
    setActiveAccessPoint(qdbus_cast<QDBusObjectPath>(m_wirelessInterface->property("ActiveAccessPoint")));
}

/*! Returns the mac address of this \l{WirelessNetworkDevice}. */
QString WirelessNetworkDevice::macAddress() const
{
    return m_macAddress;
}

/*! Returns the bit rate [Mb/s] of this \l{WirelessNetworkDevice}. */
int WirelessNetworkDevice::bitRate() const
{
    return m_bitRate;
}

WirelessNetworkDevice::WirelessMode WirelessNetworkDevice::wirelessMode() const
{
    return m_wirelessMode;
}

/*! Returns the current active \l{WirelessAccessPoint} of this \l{WirelessNetworkDevice}. */
WirelessAccessPoint *WirelessNetworkDevice::activeAccessPoint()
{
    return m_activeAccessPoint;
}

/*! Perform a wireless network scan on this \l{WirelessNetworkDevice}. */
void WirelessNetworkDevice::scanWirelessNetworks()
{
    qCDebug(dcNetworkManager()) << "Request scan" << this;
    QDBusMessage query = m_wirelessInterface->call("RequestScan", QVariantMap());
    if (query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << "Scan error:" << query.errorName() << query.errorMessage();
        return;
    }
}

/*! Returns the list of all \l{WirelessAccessPoint}{WirelessAccessPoints} of this \l{WirelessNetworkDevice}. */
QList<WirelessAccessPoint *> WirelessNetworkDevice::accessPoints()
{
    return m_accessPointsTable.values();
}

/*! Returns the \l{WirelessAccessPoint} with the given \a ssid. If the \l{WirelessAccessPoint} could not be found, return nullptr. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QString &ssid)
{
    foreach (WirelessAccessPoint *accessPoint, m_accessPointsTable.values()) {
        if (accessPoint->ssid() == ssid)
            return accessPoint;
    }
    return nullptr;

}

/*! Returns the \l{WirelessAccessPoint} with the given \a objectPath. If the \l{WirelessAccessPoint} could not be found, return nullptr. */
WirelessAccessPoint *WirelessNetworkDevice::getAccessPoint(const QDBusObjectPath &objectPath)
{
    return m_accessPointsTable.value(objectPath);
}

void WirelessNetworkDevice::readAccessPoints()
{
    QDBusMessage query = m_wirelessInterface->call("GetAccessPoints");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    argument.beginArray();
    while (!argument.atEnd()) {
        QDBusObjectPath accessPointObjectPath = qdbus_cast<QDBusObjectPath>(argument);
        accessPointAdded(accessPointObjectPath);
    }
    argument.endArray();
}

void WirelessNetworkDevice::setActiveAccessPoint(const QDBusObjectPath &activeAccessPointObjectPath)
{
    if (m_activeAccessPointObjectPath != activeAccessPointObjectPath) {
        m_activeAccessPointObjectPath = activeAccessPointObjectPath;
        if (m_accessPointsTable.contains(m_activeAccessPointObjectPath)) {
            if (m_activeAccessPoint)
                disconnect(m_activeAccessPoint, &WirelessAccessPoint::signalStrengthChanged, this, &WirelessNetworkDevice::deviceChanged);

            // Set new access point object
            m_activeAccessPoint = m_accessPointsTable.value(activeAccessPointObjectPath);
            // Update the device when the signalstrength changed
            connect(m_activeAccessPoint, &WirelessAccessPoint::signalStrengthChanged, this, &WirelessNetworkDevice::deviceChanged);
        } else {
            m_activeAccessPoint = nullptr;
        }
        emit deviceChanged();
    }
}

void WirelessNetworkDevice::accessPointAdded(const QDBusObjectPath &objectPath)
{
    QDBusInterface accessPointInterface(NetworkManagerUtils::networkManagerServiceString(), objectPath.path(), NetworkManagerUtils::accessPointInterfaceString(), QDBusConnection::systemBus());
    if (!accessPointInterface.isValid()) {
        qCWarning(dcNetworkManager()) << this << "Invalid access point dbus interface";
        return;
    }

    if (m_accessPointsTable.keys().contains(objectPath)) {
        qCWarning(dcNetworkManager()) << this << "Access point already added" << objectPath.path();
        return;
    }

    WirelessAccessPoint *accessPoint = new WirelessAccessPoint(objectPath, this);
    qCDebug(dcNetworkManager()) << interface() << "[+]" << accessPoint;
    m_accessPointsTable.insert(objectPath, accessPoint);
}

void WirelessNetworkDevice::accessPointRemoved(const QDBusObjectPath &objectPath)
{
    if (!m_accessPointsTable.keys().contains(objectPath))
        return;

    WirelessAccessPoint *accessPoint = m_accessPointsTable.take(objectPath);
    if (accessPoint == m_activeAccessPoint)
        m_activeAccessPoint = nullptr;

    qCDebug(dcNetworkManager()) << interface() << "[-]" << accessPoint;
    accessPoint->deleteLater();
}

void WirelessNetworkDevice::propertiesChanged(const QString &interface_name, const QVariantMap &changed_properties, const QStringList &invalidated_properties)
{
    Q_UNUSED(interface_name)
    Q_UNUSED(invalidated_properties)

    //qCDebug(dcNetworkManager()) << "WirelessNetworkDevice: Property changed" << properties;

    if (changed_properties.contains("Bitrate")) {
        m_bitRate = changed_properties.value("Bitrate").toInt() / 1000;
        emit bitRateChanged(m_bitRate);
    }

    if (changed_properties.contains("Mode")) {
        m_wirelessMode = static_cast<WirelessMode>(m_wirelessInterface->property("Mode").toUInt());
        emit wirelessModeChanged(m_wirelessMode);
    }

    // Note: available since 1.12 (-1 means never scanned)
    if (changed_properties.contains("LastScan")) {
        m_lastScan = m_wirelessInterface->property("LastScan").toInt();
        emit lastScanChanged(m_lastScan);
    }

    if (changed_properties.contains("ActiveAccessPoint")) {
        setActiveAccessPoint(qdbus_cast<QDBusObjectPath>(changed_properties.value("ActiveAccessPoint")));
    }

    emit deviceChanged();
}

/*! Writes the given \a device to the given to \a debug. \sa WirelessNetworkDevice, */
QDebug operator<<(QDebug debug, WirelessNetworkDevice *device)
{
    debug.nospace() << "WirelessNetworkDevice(" << device->interface() << ", ";
    debug.nospace() << device->macAddress() <<  ", ";
    debug.nospace() << device->wirelessMode() <<  ", ";
    debug.nospace() << device->bitRate() <<  " [Mb/s], ";
    debug.nospace() << device->deviceStateString() <<  ") ";
    return debug.space();
}
