#include "taskUART.h"
#include "taskLED.h"
#include "buffer.h"

#define UART_IS_STRING_EQUAL(str1, str2) (strstr(str1, str2) != 0)
#define UART_DATA_TERMINATOR '\r'
#define ECHO_MAX_BUFFER_SIZE 32
#define UART_MAX_DATA 16
#define UART_MAX_INDEX_VAL (UART_MAX_DATA - 1)

// Buffer
Buffer_t ECHO_Buffer;
volatile int8_t ECHO_Buffer_Array[ECHO_MAX_BUFFER_SIZE];

// Transmit Data
bool uart_transmit_ready = true;

// Data
bool uart_data_ready = false;
char uart_data[UART_MAX_DATA] = {0};
int uart_data_index = 0;

// State
UART_TASK_STATE uart_taskState = UART_TASK_STATE_INIT;

// ISR
volatile char uartRxChar;

void USART1_IRQHandler(void)
{
	if ( (USART1->SR & USART_SR_TC) == USART_SR_TC )
	{
		// Clear flag
		USART1->SR &= ~USART_SR_TC;
		
		// Transmit ready
		uart_transmit_ready = true;
	}
	
	if( (USART1->SR & USART_SR_RXNE) == USART_SR_RXNE )
	{
		// Flag
		USART1->SR &= ~USART_SR_RXNE;
		
		// Read char
		uartRxChar = USART1->DR;
		
		// Add char to echo buffer
		Buffer_Write(&ECHO_Buffer, (void*)&uartRxChar);
		
		// Add char to data array
		uart_data[uart_data_index++] = uartRxChar;
		if (uart_data_index >= UART_MAX_INDEX_VAL) { uart_data_index = 0; }
		
		// Check data
		if (uartRxChar == UART_DATA_TERMINATOR) 
		{
			uart_data_ready = true;
			uart_data[uart_data_index] = '\0';
		}
		
		// Parse data array
		if (uart_data_ready)
		{
			int parsed_int_data = 0;
			
			// Reset flag and index
			uart_data_index = 0; 
			uart_data_ready = false;
			
			// Uart'tan "stop" stringi gönderdigimizde echo taski sonlanacak ve led 1sn araliklarla yanip sönecek.
			if (UART_IS_STRING_EQUAL(uart_data, "stop")) 
			{
				uart_setState(UART_TASK_STATE_ECHO_OFF);
				led_setState(LED_TASK_STATE_STOP);
			}
			// Uart'tan "start" stringi gönderdigimizde echo taski yeniden baslayacak ve led bir önceki ayarlarla çalismaya devam edecek.
			else if (UART_IS_STRING_EQUAL(uart_data, "start")) 
			{
				uart_setState(UART_TASK_STATE_ECHO_ON);
				led_setState(LED_TASK_STATE_ON);
			}
			// Uart'tan "ledon=500" yazarsak led on süresi 500 ms olacak.
			else if (sscanf(uart_data, "ledon=%d", &parsed_int_data)) 
			{
				led_setOnTime(parsed_int_data);
			}
			// Uart'tan "ledoff=500" yazarsak led off süresi 500 ms olacak.
			else if (sscanf(uart_data, "ledoff=%d", &parsed_int_data))  
			{
				led_setOffTime(parsed_int_data);
			}
		}
	}
}

void uart_init(uint32_t baudrate, UART_DATA_BITS databits, UART_PARITY parity, UART_STOP_BITS stopbits)
{
	// Buffer init
	Buffer_InitStatic(&ECHO_Buffer, ECHO_Buffer_Array, ECHO_MAX_BUFFER_SIZE, ECHO_MAX_BUFFER_SIZE, sizeof(char));
	
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
	
	// Receive and transmit ISR
	USART1->CR1 |= (USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_TCIE | USART_CR1_TE);
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

void uart_setState(UART_TASK_STATE newState)
{
	uart_taskState = newState;
}

void uart_task(void)
{
	switch(uart_taskState)
	{
		// Default state: 
		case UART_TASK_STATE_INIT:
		{
			// Uart init
			uart_init(115200, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
			
			// Start echo
			uart_taskState = UART_TASK_STATE_ECHO_ON;
			
			break;
		}
		
		case UART_TASK_STATE_ECHO_ON:
		{
			if (!Buffer_IsEmpty(&ECHO_Buffer) && uart_transmit_ready)
			{
				char c;
				if (Buffer_Read(&ECHO_Buffer, &c))
				{
					USART1->DR = c;
					uart_transmit_ready = false;
				}
			}
			
			break;
		}
		
		case UART_TASK_STATE_ECHO_OFF:
		{
			Buffer_Reset(&ECHO_Buffer);
			break;
		}
		
		default: 
		{
			uart_taskState = UART_TASK_STATE_INIT;
			break; 
		}
	}
}