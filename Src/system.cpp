/*
 * system.cpp
 *
 *  Created on: 18 cze 2019
 *      Author: mk
 */

#include "system.h"

System::System()
{
  //configuration for flash */
  FLASH->ACR |=
      FLASH_ACR_LATENCY_5WS |       // 6 CPU cycle wait
      FLASH_ACR_ICEN |              // instruction cache enable */
      FLASH_ACR_DCEN |              // data cache enable
      FLASH_ACR_PRFTEN;             // enable prefetch

  /*  configure clocks
   *  Max SYSCLK: 168MHz
   *  Max AHB: SYSCLK
   *  Max APB1: SYSCLK/4
   *  Max APB2: SYSCLK/2
   *  How to set RCC:
   *  SystemCoreClock = ((INPUT_CLOCK (HSE_OR_HSI_IN_HZ) / PLL_M) * PLL_N) / PLL_P
   *  HSE = 8MHz
   *  PLLM = 8
   *  PLLN = 336
   *  PLLP = 2
   *  PLLQ = 7 (336/7 = 48, freq for USB)
   *
   */
  RCC->CFGR = 0;                          // System clock (SYSCLK) selected
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2 |      // APB2 prescaler, max 84MHz
      RCC_CFGR_PPRE1_DIV4;                // APB1 prescaler, max 42MHz


  RCC->CR |= RCC_CR_HSEON;                // enable HSE
  while(!(RCC->CR & RCC_CR_HSERDY));      // wait until HSE ready

  //PLL config
  RCC->PLLCFGR = 0;
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE |  // PLL - HSE
      RCC_PLLCFGR_PLLM_3 |                  // PLLM = 8
      (336 << 6) |                          // PLLN = 336
      (0 << 16) |                           // PLLP = 2
      (7 << 24 );


  RCC->CR |= RCC_CR_PLLON;                  // enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));        // check if PLL is ready

  RCC->CFGR &= ~RCC_CFGR_SW;                // reset settings of SW (system clock switch
  RCC->CFGR |= RCC_CFGR_SW_PLL;             // SYSCLK = PLL
  while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));   // wait until PLL will be sysclk

  // Configure timer for system delay
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  TIM6->ARR = 41;
  TIM6->PSC = 1;

  // led config
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  GPIOD->MODER |= GPIO_MODER_MODE12_0 |
      GPIO_MODER_MODE13_0 |
      GPIO_MODER_MODE14_0 |
      GPIO_MODER_MODE15_0;
}

void System::delay(uint32_t period, time unit)
{
  if(flag != unit)
  {
    switch(unit)
    {
    case time::us:
      TIM6->ARR = 41;
      TIM6->PSC = 1;
      flag = time::us;
      break;

    case time::ms:
      TIM6->ARR = 41999;
      TIM6->PSC = 1;
      flag = time::ms;
      break;

    case time::s:
      TIM6->ARR = 9999;
      TIM6->PSC = 8399;
      flag = time::s;
      break;
    }
  }

  TIM6->CR1 |=TIM_CR1_CEN;            // enable timer;
  while(period--)
  {
    while(!(TIM6->SR & TIM_SR_UIF));
    TIM6->SR &=~TIM_SR_UIF;
  }
  TIM6->CR1 &=~TIM_CR1_CEN;           // disable timer
}


