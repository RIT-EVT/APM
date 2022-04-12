/**
* Source code for LTC2695IMS Class
*/

#include <APM/dev/LTC2965IMS.hpp>


namespace APM::DEV {
    namespace IO = EVT::core::IO;

    LTC2965IMS::LTC2965IMS(IO::GPIO &gpio) : _gpio(gpio) {
        this->_gpio = gpio;
    }

    /**
     * GPIO pin on MCU. PC6 is connected to output of IC
     * Have as GPIO object in code
     * EVT Core has gpio read pin.
     * Read the pin and tell high or low return
     */
    EVT::core::IO::GPIO::State LTC2965IMS::checkLogicLevel() {
        return _gpio.readPin();
    }
}
