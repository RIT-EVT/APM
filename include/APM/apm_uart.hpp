/**
 * This file will contain commonly used UART methods useful for interfacing with a debugger
 * or operator
 */

#ifndef APM_APM_UART_H
#define APM_APM_UART_H

#include <EVT/io/UART.hpp>

namespace IO = EVT::core::IO;

namespace APM {

/**
 * Method to print the startup message to the user
 * @param uart the uart device to send the message over
 */
void startup_message(IO::UART &uart);

/**
 * Prompt to the user while bike is in standby state waiting for ON Switch to start
 * @param uart reference to the IO::UART object for interfacing with the user
 * @return 0 on success, 1 if a failure has occurred
 */
int accessory_prompt(const IO::UART &uart);

} // namespace APM

#endif //APM_APM_UART_H
