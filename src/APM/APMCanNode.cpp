/**
 * Implementation of APMCanNode.hpp
 */

#include <APM/APMCanNode.hpp>
#include "EVT/io/CAN.hpp"
#include "EVT/utils/types/FixedQueue.hpp"
#include "EVT/io/CANopen.hpp"

namespace IO = EVT::core::IO;

/**
 * Interrupt handler to get CAN messages. A function pointer to this function
 * will be passed to the EVT-core CAN interface which will in turn call this
 * function each time a new CAN message comes in.
 *
 * NOTE: For this sample, every non-extended (so 11 bit CAN IDs) will be
 * assummed to be intended to be passed as a CANopen message.
 *
 * @param message[in] The passed in CAN message that was read.
 */
void canInterrupt(IO::CANMessage& message, void* priv) {
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>* queue =
            (EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage>*) priv;
    if (queue != nullptr)
        queue->append(message);
}

namespace APM {

APMCanNode::APMCanNode() {

}

CO_OBJ_T *APMCanNode::getObjectDictionary() {
    return &objectDictionary[0];
}

uint8_t APMCanNode::getNumElements() {
    return OBJECT_DIRECTIONARY_SIZE;
}
} // APM

