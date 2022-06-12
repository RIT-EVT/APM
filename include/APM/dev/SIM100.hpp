#ifndef EVT_SIM100_H
#define EVT_SIM100_H

#include <EVT/io/CAN.hpp>
#include <EVT/io/types/CANMessage.hpp>
#include <cstddef>

namespace APM::DEV {

namespace IO = EVT::core::IO;

/**
 * This class represents the interface for interacting with the SIM100 ground
 * fault detection board.  For documentation on the CAN interface please see:
 * {@link https://sendyne.com/Products/SIM100%20Isolation%20Monitor.html}
 */
class SIM100 {
public:
  constexpr static uint32_t CAN_RESPONSE_ID = 0x0A100100;
  constexpr static uint32_t CAN_REQUEST_ID = 0x0A100101;

  // Max voltage for DEV1 main battery pack in volts
  // Taken as 5V below max overvoltage limit on the Motor Controller
  // {@link https://wiki.rit.edu/display/EVT/DEV1+Motor+Controller}
  const static uint16_t DEV1_MAX_BATTERY_VOLTAGE = 140;

  /**
   * Enumeration to hold the possible responses for the isolation state from the
   * SIM100 board
   */
  enum class IsolationStateResponse {
    CANError = 1,
    HardwareError = 2,
    HighBatteryVoltage = 3,
    LowBatteryVoltage = 4,
    IsolationError = 5,
    NoError = 6
  };

  /**
   *
   * @param can
   */
  explicit SIM100(IO::CAN &can);

  /**
   * Returns the part name into the relevant buf variable.  The max length of
   * the part name is 16 bytes.
   * @param buf the buffer to store the part name into
   * @param size the allocated size of buf
   * @return 0 on success
   */
  int getPartName(char *buf, size_t size);

  /**
   * Reads the firmware version from the board
   * @param buf the char array to store the firmware version into
   * @param size the allocated size of buf
   * @return 0 on success
   */
  int getVersion(char *buf, size_t size);

  /**
   * Sets the max working voltage for the SIM100 load
   * @param maxVoltage the maximum voltage as expressed as a 2 byte unsigned
   * integer. defaults to the max operating voltage for the DEV1 pack
   * @return The maxVoltage response from the GFD Board.
   */
  uint16_t setMaxWorkingVoltage(uint16_t maxVoltage = DEV1_MAX_BATTERY_VOLTAGE);

  /**
   * Reads the isolation state of the GFD board.
   * @return 0 on success.  Returns a relevant error code on failure.
   */
  IsolationStateResponse getIsolationState();

  /**
   * Restarts the SIM100 board.
   * @return 0 on success.  Error code on failure
   */
  int restartSIM100();

private:
  /**
   * Enumeration to map the Request_mux byte of the CAN message to the request
   * name
   */
  enum class RequestMux {
    PART_NAME_0 = 0x01,
    PART_NAME_1 = 0x02,
    PART_NAME_2 = 0x03,
    PART_NAME_3 = 0x04,
    VERSION_0 = 0x05,
    VERSION_1 = 0x06,
    VERSION_2 = 0x07,
    RESTART_SIM100 = 0xC1,
    SET_MAX_BATTERY_VOLTAGE = 0xF0,
    ISOLATION_STATE = 0xE0
  };

  /**
   * Enumeration to hold the shift values for the StatusBits byte
   */
  enum class StatusBitShift {
    HARDWARE_ERROR = 7,
    NO_NEW_ESTIMATES = 6,
    HIGH_UNCERTAINTY = 5,
    HIGH_BATTERY_VOLTAGE = 3,
    LOW_BATTERY_VOLTAGE = 2,
    ISO1 = 1,
    ISO0 = 0
  };

  // The timeout in ms to wait for a response from the SIM100 device
  constexpr static int RESPONSE_TIMEOUT = 3000;

  // Max length for the part name based on SIM100 CAN datasheet
  constexpr static size_t MAX_PART_NAME_LEN = 16;

  // Max length for the version string based on SIM100 CAN datasheet
  constexpr static size_t MAX_VERSION_LEN = 12;

  // The CAN device to send and receive CAN messages with
  IO::CAN &can;

  /**
   * Sends the requested CAN Message.  To be used for Data request messages. Not
   * for state control commands Returns a pointer to the response message from
   * the SIM100 device. Waits for RESPONSE_TIMEOUT before returning
   * @param requestType The type of message to request
   * @param responseMessage The response message corresponding to the data
   * request
   * @return 0 if successful
   */
  int sendDataRequestMessage(RequestMux requestType,
                             IO::CANMessage &responseMessage);

  /**
   * Send any CAN message to the SIM100 board.  Constructs to CANMessage and
   * fills the proper response message by reference.
   * @param dataLength the length of the CAN message to send
   * @param payload the payload of the CAN message to send
   * @param responseMessage a reference to the response message to send back
   * @return 0 if successful
   */
  int sendMessage(uint8_t dataLength, uint8_t *payload,
                  IO::CANMessage &responseMessage, bool expectResponse = true);
};

} // namespace APM::DEV

#endif // EVT_SIM100_H
