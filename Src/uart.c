/*
 * uart.c
 *
 *  Created on: Feb 14, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"

void USART1_Init(void)
{
	// Enable USART1 peripheral
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// GPIO configuration for USART TX PA9
	GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);	// Clear both CNF and MODE
	GPIOA->CRH |= GPIO_CRH_CNF9_1;	// CNF = 10 (Alternate function push-pull)
	GPIOA->CRH |= GPIO_CRH_MODE9;	// MODE = 11 (50 MHz speed)

	// GPIO configuration for USART RX PA10
	GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);	// Clear both CNF and MODE
	GPIOA->CRH |= GPIO_CRH_CNF10_0;	// CNF = 01 (Floating input)

	// Set Baud Rate
	USART1->BRR = 0x271;

	// Disable USART1 temporarily
	USART1->CR1 &= ~USART_CR1_UE;

	// Clear any pending flags
	USART1->SR = 0;

	// Enable Interrupts
	NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= USART_CR1_RXNEIE;	// Enable RX Not Empty Interrupt
	USART1->CR1 |= USART_CR1_RE;		// Enable Receiver
	USART1->CR1 |= USART_CR1_TE;		// Enable Transmitter

	// Re-enable USART1
	USART1->CR1 |= USART_CR1_UE;
}



