#ifndef MOTOR_FAULTS_H
#define MOTOR_FAULTS_H

#include <cstdint>

static constexpr float m_pi = 3.14159;

// ORDER THESE BY PRIORITY. WORST FAULTS FIRST
// FYI, WE CAN HAVE ONLY 31 DIFFERENT KINDS OF FAULTS
// TODO: Encode/decode faults to have any combination of them

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
  static constexpr float max_duty = 60.f;
  static constexpr float t_duty_ramp_ms = 2000;  // Reaches max duty in this time

  // HARDWARE CONSTRAINTS
  static constexpr float wheel_radius_m = 0.01313f;
  static constexpr float v_limit = 7.f;
  static constexpr float a_limit = 4.f;

  // Velocity
  static constexpr float max_rpm = max_duty * 50;  // ROUGH CALCULATION
  static constexpr float w_max = max_rpm * 2 * m_pi / 60.f;
  static constexpr float v_max = wheel_radius_m * w_max;  // v = r * w
  static_assert(v_max <= v_limit && "Velocity exceeds limit");
  static_assert(max_duty <= 98.f && "Are you crazy?");

  // Acceleration
  static constexpr float duty_per_ms = max_duty / t_duty_ramp_ms;
  static constexpr float alpha_max = w_max / t_duty_ramp_ms;
  static constexpr float a_max = wheel_radius_m * alpha_max;
  static_assert(a_max <= a_limit && "Acc exceeds limit");
};

struct MotorComm {
  // UART COMMUNICATION BETWEEN MOTOR AND MAIN BOARD
  static constexpr uint8_t msg_size = 64;
  static constexpr uint8_t msg_mask = msg_size - 1u;
  static constexpr uint8_t temp_buffer_size = 128u;
  static uint8_t temp_receive_buffer[temp_buffer_size];
};

#endif  // MOTOR_FAULTS_H