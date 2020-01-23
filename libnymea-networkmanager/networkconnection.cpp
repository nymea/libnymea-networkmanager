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
    \class NetworkConnection
    \brief Represents a network connection configuration.
    \inmodule nymea-networkmanager
    \ingroup networkmanager

*/


#include "networkconnection.h"
#include "networkmanagerutils.h"

#include <QDebug>
#include <QJsonDocument>

/*! Constructs a new \l{NetworkConnection} with the given dbus \a objectPath and \a parent. */
NetworkConnection::NetworkConnection(const QDBusObjectPath &objectPath, QObject *parent) :
    QObject(parent),
    m_objectPath(objectPath)
{
    m_connectionInterface = new QDBusInterface(NetworkManagerUtils::networkManagerServiceString(), m_objectPath.path(), NetworkManagerUtils::connectionsInterfaceString(), QDBusConnection::systemBus(), this);
    if(!m_connectionInterface->isValid()) {
        qCWarning(dcNetworkManager()) << "Invalid connection dbus interface";
        return;
    }

    QDBusMessage query = m_connectionInterface->call("GetSettings");
    if(query.type() != QDBusMessage::ReplyMessage) {
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();
        return;
    }

    if (query.arguments().isEmpty())
        return;

    const QDBusArgument &argument = query.arguments().at(0).value<QDBusArgument>();
    m_connectionSettings = qdbus_cast<ConnectionSettings>(argument);

//    foreach (const QVariant &connectionVariant, m_connectionSettings.values()) {
//        qCDebug(dcNetworkManager()) << connectionVariant;
//    }
}

/*! Delete this \l{NetworkConnection} in the \l{NetworkManager}. */
void NetworkConnection::deleteConnection()
{
    QDBusMessage query = m_connectionInterface->call("Delete");
    if(query.type() != QDBusMessage::ReplyMessage)
        qCWarning(dcNetworkManager()) << query.errorName() << query.errorMessage();

}

void NetworkConnection::registerTypes()
{
    qRegisterMetaType<ConnectionSettings>("ConnectionSettings");
    qDBusRegisterMetaType<ConnectionSettings>();
}

/*! Returns the dbus object path of this \l{NetworkConnection}. */
QDBusObjectPath NetworkConnection::objectPath() const
{
    return m_objectPath;
}

/*! Returns the connection settings of this \l{NetworkConnection}. */
ConnectionSettings NetworkConnection::connectionSettings() const
{
    return m_connectionSettings;
}

/*! Returns the id of this \l{NetworkConnection}. */
QString NetworkConnection::id() const
{
    return m_connectionSettings.value("connection").value("id").toString();
}

/*! Returns the name of this \l{NetworkConnection}. */
QString NetworkConnection::name() const
{
    return m_connectionSettings.value("connection").value("name").toString();
}

/*! Returns the type of this \l{NetworkConnection}. */
QString NetworkConnection::type() const
{
    return m_connectionSettings.value("connection").value("type").toString();
}

/*! Returns the uuid of this \l{NetworkConnection}. */
QUuid NetworkConnection::uuid() const
{
    return m_connectionSettings.value("connection").value("uuid").toUuid();
}

/*! Returns the interface name of this \l{NetworkConnection}. */
QString NetworkConnection::interfaceName() const
{
    return m_connectionSettings.value("connection").value("interface-name").toString();
}

/*! Returns true if this \l{NetworkConnection} will autoconnect if available. */
bool NetworkConnection::autoconnect() const
{
    return m_connectionSettings.value("connection").value("autoconnect").toBool();
}

/*! Returns the timestamp of this \l{NetworkConnection} from the last connection. */
QDateTime NetworkConnection::timeStamp() const
{
    return QDateTime::fromTime_t(m_connectionSettings.value("connection").value("timestamp").toUInt());
}

QDebug operator<<(QDebug debug, NetworkConnection *networkConnection)
{
    debug.nospace() << "NetworkConnection(" << networkConnection->id() << ", ";
    debug.nospace() << networkConnection->uuid().toString() << ", ";
    debug.nospace() << networkConnection->interfaceName() << ", ";
    debug.nospace() << networkConnection->type() << ", ";
    debug.nospace() << networkConnection->timeStamp().toString("dd.MM.yyyy hh:mm") << ") ";
    return debug.space();
}
