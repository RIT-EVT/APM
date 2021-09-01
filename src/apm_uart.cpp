/**
 * Source file for apm_uart.hpp
 */

#include <APM/apm_uart.hpp>

constexpr char MINICOM_CLEAR_DISPLAY[5]= "\x1B\x5B\x32\x4A";

IO::UART *apmUart = nullptr;
bool apmDebugPrint = false;

void APM::startupMessage() {
    apmUart->printf("%s\n\r", MINICOM_CLEAR_DISPLAY);    // Escape sequence for minicom terminal to clear display

    apmUart->printf("                       @@@@@@@@@@@@@@@@@@@@@@@@@@                      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\r");
    apmUart->printf("                      @@@@@@@@@@@@@@@@@@@@@@@@@@                     @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n\r");
    apmUart->printf("                    (@@@@@@                                                     @@@@@@              \n\r");
    apmUart->printf("        ////       @@@@@@                                                      @@@@@@               \n\r");
    apmUart->printf("      //*//       @@@@@@                                                      @@@@@@                \n\r");
    apmUart->printf("  ////////////   @@@@@@@@@@@@@@@@@@   //////////*//         ////*/////////   @@@@@@   ./////////*/  \n\r");
    apmUart->printf("    ////*       @@@@@@                         /*///      //////            @@@@@@                  \n\r");
    apmUart->printf("   ////        @@@@@@                          /*///   //////             @@@@@@                    \n\r");
    apmUart->printf("              @@@@@@                           /*/*/ /*/*/*              @@@@@@                     \n\r");
    apmUart->printf("            @@@@@@@@@@@@@@@@@@@@@@@@@@@         *///////                @@@@@@                      \n\r");
    apmUart->printf("           @@@@@@@@@@@@@@@@@@@@@@@@@@@          */////                 @@@@@@                       \n\r");

    apmUart->printf("\nDEV1 APM Initializing...\n\n\r");
}

void APM::setUART(IO::UART *uart) {
    apmUart = uart;
}

void APM::setDebugPrint(bool debugPrint) {
    apmDebugPrint = debugPrint;
}

void APM::printDebugString(const char* message) {
    if (apmUart != nullptr) {
        if (apmDebugPrint) {
            apmUart -> printf(message);
        }
    }
}
