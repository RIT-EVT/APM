#ifndef APM_LTC2695IMS_H
#define APM_LTC2695IMS_H

#include <EVT/io/GPIO.hpp>

/**
 * Class representing the LTC2965IMS#PBF Driver interface
 * Datasheet can be found here:
 * @link https://www.rlocman.ru/i/File/2017/10/25/2965fc.pdf
 */
namespace APM::DEV {
    namespace IO = EVT::core::IO;

    class LTC2965IMS {
    public:

        /**
         * LTC2695IMS constructor
         *
         * @param GPIO Pin - GPIO Pin to read from
         */
        explicit LTC2965IMS(IO::GPIO &gpio);

        /**
         * Return GPIO State Low or High based on current voltage
         * @return GPIO State
         */
        EVT::core::IO::GPIO::State checkLogicLevel();

    private:
        IO::GPIO &_gpio;
    };


}

#endif //APM_LTC2695IMS_H
