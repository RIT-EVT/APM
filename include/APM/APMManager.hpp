/**
 * Class to represent the APMManager Object.  Holds references
 * to many of the key objects used by this device
 */

#ifndef APM_APMMANAGER_HPP
#define APM_APMMANAGER_HPP

#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/dev/SIM100.hpp>
#include <EVT/dev/Timer.hpp>
#include <EVT/dev/platform/f3xx/f302x8/Timerf302x8.hpp>
#include "APMUart.hpp"

namespace APM {

namespace IO = EVT::core::IO;

enum class APMMode {
    OFF = 0u,
    ACCESSORY = 1u,
    ON = 2u
};

class APMManager {
public:
    static constexpr IO::Pin ACCESSORY_SW = IO::Pin::PA_5;
    static constexpr IO::Pin CHARGE_SW = IO::Pin::PA_6;
    static constexpr IO::Pin VICOR_SW = IO::Pin::PA_4;
    static constexpr IO::Pin KEY_ON_UC = IO::Pin::PB_5;

    // Time required for SIM100 to start up before it is ready for operation
    static constexpr uint32_t SIM100_STARTUP_PERIOD = 5000;

    // Want to poll the SIM100 GFD board for updates every 500 ms
    static constexpr uint32_t SIM100_POLLING_PERIOD = 500;

    /**
     * Create a new APMManager
     * Initializes the IO Devices
     * @param baud the baudrate for the UART device
     */
    explicit APMManager(APMUart &apmUart, DEV::SIM100 &sim100, IO::GPIO &accessorySwGpio, IO::GPIO &chargeSwGpio,
                        IO::GPIO &keyOnSwGpio, IO::GPIO &vicorSwGpio, EVT::core::DEV::Timerf302x8 &gfdTimer);

    /**
     * Returns the APMUart object by reference
     * @return const reference to the APMUart object
     */
    [[nodiscard]] APMUart& getApmUart() const;


    /**
     * Returns a reference to the Sim100 object for this class
     * @return reference to the SIM100 module
     */
    [[nodiscard]] DEV::SIM100& getSim100() const;

    /**
     * Gets a reference of the held GFD Timer.  This is used
     * for interrupts to update the timer behavior as necessary.
     * @return reference to the Timer object this->gfdTimer
     */
    [[nodiscard]] EVT::core::DEV::Timer& getGFDTimer() const;

    /**
     * Returns the current mode
     * @return the current mode
     */
    [[nodiscard]] APMMode getCurrentMode() const;

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
     * Function to transition from On Mode to Accessory Mode
     * @return 0 on success
     */
    int onToAccessoryMode();

    /**
     * Manually checks the device on switch.  Will transition the bike to On mode accordingly
     * @return 0 on success
     */
    int checkOnSw();

    /**
     * Check the debug status to determine if SIM100 should be checked for isolation faults.
     * @return True if the SIM100 should be checked.  False if not.
     */
    bool isIsolationChecking();

    /**
     * Sets the variable checkGFDIsolationState for debugging.  If set false then the
     * isolation check is not performed.
     * @param state The bool state to update the variable to
     */
    void setCheckGFDIsolationState(bool state);

private:

    // Holds the current mode of the APMManager device
    APMMode currentMode = APMMode::OFF;

    // Holds a reference to the APMUart device
    APMUart &apmUart;

    // Holds a reference to the SIM100 GFD device
    DEV::SIM100 &sim100;

    // Controls battery power to bike electronics.
    IO::GPIO &accessorySW_GPIO;

    // Controls charging the APMManager backup battery.
    IO::GPIO &chargeSW_GPIO;

    // Controls turning on the Vicor Power.  Uses main pack to power electronics
    IO::GPIO &vicorSW_GPIO;

    // GPIO input that reads the value of the key signal
    IO::GPIO &keyOnSw_GPIO;

    // Timer instance used to control the GFD polling logic
    EVT::core::DEV::Timer &gfdTimer;

    // Debug Boolean to turn off SIM100 GFD Checking
    bool checkGFDIsolationState = true;

};

} // namespace APM

#endif //APM_APMMANAGER_HPP
