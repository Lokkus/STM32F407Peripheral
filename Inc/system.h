/*
 * system.h
 *
 *  Created on: 18 cze 2019
 *      Author: mk
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "stm32f407xx.h"
#include <string>

class System
{
public:
  System();
  enum class time
  {
    us,
    ms,
    s
  };
  void delay(uint32_t period, time unit);
private:
  time flag = time::us;
};


#endif /* SYSTEM_H_ */
