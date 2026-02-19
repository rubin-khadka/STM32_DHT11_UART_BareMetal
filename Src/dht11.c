/*
 * dht11.c
 *
 *  Created on: Feb 19, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "dht11.h"
#include "timer1.h"
#include "uart.h"


// Initialize DHT11 pin
void DHT11_Init(void) {
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

	// Configure GPIO PB0 for DHT11
	GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
	GPIOB->CRL |= GPIO_CRL_MODE0_0;

	// Set pin high initially
	DHT11_SET_HIGH();
}

int DHT11_Start(void)
{
    uint32_t timeout;

    USART1_SendString("Step 1: Sending start signal...\r\n");

    // Send start signal
    DHT11_OUTPUT();
    DHT11_SET_LOW();
    TIMER1_Delay_ms(20);
    DHT11_SET_HIGH();
    TIMER1_Delay_us(30);
    DHT11_INPUT();

    // ===== DEBUG: Check line state after releasing =====
    if(DHT11_READ()) {
        USART1_SendString("  Line is HIGH after release (good)\r\n");
    } else {
        USART1_SendString("  Line is LOW after release (bad - pull-up?)\r\n");
    }

    // Step 1: Wait for sensor to pull LOW
    USART1_SendString("Step 2: Waiting for sensor to pull LOW...\r\n");
    timeout = 200;
    while(DHT11_READ())
    {
        if(--timeout == 0) {
            USART1_SendString("  TIMEOUT at step 1 - never went LOW\r\n");
            return 0;
        }
    }
    USART1_SendString("  ✓ Got LOW\r\n");

    // Step 2: Wait for LOW to end (sensor pulls HIGH)
    USART1_SendString("Step 3: Waiting for sensor to pull HIGH...\r\n");
    timeout = 200;
    while(!DHT11_READ())
    {
        if(--timeout == 0) {
            USART1_SendString("  TIMEOUT at step 2 - never went HIGH\r\n");
            return 0;
        }
    }
    USART1_SendString("  ✓ Got HIGH\r\n");

    // Step 3: Wait for HIGH to end (sensor pulls LOW again)
    USART1_SendString("Step 4: Waiting for final LOW...\r\n");
    timeout = 200;
    while(DHT11_READ())
    {
        if(--timeout == 0) {
            USART1_SendString("  TIMEOUT at step 3 - stuck HIGH\r\n");
            return 0;
        }
    }
    USART1_SendString("  ✓ Got final LOW\r\n");

    USART1_SendString("✓ Sensor responded successfully!\r\n");
    return 1;
}

void Debug_CheckRegisters(void)
{
    USART1_SendString("\r\n=== REGISTER DEBUG ===\r\n");

    // Check GPIOB clock
    if(RCC->APB2ENR & RCC_APB2ENR_IOPBEN)
        USART1_SendString("✓ GPIOB clock: ENABLED\r\n");
    else
        USART1_SendString("✗ GPIOB clock: DISABLED\r\n");

    // Check CRL register (PB0 configuration)
    uint32_t crl = GPIOB->CRL;
    uint32_t pb0_config = (crl >> 0) & 0xF;  // Get bits 0-3

    USART1_SendString("PB0 config bits: ");
    if(pb0_config == 0x1)
        USART1_SendString("0x1 (Output 10MHz, push-pull)\r\n");
    else if(pb0_config == 0x4)
        USART1_SendString("0x4 (Input floating)\r\n");
    else if(pb0_config == 0x0)
        USART1_SendString("0x0 (Input analog)\r\n");
    else
    {
        USART1_SendString("0x");
        // Send hex value - you'll need a hex print function
    }

    // Read IDR directly
    uint32_t idr = GPIOB->IDR;
    if(idr & 1)
        USART1_SendString("✓ PB0 current state: HIGH (1)\r\n");
    else
        USART1_SendString("✗ PB0 current state: LOW (0)\r\n");

    USART1_SendString("=== DEBUG END ===\r\n\r\n");
}
