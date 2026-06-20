// Uart.h
#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stddef.h>  // size_t

#define ENABLE_UART_PRINT 1  // Comment to disable

#ifdef ENABLE_UART_PRINT
#define UART_PRINT(data) uart_print(data)
#define UART_PRINTF(format, ...) uart_printf(format, ##__VA_ARGS__)
#else
#define UART_PRINT(data) ((void)0)
#define UART_PRINTF(format, ...) ((void)0)
#endif

#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;36m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0m"
#define COLOR_YELLOW "\033[33m"

typedef void (*HALUartTransmitFunctionPtr)(const uint8_t* data, uint16_t length);
void RegisterUartTransmitFunction(HALUartTransmitFunctionPtr function_name);

void uart_print(const char* data);
void uart_printf(const char* format, ...);

#endif  // UART_H