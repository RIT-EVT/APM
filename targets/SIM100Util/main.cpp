/**
 * This is a basic sample for interfacing with the GFD board
 */

#include <EVT/io/manager.hpp>
#include <EVT/io/pin.hpp>
#include <EVT/io/UART.hpp>
#include <cstring>
#include <APM/dev/SIM100.hpp>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
namespace IO = EVT::core::IO;

constexpr int BAUD_RATE = 115200;
constexpr int BUF_SIZE = 256;

int main() {
    // Initialize system
    IO::init();

    // Setup UART
    IO::UART& uart = IO::getUART<IO::Pin::UART_TX, IO::Pin::UART_RX>(BAUD_RATE);
    IO::CAN& can = IO::getCAN<IO::Pin::PA_12, IO::Pin::PA_11>();

    // Setup SIM100 Device
    auto sim100 = APM::DEV::SIM100(can);

    // String to store user input
    char buf[BUF_SIZE];

    uart.printf("\n\n\r");

    while (true) {
        // Read user input
        uart.printf("Enter Command\n\r");
        uart.printf("Enter 'h' for help message\n\r");
        uart.printf(">> ");

        memset(buf, 0, BUF_SIZE);
        uart.gets(buf, BUF_SIZE);

        uart.printf("\n\r");

        if (strncmp("h", buf, BUF_SIZE) == 0) {
            uart.printf("List of possible commands\n\r");
            uart.printf("\t'h': Help Message\n\r");
            uart.printf("\t'n': Get the SIM100 Manufacturer Name\n\r");
            uart.printf("\t'm': Set the maximum voltage.  Uses DEV1_MAX_VOLTAGE\n\r");
            uart.printf("\t'i': Read the isolation status.\n\r");
            uart.printf("\t'r': Restarts the SIM100 device.\n\r");
            uart.printf("\r'v': Gets the SIM100 firmware version\n\r");
        } else if (strncmp("n", buf, BUF_SIZE) == 0) {
            uart.printf("Getting Device Manufacturer name\n\r");
            if (sim100.getPartName(buf, BUF_SIZE) != 0) {
                uart.printf("An error occurred when retrieving the device name\n\r");
            } else {
                uart.printf("The device name is: %s\n\r", buf);
            }

        } else if (strncmp("m", buf, BUF_SIZE) == 0) {
//            uint16_t setVoltage = APM::DEV::SIM100::DEV1_MAX_BATTERY_VOLTAGE;
            uint16_t setVoltage = 50;
            uint16_t receivedVoltage = 0;
            uart.printf("Setting GFD Max Voltage to %d V\n\r", setVoltage);

            receivedVoltage = sim100.setMaxWorkingVoltage(setVoltage);
            if (setVoltage != receivedVoltage) {
                uart.printf("An error occurred when setting the max working voltage\n\r");
                uart.printf("Expected: %d\n\r", setVoltage);
                uart.printf("Received: %d\n\r", receivedVoltage);
            } else {
                uart.printf("Successfully set max voltage: %d\n\r", receivedVoltage);
            }
        } else if (strncmp("i", buf, BUF_SIZE) == 0) {
            uart.printf("Reading the isolation status of the SIM100 board\n\r");
            auto errorCode = sim100.getIsolationState();
            if (errorCode == APM::DEV::SIM100::IsolationStateResponse::NoError) {
                uart.printf("No Errors detected!\n\r");
            } else {
                uart.printf("Error detected: %d\n\r", static_cast<uint8_t>(errorCode));
            }
        } else if (strncmp("r", buf, BUF_SIZE) == 0) {
            uart.printf("Restarting SIM100\n\r");
            if (sim100.restartSIM100() == 0) {
                uart.printf("Restart successful!!\n\r");
            } else {
                uart.printf("Restart failed.\n\r");
            }
        } else if (strncmp("v", buf, BUF_SIZE) == 0) {
            uart.printf("Reading Version Number\n\r");
            if (sim100.getVersion(buf, BUF_SIZE) != 0) {
                uart.printf("An error occurred when retrieving the device version\n\r");
            } else {
                uart.printf("The device version is: %s\n\r", buf);
            }
        } else {
                uart.printf("Unrecognized command\n\r");
        }

        uart.printf("\n\r");
    }
}

#pragma clang diagnostic pop
