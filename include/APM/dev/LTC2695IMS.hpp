#ifndef APM_LTC2695IMS_H
#define APM_LTC2695IMS_H


/**
 * Class representing the LTC2695IMS#PBF Driver interface
 * Datasheet can be found here:
 * @link https://www.rlocman.ru/i/File/2017/10/25/2965fc.pdf
 */
namespace APM::DEV {
    namespace IO = EVT::core::IO;

    class LTC2695IMS {
    public:

        /**
         * LTC2695IMS constructor
         *
         * @param can - CAN bus to use
         */
        explicit LTC2965IMS(IO::CAN& can);

        void highComparatorRefIn();

        void lowComparatorRefIn();

        void comparatorRefOut();

        void polaritySelect();

        void referenceOutput();

        void rangeSelectInput();

        void vIn();





    private:




    };


}

#endif //APM_LTC2695IMS_H
