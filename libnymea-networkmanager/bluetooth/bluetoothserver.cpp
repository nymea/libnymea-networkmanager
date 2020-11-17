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
    \class BluetoothServer
    \brief Represents a bluetooth LE server for network-manager remote configuration.
    \inmodule nymea-networkmanager
    \ingroup networkmanager-bluetooth

*/

#include "bluetoothserver.h"
#include "../networkmanager.h"
#include "../networkmanagerutils.h"

#include <QFile>
#include <QSysInfo>

BluetoothServer::BluetoothServer(NetworkManager *networkManager) :
    QObject(networkManager),
    m_networkManager(networkManager)
{

}

BluetoothServer::~BluetoothServer()
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Destroy bluetooth server.";
    if (m_controller)
        m_controller->stopAdvertising();

    if (m_localDevice)
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);

}

QString BluetoothServer::advertiseName() const
{
    return m_advertiseName;
}

void BluetoothServer::setAdvertiseName(const QString &advertiseName)
{
    m_advertiseName = advertiseName;
}

QString BluetoothServer::modelName() const
{
    return m_modelName;
}

void BluetoothServer::setModelName(const QString &modelName)
{
    m_modelName = modelName;
}

QString BluetoothServer::softwareVersion() const
{
    return m_softwareVersion;
}

void BluetoothServer::setSoftwareVersion(const QString &softwareVersion)
{
    m_softwareVersion = softwareVersion;
}

QString BluetoothServer::hardwareVersion() const
{
    return m_hardwareVersion;
}

void BluetoothServer::setHardwareVersion(const QString &hardwareVersion)
{
    m_hardwareVersion = hardwareVersion;
}

QString BluetoothServer::serialNumber() const
{
    return m_serialNumber;
}

void BluetoothServer::setSerialNumber(const QString &serialNumber)
{
    m_serialNumber = serialNumber;
}

bool BluetoothServer::running() const
{
    return m_running;
}

bool BluetoothServer::connected() const
{
    return m_connected;
}

QLowEnergyServiceData BluetoothServer::deviceInformationServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::DeviceInformation);

    // Model number string 0x2a24
    QLowEnergyCharacteristicData modelNumberCharData;
    modelNumberCharData.setUuid(QBluetoothUuid::ModelNumberString);
    if (m_modelName.isEmpty()) {
        modelNumberCharData.setValue(QString("N.A.").toUtf8());
    } else {
        modelNumberCharData.setValue(m_modelName.toUtf8());
    }

    modelNumberCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(modelNumberCharData);

    // Serial number string 0x2a25
    QLowEnergyCharacteristicData serialNumberCharData;
    serialNumberCharData.setUuid(QBluetoothUuid::SerialNumberString);
    if (m_serialNumber.isNull()) {
        // Note: if no serialnumber specified use the system uuid from /etc/machine-id
        qCDebug(dcNetworkManagerBluetoothServer()) << "Serial number not specified. Using system uuid from /etc/machine-id as serialnumber.";
        m_serialNumber = readMachineId().toString();
    }
    serialNumberCharData.setValue(m_serialNumber.toUtf8());
    serialNumberCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(serialNumberCharData);

    // Firmware revision string 0x2a26
    QLowEnergyCharacteristicData firmwareRevisionCharData;
    firmwareRevisionCharData.setUuid(QBluetoothUuid::FirmwareRevisionString);
    firmwareRevisionCharData.setValue(QString(VERSION_STRING).toUtf8());
    firmwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(firmwareRevisionCharData);

    // Hardware revision string 0x2a27
    QLowEnergyCharacteristicData hardwareRevisionCharData;
    hardwareRevisionCharData.setUuid(QBluetoothUuid::HardwareRevisionString);
    hardwareRevisionCharData.setValue(m_hardwareVersion.toUtf8());
    hardwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(hardwareRevisionCharData);

    // Software revision string 0x2a28
    QLowEnergyCharacteristicData softwareRevisionCharData;
    softwareRevisionCharData.setUuid(QBluetoothUuid::SoftwareRevisionString);
    softwareRevisionCharData.setValue(m_softwareVersion.toUtf8());
    softwareRevisionCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(softwareRevisionCharData);

    // Manufacturer name string 0x2a29
    QLowEnergyCharacteristicData manufacturerNameCharData;
    manufacturerNameCharData.setUuid(QBluetoothUuid::ManufacturerNameString);
    manufacturerNameCharData.setValue(QString("nymea GmbH").toUtf8());
    manufacturerNameCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(manufacturerNameCharData);

    return serviceData;
}

