/**
 * This is the entry point for the code to run on the dev1_apm.
 * 08/24/21
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/ApmUart.hpp>
#include <cstring>
#include <APM/ApmDevice.hpp>
#include <cstdio>

namespace IO = EVT::core::IO;

namespace APM {

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

char buf[BUF_SIZE];
APM::ApmDevice* apmDevicePtr;

void handleKeyOnInterrupt(IO::GPIO* gpio) {
    auto apmUart = apmDevicePtr->getApmUart();
    auto state = gpio->readPin();
    char outStr[BUF_SIZE];

    sprintf(outStr, "Key Interrupt Triggered: %s edge\n\r",
            state == IO::GPIO::State::HIGH ? "rising" : "falling");
    apmUart.printDebugString(outStr);

    if (state == IO::GPIO::State::HIGH && apmDevicePtr->getCurrentMode() == ApmMode::ACCESSORY) {
        apmDevicePtr -> accessoryToOnMode();
    }
    else if (state == IO::GPIO::State::LOW && apmDevicePtr->getCurrentMode() == ApmMode::ON) {
        apmDevicePtr -> onToAccessoryMode();
    }
    else {
        // TODO: Handle error
        // Probably just print out error
    }
}

/**
* Prompt to the user for interfacing with the board over UART Debug
* @param uart reference to the IO::UART object for interfacing with the user
* @return 0 on success, 1 if a failure has occurred
*/
int userPrompt(ApmDevice& apmDevice, ApmUart& apmUart) {
    // TODO: Figure out how to pass uart as const reference
    apmUart.printString("\n\rPlease enter a command\n\r");
    apmUart.printString("Enter 'h' for help\n\r");
    apmUart.printString(">> ");

    apmUart.gets(buf, BUF_SIZE);
    apmUart.printString("\n\r");

    if (strncmp("h", buf, BUF_SIZE) == 0) {
        apmUart.printString("List of possible commands:\n\r");
        apmUart.printString("\t'h': Help Message\n\r");
        apmUart.printString("\t'd': Debug Mode.  Prints out all debug statements to terminal.  Exit\n\r");
        apmUart.printString("\t     on key press\n\r");
        apmUart.printString("\t'm': Get Mode.  Returns accessory or on respectively");
    }
    else if (strncmp("m", buf, BUF_SIZE) == 0) {
        char modeString[10];
        switch (apmDevice.getCurrentMode()) {
            case ApmMode::OFF:
                strncpy(modeString, "OFF", 10);
                break;
            case ApmMode::ACCESSORY:
                strncpy(modeString, "ACCESSORY", 10);
                break;
            case ApmMode::ON:
                strncpy(modeString, "ON", 10);
                break;
        }

        sprintf(buf, "Current ApmDevice Mode: %s\n\r", modeString);
        apmUart.printString(buf);
    }
    else if (strncmp("d", buf, BUF_SIZE) == 0) {
        apmUart.printString("Entering Debug Mode.  Will print out all debug messages to terminal\n\r");
        apmUart.printString("Must reset the device to exit DEBUG mode\n\r");
//        apmUart.printString("Press any key to exit...\n\r");
        apmUart.setDebugPrint(true);
//        apmUart.getc();
//        apmUart.setDebugPrint(false);
        while (1) {} // TODO: Update to wait for user input once UART interrupts are implemented
        // Currently it requires an entire device reset to exit blocking mode.
    }

    return 0;
}

} // namespace ApmDevice

using namespace APM;

int main() {
    int prompt_status = 1;

    // Initialize IO Objects
    IO::init();
    IO::UART &uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);
    IO::GPIO &accessorySW_GPIO = IO::getGPIO<ApmDevice::ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeSW_GPIO = IO::getGPIO<ApmDevice::CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorSW_GPIO = IO::getGPIO<ApmDevice::VICOR_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &keyOnSw_GPIO = IO::getGPIO<ApmDevice::KEY_ON_UC>(IO::GPIO::Direction::INPUT);

    ApmUart apmUart = APM::ApmUart(&uart);

    // Create Data Objects
    ApmDevice apmDevice = APM::ApmDevice(BAUD_RATE, accessorySW_GPIO, apmUart, chargeSW_GPIO,
                                         keyOnSw_GPIO, vicorSW_GPIO);
    apmDevicePtr = &apmDevice;
    // TODO figure out this pointer better

    apmUart.setDebugPrint(true);
    apmUart.startupMessage();

    // Initially Load Device into Accessory Mode on Power On
    apmDevicePtr->offToAccessoryMode();

    // Check if key_sw is high
    apmDevicePtr->checkOnSw();

    // Set up interrupt for key signal
    keyOnSw_GPIO.registerIRQ(IO::GPIO::TriggerEdge::RISING_FALLING, handleKeyOnInterrupt);

    // Display Prompt to user
    apmUart.setDebugPrint(false);
    while (1) {
        prompt_status = userPrompt(*apmDevicePtr, apmUart);
    }
}
