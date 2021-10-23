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
    if (!apmManagerPtr1->isIsolationChecking()) {
        // Do not perform GFD Checking
        return;
    }

    if (apmManagerPtr1->getCurrentMode() != APM::APMMode::ON) {
        // Shouldn't happen, but disable timer if you get here while device isn't on
        auto& timer = apmManagerPtr1->getGFDTimer();
        timer.stopTimer();
        return;
    }
    auto sim100State = apmManagerPtr1->getSim100().getIsolationState();
    if (sim100State != APM::DEV::SIM100::IsolationStateResponse::NoError) {
        apmManagerPtr1->getApmUart().printDebugString("SIM100 Error Occurred\n\r");
        apmManagerPtr1->onToAccessoryMode();
    }
    apmManagerPtr1->getApmUart().printDebugString("SIM100 No Error\n\r");
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
                       IO::GPIO &vicorSwGpio, EVT::core::DEV::Timerf302x8 &gfdTimer,
                       IO::GPIO &accessoryLed, IO::GPIO &onLed, IO::GPIO &mcRelayGpio)
        : apmUart(apmUart), sim100(sim100),
          mc_relay_GPIO(mcRelayGpio), accessorySW_GPIO(accessorySwGpio),
          chargeSW_GPIO(chargeSwGpio), vicorSW_GPIO(vicorSwGpio), accessory_LED(accessoryLed),
          on_LED(onLed), gfdTimer(gfdTimer) {
    apmManagerPtr1 = this;
}

int APMManager::offToAccessoryMode() {
    apmUart.printDebugString("Transitioning from OFF -> ACCESSORY\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    // TODO: Send Accessory Mode CAN Message and start sending on timer (interrupt)
    currentMode = APMMode::ACCESSORY;
    accessory_LED.writePin(EVT::core::IO::GPIO::State::HIGH);
    on_LED.writePin(EVT::core::IO::GPIO::State::LOW);

    apmUart.printDebugString("Accessory_SW Closed\n\r");
    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

int APMManager::accessoryToOnMode() {
    mc_relay_GPIO.writePin(EVT::core::IO::GPIO::State::HIGH);
    apmUart.printDebugString("Providing Power to MC\n\r");
    // TODO: Wait for contactor closed!
    // Use 96V sensing circuitry

    apmUart.printDebugString("Transitioning from ACCESSORY -> ON\n\r");
    vicorSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Vicor_SW Closed\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Accessory_SW Opened\n\r");
    chargeSW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Charge_SW Closed\n\r");

    // TODO: Send CAN Message for ON Mode on timer

    currentMode = APMMode::ON;
    accessory_LED.writePin(EVT::core::IO::GPIO::State::LOW);
    on_LED.writePin(EVT::core::IO::GPIO::State::HIGH);

    apmUart.printDebugString("Entered On Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    // Set up GFD with interrupts.
    if (isIsolationChecking()) {
        sim100.restartSIM100();
        EVT::core::time::wait(100);  // TODO: Remove once CAN Open support is added.
        sim100.setMaxWorkingVoltage(DEV::SIM100::DEV1_MAX_BATTERY_VOLTAGE);
        this->gfdTimer.setPeriod(SIM100_STARTUP_PERIOD);
        this->gfdTimer.startTimer(sim100StartupTimerIRQHandler);
    } else {
        this->gfdTimer.stopTimer();  // Stop timer just in case it is already running
    }

    return 0;
}

int APMManager::onToAccessoryMode() {
    // Turn off GFD Isolation Check
    this->gfdTimer.stopTimer();

    // TODO: Send Accessory Mode CAN Message
    // Alerts other boards to begin transition to accessory mode

    chargeSW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Charge_SW opened\n\r");
    accessorySW_GPIO.writePin(IO::GPIO::State::HIGH);
    apmUart.printDebugString("Accessory_SW closed\n\r");
    vicorSW_GPIO.writePin(IO::GPIO::State::LOW);
    apmUart.printDebugString("Vicor_SW opened\n\r");

    mc_relay_GPIO.writePin(EVT::core::IO::GPIO::State::LOW);
    apmUart.printDebugString("Closing MC Relay\n\r");

    // TODO: Wait for CAN handshakes to verify all other boards
    // have returned to accessory mode

    currentMode = APMMode::ACCESSORY;
    accessory_LED.writePin(EVT::core::IO::GPIO::State::HIGH);
    on_LED.writePin(EVT::core::IO::GPIO::State::LOW);

    // TODO: Enable Accessory Mode Message on Timer

    apmUart.printDebugString("Entered Accessory Mode\n\r");
    apmUart.printDebugString("---------------------------------------------\n\r");

    return 0;
}

APMUart & APMManager::getApmUart() const {
    return apmUart;
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

bool APMManager::isIsolationChecking() const {
    return this->checkGFDIsolationState;
}

void APMManager::setCheckGFDIsolationState(bool state) {
    this->checkGFDIsolationState = state;
}

}  // namespace APM
