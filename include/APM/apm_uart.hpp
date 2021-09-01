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
 * Function to print the startup message to the user
 * @param uart the uart device to send the message over
 */
void startupMessage();

/**
 * Function to set the reference used by this device for future uart print
 * statements.
 * @param uart Reference to the IO::UART object
 */
void setUART(IO::UART *uart);

/**
 * Set the boolean variable to determine whether or not debug
 * statements will be print to the UART device.
 * @param debugPrint True if messages should be printed.  False if not.
 */
void setDebugPrint(bool debugPrint);

/**
 * Function to conditionally print a debug string
 * @param message The string to print
 */
void printDebugString(const char* message);

} // namespace APM

#endif //APM_APM_UART_H
