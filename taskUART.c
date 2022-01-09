#include "taskUART.h"

// ISR
volatile char uartRxChar = 0;

void USART1_IRQHandler(void)
{
	if( (USART1->SR & USART_SR_RXNE) == USART_SR_RXNE )
	{
		// Flag
		USART1->SR &= ~USART_SR_RXNE;
		
		// Read char
		uartRxChar = USART1->DR;
		
		// Add char to buffer
		
	}
}

void uart_init(uint32_t baudrate, UART_DATA_BITS databits, UART_PARITY parity, UART_STOP_BITS stopbits)
{
	// UART
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// Baud
	float USARTDIV = (float)((SystemCoreClock / 2) / (8.0 * (2 - ((USART1->CR1 & USART_CR1_OVER8) ? 1U : 0U)) * baudrate));
	uint8_t DIV_Mantissa = (uint32_t)USARTDIV;
	uint8_t DIV_Fraction = (uint32_t)roundf(16.0 * (USARTDIV - DIV_Mantissa));
	if(DIV_Fraction > 15)
	{
		DIV_Fraction = 0;
		DIV_Mantissa += 1;
	}
	USART1->BRR = (DIV_Mantissa << 4 | DIV_Fraction);
	
	// Stop bits
	USART1->CR2 &= ~(USART_CR2_STOP);
	USART1->CR2 |= (stopbits << USART_CR2_STOP_Pos);
	
	// Databits
	USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_OVER8);
	USART1->CR1 |= (databits << USART_CR1_M_Pos);
	USART1->CR1 |= (parity << USART_CR1_PS_Pos);
	
	// Receive and ISR
	USART1->CR1 |= (USART_CR1_RE | USART_CR1_RXNEIE);
	USART1->CR3 &= ~(USART_CR3_DMAR);
	
	// UART GPIO A9-A10
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER  |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
	GPIOA->MODER  &= ~(GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD9_0 | GPIO_PUPDR_PUPD10_0);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10);
	GPIOA->AFR[1] |= ((7U << GPIO_AFRH_AFSEL9_Pos) | (7U << GPIO_AFRH_AFSEL10_Pos));
	
	// ISR Priority
	NVIC_SetPriorityGrouping(3);
	NVIC_SetPriority(USART1_IRQn, 5);
	NVIC_EnableIRQ(USART1_IRQn);
	
	// Enable UART
	USART1->CR1 |= (USART_CR1_UE);
}

void uart_task(void)
{
	
}