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

char buf[BUF_SIZE];

void handleKeyOnInterrupt(IO::GPIO *gpio) {
    auto apmUart = apmManagerPtr->getApmUart();
    auto state = gpio->readPin();
    char outStr[BUF_SIZE];

    snprintf(outStr, BUF_SIZE, "Key Interrupt Triggered: %s edge\n\r",
             state == IO::GPIO::State::HIGH ? "rising" : "falling");
    apmUart.printDebugString(outStr);

    if (state == IO::GPIO::State::HIGH && apmManagerPtr->getCurrentMode() == APMMode::ACCESSORY) {
        apmManagerPtr->accessoryToOnMode();
    } else if (state == IO::GPIO::State::LOW && apmManagerPtr->getCurrentMode() == APMMode::ON) {
        apmManagerPtr->onToAccessoryMode();
    } else {
        // TODO: Handle error
        apmUart.printString("ERROR: Key_On_SW state does not align with current mode\n\r");
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
        apmUart->printString("\t'm': Get Mode.  Returns accessory or on respectively");
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
    }

    return 0;
}

}  // namespace APM

int main() {
    // Initialize IO Objects
    IO::init();
    IO::UART &uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(APM::BAUD_RATE);
    IO::GPIO &accessorySW_GPIO = IO::getGPIO<APM::APMManager::ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeSW_GPIO = IO::getGPIO<APM::APMManager::CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorSW_GPIO = IO::getGPIO<APM::APMManager::VICOR_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &keyOnSw_GPIO = IO::getGPIO<APM::APMManager::KEY_ON_UC>(IO::GPIO::Direction::INPUT);

    IO::CAN& can = IO::getCAN<APM::CAN_TX, APM::CAN_RX>();

    auto apmUart = APM::APMUart(&uart);
    auto sim100 = APM::DEV::SIM100(can);

    auto apmTimer = EVT::core::DEV::Timerf302x8(TIM2, 5000);

    // Create Data Objects
    APM::APMManager apmManager = APM::APMManager(apmUart, sim100, accessorySW_GPIO, chargeSW_GPIO,
                                                 keyOnSw_GPIO, vicorSW_GPIO, apmTimer);
    apmManagerPtr = &apmManager;

    apmUart.setDebugPrint(true);
    apmUart.startupMessage();

    // Initially Load Device into Accessory Mode on Power On
    apmManagerPtr->offToAccessoryMode();

    // Check if key_sw is high
    apmManagerPtr->checkOnSw();

    // Set up interrupt for key signal
    keyOnSw_GPIO.registerIRQ(IO::GPIO::TriggerEdge::RISING_FALLING, APM::handleKeyOnInterrupt);

    // Display Prompt to user
    apmUart.setDebugPrint(false);
    while (true) {
        userPrompt(*apmManagerPtr, &apmUart);
    }
}
