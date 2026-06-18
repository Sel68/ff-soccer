#ifndef LOGGER_PC_H
#define LOGGER_PC_H

#ifdef BUILD_ON_STM
#else
/******************** FOR PC and PI *****************/
#include <iostream>

#define LogStream std::ostream

namespace debug {
// Simple wrapper that returns std::cout
inline std::ostream& Log() {
  return std::cout;
}

}  // namespace debug

#endif // NOT BUILD_ON_STM

#endif // LOGGER_PC_H