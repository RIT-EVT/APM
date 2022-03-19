/**
 * This is the entry point for the code to run on the dev1_apm.
 * 08/24/21
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/APMUart.hpp>
#include <cstring>
#include <APM/APMManager.hpp>
#include <cstdio>
#include <APM/dev/SIM100.hpp>
#include <EVT/dev/platform/f3xx/f302x8/Timerf302x8.hpp>

// Pointer to the APM Manager
APM::APMManager *apmManagerPtr = nullptr;

namespace IO = EVT::core::IO;

namespace APM {

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

constexpr IO::Pin CAN_TX = IO::Pin::PA_12;
constexpr IO::Pin CAN_RX = IO::Pin::PA_11;

#ifdef NUCLEO_COMPILATION
constexpr IO::Pin UART_TX = IO::Pin::UART_TX;
constexpr IO::Pin UART_RX = IO::Pin::UART_RX;
#else
constexpr IO::Pin UART_TX = IO::Pin::PB_10;
constexpr IO::Pin UART_RX = IO::Pin::PB_11;
#endif

char buf[BUF_SIZE];

void handleOnButtonInterrupt(IO::GPIO *gpio) {
    auto apmUart = apmManagerPtr->getApmUart();

    apmUart.printDebugString("On button pressed\n\r");

    if (apmManagerPtr->getCurrentMode() == APMMode::ACCESSORY) {
        apmManagerPtr->accessoryToOnMode();
    } else {
        apmUart.printString("WARN: On button pressed while bike was not in ACCESSORY mode\n\r");
    }
}

/**
* Prompt to the user for interfacing with the board over UART Debug
* @param uart reference to the IO::UART object for interfacing with the user
* @return 0 on success, 1 if a failure has occurred
*/
int userPrompt(const APMManager &apmDevice, APMUart *apmUart) {
    apmUart->printString("\n\rPlease enter a command\n\r");
    apmUart->printString("Enter 'h' for help\n\r");
    apmUart->printString(">> ");

    apmUart->gets(buf, BUF_SIZE);
    apmUart->printString("\n\r");

    if (strncmp("h", buf, BUF_SIZE) == 0) {
        apmUart->printString("List of possible commands:\n\r");
        apmUart->printString("\t'h': Help Message\n\r");
        apmUart->printString("\t'd': Debug Mode.  Prints out all debug statements to terminal.  Exit\n\r");
        apmUart->printString("\t     on key press\n\r");
        apmUart->printString("\t'm': Get Mode.  Returns accessory or on respectively\n\r");
        apmUart->printString("\t'g': Toggle GFD Checking.  Used for debugging\n\r");
    } else if (strncmp("m", buf, BUF_SIZE) == 0) {
        char modeString[10];
        switch (apmDevice.getCurrentMode()) {
            case APMMode::OFF:
                strncpy(modeString, "OFF", 10);
                break;
            case APMMode::ACCESSORY:
                strncpy(modeString, "ACCESSORY", 10);
                break;
            case APMMode::ON:
                strncpy(modeString, "ON", 10);
                break;
        }

        snprintf(buf, BUF_SIZE, "Current APMManager Mode: %s\n\r", modeString);
        apmUart->printString(buf);
    } else if (strncmp("d", buf, BUF_SIZE) == 0) {
        apmUart->printString("Entering Debug Mode.  Will print out all debug messages to terminal\n\r");
        apmUart->printString("Must reset the device to exit DEBUG mode\n\r");
//        apmUart -> printString("Press any key to exit...\n\r");
        apmUart->setDebugPrint(true);
//        apmUart -> getc();
//        apmUart -> setDebugPrint(false);
        while (true) {}  // TODO: Update to wait for user input once UART interrupts are implemented
        // Currently it requires an entire device reset to exit blocking mode.
    } else if (strncmp("g", buf, BUF_SIZE) == 0) {
        bool previousState = apmManagerPtr->isIsolationChecking();
        bool newState = !previousState;
        apmManagerPtr->setCheckGFDIsolationState(newState);
        snprintf(buf, BUF_SIZE, "GFD Isolation Checking has been turned %s\n\r", (newState ? "ON" : "OFF"));
        apmUart->printString(buf);
    } else {
        apmUart->printString("Unrecognized Command\n\r");
    }

    return 0;
}

}  // namespace APM

int main() {
    // Initialize IO Objects
    IO::init();
    IO::UART &uart = IO::getUART<APM::UART_TX, APM::UART_RX>(APM::BAUD_RATE);
    IO::GPIO &accessorySW_GPIO =
            IO::getGPIO<APM::APMManager::ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeSW_GPIO =
            IO::getGPIO<APM::APMManager::CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorSW_GPIO =
            IO::getGPIO<APM::APMManager::VICOR_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &keyOnSw_GPIO =
            IO::getGPIO<APM::APMManager::KEY_ON_UC>(IO::GPIO::Direction::INPUT);
    IO::GPIO &onIndicator_GPIO =
            IO::getGPIO<APM::APMManager::ON_INDICATOR>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &accessoryIndicator_GPIO =
            IO::getGPIO<APM::APMManager::ACCESSORY_INDICATOR>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &mcOnSw_GPIO =
            IO::getGPIO<APM::APMManager::MC_ON>(EVT::core::IO::GPIO::Direction::OUTPUT);
    IO::GPIO &batteryChargeLimiter_GPIO =
            IO::getGPIO<IO::Pin::PB_13>(EVT::core::IO::GPIO::Direction::OUTPUT);

    IO::CAN& can = IO::getCAN<APM::CAN_TX, APM::CAN_RX>();

    auto apmUart = APM::APMUart(&uart);
    auto sim100 = APM::DEV::SIM100(can);

    auto apmTimer = EVT::core::DEV::Timerf302x8(TIM2, 5000);

    // Create Data Objects
    APM::APMManager apmManager = APM::APMManager(apmUart, sim100, accessorySW_GPIO, chargeSW_GPIO,
                                                 vicorSW_GPIO, apmTimer, accessoryIndicator_GPIO,
                                                 onIndicator_GPIO, mcOnSw_GPIO);
    apmManagerPtr = &apmManager;

    apmUart.setDebugPrint(true);
    apmUart.startupMessage();

    // Initially Load Device into Accessory Mode on Power On
    apmManagerPtr->offToAccessoryMode();

    // By default do not perform GFD Isolation Checking yet
    apmManagerPtr->setCheckGFDIsolationState(false);

    // Set up interrupt for key signal
    keyOnSw_GPIO.registerIRQ(IO::GPIO::TriggerEdge::RISING, APM::handleOnButtonInterrupt);

    // Pull battery charge limiter high
    batteryChargeLimiter_GPIO.writePin(IO::GPIO::State::HIGH);

    // Display Prompt to user
    apmUart.setDebugPrint(false);
    while (true) {
        userPrompt(*apmManagerPtr, &apmUart);
    }
}
