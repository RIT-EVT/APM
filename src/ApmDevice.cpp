/**
 * Source code for ApmDevice class
 */

#include <EVT/io/GPIO.hpp>
#include <EVT/utils/time.hpp>
#include "APM/ApmDevice.hpp"

namespace APM {

ApmDevice::ApmDevice(IO::GPIO &accessorySwGpio, ApmUart& apmUart, IO::GPIO &chargeSwGpio,
                     IO::GPIO &keyOnSwGpio, IO::GPIO &vicorSwGpio)
        : apmUart(apmUart), accessorySW_GPIO(accessorySwGpio), chargeSW_GPIO(chargeSwGpio), vicorSW_GPIO(vicorSwGpio),
          keyOnSw_GPIO(keyOnSwGpio) {
}

int ApmDevice::offToAccessoryMode() {
    apmUart.printDebugString("Transitioning from OFF -> ACCESSORY\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    // TODO: Send Accessory Mode CAN Message and start sending on timer (interrupt)
    currentMode = ApmMode::ACCESSORY;

    apmUart.printDebugString("Accessory_SW Closed\n\r");
    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

int ApmDevice::accessoryToOnMode() {
    apmUart.printDebugString("Transitioning from ACCESSORY -> ON\n\r");
    vicorSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Vicor_SW Closed\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Accessory_SW Opened\n\r");
    chargeSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Charge_SW Closed\n\r");

    // TODO: Send CAN Message for ON Mode on timer

    currentMode = ApmMode::ON;

    apmUart.printDebugString("Entered On Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

int ApmDevice::onToAccessoryMode() {
    // TODO: Send Accessory Mode CAN Message
    // Alerts other boards to begin transition to accessory mode

    chargeSW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Charge_SW opened\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Accessory_SW closed\n\r");
    vicorSW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Vicor_SW opened\n\r");

    // TODO: Wait for CAN handshakes to verify all other boards
    // have returned to accessory mode

    // Wait for the On key to return to accessory state
    while(keyOnSw_GPIO.readPin() == IO::GPIO::State::HIGH) {
        apmUart.printDebugString("Waiting for Key to return to accessory mode state\n\r");
        EVT::core::time::wait(500);
    }

    currentMode = ApmMode::ACCESSORY;

    // TODO: Enable Accessory Mode Message on Timer

    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

ApmUart & ApmDevice::getApmUart() {
    return apmUart;
}

int ApmDevice::checkOnSw() {

    apmUart.printDebugString("Manually checking on switch status\n\r");

    IO::GPIO::State keyOnSwState = keyOnSw_GPIO.readPin();
    if (keyOnSwState == IO::GPIO::State::HIGH) {
        switch (currentMode) {
            case ApmMode::OFF:
                offToAccessoryMode();
            case ApmMode::ACCESSORY:
                accessoryToOnMode();
                break;
            case ApmMode::ON:
            default:
                apmUart.printDebugString("Device is already On\n\r");
        }
    }

    return 0;
}

ApmMode ApmDevice::getCurrentMode() {
    return currentMode;
}

} //namespace APM
