/**
 * Class to represent the ApmDevice Object.  Holds references
 * to many of the key objects used by this device
 */

#ifndef APM_APMDEVICE_HPP
#define APM_APMDEVICE_HPP

#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include "ApmUart.hpp"

namespace APM {

namespace IO = EVT::core::IO;

enum class ApmMode {
    OFF = 0u,
    ACCESSORY = 1u,
    ON = 2u
};

class ApmDevice {
public:
    static constexpr IO::Pin ACCESSORY_SW = IO::Pin::PA_5;
    static constexpr IO::Pin CHARGE_SW = IO::Pin::PA_6;
    static constexpr IO::Pin VICOR_SW = IO::Pin::PA_4;
    static constexpr IO::Pin KEY_ON_UC = IO::Pin::PB_5;

    /**
     * Create a new ApmDevice
     * Initializes the IO Devices
     * @param baud the baudrate for the UART device
     */
    explicit ApmDevice(int baud, IO::GPIO &accessorySwGpio, ApmUart apmUart, IO::GPIO &chargeSwGpio,
                       IO::GPIO &keyOnSwGpio, IO::GPIO &vicorSwGpio);

    /**
     * Returns the ApmUart object by reference
     * @return const reference to the ApmUart object
     */
    ApmUart& getApmUart();

    /**
     * Returns the current mode
     * @return the current mode
     */
    ApmMode getCurrentMode();

    /**
    * Function to handle the transition from OFF mode to Accessory Mode
    * @return 0 on success.
    */
    int offToAccessoryMode();

    /**
    * Function to transition from Accessory Mode to On Mode
    * @return 0 on success.
    */
    int accessoryToOnMode();

    /**
     * Manually checks the device on switch.  Will transition the bike to On mode accordingly
     * @return 0 on success
     */
    int checkOnSw();

private:
    // Holds the current mode of the APM device
    ApmMode currentMode = ApmMode::OFF;

    // Holds a reference to the ApmUart device
    ApmUart apmUart;
    IO::GPIO &accessorySW_GPIO;
    IO::GPIO &chargeSW_GPIO;
    IO::GPIO &vicorSW_GPIO;
    IO::GPIO &keyOnSw_GPIO;

};

} // namespace ApmDevice

#endif //APM_APMDEVICE_HPP
