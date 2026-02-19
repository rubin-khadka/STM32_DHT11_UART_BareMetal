/*
 * dht11.h
 *
 *  Created on: Feb 19, 2026
 *      Author: Rubin Khadka
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "stm32f103xb.h"

// Pin control macros
#define DHT11_OUTPUT() \
    do { \
        GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); \
        GPIOB->CRL |= GPIO_CRL_MODE0_0; \
    } while(0)

#define DHT11_INPUT() \
    do { \
        GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); \
        GPIOB->CRL |= GPIO_CRL_CNF0_0; \
    } while(0)

#define DHT11_SET_HIGH()    (GPIOB->BSRR |= GPIO_BSRR_BS0)
#define DHT11_SET_LOW()     (GPIOB->BRR |= GPIO_BRR_BR0)
#define DHT11_READ()        ((GPIOB->IDR >> 0) & 1)

// Structure for return status codes
typedef enum {
	DHT11_OK = 0,
	DHT11_ERROR_NO_RESPONSE,
	DHT11_ERROR_CHECKSUM,
	DHT11_ERROR_TIMEOUT
} DHT11_Status_t;

// Data Structure for sensor readings
typedef struct {
	int16_t temperature;
	int16_t humidity;
	uint8_t checksum;
} DHT11_Data_t;

// Function Prototypes
void DHT11_Init(void);
int DHT11_Start(void);
void Debug_CheckRegisters(void);

#endif /* DHT11_H_ */
