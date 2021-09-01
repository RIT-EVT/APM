/**
 * Source file for apm_uart.hpp
 */

#include <APM/ApmUart.hpp>

namespace APM {

ApmUart::ApmUart(IO::UART *apmUart, bool apmDebugPrint) {
    this -> apmUart = apmUart;
    this -> apmDebugPrint = apmDebugPrint;
}

void ApmUart::startupMessage() const {
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

    apmUart->printf("\nDEV1 ApmDevice Initializing...\n\n\r");
}

void ApmUart::setDebugPrint(bool debugPrint) {
    apmDebugPrint = debugPrint;
}

void ApmUart::printDebugString(const char* message) const {
    if (apmUart != nullptr) {
        if (apmDebugPrint) {
            apmUart -> printf("%s", message);
        }
    }
}

void ApmUart::printString(const char *message) const {
    apmUart ->printf("%s", message);
}

char ApmUart::getc() const {
    return apmUart->getc();
}

char *ApmUart::gets(char *buf, size_t size) const {
    return apmUart->gets(buf, size);
}

}  // namespace APM
