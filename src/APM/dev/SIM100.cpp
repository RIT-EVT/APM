/**
 * Contains the source for class SIM100
 */

#include <cstring>
#include <APM/dev/SIM100.hpp>
#include <EVT/utils/time.hpp>

namespace APM::DEV {

namespace IO = EVT::core::IO;

SIM100::SIM100(IO::CAN &can) : can(can) {
}

int SIM100::sendDataRequestMessage(RequestMux requestType, IO::CANMessage &responseMessage) {
    constexpr uint8_t payloadSize = 1;
    auto requestMuxByte = static_cast<uint8_t>(requestType);
    uint8_t payload[payloadSize] = {requestMuxByte};

    return sendMessage(payloadSize, &payload[0], responseMessage);
}

int SIM100::sendMessage(uint8_t dataLength, uint8_t *payload, IO::CANMessage &responseMessage,
                        bool expectResponse) {
    if (dataLength < 1) {
        return 1;
    }

    uint8_t requestMuxByte = payload[0];
    // TODO: Rework CAN completely.  Currently will not work!
//     IO::CANMessage requestMessage(SIM100::CAN_REQUEST_ID, dataLength, &payload[0]);
//    can.transmit(requestMessage);
    /* TODO: Implement CAN filtering to only receive SIM100 CAN messages here.
       This entire function will be updated for better handling with CAN Open integration so this
       is a temporary fix */

    if (!expectResponse) {
        return 0;
    }

    auto response = can.receive(&responseMessage, true);
    while (response == EVT::core::IO::CAN::CANStatus::OK || responseMessage.getId() != CAN_RESPONSE_ID  ||
           responseMessage.getPayload()[0] != requestMuxByte) {
        response = can.receive(&responseMessage, false);
        EVT::core::time::wait(500);
        // TODO: Implement timeout and return something accordingly.  Also better error handling
    }

    return 0;
}

int SIM100::getPartName(char *buf, size_t size) {
    if (size < (MAX_PART_NAME_LEN + 1)) {  // +1 for null terminator
        return 1;
    }

    memset(buf, 0, MAX_PART_NAME_LEN + 1);

    IO::CANMessage responseMessage;
    RequestMux partNameRequestMux;
    int bufOffset = 0;

    for (int partNameIdx = 0; partNameIdx <= 3; partNameIdx++) {
        switch (partNameIdx) {
            case(0):
                partNameRequestMux = RequestMux::PART_NAME_0;
                break;
            case(1):
                partNameRequestMux = RequestMux::PART_NAME_1;
                break;
            case(2):
                partNameRequestMux = RequestMux::PART_NAME_2;
                break;
            case(3):
                partNameRequestMux = RequestMux::PART_NAME_3;
                break;
            default:
                return 1;
        }

        sendDataRequestMessage(partNameRequestMux, responseMessage);
        uint8_t dataLength = responseMessage.getDataLength();
        if (dataLength != 5) {
            return 1;
        }

        uint8_t* payload = responseMessage.getPayload();

        // Copy values from payload into the buf
        for (int idx = 1; idx <= dataLength; idx++) {
            buf[(idx-1) + bufOffset] = payload[idx];
        }

        bufOffset += 4;  // 4 bytes per message
    }

    return 0;
}

uint16_t SIM100::setMaxWorkingVoltage(uint16_t maxVoltage) {
    constexpr uint8_t payloadSize = 3;  // 1 byte for request type.  2 bytes for max voltage data
    uint16_t returnVoltage = 0;
    auto requestMuxByte = static_cast<uint8_t>(RequestMux::SET_MAX_BATTERY_VOLTAGE);

    // Get upper and lower bytes for voltage value
    auto maxVoltageHighByte = static_cast<uint8_t>((maxVoltage & 0xFF00) >> 8);
    auto maxVoltageLowByte = static_cast<uint8_t>(maxVoltage & 0x00FF);

    IO::CANMessage responseMessage;
    uint8_t *responsePayload;

    uint8_t payload[payloadSize] = {requestMuxByte, maxVoltageHighByte, maxVoltageLowByte};

    if (sendMessage(payloadSize, &payload[0], responseMessage) != 0) {
        return 1;
    }

    // Verify the response contains the same voltage as sent by the request
    if (responseMessage.getDataLength() < 3) {
        return 2;
    }
    responsePayload = responseMessage.getPayload();

    uint8_t responseHigh = responsePayload[1];
    uint8_t responseLow = responsePayload[2];

    returnVoltage = returnVoltage | (responseHigh << 8);
    returnVoltage = returnVoltage | (responseLow);

    return returnVoltage;
}

SIM100::IsolationStateResponse SIM100::getIsolationState() {
    IO::CANMessage responseMessage;
    uint8_t *responsePayload;
    uint8_t statusByte;

    bool queryAgain = false;

    do {
        if (sendDataRequestMessage(RequestMux::ISOLATION_STATE, responseMessage) != 0) {
            return IsolationStateResponse::CANError;
        }

        if (responseMessage.getDataLength() != 8) {  // Expect 1 MUX byte and 7 data bytes
            return IsolationStateResponse::CANError;
        }

        responsePayload = responseMessage.getPayload();

        // Read Status bits
        statusByte = responsePayload[1];

        // Check for hardware error
        if (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::HARDWARE_ERROR))) {
            return IsolationStateResponse::HardwareError;  // Diagnose and service the system
        }

