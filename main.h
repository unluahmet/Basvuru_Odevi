#ifndef MAIN_H
#define MAIN_H

#include "stdio.h"
#include "stdarg.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"


#include "stm32f4xx.h"


#define BIT_SET(REG, BIT)					( REG |= BIT )
#define BIT_CLR(REG, BIT)					( REG &= ~BIT )
#define WAIT_IF_BITSET(REG, BIT)	while( (REG & BIT) == BIT )
#define WAIT_IF_BITCLR(REG, BIT)	while( (REG & BIT) != BIT )


void system_init();


#endif