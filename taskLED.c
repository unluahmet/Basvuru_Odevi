#include "taskLED.h"

uint32_t led_onTime = 300;
uint32_t led_offTime = 700;
LED_TASK_STATE led_taskState = LED_TASK_STATE_ON;

void led_init()
{
	// GPIOG
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOGEN);
	
	// LED - G14
	GPIOG->MODER |= GPIO_MODER_MODE14_0;
	GPIOG->PUPDR |= GPIO_PUPDR_PUPD14_0;
	
	// Initial State
	led_off();
}

void led_setState(LED_TASK_STATE newState)
{
	led_taskState = newState;
}

void led_setOnTime(uint32_t newOnTime) { led_onTime = newOnTime; }
void led_setOffTime(uint32_t newOffTime) { led_offTime = newOffTime; }

void led_on() { GPIOG->BSRR |= (1UL << 14U); }
void led_off() { GPIOG->BSRR |= (1UL << (14U + 16U)); }
void led_toggle() { GPIOG->ODR ^= (1UL << 14U); }


void led_task(void)
{
	static uint32_t lastCall = 0;
	static uint32_t waitTime = 0;
	if (system_getTick()-lastCall < waitTime) { return; } 
	
	// Set last call tick
	lastCall = system_getTick();
	
	// State
	switch (led_taskState)
	{
		case LED_TASK_STATE_STOP:
		{
			// 1000 ms on, 1000 ms off
			waitTime = 1000;
			led_toggle();
			break;
		}
		
		case LED_TASK_STATE_ON:
		{
			// wait led_onTime
			led_on();
			waitTime = led_onTime;
			led_taskState = LED_TASK_STATE_OFF;
			break;
		}
		
		case LED_TASK_STATE_OFF:
		{
			// wait led_offTime
			led_off();
			waitTime = led_offTime;
			led_taskState = LED_TASK_STATE_ON;
			break;
		}
		
		default:
		{
			led_taskState = LED_TASK_STATE_ON;
		}
	}
}