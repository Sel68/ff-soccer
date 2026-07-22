#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "Manipulators.h"
#include "LoggerPC.h"

#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0m"

#ifdef LOG_NO_OP  // from cmake
#include "LoggerNoOp.h"
#endif

/**************************** THIS REDIRECTS THE OUTPUT TO UART ****************************/
// If LOGGER NO_OP is used, then this will not be used
#ifdef BUILD_ON_STM
#ifndef LOGGER_NO_OP_H

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <cstring>
#include <stdarg.h>  // va_list, va_start, ...

#include "Uart.h"

namespace debug {

class fixed_manip;
class setprecision_manip;
class setw_manip;
class dec_manip;
class hex_manip;

// Add this buffer for RAM-based logging
class UartBuffer {
 private:
  static constexpr size_t BUFFER_SIZE = 256;
  static char buffer[BUFFER_SIZE];
  static size_t position;

 public:
  UartBuffer() {}

  ~UartBuffer() {}

  static void flush() {
    if (position > 0) {
      UART_PRINT(buffer);
      position = 0;
      buffer[0] = '\0';
    }
  }

  static void append(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int remaining = BUFFER_SIZE - position - 1;  // -1 for null terminator
    if (remaining > 0) {
      int written = vsnprintf(buffer + position, remaining, format, args);
      if (written > 0) {
        position += (written < remaining) ? written : remaining;
      }
    }
    va_end(args);
  }

  static void clear() {
    position = 0;
    buffer[0] = '\0';
  }
};

class UartStream {
 public:
  ~UartStream() {}

  // UartStream(const UartStream&) = delete;
  // UartStream& operator=(const UartStream&) = delete;

  UartStream& operator<<(const fixed_manip&) { return *this; }
  UartStream& operator<<(const setprecision_manip&) { return *this; }
  UartStream& operator<<(const setw_manip&) { return *this; }
  UartStream& operator<<(const dec_manip&) { return *this; }
  UartStream& operator<<(const hex_manip&) { return *this; }

  // Data types - modified to use buffer
  UartStream& operator<<(const char* str) {
    UartBuffer::append("%s", str);
    // Only flush if string contains newline
    if (strchr(str, '\n')) {
      UartBuffer::flush();
    }
    return *this;
  }

  UartStream& operator<<(char c) {
    UartBuffer::append("%c", c);
    if (c == '\n') {
      UartBuffer::flush();
    }
    return *this;
  }

  UartStream& operator<<(int val) {
    UartBuffer::append("%d", val);
    UartBuffer::flush();
    return *this;
  }

  UartStream& operator<<(unsigned val) {
    UartBuffer::append("%u", val);
    return *this;
  }

  UartStream& operator<<(long val) {
    UartBuffer::append("%ld", val);
    return *this;
  }

  UartStream& operator<<(unsigned long val) {
    UartBuffer::append("%lu", val);
    return *this;
  }

  // Convert to integer representation (comment out the return to see everywhere its being called)
  debug::UartStream& operator<<(float val) {
    int whole = (int)val;
    int frac = (int)((val - whole) * 1000);  // 3 decimal places
    if (val < 0) {
      UartBuffer::append("-%d.%03d", -whole, abs(frac));
    } else {
      UartBuffer::append("%d.%03d", whole, frac);
    }
    return *this;
  }

  debug::UartStream& operator<<(double val) {
    int whole = (int)val;
    int frac = (int)((val - whole) * 1000);  // 3 decimal places
    if (val < 0) {
      UartBuffer::append("-%d.%03d", -whole, abs(frac));
    } else {
      UartBuffer::append("%d.%03d", whole, frac);
    }
    return *this;
  }

  // Don't want to support in order to avoid using doubles!
  // UartStream& operator<<(double val) {
  //   buffer.append("%g", val);
  //   UartBuffer::flush();
  //   return *this;
  // }

  UartStream& operator<<(const std::string& str) {
    UartBuffer::append("%s", str.c_str());
    // Only flush if string contains newline
    if (str.find('\n') != std::string::npos) {
      UartBuffer::flush();
    }
    return *this;
  }
};

extern UartStream uart_cout;
UartStream& Log();
}  // namespace debug

#define LogStream debug::UartStream

#endif  // BUILD_ON_STM
#endif  // NOT LOGGER_NO_OP_H
#endif  // LOGGER_H