/******************************** THIS SIMPLY DISCARDS THE OUTPUT ************************ */
#ifndef LOGGER_NO_OP_H
#define LOGGER_NO_OP_H

#ifdef BUILD_ON_STM

namespace debug {

class Logger {
 public:
  // Get singleton instance
  static Logger& instance() {
    static Logger instance;
    return instance;
  }

  // Direct logging methods (no-op)
  void log(const char*) {}

  // Stream-like interface that does nothing
  class LogStream {
   public:
    LogStream(Logger&) {}
    ~LogStream() {}

    // Template for handling any type - discards everything
    template <typename T>
    LogStream& operator<<(const T&) {
      return *this;
    }

    // Handle manipulators - also does nothing
    LogStream& operator<<(void* (*)(void*)) { return *this; }
  };

  // Create a new LogStream
  LogStream stream() { return LogStream(*this); }

 private:
  Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
};

// Convenience global function
inline Logger::LogStream Log() { return Logger::instance().stream(); }

}  // namespace debug

#include <ostream>
#define LogStream std::ostream

#endif // BUILD_ON_STM

#endif // LOGGER_NO_OP_H