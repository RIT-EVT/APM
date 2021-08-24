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

    IO::init();

    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);

    uart.printf("DEV1 APM Init...\n\n\r");
    while (!APM::Startup::wait_for_startup()) {
        uart.printf("Waiting for startup...\n\r");
    }

}
