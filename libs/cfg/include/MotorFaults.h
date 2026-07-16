#ifndef MOTOR_FAULTS_H
#define MOTOR_FAULTS_H

#ifndef M_PI
#define M_PI 3.14159265
#endif

// ORDER THESE BY PRIORITY. WORST FAULTS FIRST
// FYI, WE CAN HAVE ONLY 31 DIFFERENT KINDS OF FAULTS

enum class MotorFault {
  NONE,
  BREAK_PIN_PULLED_ACTIVE,  // WORST
  SOMETHING_ELSE,
  I_HAVE_NO_IDEA,
  INVALID_COMMUTATION_STATE,
  ENTER_FAULT_FUNCTION,
  TOO_SLOW_CONTROL_STEP,
  ADC_PROBLEM
};

struct MotorConstants {
  // THINGS I CAN CHANGE
  static constexpr float max_duty = 95.f;
  static constexpr float t_duty_ramp_ms = 2000;  // Reaches max duty in this time

  // HARDWARE CONSTRAINTS
  static constexpr float wheel_radius_m = 0.01313f;
  static constexpr float v_limit = 7.f;
  static constexpr float a_limit = 4.f;

  // Velocity
  static constexpr float max_rpm = max_duty * 50;  // ROUGH CALCULATION
  static constexpr float w_max = max_rpm * 2 * M_PI / 60.f;
  static constexpr float v_max = wheel_radius_m * w_max;  // v = r * w
  static_assert(v_max <= v_limit && "Velocity exceeds limit");
  static_assert(max_duty <= 95.f && "Are you crazy?");

  // Acceleration
  static constexpr float duty_per_ms = max_duty / t_duty_ramp_ms;
  static constexpr float alpha_max = w_max / t_duty_ramp_ms;
  static constexpr float a_max = wheel_radius_m * alpha_max;
  static_assert(a_max <= a_limit && "Acc exceeds limit");
};

#endif  // MOTOR_FAULTS_H