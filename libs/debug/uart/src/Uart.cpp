#include "Uart.h"

#include <stdarg.h>  // va_list, va_start, ...
#include <stdio.h>
#include <string.h>  // strlen

#if (USE_SMALL_BUFFER == 1)
#define BUFFER_SIZE 60
#warning BUFFER SIZE IS 60
#else
#define BUFFER_SIZE 256
#warning BUFFER SIZE IS 256
#endif

char msg[BUFFER_SIZE];

static HALUartTransmitFunctionPtr HALUartTransmitFunction = NULL;

void RegisterUartTransmitFunction(HALUartTransmitFunctionPtr function_name) {
  HALUartTransmitFunction = function_name;
}

// ***************************************************
/*                  Debugging                */
void uart_print(const char* data) {
  if (HALUartTransmitFunction) HALUartTransmitFunction((const uint8_t*)data, strlen(data));
}

void uart_printf(const char* format, ...) {
  va_list args;
  va_start(args, format);

  int length = vsnprintf(msg, BUFFER_SIZE, format, args);
  va_end(args);

  if (length >= BUFFER_SIZE) {
    msg[BUFFER_SIZE - 1] = '\0';
  }

  if (HALUartTransmitFunction) HALUartTransmitFunction((const uint8_t*)msg, length);
}