/**
 * This code will allow for an easy testing of the APM MOSFET switches
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <APM/APMUart.hpp>
#include <cstring>
#include <APM/APMManager.hpp>
#include <string>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

namespace IO = EVT::core::IO;

namespace APM {

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

#ifdef NUCLEO_COMPILATION
constexpr IO::Pin UART_TX = IO::Pin::UART_TX;
constexpr IO::Pin UART_RX = IO::Pin::UART_RX;
#else
constexpr IO::Pin UART_TX = IO::Pin::PB_10;
constexpr IO::Pin UART_RX = IO::Pin::PB_11;
#endif

char buf[BUF_SIZE];

}  // namespace APM

/**
 * Returns the inverse of the current GPIO state
 * @param state the current state of a GPIO pin
 * @return the opposite state
 */
EVT::core::IO::GPIO::State getToggle(EVT::core::IO::GPIO::State state) {
    if (state == EVT::core::IO::GPIO::State::LOW) {
        return EVT::core::IO::GPIO::State::HIGH;
    } else {
        return EVT::core::IO::GPIO::State::LOW;
    }
}

int main() {
    // Initialize IO Objects
    IO::init();
    IO::UART &uart = IO::getUART<APM::UART_TX, APM::UART_RX>(APM::BAUD_RATE);
    IO::GPIO &accessorySW_GPIO =
            IO::getGPIO<APM::APMManager::ACCESSORY_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeSW_GPIO =
            IO::getGPIO<APM::APMManager::CHARGE_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeEnable_GPIO =
            IO::getGPIO<APM::APMManager::CHARGE_LIMITER_ENABLE>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorSW_GPIO =
            IO::getGPIO<APM::APMManager::VICOR_SW>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &accessoryIndicator_GPIO =
            IO::getGPIO<APM::APMManager::ACCESSORY_INDICATOR>(IO::GPIO::Direction::OUTPUT);
    IO::GPIO &chargeIndicator_GPIO =
            IO::getGPIO<APM::APMManager::Test_LED_0>(EVT::core::IO::GPIO::Direction::OUTPUT);
    IO::GPIO &vicorIndicator_GPIO =
            IO::getGPIO<APM::APMManager::Test_LED_1>(EVT::core::IO::GPIO::Direction::OUTPUT);

    auto apmUart = APM::APMUart(&uart);

    while (true) {
        apmUart.printString("\n\rPlease enter a command\n\r");
        apmUart.printString("Enter 'h' for help\n\r");
        apmUart.printString(">> ");

        apmUart.gets(APM::buf, APM::BUF_SIZE);
        apmUart.printString("\n\r");

        if (strncmp("h", APM::buf, APM::BUF_SIZE) == 0) {
            apmUart.printString("List of possible commands:\n\r");
            apmUart.printString("\t'A' - Toggle Accessory Switch\n\r");
            apmUart.printString("\t'a' - Read Accessory Switch\n\r");
            apmUart.printString("\t'V' - Toggle Vicor Switch\n\r");
            apmUart.printString("\t'v' - Read Vicor Switch\n\r");
            apmUart.printString("\t'C' - Toggle Charging Switch\n\r");
            apmUart.printString("\t'c' - Read Charging Switch\n\r");
        } else if (strncmp("A", APM::buf, APM::BUF_SIZE) == 0) {
            auto state = getToggle(accessorySW_GPIO.readPin());
            accessorySW_GPIO.writePin(state);
            accessoryIndicator_GPIO.writePin(state);
            sprintf(APM::buf, "Accessory Switch: %d\n\r", static_cast<unsigned int>(accessorySW_GPIO.readPin()));
            apmUart.printString(APM::buf);
        } else if (strncmp("a", APM::buf, APM::BUF_SIZE) == 0) {
            sprintf(APM::buf, "Accessory Switch: %d\n\r", static_cast<unsigned int>(accessorySW_GPIO.readPin()));
            apmUart.printString(APM::buf);
        } else if (strncmp("V", APM::buf, APM::BUF_SIZE) == 0) {
            auto state = getToggle(vicorSW_GPIO.readPin());
            vicorSW_GPIO.writePin(state);
            vicorIndicator_GPIO.writePin(state);
            sprintf(APM::buf, "Vicor Switch: %d\n\r", static_cast<unsigned int>(vicorSW_GPIO.readPin()));
            apmUart.printString(APM::buf);
        } else if (strncmp("v", APM::buf, APM::BUF_SIZE) == 0) {
            sprintf(APM::buf, "Vicor Switch: %d\n\r", static_cast<unsigned int>(vicorSW_GPIO.readPin()));
            apmUart.printString(APM::buf);
        } else if (strncmp("C", APM::buf, APM::BUF_SIZE) == 0) {
            auto state = getToggle(chargeSW_GPIO.readPin());
            chargeSW_GPIO.writePin(state);
            chargeEnable_GPIO.writePin(state);
            chargeIndicator_GPIO.writePin(state);
            sprintf(APM::buf, "Charge Switch: %d\n\r", static_cast<unsigned int>(chargeSW_GPIO.readPin()));
            apmUart.printString(APM::buf);
            sprintf(APM::buf, "Charge Enable: %d\n\r", static_cast<unsigned int>(chargeEnable_GPIO.readPin()));
            apmUart.printString(APM::buf);
        } else if (strncmp("c", APM::buf, APM::BUF_SIZE) == 0) {
            sprintf(APM::buf, "Charge Switch: %d\n\r", static_cast<unsigned int>(chargeSW_GPIO.readPin()));
            apmUart.printString(APM::buf);
            sprintf(APM::buf, "Charge Enable: %d\n\r", static_cast<unsigned int>(chargeEnable_GPIO.readPin()));
            apmUart.printString(APM::buf);
        }
    }
}

#pragma clang diagnostic pop