QLowEnergyServiceData BluetoothServer::genericAccessServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::GenericAccess);

    // Device name 0x2a00
    QLowEnergyCharacteristicData nameCharData;
    nameCharData.setUuid(QBluetoothUuid::DeviceName);
    if (m_advertiseName.isNull()) {
        qCWarning(dcNetworkManagerBluetoothServer()) << "Advertise name not specified. Using system host name as device name.";
        m_advertiseName = QSysInfo::machineHostName();
    }
    nameCharData.setValue(m_advertiseName.toUtf8());
    nameCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(nameCharData);

    // Appearance 0x2a01
    QLowEnergyCharacteristicData appearanceCharData;
    appearanceCharData.setUuid(QBluetoothUuid::Appearance);
    appearanceCharData.setValue(QByteArray(4, 0));
    appearanceCharData.setProperties(QLowEnergyCharacteristic::Read);
    serviceData.addCharacteristic(appearanceCharData);

    // Peripheral Privacy Flag 0x2a02
    QLowEnergyCharacteristicData privacyFlagCharData;
    privacyFlagCharData.setUuid(QBluetoothUuid::PeripheralPrivacyFlag);
    privacyFlagCharData.setValue(QByteArray(2, 0));
    privacyFlagCharData.setProperties(QLowEnergyCharacteristic::Read | QLowEnergyCharacteristic::Write);
    serviceData.addCharacteristic(privacyFlagCharData);

    // Reconnection Address 0x2a03
    QLowEnergyCharacteristicData reconnectionAddressCharData;
    reconnectionAddressCharData.setUuid(QBluetoothUuid::ReconnectionAddress);
    reconnectionAddressCharData.setValue(QByteArray());
    reconnectionAddressCharData.setProperties(QLowEnergyCharacteristic::Write);
    serviceData.addCharacteristic(reconnectionAddressCharData);

    return serviceData;
}

QLowEnergyServiceData BluetoothServer::genericAttributeServiceData()
{
    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid::GenericAttribute);

    QLowEnergyCharacteristicData charData;
    charData.setUuid(QBluetoothUuid::ServiceChanged);
    charData.setProperties(QLowEnergyCharacteristic::Indicate);

    serviceData.addCharacteristic(charData);

    return serviceData;
}

void BluetoothServer::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(m_running);
}

void BluetoothServer::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged(m_connected);
}

QUuid BluetoothServer::readMachineId()
{
    QUuid systemUuid;
    QFile systemUuidFile("/etc/machine-id");
    if (systemUuidFile.open(QFile::ReadOnly)) {
        QString tmpId = QString::fromLatin1(systemUuidFile.readAll()).trimmed();
        tmpId.insert(8, "-");
        tmpId.insert(13, "-");
        tmpId.insert(18, "-");
        tmpId.insert(23, "-");
        systemUuid = QUuid(tmpId);
    } else {
        qCWarning(dcNetworkManagerBluetoothServer()) << "Failed to open /etc/machine-id for reading the system uuid as device information serialnumber.";
    }
    systemUuidFile.close();

    return systemUuid;
}

void BluetoothServer::onHostModeStateChanged(const QBluetoothLocalDevice::HostMode mode)
{
    switch (mode) {
    case QBluetoothLocalDevice::HostConnectable:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Bluetooth host in connectable mode.";
        break;
    case QBluetoothLocalDevice::HostDiscoverable:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Bluetooth host in discoverable mode.";
        break;
    case QBluetoothLocalDevice::HostPoweredOff:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Bluetooth host in power off mode.";
        break;
    case QBluetoothLocalDevice::HostDiscoverableLimitedInquiry:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Bluetooth host in discoverable limited inquiry mode.";
        break;
    }
}

void BluetoothServer::onDeviceConnected(const QBluetoothAddress &address)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Device connected" << address.toString();
}

void BluetoothServer::onDeviceDisconnected(const QBluetoothAddress &address)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Device disconnected" << address.toString();
}

void BluetoothServer::onError(QLowEnergyController::Error error)
{
    qCWarning(dcNetworkManagerBluetoothServer()) << "Bluetooth error occured:" << error << m_controller->errorString();
}

void BluetoothServer::onConnected()
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Client connected" << m_controller->remoteName() << m_controller->remoteAddress();
    setConnected(true);
}

void BluetoothServer::onDisconnected()
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Client disconnected";
    setConnected(false);
    stop();
}

void BluetoothServer::onControllerStateChanged(QLowEnergyController::ControllerState state)
{
    switch (state) {
    case QLowEnergyController::UnconnectedState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state disonnected.";
        setConnected(false);
        break;
    case QLowEnergyController::ConnectingState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state connecting...";
        setConnected(false);
        break;
    case QLowEnergyController::ConnectedState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state connected." << m_controller->remoteName() << m_controller->remoteAddress();
        setConnected(true);
        break;
    case QLowEnergyController::DiscoveringState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state discovering...";
        break;
    case QLowEnergyController::DiscoveredState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state discovered.";
        break;
    case QLowEnergyController::ClosingState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state closing...";
        break;
    case QLowEnergyController::AdvertisingState:
        qCDebug(dcNetworkManagerBluetoothServer()) << "Controller state advertising...";
        setRunning(true);
        break;
    }
}

void BluetoothServer::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Service characteristic changed" << characteristic.uuid() << value;
}

void BluetoothServer::characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Service characteristic read" << characteristic.uuid() << value;
}

void BluetoothServer::characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Service characteristic written" << characteristic.uuid() << value;
}

void BluetoothServer::descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Descriptor read" << descriptor.uuid() << value;
}

