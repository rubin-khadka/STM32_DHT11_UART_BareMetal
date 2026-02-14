/*
 * uart.h
 *
 *  Created on: Feb 14, 2026
 *      Author: Rubin Khadka
 */

#ifndef UART_H_
#define UART_H_

#include "stdint.h"
#include "stdbool.h"

// Circular Buffer Structure for USART1
typedef struct
{
  uint8_t          *buffer; 	// Pointer to buffer memory
  uint16_t          size;   	// Total buffer size
  volatile uint16_t head;   	// Write Position
  volatile uint16_t tail;   	// Read Position
  volatile uint16_t count;  	// Number of bytes in the buffer
  volatile bool     overflow;   // Overflow flag
} UART_Buffer_t;

// External declarations
extern volatile UART_Buffer_t usart1_rx_buf;
extern volatile UART_Buffer_t usart1_tx_buf;

// Function Definitions
void USART1_Init(void);

// Core buffer functions
void UART1_Buffer_Init(volatile UART_Buffer_t *buff, uint8_t *storage, uint16_t size);
bool UART1_Buffer_Full(volatile UART_Buffer_t *buff);
bool UART1_Buffer_Empty(volatile UART_Buffer_t *buff);
bool UART1_Buffer_Write(volatile UART_Buffer_t *buff, uint8_t data);
uint8_t UART1_Buffer_Read(volatile UART_Buffer_t *buff);

// Interrupt function
void USART1_IRQHandler(void);

#endif /* UART_H_ */
