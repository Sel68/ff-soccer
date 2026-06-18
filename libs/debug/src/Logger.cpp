#ifdef BUILD_ON_STM
#ifndef LOG_NO_OP

#include "Logger.h"
namespace debug {
// Define the global manipulator objects
const fixed_manip fixed = fixed_manip();
const dec_manip dec = dec_manip();
const hex_manip hex = hex_manip();

// Define the static members
char UartBuffer::buffer[256];
size_t UartBuffer::position = 0;

// Create the global instance
UartStream uart_cout;

UartStream& Log() {
  UartBuffer::flush();
  return uart_cout;
}

}  // namespace debug

#endif  // NOT LOG_NO_OP
#endif  // BUILD_ON_STM