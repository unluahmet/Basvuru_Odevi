#ifndef TASK_UART_H
#define TASK_UART_H

#include "main.h"

typedef enum {
	UART_STOP_BITS_1 = 0U,
	UART_STOP_BITS_0_5 = 1U,
	UART_STOP_BITS_2 = 2U,
	UART_STOP_BITS_1_5 = 3U
} UART_STOP_BITS;

typedef enum {
	UART_DATA_BITS_8 = 0U,
	UART_DATA_BITS_9 = 1U
} UART_DATA_BITS;

typedef enum {
	UART_PARITY_EVEN 	= 0b10,
	UART_PARITY_ODD 	= 0b11,
	UART_PARITY_NONE 	= 0b00,
} UART_PARITY;

void uart_init(uint32_t baudrate, UART_DATA_BITS databits, UART_PARITY parity, UART_STOP_BITS stopbits);
void uart_task(void);


#endif