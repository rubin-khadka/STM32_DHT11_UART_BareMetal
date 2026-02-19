/*
 * system_stm32f1xx.c
 *
 *  Created on: Feb 12, 2026
 *      Author: Rubin Khadka
 */

#include "stm32f103xb.h"
#include "stdint.h"

// System Core Clock Frequency
uint32_t SystemCoreClock = 8000000; 

void SystemInit(void)
{
	// Enable HSI (8MHz internal RC oscillator)
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);
	
	// Configure Flash for 8MHz 
	FLASH->ACR = FLASH_ACR_PRFTBE;
	
	// Clear PLL and clock configuration 
	RCC->CFGR &= ~(RCC_CFGR_SW | RCC_CFGR_PLLMULL | RCC_CFGR_PLLSRC);
	
	// Switch system clock to HSI
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
	
	// Turn off HSE and PLL to save power
	RCC->CR &= ~RCC_CR_HSEON;
	RCC->CR &= ~RCC_CR_PLLON;
	
	// Update SystemCoreClock
	SystemCoreClock = 8000000;
}
