#include "main.h"
#include "taskUART.h"
#include "taskLED.h"

int main(void)
{
	// Clock
	system_init();
	
	// Led
	led_init();
	
	// Loop
	while (1)
	{
		uart_task();
		led_task();
	}
}