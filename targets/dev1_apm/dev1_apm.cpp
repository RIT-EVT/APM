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

    uart.printf("\n\rCommand Entered: %s\n\r", buf);

    return 0;
}

int main() {
    int prompt_status = 1;

    // Initialize IO Devices
    IO::init();
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);

    APM::startup_message(uart);

    // Initially Load Device into Accessory Mode on Power On
    // TODO: Load device in accessory mode

    // Display Prompt to user
    while (1) {
        prompt_status = user_prompt(uart);
    }

    // TODO: Handle case where somehow user_prompt exits
}
