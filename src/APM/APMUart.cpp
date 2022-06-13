/**
 * Source file for apm_uart.hpp
 */

#include <APM/APMUart.hpp>

namespace APM {

APMUart::APMUart(IO::UART *apmUart, bool apmDebugPrint) {
  this->apmUart = apmUart;
  this->apmDebugPrint = apmDebugPrint;
}

// clang-format off
void APMUart::startupMessage() const {
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

    apmUart->printf("\nDEV1 APMManager Initializing...\n\n\r");
}
// clang-format on

void APMUart::setDebugPrint(bool debugPrint) { apmDebugPrint = debugPrint; }

void APMUart::printDebugString(const char *message) const {
  if (apmUart != nullptr) {
    if (apmDebugPrint) {
      apmUart->printf("%s", message);
    }
  }
}

void APMUart::printString(const char *message) const {
  apmUart->printf("%s", message);
}

char APMUart::getc() const { return apmUart->getc(); }

char *APMUart::gets(char *buf, size_t size) const {
  return apmUart->gets(buf, size);
}

} // namespace APM
