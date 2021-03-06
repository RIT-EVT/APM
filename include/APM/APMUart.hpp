/**
 * This file will contain commonly used UART methods useful for interfacing with
 * a debugger or operator
 */

#ifndef APM_APM_UART_H
#define APM_APM_UART_H

#include <EVT/io/UART.hpp>

namespace IO = EVT::core::IO;

namespace APM {

class APMUart {
public:
  /**
   * Creates the APMUart class
   * @param apmUart pointer to the Uart device used for printing
   * @param apmDebugPrint boolean to determine whether or not debug statements
   * should be printed.
   */
  explicit APMUart(IO::UART *apmUart, bool apmDebugPrint = false);

  /**
   * Function to print the startup message to the user
   * @param uart the uart device to send the message over
   */
  void startupMessage() const;

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
  void printDebugString(const char *message) const;

  /**
   * Function to always print a message using the UART device
   * @param message the message to print
   */
  void printString(const char *message) const;

  /**
   * Passthrough for apmUart->getc()
   *
   * @return The character read in over UART.
   */
  [[nodiscard]] char getc() const;

  /**
   * Passthrough for apmUart->gets()
   *
   * @param buf The character array to fill
   * @param size The maximum number of characters to read.
   *
   * @return The buf pointer on success, NULL otherwise
   */
  char *gets(char *buf, size_t size) const;

private:
  static constexpr char MINICOM_CLEAR_DISPLAY[5] = "\x1B\x5B\x32\x4A";

  // Pointer to the APMManager UART device
  IO::UART *apmUart = nullptr;

  // Boolean determining whether or not debug statements will be printed to the
  // user
  bool apmDebugPrint;
};

} // namespace APM

#endif // APM_APM_UART_H
