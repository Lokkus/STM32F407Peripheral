/*
 * logger.cpp
 *
 *  Created on: 18 cze 2019
 *      Author: mk
 */

#include "logger.h"
#include "system.h"

using namespace std;

Logger::Logger()
{
  // enable clock for USART1 (GPIOB) and DMA2
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN |
      RCC_AHB1ENR_DMA2EN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // config GPIOB6(TX) and GPIOB7(RX)
  GPIOB->MODER |= GPIO_MODER_MODE6_1 |        // GPIO mode as alternate function
      GPIO_MODER_MODE7_1;
  GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_1 |
      GPIO_PUPDR_PUPD7_1;
  GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1 |
      GPIO_OSPEEDER_OSPEEDR7_1;
  GPIOB->AFR[0] |= AF7 << 24 |                  // 7 << 4(bits) * PIN(6)
      AF7 << 28;                                // 7 << 4(bits) * PIN(7)

  // config USART1
  USART1->BRR = CLOCK/BAUD;
  USART1->CR1 |= USART_CR1_TE |                 // transmitter enable
      USART_CR1_RE |                            // receiver enable
      USART_CR1_IDLEIE;                         // IDLE interrupt enable
  USART1->CR3 |= USART_CR3_DMAT |               // DMA transmitter enable
      USART_CR3_DMAR;                           // DMA receiver enable

  // config DMA for TX
  DMA2_Stream7->PAR = (uint32_t)&USART1->DR;
  DMA2_Stream7->CR |= (4 << 25) |               // Channel 4 selected
      DMA_SxCR_PL_1 |                           // Priority level = 2
      DMA_SxCR_MINC |                           // Memory increment mode
      DMA_SxCR_DIR_0 |                          // Data transfer - mem to per
      DMA_SxCR_TCIE;                            // Transfer interrupt enable

  // config DMA for RX TBD;
  DMA2_Stream2->PAR = (uint32_t)&USART1->DR;
  DMA2_Stream2->M0AR = (uint32_t)dataReceived;
  DMA2_Stream2->NDTR = RECEIVE_BUFFER;          // temp
  DMA2_Stream2->CR |= (4 << 25) |               // Channel 4 selected
      DMA_SxCR_MINC |                           // Memory increment mode
      DMA_SxCR_TCIE;                            // Transfer complete interrupt enable

  DMA2_Stream2->CR |=DMA_SxCR_EN;               // enable DMA2
  USART1->CR1 |= USART_CR1_UE;                  // enable USART1

  // enable interrupts
  __NVIC_SetPriority(DMA2_Stream7_IRQn, 0);
  __NVIC_EnableIRQ(DMA2_Stream7_IRQn);

  __NVIC_SetPriority(DMA2_Stream2_IRQn, 0);
  __NVIC_EnableIRQ(DMA2_Stream2_IRQn);

  __NVIC_SetPriority(USART1_IRQn, 0);
  __NVIC_EnableIRQ(USART1_IRQn);

}

void Logger::sendData(const string& data)
{
  size_t len = data.size();
  if (dataToSend == nullptr)
    dataToSend = new char [len];
  else
  {
    delete [] dataToSend;
    dataToSend = new char [len];
  }
  data.copy(dataToSend, len);

  DMA2_Stream7->M0AR = (uint32_t)dataToSend;
  DMA2_Stream7->NDTR = len;
  USART1->SR &=~USART_SR_TC;
  USART1->CR1 |= USART_CR1_UE;
  DMA2_Stream7->CR |= DMA_SxCR_EN;
  while(DMA2_Stream7->CR & DMA_SxCR_EN);
  delete [] dataToSend;
  dataToSend = nullptr;

}

void Logger::setFlag(bool val)
{
  flag = val;
}

bool Logger::getFlag()
{
  return flag;
}
uint32_t Logger::getReceivedBuffer()
{
  return RECEIVE_BUFFER;
}

string Logger::getReceivedData()
{
  string tmp;
  tmp = dataReceived;
  memset(dataReceived, 0, RECEIVE_BUFFER);
  return tmp;
}

void Logger::clearFlag()
{
  std::ostringstream ostr;
  oss.copyfmt(ostr);
}

