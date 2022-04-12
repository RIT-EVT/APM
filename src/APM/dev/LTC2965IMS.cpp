/**
* Source code for LTC2695IMS Class
*/

#include <APM/dev/LTC2965IMS.hpp>


namespace APM::DEV {
    namespace IO = EVT::core::IO;

    LTC2965IMS::LTC2965IMS(IO::GPIO &gpio) : _gpio(gpio) {
        this->_gpio = gpio;
    }

    EVT::core::IO::GPIO::State LTC2965IMS::checkLogicLevel() {
        return _gpio.readPin();
    }
}