        queryAgain = (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::NO_NEW_ESTIMATES))) ||
                     (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::HIGH_UNCERTAINTY)));

        if (queryAgain) {
            EVT::core::time::wait(100);
        }
    } while (queryAgain);
    // Try to receive isolation state again if no new estimates or high uncertainty

    if (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::HIGH_BATTERY_VOLTAGE))) {
        return IsolationStateResponse::HighBatteryVoltage;
    }

    if (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::LOW_BATTERY_VOLTAGE))) {
        return IsolationStateResponse::LowBatteryVoltage;
    }

    if (statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::ISO1)) ||
            statusByte & (1 << static_cast<uint8_t>(SIM100::StatusBitShift::ISO0))) {
        return IsolationStateResponse::IsolationError;
    }

    // If no status bit errors detected, return all clear code
    return IsolationStateResponse::NoError;
}

int SIM100::restartSIM100() {
    constexpr uint8_t payloadSize = 5;  // 1 byte for request type.  4 bytes for expected command
    auto requestMuxByte = static_cast<uint8_t>(RequestMux::RESTART_SIM100);
    IO::CANMessage responseMessage;

    uint8_t payload[payloadSize] = {requestMuxByte, 0x01, 0x23, 0x45, 0x67};

    sendMessage(payloadSize, payload, responseMessage, false);

    return 0;
}

int SIM100::getVersion(char *buf, size_t size) {
    if (size < (MAX_VERSION_LEN + 1)) {  // +1 for null terminator
        return 1;
    }

    memset(buf, 0, MAX_VERSION_LEN + 1);

    IO::CANMessage responseMessage;
    RequestMux versionRequestMUX;
    int bufOffset = 0;

    for (int partNameIdx = 0; partNameIdx <= 2; partNameIdx++) {
        switch (partNameIdx) {
            case(0):
                versionRequestMUX = RequestMux::VERSION_0;
                break;
            case(1):
                versionRequestMUX = RequestMux::VERSION_1;
                break;
            case(2):
                versionRequestMUX = RequestMux::VERSION_2;
                break;
            default:
                return 1;
        }

        sendDataRequestMessage(versionRequestMUX, responseMessage);
        uint8_t dataLength = responseMessage.getDataLength();
        if (dataLength != 5) {
            return 1;
        }

        uint8_t* payload = responseMessage.getPayload();

        // Copy values from payload into the buf
        for (int idx = 1; idx <= dataLength; idx++) {
            buf[(idx-1) + bufOffset] = payload[idx];
        }

        bufOffset += 4;  // 4 bytes per message
    }

    return 0;
}


}  // namespace APM::DEV
