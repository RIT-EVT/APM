/**
 * This is the entry point for the code to run on the dev1_apm.
 * 08/24/21
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/startup.hpp>
#include <APM/apm_uart.hpp>
#include <cstring>
#include <map>

namespace IO = EVT::core::IO;

namespace APM {

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

constexpr IO::Pin ACCESSORY_SW = IO::Pin::PA_5;
constexpr IO::Pin CHARGE_SW = IO::Pin::PA_6;
constexpr IO::Pin VICOR_SW = IO::Pin::PA_4;
constexpr IO::Pin KEY_ON_UC = IO::Pin::PB_5;

char buf[BUF_SIZE];

enum class ApmMode {
    OFF = 0u,
    ACCESSORY = 1u,
    ON = 2u
};

ApmMode currentMode = ApmMode::OFF;

/**
* Prompt to the user for interfacing with the board over UART Debug
* @param uart reference to the IO::UART object for interfacing with the user
* @return 0 on success, 1 if a failure has occurred
*/
int userPrompt(IO::UART &uart) {
    // TODO: Figure out how to pass uart as const reference
    uart.printf("\n\rPlease enter a command\n\r");
    uart.printf("Enter 'h' for help\n\r");
    uart.printf(">> ");

    uart.gets(buf, BUF_SIZE);
    uart.printf("\n\r");

    // TODO: Implement Different Commands
    // 'h': Help Message
    // 'd': Debug Mode.  Prints out all debug statements to terminal.  Exit
    //    on key press
    // 'm': Get Mode.  Returns accessory or on respectively

    if (strncmp("h", buf, BUF_SIZE) == 0) {
        uart.printf("List of possible commands:\n\r");
        uart.printf("\t'h': Help Message\n\r");
        uart.printf("\t'd': Debug Mode.  Prints out all debug statements to terminal.  Exit\n\r");
        uart.printf("\t     on key press\n\r");
        uart.printf("\t'm': Get Mode.  Returns accessory or on respectively");
    }
    else if (strncmp("m", buf, BUF_SIZE) == 0) {
        char modeString[10];
        switch (currentMode) {
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

        uart.printf("Current APM Mode: %s\n\r", modeString);
    }
    else if (strncmp("d", buf, BUF_SIZE) == 0) {
        uart.printf("Entering Debug Mode.  Press any key to exit...\n\r");
        APM::setDebugPrint(true);
        uart.getc();
        APM::setDebugPrint(false);
    }

    return 0;
}

/**
* Function to handle the transition from OFF mode to Accessory Mode
* @param accessorySW_GPIO The GPIO object for the Accessory_SW
* @return 0 on success.
*/
int offToAccessoryMode(IO::GPIO &accessorySW_GPIO) {
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    // TODO: Send Accessory Mode CAN Message and start sending on timer (interrupt)
    currentMode = ApmMode::ACCESSORY;

    APM::printDebugString("Accessory_SW Closed\n\r");
    APM::printDebugString("Entered Accessory Mode\n\r");
    APM::printDebugString("---------------------------------------------\n\r");

    return 0;
}

/**
* Function to transition from Accessory Mode to On Mode
* @param accessorySW_GPIO The GPIO object for Accessory_SW
* @param chargeSW_GPIO The GPIO object for Charge_SW
* @param vicorSW_GPIO The GPIO object for Vicor_SW
* @return 0 on success.
*/
int accessoryToOnMode(IO::GPIO &accessorySW_GPIO, IO::GPIO &chargeSW_GPIO,
                      IO::GPIO &vicorSW_GPIO) {
    vicorSW_GPIO.writePin(IO::GPIO::State::HIGH);
    APM::printDebugString("Vicor_SW Closed\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::LOW);
    APM::printDebugString("Accessory_SW Opened\n\r");
    chargeSW_GPIO.writePin(IO::GPIO::State::HIGH);
    APM::printDebugString("Charge_SW Closed\n\r");

    // TODO: Send CAN Message for ON Mode on timer

    currentMode = ApmMode::ON;

    APM::printDebugString("Entered On Mode\n\r");
    APM::printDebugString("---------------------------------------------\n\r");

    return 0;
}

} // namespace APM

using namespace APM;

int main() {
    int prompt_status = 1;
    IO::GPIO::State keyOnSwState = IO::GPIO::State::LOW;

    // Initialize IO Devices
    IO::init();
    IO::UART &uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);
    IO::GPIO &accessorySW_GPIO = IO::getGPIO<ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeSW_GPIO = IO::getGPIO<CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorSW_GPIO = IO::getGPIO<VICOR_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &keyOnSw_GPIO = IO::getGPIO<KEY_ON_UC>(IO::GPIO::Direction::INPUT);

    APM::setUART(&uart);
    APM::setDebugPrint(true);
    APM::startupMessage();

    // Initially Load Device into Accessory Mode on Power On
    offToAccessoryMode(accessorySW_GPIO);

    // Check if key_sw is high
    keyOnSwState = keyOnSw_GPIO.readPin();
    if (keyOnSwState == IO::GPIO::State::HIGH) {
        APM::printDebugString("Key_On_UC is initially high.  Loading APM into ON Mode");
        accessoryToOnMode(accessorySW_GPIO, chargeSW_GPIO, vicorSW_GPIO);
    }
    // TODO: Handle interrupt case for ON Signal Turning ON.

    // Display Prompt to user
    APM::setDebugPrint(false);
    while (1) {
        prompt_status = userPrompt(uart);
    }

    // TODO: Handle case where somehow userPrompt exits
}
