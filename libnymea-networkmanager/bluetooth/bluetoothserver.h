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

#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

#include <QObject>
#include <QTimer>
#include <QObject>
#include <QPointer>
#include <QLowEnergyHandle>
#include <QLowEnergyService>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>
#include <QLowEnergyServiceData>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyAdvertisingData>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyConnectionParameters>
#include <QLowEnergyAdvertisingParameters>

#include "networkservice.h"
#include "wirelessservice.h"

class NetworkManager;

class BluetoothServer : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothServer(NetworkManager *networkManager);
    ~BluetoothServer();

    QString advertiseName() const;
    void setAdvertiseName(const QString &advertiseName, bool forceFullName = false);

    // Information for the device info service
    QString modelName() const;
    void setModelName(const QString &modelName);

    QString softwareVersion() const;
    void setSoftwareVersion(const QString &softwareVersion);

    QString hardwareVersion() const;
    void setHardwareVersion(const QString &hardwareVersion);

    QString serialNumber() const;
    void setSerialNumber(const QString &serialNumber);

    bool running() const;
    bool connected() const;

private:
    QString m_advertiseName;
    bool m_forceFullName = false;

    QString m_modelName;
    QString m_softwareVersion;
    QString m_hardwareVersion;
    QString m_serialNumber;

    NetworkManager *m_networkManager = nullptr;
    QBluetoothLocalDevice *m_localDevice = nullptr;
    QLowEnergyController *m_controller = nullptr;

    QLowEnergyService *m_deviceInfoService = nullptr;
    QLowEnergyService *m_genericAccessService = nullptr;
    QLowEnergyService *m_genericAttributeService = nullptr;

    NetworkService *m_networkService = nullptr;
    WirelessService *m_wirelessService = nullptr;

    bool m_running = false;
    bool m_connected = false;

    QLowEnergyServiceData deviceInformationServiceData();
    QLowEnergyServiceData genericAccessServiceData();
    QLowEnergyServiceData genericAttributeServiceData();

    void setRunning(bool running);
    void setConnected(bool connected);

    QUuid readMachineId();

signals:
    void runningChanged(const bool &running);
    void connectedChanged(const bool &connected);

private slots:
    // Local bluetooth device
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode mode);
    void onDeviceConnected(const QBluetoothAddress &address);
    void onDeviceDisconnected(const QBluetoothAddress &address);
    void onError(QLowEnergyController::Error error);

    // Bluetooth controller
    void onConnected();
    void onDisconnected();
    void onControllerStateChanged(QLowEnergyController::ControllerState state);

    // Services
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(QLowEnergyService::ServiceError error);

public slots:
    void start();
    void stop();

};

#endif // BLUETOOTHSERVER_H
