#ifndef MANIPULATORS_H
#define MANIPULATORS_H

/* Platform-independent manipulators */
#if defined(BUILD_ON_STM) && !defined(LOG_NO_OP)
namespace debug {
class fixed_manip {};
class hex_manip {};
class dec_manip {};

class setw_manip {
 public:
  explicit setw_manip(int w) : width(w) {}
  int width;
};

class setprecision_manip {
 public:
  explicit setprecision_manip(int p) : precision(p) {}
  int precision;
};

inline setprecision_manip setprecision(int p) { return setprecision_manip(p); }
inline setw_manip setw(int w) { return setw_manip(w); }

extern const fixed_manip fixed;
extern const dec_manip dec;
extern const hex_manip hex;
}  // namespace debug

inline auto stream_fixed() { return debug::fixed; }
inline auto stream_setprecision(int n) { return debug::setprecision(n); }
inline auto stream_dec() { return debug::dec; }
inline auto stream_hex() { return debug::hex; }
inline auto stream_setw(int n) { return debug::setw(n); }

#else  // FOR PC AND PI + STM WITHOUT NO_OP
#include <iomanip>
inline auto stream_fixed() { return std::fixed; }
inline auto stream_setprecision(int n) { return std::setprecision(n); }
inline auto stream_dec() { return std::dec; }
inline auto stream_hex() { return std::hex; }
inline auto stream_setw(int n) { return std::setw(n); }
#endif // BUILD_ON_STM

#endif  // MANIPULATORS_H