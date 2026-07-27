#include "MotorConfig.h"

uint8_t MotorComm::temp_receive_buffer[MotorComm::temp_buffer_size];

const char MotorComm::registration_msg[] = "Register motor\r\n";