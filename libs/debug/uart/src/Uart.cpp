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

/*
  Notes:
    * On the worst MCU (STMF0 - Our Motor), It takes around 50us for CPU to finish
      executing a single uart_printf and assign it to the dma. It means, if it does another
  uart_printf, we will be overwriting the original message. On this MCU, it takes around 250us for
  this message to physically transferred So, if we are adding new message every 50us, then we
  should have a queue size of 5. Let's make it 6
*/
#define QUEUE_SIZE 9
static char msg[QUEUE_SIZE][BUFFER_SIZE];
static volatile uint8_t msg_index = 0;

static HALUartTransmitFunctionPtr HALUartTransmitFunction = NULL;

void RegisterUartTransmitFunction(HALUartTransmitFunctionPtr function_name) {
  HALUartTransmitFunction = function_name;
}

// ***************************************************
/*                  Debugging                */
void uart_print(const char* data) {
  if (HALUartTransmitFunction) HALUartTransmitFunction((const uint8_t*)data, strlen(data));
}

void uart_print(const uint8_t* data, uint16_t len) {
  if (HALUartTransmitFunction) HALUartTransmitFunction(data, len);
}

void uart_printf(const char* format, ...) {
  char* buffer = msg[msg_index];
  ++msg_index;
  if (msg_index >= QUEUE_SIZE) msg_index = 0;

  va_list args;
  va_start(args, format);

  int length = vsnprintf(buffer, BUFFER_SIZE, format, args);
  va_end(args);

  if (length >= BUFFER_SIZE) length = BUFFER_SIZE - 1;

  if (length < 0) {
    if (msg_index == 0)
      msg_index = QUEUE_SIZE - 1;
    else
      --msg_index;
    return;
  }

  if (HALUartTransmitFunction)
    HALUartTransmitFunction(reinterpret_cast<const uint8_t*>(buffer), length);
}