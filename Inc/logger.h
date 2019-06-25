/*
 * logger.h
 *
 *  Created on: 18 cze 2019
 *      Author: mk
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "stm32f407xx.h"
#include <string>

class Logger
{
  const uint32_t CLOCK = 84000000;
  const uint32_t BAUD = 115200;
  const uint32_t AF7 = 7UL;
  char* dataToSend = nullptr;
public:
  Logger();
  void sendData(const std::string& data);
};


#endif /* LOGGER_H_ */
