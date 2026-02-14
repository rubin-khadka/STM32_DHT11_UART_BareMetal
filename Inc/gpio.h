/*
 * gpio.h
 *
 *  Created on: Feb 12, 2026
 *      Author: Rubin Khadka

 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);
void LED_Toggle(void);
void Delay_ms(uint32_t ms);

#endif /* GPIO_H_ */
