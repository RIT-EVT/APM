/**
 * This is the CANopen node interface for the APM
 */

#ifndef APM_APMCANNODE_H
#define APM_APMCANNODE_H

#include <Canopen/co_obj.h>
#include <Canopen/co_core.h>
#include <EVT/utils/types/FixedQueue.hpp>
#include <EVT/io/CANopen.hpp>
#include <EVT/io/types/CANMessage.hpp>

namespace IO = EVT::core::IO;

namespace APM {

class APMCanNode {
public:
    APMCanNode();

    /**
     * Get a pointer to the start of the object dictionary
     *
     * @return Pointer to the start of the object dictionary
     */
    CO_OBJ_T *getObjectDictionary();

    /**
     * Get the number of elements in the object dictionary.
     *
     * @return The number of elements in the object dictionary
     */
    uint8_t getNumElements();

private:
    /**
     * The node ID used to identify the device on the CAN network.
     */
    static constexpr uint8_t NODE_ID = 0x01;

    /**
     * Have to know the size of the object dictionary for initialization
     * process.
     */
    static constexpr uint8_t OBJECT_DIRECTIONARY_SIZE = 16;

    /**
     * Message queue to hold the CAN messages
     */
    EVT::core::types::FixedQueue<CANOPEN_QUEUE_SIZE, IO::CANMessage> canMessageQueue;

    /**
     * The object dictionary itself. Will be populated by this object during
     * construction.
     *
     * The plus one is for the special "end of dictionary" marker.
     */
    CO_OBJ_T objectDictionary[OBJECT_DIRECTIONARY_SIZE + 1] = {
            // Sync ID, defaults to 0x80
            {CO_KEY(0x1005, 0, CO_UNSIGNED32 | CO_OBJ_D__R_), 0, (uintptr_t) 0x80},

            // Information about the hardware, hard coded sample values for now
            // 1: Vendor ID
            // 2: Product Code
            // 3: Revision Number
            // 4: Serial Number
            {
                    .Key = CO_KEY(0x1018, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x10},
            {.Key = CO_KEY(0x1018, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x11},
            {.Key = CO_KEY(0x1018, 3, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x12},
            {.Key = CO_KEY(0x1018, 4, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x13},

            // SDO CAN message IDS.
            // 1: Client -> Server ID, default is 0x600 + NODE_ID
            // 2: Server -> Client ID, default is 0x580 + NODE_ID
            {
                    .Key = CO_KEY(0x1200, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x600 + NODE_ID},
            {.Key = CO_KEY(0x1200, 2, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0x580 + NODE_ID},

            // TPDO0 settings
            // 0: The TPDO number, default 0
            // 1: The COB-ID used by TPDO0, provided as a function of the TPDO number
            // 2: How the TPO is triggered, default to manual triggering
            // 3: Inhibit time, defaults to 0
            // 5: Timer trigger time in 1ms units, 0 will disable the timer based triggering
            {
                    .Key = CO_KEY(0x1800, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0},
            {.Key = CO_KEY(0x1800, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) CO_COBID_TPDO_DEFAULT(0)},
            {.Key = CO_KEY(0x1800, 2, CO_UNSIGNED8 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0xFE},
            {.Key = CO_KEY(0x1800, 3, CO_UNSIGNED16 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 0},
            {.Key = CO_KEY(0x1800, 5, CO_UNSIGNED16 | CO_OBJ_D__R_),
                    .Type = CO_TEVENT,
                    .Data = (uintptr_t) 2000},

            // TPDO0 mapping, determins the PDO messages to send when TPDO1 is triggered
            // 0: The number of PDO message associated with the TPDO
            // 1: Link to the first PDO message
            // n: Link to the nth PDO message
            {
                    .Key = CO_KEY(0x1A00, 0, CO_UNSIGNED8 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = (uintptr_t) 1},
            {
                    .Key = CO_KEY(0x1A00, 1, CO_UNSIGNED32 | CO_OBJ_D__R_),
                    .Type = 0,
                    .Data = CO_LINK(0x2100, 0, 8)// Link to sample data position in dictionary
            },

            // End of dictionary marker
            CO_OBJ_DIR_ENDMARK};
};

} // namespace APM

#endif //APM_APMCANNODE_H
