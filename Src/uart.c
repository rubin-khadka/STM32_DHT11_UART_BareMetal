/*
 * uart.c
 *
 *  Created on: Feb 14, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "uart.h"

// Define buffer size
#define USART1_RX_BUF_SIZE 256  // RX buffer size
#define USART1_TX_BUF_SIZE 256  // TX buffer size

// Global buffer instances for USART1
static uint8_t usart1_rxbuf_storage[USART1_RX_BUF_SIZE];
static uint8_t usart1_txbuf_storage[USART1_TX_BUF_SIZE];
volatile UART_Buffer_t usart1_rx_buf;
volatile UART_Buffer_t usart1_tx_buf;

void USART1_Init(void) {

	// Enable clock for GPIOA and alternate function
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;

	// Enable USART1 peripheral
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// GPIO configuration for USART TX PA9
	GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);// Clear both CNF and MODE
	GPIOA->CRH |= GPIO_CRH_CNF9_1;	// CNF = 10 (Alternate function push-pull)
	GPIOA->CRH |= GPIO_CRH_MODE9;	// MODE = 11 (50 MHz speed)

	// GPIO configuration for USART RX PA10
	GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);// Clear both CNF and MODE
	GPIOA->CRH |= GPIO_CRH_CNF10_0;	// CNF = 01 (Floating input)

		// Disable USART1 temporarily
	USART1->CR1 &= ~USART_CR1_UE;

	// Set Baud Rate
	USART1->BRR = 0x271; // 115200 baud rate

	// Clear any pending flags
	USART1->SR = 0;

	// Enable Interrupts
	NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= USART_CR1_RXNEIE;	// Enable RX Not Empty Interrupt
	USART1->CR1 |= USART_CR1_RE;		// Enable Receiver
	USART1->CR1 |= USART_CR1_TE;		// Enable Transmitter

	// Re-enable USART1
	USART1->CR1 |= USART_CR1_UE;

	USART1_BufferInit(&usart1_rx_buf, usart1_rxbuf_storage, USART1_RX_BUF_SIZE);
	USART1_BufferInit(&usart1_tx_buf, usart1_txbuf_storage, USART1_TX_BUF_SIZE);
}

void USART1_BufferInit(volatile UART_Buffer_t *buff, uint8_t *storage,
		uint16_t size) {
	buff->buffer = storage;
	buff->size = size;
	buff->head = 0;
	buff->tail = 0;
	buff->count = 0;
	buff->overflow = false;
}

bool USART1_BufferFull(volatile UART_Buffer_t *buff) {
	return (buff->count >= buff->size);
}

bool USART1_BufferEmpty(volatile UART_Buffer_t *buff) {
	return (buff->count == 0);
}

bool USART1_BufferWrite(volatile UART_Buffer_t *buff, uint8_t data) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (USART1_BufferFull(buff)) {
		buff->overflow = true;
		__set_PRIMASK(primask);
		return false;
	}

	buff->buffer[buff->head] = data;
	buff->head = (buff->head + 1) % buff->size;
	buff->count++;

	__set_PRIMASK(primask);

	return true;
}

uint8_t USART1_BufferRead(volatile UART_Buffer_t *buff) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	if (USART1_BufferEmpty(buff)) {
		__set_PRIMASK(primask);
		return 0;
	}

	uint8_t data = buff->buffer[buff->tail];
	buff->tail = (buff->tail + 1) % buff->size;
	buff->count--;

	__set_PRIMASK(primask);
	return data;
}

// Interrupt Functions
void USART1_IRQHandler(void) {
	// Check if RXNE interrupt
	if (USART1->SR & USART_SR_RXNE) {
		uint8_t data = USART1->DR;
		USART1_BufferWrite(&usart1_rx_buf, data);
	}

	// TX interrupt - reads from TX buffer
	if ((USART1->CR1 & USART_CR1_TXEIE) && (USART1->SR & USART_SR_TXE)) {
		if (!USART1_BufferEmpty(&usart1_tx_buf)) {
			uint8_t data = USART1_BufferRead(&usart1_tx_buf); // READS from TX buffer
			USART1->DR = data;
		}
	}
}

