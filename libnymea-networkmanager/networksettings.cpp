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
    \class NetworkSettings
    \brief Represents a setting for a network connection.
    \inmodule nymea-networkmanager
    \ingroup networkmanager


*/

#include "networksettings.h"
#include "networkmanagerutils.h"

#include <QDebug>

/*! Constructs a new \l{NetworkSettings} object with the given \a parent. */
NetworkSettings::NetworkSettings(QObject *parent) :
    QObject(parent)
{
    qDBusRegisterMetaType<NMVariantMapList>();
    qDBusRegisterMetaType<NMIntListList>();
    qDBusRegisterMetaType<NMIntList>();

    m_settingsInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), NetworkManagerUtils::settingsPathString(), NetworkManagerUtils::settingsInterfaceString(), QDBusConnection::systemBus(), this);
    if(!m_settingsInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "Invalid DBus network settings interface";
        return;
    }

    loadConnections();

    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), NetworkManagerUtils::settingsPathString(), NetworkManagerUtils::settingsInterfaceString(), "NewConnection", this, SLOT(connectionAdded(QDBusObjectPath)));
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), NetworkManagerUtils::settingsPathString(), NetworkManagerUtils::settingsInterfaceString(), "ConnectionRemoved", this, SLOT(connectionRemoved(QDBusObjectPath)));
    // Networkmanager < 1.2.0 uses custom signal instead of the standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), NetworkManagerUtils::settingsPathString(), NetworkManagerUtils::settingsInterfaceString(), "PropertiesChanged", this, SLOT(processProperties(QVariantMap)));
    // Networkmanager >= 1.2.0 uses standard D-Bus properties changed signal
    QDBusConnection::systemBus().connect(NetworkManagerUtils::networkManagerServiceString(), NetworkManagerUtils::settingsPathString(),  "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(onPropertiesChanged(QString,QVariantMap,QStringList)));
}

/*! Add the given \a settings to this \l{NetworkSettings}. Returns the dbus object path from the new settings. */
QDBusObjectPath NetworkSettings::addConnection(const ConnectionSettings &settings)
{
    QDBusMessage query = m_settingsInterface->call("AddConnection", QVariant::fromValue(settings));
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return QDBusObjectPath();
    }

    if (query.arguments().isEmpty())
        return QDBusObjectPath();

    return query.arguments().at(0).value<QDBusObjectPath>();
}

/*! Returns the list of current \l{NetworkConnection}{NetworkConnections} from this \l{NetworkSettings}. */
QList<NetworkConnection *> NetworkSettings::connections() const
{
    return m_connections.values();
}

void NetworkSettings::loadConnections()
{
    qCDebug(dcNetworkManager()) << "Load connection list";
    QDBusMessage query = m_settingsInterface->call("ListConnections");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    argument.beginArray();
    while(!argument.atEnd()) {
        QDBusObjectPath objectPath = qdbus_cast<QDBusObjectPath>(argument);
        connectionAdded(objectPath);
    }
    argument.endArray();

}

void NetworkSettings::connectionAdded(const QDBusObjectPath &objectPath)
{
    NetworkConnection *connection = new NetworkConnection(objectPath, this);
    m_connections.insert(objectPath, connection);

    qCDebug(dcNetworkManager()) << "Settings: [+]" << connection;
}

void NetworkSettings::connectionRemoved(const QDBusObjectPath &objectPath)
{
    NetworkConnection *connection = m_connections.take(objectPath);
    qCDebug(dcNetworkManager()) << "Settings: [-]" << connection;
    connection->deleteLater();
}

void NetworkSettings::onPropertiesChanged(const QString &interface, const QVariantMap &changedProperties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interface)
    Q_UNUSED(invalidatedProperties)
    //qCDebug(dcNetworkManager()) << "Settins: Properties changed" << interface << changedProperties << invalidatedProperties;
    processProperties(changedProperties);
}

void NetworkSettings::processProperties(const QVariantMap &properties)
{
    Q_UNUSED(properties)
}


