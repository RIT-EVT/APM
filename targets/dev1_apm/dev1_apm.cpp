/**
 * This is the entry point for the code to run on the dev1_apm.
 * 08/24/21
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/startup.hpp>
#include <APM/apm_uart.hpp>

namespace IO = EVT::core::IO;

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

constexpr IO::Pin ACCESSORY_SW = IO::Pin::PA_5;
constexpr IO::Pin CHARGE_SW = IO::Pin::PA_6;
constexpr IO::Pin VICOR_SW = IO::Pin::PA_4;
constexpr IO::Pin KEY_ON_UC = IO::Pin::PB_5;

char buf[BUF_SIZE];

/**
 * Prompt to the user for interfacing with the board over UART Debug
 * @param uart reference to the IO::UART object for interfacing with the user
 * @return 0 on success, 1 if a failure has occurred
 */
int user_prompt(IO::UART& uart) {
    // TODO: Figure out how to pass uart as const reference
    uart.printf("\n\rPlease enter a command\n\r");
    uart.printf("Enter 'h' for help\n\r");
    uart.printf(">> ");

    uart.gets(buf, BUF_SIZE);

    // TODO: Implement Different Commands
    // h: Help Message
    // d: Debug Mode.  Prints out all debug statements to terminal.  Exit
    //    on key press
    // m: Get Mode.  Returns accessory or on respectively

    uart.printf("\n\rCommand Entered: %s\n\r", buf);

    return 0;
}

int main() {
    int prompt_status = 1;

    // Initialize IO Devices
    IO::init();
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);
    IO::GPIO& AccessorySW_GPIO = IO::getGPIO<ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& ChargeSW_GPIO = IO::getGPIO<CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO& VicorSW_GPIO = IO::getGPIO<VICOR_SW>(IO::GPIO::Direction::OUTPUT);

    APM::startup_message(uart);

    // Initially Load Device into Accessory Mode on Power On
    AccessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    // TODO: Send Accessory Mode CAN Message and start sending on timer (interrupt)
    uart.printf("Accessory Switch Closed\n\r");
    uart.printf("Entered Accessory Mode\n\r");
    uart.printf("---------------------------------------------\n\r");

    // Check if key_sw is high

    // TODO: Handle case where ON signal is initially high when APM loads up.
    // TODO: Handle interrupt case for ON Signal Turning ON.

    // Display Prompt to user
    while (1) {
        prompt_status = user_prompt(uart);
    }

    // TODO: Handle case where somehow user_prompt exits
}
