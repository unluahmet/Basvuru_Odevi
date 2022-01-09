#ifndef TASK_LED_H
#define TASK_LED_H

#include "main.h"

typedef enum {
	LED_TASK_STATE_STOP = 0,
	LED_TASK_STATE_ON = 1,
	LED_TASK_STATE_OFF = 2
} LED_TASK_STATE;

void led_init(void);

void led_on(void);
void led_off(void);
void led_toggle(void);

void led_task(void);

#endif