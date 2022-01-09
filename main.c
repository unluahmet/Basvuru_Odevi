#include "main.h"
#include "taskUART.h"
#include "taskLED.h"

int main(void)
{
	// Clock
	system_init();
	
	// Uart
	uart_init(115200, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
	
	// Led
	led_init();
	
	// Loop
	while (1)
	{
		uart_task();
		led_task();
	}
}