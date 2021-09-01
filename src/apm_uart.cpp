/**
 * Source file for apm_uart.hpp
 */

#include <APM/apm_uart.hpp>

constexpr char MINICOM_CLEAR_DISPLAY[5]= "\x1B\x5B\x32\x4A";

void APM::startup_message(IO::UART &uart) {
    uart.printf("%s\n\r", MINICOM_CLEAR_DISPLAY);    // Escape sequence for minicom terminal to clear display

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
}
