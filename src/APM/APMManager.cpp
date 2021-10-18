/**
 * Source code for APMManager class
 */

#include <EVT/io/GPIO.hpp>
#include <EVT/utils/time.hpp>
#include <APM/APMManager.hpp>

APM::APMManager *apmManagerPtr1 = nullptr;

/**
 * Handler for timer to poll the SIM100 board to check for isolation faults
 * @param htim pointer to the timer device struct
 */
void sim100IsolationCheckIRQHandler(void *htim) {
    if (apmManagerPtr1->getCurrentMode() != APM::APMMode::ON) {
        // Shouldn't happen, but disable timer if you get here while device isn't on
        auto& timer = apmManagerPtr1->getGFDTimer();
        timer.stopTimer();
    }
    auto sim100State = apmManagerPtr1->getSim100().getIsolationState();
    if (sim100State != APM::DEV::SIM100::IsolationStateResponse::NoError) {
        apmManagerPtr1->onToAccessoryMode();
    }
}

/**
 * Method to trigger an interrupt once sufficient time has passed for the SIM100 GFD to start up.
 * Will update the timer to poll the SIM100 for isolation faults using update rate APMManager.SIM100_POLLING_PERIOD
 * @param htim Pointer to the timer struct for which the interrupt was triggered.
 */
void sim100StartupTimerIRQHandler(void *htim) {
    // Start the SIM100 polling check
    auto& timer = apmManagerPtr1->getGFDTimer();
    timer.stopTimer();
    timer.setPeriod(APM::APMManager::SIM100_POLLING_PERIOD);
    timer.startTimer(sim100IsolationCheckIRQHandler);
}

namespace APM {

APMManager::APMManager(APMUart &apmUart, DEV::SIM100 &sim100, IO::GPIO &accessorySwGpio, IO::GPIO &chargeSwGpio,
                       IO::GPIO &keyOnSwGpio, IO::GPIO &vicorSwGpio, EVT::core::DEV::Timerf302x8 &gfdTimer)
        : apmUart(apmUart), sim100(sim100),
          accessorySW_GPIO(accessorySwGpio), chargeSW_GPIO(chargeSwGpio),
          vicorSW_GPIO(vicorSwGpio), keyOnSw_GPIO(keyOnSwGpio), gfdTimer(gfdTimer) {
    apmManagerPtr1 = this;
}

int APMManager::offToAccessoryMode() {
    apmUart.printDebugString("Transitioning from OFF -> ACCESSORY\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    // TODO: Send Accessory Mode CAN Message and start sending on timer (interrupt)
    currentMode = APMMode::ACCESSORY;

    apmUart.printDebugString("Accessory_SW Closed\n\r");
    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

int APMManager::accessoryToOnMode() {
    apmUart.printDebugString("Transitioning from ACCESSORY -> ON\n\r");
    vicorSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Vicor_SW Closed\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Accessory_SW Opened\n\r");
    chargeSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Charge_SW Closed\n\r");

    // TODO: Send CAN Message for ON Mode on timer

    currentMode = APMMode::ON;

    apmUart.printDebugString("Entered On Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    // Set up GFD with interrupts.
    sim100.restartSIM100();
    sim100.setMaxWorkingVoltage(DEV::SIM100::DEV1_MAX_BATTERY_VOLTAGE);
    this->gfdTimer.setPeriod(SIM100_STARTUP_PERIOD);
    this->gfdTimer.startTimer(sim100StartupTimerIRQHandler);

    return 0;
}

int APMManager::onToAccessoryMode() {
    // TODO: Turn off GFD Isolation Check

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
    while (keyOnSw_GPIO.readPin() == IO::GPIO::State::HIGH) {
        apmUart.printDebugString("Waiting for Key to return to accessory mode state\n\r");
        EVT::core::time::wait(500);
    }

    currentMode = APMMode::ACCESSORY;

    // TODO: Enable Accessory Mode Message on Timer

    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

APMUart & APMManager::getApmUart() const {
    return apmUart;
}

int APMManager::checkOnSw() {
    apmUart.printDebugString("Manually checking on switch status\n\r");

    IO::GPIO::State keyOnSwState = keyOnSw_GPIO.readPin();
    if (keyOnSwState == IO::GPIO::State::HIGH) {
        switch (currentMode) {
            case APMMode::OFF:
                offToAccessoryMode();
            case APMMode::ACCESSORY:
                accessoryToOnMode();
                break;
            case APMMode::ON:
            default:
                apmUart.printDebugString("Device is already On\n\r");
        }
    }

    return 0;
}

APMMode APMManager::getCurrentMode() const {
    return currentMode;
}

DEV::SIM100 &APMManager::getSim100() const {
    return sim100;
}

EVT::core::DEV::Timer &APMManager::getGFDTimer() const {
    return gfdTimer;
}

}  // namespace APM
