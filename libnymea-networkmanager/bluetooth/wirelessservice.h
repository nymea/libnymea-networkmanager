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

#ifndef WIRELESSSERVICE_H
#define WIRELESSSERVICE_H

#include <QObject>
#include <QVariantMap>
#include <QLowEnergyService>
#include <QLowEnergyServiceData>

#include "../networkmanager.h"
#include "../wirelessaccesspoint.h"
#include "../wirelessnetworkdevice.h"

class WirelessService : public QObject
{
    Q_OBJECT

public:
    enum WirelessServiceCommand {
        WirelessServiceCommandInvalid               = -1,
        WirelessServiceCommandGetNetworks           = 0x00,
        WirelessServiceCommandConnect               = 0x01,
        WirelessServiceCommandDisconnect            = 0x03,
        WirelessServiceCommandScan                  = 0x04,
        WirelessServiceCommandGetCurrentConnection  = 0x05,
        WirelessServiceCommandStartAccessPoint      = 0x06
    };
    Q_ENUM(WirelessServiceCommand)

    enum WirelessServiceResponse {
        WirelessServiceResponseSuccess                     = 0x00,
        WirelessServiceResponseIvalidCommand               = 0x01,
        WirelessServiceResponseIvalidParameters            = 0x02,
        WirelessServiceResponseNetworkManagerNotAvailable  = 0x03,
        WirelessServiceResponseWirelessNotAvailable        = 0x04,
        WirelessServiceResponseWirelessNotEnabled          = 0x05,
        WirelessServiceResponseNetworkingNotEnabled        = 0x06,
        WirelessServiceResponseUnknownError                = 0x07
    };
    Q_ENUM(WirelessServiceResponse)

    explicit WirelessService(QLowEnergyService *service, NetworkManager *networkManager, QObject *parent = nullptr);
    QLowEnergyService *service();

    static QLowEnergyServiceData serviceData(NetworkManager *networkManager);

private:
    QLowEnergyService *m_service = nullptr;
    NetworkManager *m_networkManager = nullptr;
    WirelessNetworkDevice *m_device = nullptr;

    bool m_readingInputData = false;
    QByteArray m_inputDataStream;

    WirelessServiceResponse checkWirelessErrors();

    // Note: static to be available in serviceData
    static QByteArray getWirelessNetworkDeviceState(const NetworkDevice::NetworkDeviceState &state);
    static QByteArray getWirelessMode(WirelessNetworkDevice::WirelessMode mode);

    void streamData(const QVariantMap &responseMap);

    QVariantMap createResponse(const WirelessServiceCommand &command, const WirelessServiceResponse &responseCode = WirelessServiceResponseSuccess);

    // Methods
    void commandGetNetworks(const QVariantMap &request);
    void commandConnect(const QVariantMap &request);
    void commandDisconnect(const QVariantMap &request);
    void commandScan(const QVariantMap &request);
    void commandGetCurrentConnection(const QVariantMap &request);
    void commandStartAccessPoint(const QVariantMap &request);

private slots:
    // Service
    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void descriptorRead(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void serviceError(QLowEnergyService::ServiceError error);

    // Commands
    void processCommand(const QVariantMap &request);

    // Wireless network device
    void onWirelessDeviceBitRateChanged(const int &bitRate);
    void onWirelessDeviceStateChanged(const NetworkDevice::NetworkDeviceState &state);
    void onWirelessModeChanged(WirelessNetworkDevice::WirelessMode mode);

};

#endif // WIRELESSSERVICE_H