void BluetoothServer::descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "Descriptor written" << descriptor.uuid() << value;
}

void BluetoothServer::serviceError(QLowEnergyService::ServiceError error)
{
    QString errorString;
    switch (error) {
    case QLowEnergyService::NoError:
        errorString = "No error";
        break;
    case QLowEnergyService::OperationError:
        errorString = "Operation error";
        break;
    case QLowEnergyService::CharacteristicReadError:
        errorString = "Characteristic read error";
        break;
    case QLowEnergyService::CharacteristicWriteError:
        errorString = "Characteristic write error";
        break;
    case QLowEnergyService::DescriptorReadError:
        errorString = "Descriptor read error";
        break;
    case QLowEnergyService::DescriptorWriteError:
        errorString = "Descriptor write error";
        break;
    case QLowEnergyService::UnknownError:
        errorString = "Unknown error";
        break;
    }

    qCWarning(dcNetworkManagerBluetoothServer()) << "Service error:" << errorString;
}

void BluetoothServer::start()
{
    if (running()) {
        qCDebug(dcNetworkManagerBluetoothServer()) << "Start Bluetooth server called but the server is already running.  Doing nothing.";
        return;
    }

    if (connected()) {
        qCDebug(dcNetworkManagerBluetoothServer()) << "Start Bluetooth server called but the server is running and a client is connected. Doing nothing.";
        return;
    }

    qCDebug(dcNetworkManagerBluetoothServer()) << "-------------------------------------";
    qCDebug(dcNetworkManagerBluetoothServer()) << "Starting bluetooth server...";
    qCDebug(dcNetworkManagerBluetoothServer()) << "-------------------------------------";

    // Local bluetooth device
    m_localDevice = new QBluetoothLocalDevice(this);
    if (!m_localDevice->isValid()) {
        qCWarning(dcNetworkManagerBluetoothServer()) << "Local bluetooth device is not valid.";
        delete m_localDevice;
        m_localDevice = nullptr;
        return;
    }

    connect(m_localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BluetoothServer::onHostModeStateChanged);
    connect(m_localDevice, &QBluetoothLocalDevice::deviceConnected, this, &BluetoothServer::onDeviceConnected);
    connect(m_localDevice, &QBluetoothLocalDevice::deviceDisconnected, this, &BluetoothServer::onDeviceDisconnected);

    qCDebug(dcNetworkManagerBluetoothServer()) << "Local device" << m_localDevice->name() << m_localDevice->address().toString();
    m_localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    m_localDevice->powerOn();

    // Bluetooth low energy periperal controller
    m_controller = QLowEnergyController::createPeripheral(this);
    connect(m_controller, &QLowEnergyController::stateChanged, this, &BluetoothServer::onControllerStateChanged);
    connect(m_controller, &QLowEnergyController::connected, this, &BluetoothServer::onConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BluetoothServer::onDisconnected);
    connect(m_controller, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(onError(QLowEnergyController::Error)));

    // Note: https://www.bluetooth.com/specifications/gatt/services
    m_deviceInfoService = m_controller->addService(deviceInformationServiceData(), m_controller);
    m_genericAccessService = m_controller->addService(genericAccessServiceData(), m_controller);
    m_genericAttributeService = m_controller->addService(genericAttributeServiceData(), m_controller);

    // Create custom services
    m_networkService = new NetworkService(m_controller->addService(NetworkService::serviceData(m_networkManager), m_controller),
                                          m_networkManager, m_controller);
    m_wirelessService = new WirelessService(m_controller->addService(WirelessService::serviceData(m_networkManager), m_controller),
                                            m_networkManager, m_controller);

    QLowEnergyAdvertisingData advertisingData;
    advertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    advertisingData.setIncludePowerLevel(true);
    advertisingData.setLocalName(m_advertiseName);
    // FIXME: set guh manufacturer SIG data once available

    // Note: start advertising in 100 ms interval, this makes the device better discoverable on certain phones
    QLowEnergyAdvertisingParameters advertisingParameters;
    advertisingParameters.setInterval(100, 100);

    qCDebug(dcNetworkManagerBluetoothServer()) << "Start advertising" << m_advertiseName << m_localDevice->address().toString();
    m_controller->startAdvertising(advertisingParameters, advertisingData, advertisingData);
    // Note: setRunning(true) will be called when the service is really advertising, see onControllerStateChanged()
}

void BluetoothServer::stop()
{
    qCDebug(dcNetworkManagerBluetoothServer()) << "-------------------------------------";
    qCDebug(dcNetworkManagerBluetoothServer()) << "Stopping bluetooth server.";
    qCDebug(dcNetworkManagerBluetoothServer()) << "-------------------------------------";

    if (m_localDevice) {
        qCDebug(dcNetworkManagerBluetoothServer()) << "Set host mode to connectable.";
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);
        delete m_localDevice;
        m_localDevice = nullptr;
    }

    if (m_controller) {
        qCDebug(dcNetworkManagerBluetoothServer()) << "Stop advertising.";
        m_controller->stopAdvertising();
        delete m_controller;
        m_controller = nullptr;
    }

    setConnected(false);
    setRunning(false);
}
