/**
 * This is the entry point for the code to run on the dev1_apm.
 * 08/24/21
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/startup.hpp>

namespace IO = EVT::core::IO;

constexpr int BAUD_RATE = 115200;
char buf[256];

int main() {
    // Initialize IO Devices
    IO::init();
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);

    uart.printf("\x1B\x5B\x32\x4A");    // Escape sequence for minicom terminal to clear display

    uart.printf("                       @@@@@@@@@@@@@@@@@@@@@@@@@@                      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
    uart.printf("                      @@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n\r");
    uart.printf("                    (@@@@@@                                                     @@@@@@              \n\r");
    uart.printf("        ////       @@@@@@                                                      @@@@@@               \n\r");
    uart.printf("      //*//       @@@@@@                                                      @@@@@@                \n\r");
    uart.printf("  ////////////   @@@@@@@@@@@@@@@@@@   //////////*//         ////*/////////   @@@@@@   ./////////*/  \n\r");
    uart.printf("    ////*       @@@@@@                         /*///      //////            @@@@@@                  \n\r");
    uart.printf("   ////        @@@@@@                          /*///   //////             @@@@@@                    \n\r");
    uart.printf("              @@@@@@                           /*/*/ /*/*/*              @@@@@@                     \n\r");
    uart.printf("            @@@@@@@@@@@@@@@@@@@@@@@@@@@         *///////                @@@@@@                      \n\r");
    uart.printf("           @@@@@@@@@@@@@@@@@@@@@@@@@@@          */////                 @@@@@@                       \n\r");

    uart.printf("\nDEV1 APM Initializing...\n\n\r");

    while (!APM::Startup::wait_for_startup()) {
        uart.printf("Waiting for startup...\n\r");
    }
}

/**
 * Prompt to the user while bike is in standby state waiting for ON Switch to start
 * @param uart reference to the IO::UART object for interfacing with the user
 * @return 0 on success, 1 if a failure has occurred
 */
int standby_prompt(const IO::UART& uart) {
    return 0;
}
