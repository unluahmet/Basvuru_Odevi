#include "main.h"

volatile uint32_t SysTickVar;

void SysTick_Handler(void)
{
	SysTickVar++;
}

void system_init()
{
	// 168 MHZ
	RCC->CR |= RCC_CR_HSEON;
	while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);
	RCC->PLLCFGR =  336U<<RCC_PLLCFGR_PLLN_Pos|8U<<RCC_PLLCFGR_PLLM_Pos|
					(2U/2U-1U)<<RCC_PLLCFGR_PLLP_Pos|2U<<RCC_PLLCFGR_PLLQ_Pos|
					RCC_PLLCFGR_PLLSRC_HSE;
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);
	FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
	RCC->CFGR = RCC_CFGR_SW_PLL | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
	
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000U);
	NVIC_SetPriority(SysTick_IRQn, 15);
}