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
    \class WirelessAccessPoint
    \brief Represents a discovered wireless access point.
    \inmodule nymea-networkmanager
    \ingroup networkmanager


*/

/*! \enum WirelessAccessPoint::ApSecurityMode
    \value ApSecurityModeNone
    \value ApSecurityModePairWep40
    \value ApSecurityModePairWep104
    \value ApSecurityModePairTkip
    \value ApSecurityModePairCcmp
    \value ApSecurityModeGroupWep40
    \value ApSecurityModeGroupWep104
    \value ApSecurityModeGroupTkip
    \value ApSecurityModeGroupCcmp
    \value ApSecurityModeKeyMgmtPsk
    \value ApSecurityModeKeyMgmt8021X
    \value ApSecurityModeKeyMgmtSae
*/

/*! \fn void WirelessAccessPoint::signalStrengthChanged();
    This signal will be emitted when the signalStrength of this \l{WirelessAccessPoint} has changed.
*/

#include "wirelessaccesspoint.h"
#include "networkmanagerutils.h"

/*! Constructs a new \l{WirelessAccessPoint} with the given dbus \a objectPath and \a parent. */
WirelessAccessPoint::WirelessAccessPoint(const QDBusObjectPath &objectPath, QObject *parent) :
    QObject(parent),
    m_objectPath(objectPath)
{
    QDBusInterface accessPointInterface(NetworkManagerUtils::networkManagerServiceString(), m_objectPath.path(), NetworkManagerUtils::accessPointInterfaceString(), QDBusConnection::systemBus());
    if(!accessPointInterface.isValid()) {
        qCWarning(dcNetworkManager()) << "Invalid access point dbus interface";
        return;
    }

    // Init properties
    setSsid(accessPointInterface.property("Ssid").toString());
    setMacAddress(accessPointInterface.property("HwAddress").toString());
    setFrequency(accessPointInterface.property("Frequency").toDouble() / 1000);
    setSignalStrength(accessPointInterface.property("Strength").toInt());
    m_capabilities = static_cast<WirelessAccessPoint::ApFlags>(accessPointInterface.property("Flags").toUInt());
    setWpaFlags(WirelessAccessPoint::ApSecurityModes(accessPointInterface.property("WpaFlags").toUInt()));
    setRsnFlags(WirelessAccessPoint::ApSecurityModes(accessPointInterface.property("RsnFlags").toUInt()));
    setIsProtected(m_rsnFlags != 0);

    qCDebug(dcNetworkManager()) << ssid() << "WPA flags:" << m_wpaFlags;
    qCDebug(dcNetworkManager()) << ssid() << "RSN flags:" << m_rsnFlags;
    qCDebug(dcNetworkManager()) << ssid() << "Capabilities:" << m_capabilities;

    // Networkmanager < 1.2.0 uses custom signal instead of the standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), this->objectPath().path(), NetworkManagerUtils::accessPointInterfaceString(), "PropertiesChanged", this, SLOT(processProperties(QVariantMap)));
    // Networkmanager >= 1.2.0 uses standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), objectPath.path(),  "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(onPropertiesChanged(QString,QVariantMap,QStringList)));
}

/*! Returns the dbus object path of this \l{WirelessAccessPoint}. */
QDBusObjectPath WirelessAccessPoint::objectPath() const
{
    return m_objectPath;
}

/*! Returns the ssid of this \l{WirelessAccessPoint}. */
QString WirelessAccessPoint::ssid() const
{
    return m_ssid;
}

void WirelessAccessPoint::setSsid(const QString &ssid)
{
    m_ssid = ssid;
}

/*! Returns the mac address of this \l{WirelessAccessPoint}. */
QString WirelessAccessPoint::macAddress() const
{
    return m_macAddress;
}

void WirelessAccessPoint::setMacAddress(const QString &macAddress)
{
    m_macAddress = macAddress;
}

/*! Returns the frequency of this \l{WirelessAccessPoint}. (2.4 GHz or 5GHz) */
double WirelessAccessPoint::frequency() const
{
    return m_frequency;
}

void WirelessAccessPoint::setFrequency(double frequency)
{
    m_frequency = frequency;
}

/*! Returns the signal strength in percentage [0, 100] % of this \l{WirelessAccessPoint}. */
int WirelessAccessPoint::signalStrength() const
{
    return m_signalStrength;
}

void WirelessAccessPoint::setSignalStrength(int signalStrength)
{
    m_signalStrength = signalStrength;
    emit signalStrengthChanged();
}

void WirelessAccessPoint::setWpaFlags(WirelessAccessPoint::ApSecurityModes wpaFlags)
{
    m_wpaFlags = wpaFlags;
}

void WirelessAccessPoint::setRsnFlags(WirelessAccessPoint::ApSecurityModes rsnFlags)
{
    m_rsnFlags = rsnFlags;
}

void WirelessAccessPoint::setIsProtected(bool isProtected)
{
    m_isProtected = isProtected;
}

void WirelessAccessPoint::onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interface)
    Q_UNUSED(invalidatedProperties)
    //qCDebug(dcNetworkManager()) << "WirelessAccessPoint: Properties changed" << interface << changedProperties << invalidatedProperties;
    processProperties(changedProperties);
}

void WirelessAccessPoint::processProperties(const QVariantMap &properties)
{
    if (properties.contains("Strength"))
        setSignalStrength(properties.value("Strength").toInt());
}

/*! Returns true if this \l{WirelessAccessPoint} is password protected. */
bool WirelessAccessPoint::isProtected() const
{
    return m_isProtected;
}

WirelessAccessPoint::ApFlags WirelessAccessPoint::capabilities() const
{
    return m_capabilities;
}

/*! Returns the WPA security flags of this \l{WirelessAccessPoint}.

    This flags are describing the access point's capabilities according to WPA (Wifi Protected Access).

    \sa WirelessAccessPoint::ApSecurityModes
*/
WirelessAccessPoint::ApSecurityModes WirelessAccessPoint::wpaFlags() const
{
    return m_wpaFlags;
}

/*! Returns the RSN security flags of this \l{WirelessAccessPoint}.

    Flags describing the access point's capabilities according to the RSN (Robust Secure Network) protocol.

    \sa WirelessAccessPoint::ApSecurityModes
*/
WirelessAccessPoint::ApSecurityModes WirelessAccessPoint::rsnFlags() const
{
    return m_rsnFlags;
}

QDebug operator<<(QDebug debug, WirelessAccessPoint *accessPoint)
{
    debug.nospace() << "AccessPoint(" << accessPoint->signalStrength() << "%, "
                           <<  accessPoint->frequency()<< " GHz, "
                            << accessPoint->ssid() << ", " <<
                               (accessPoint->isProtected() ? "protected" : "open" )
                            << ")";
    return debug.space();
}
