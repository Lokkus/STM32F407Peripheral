/*
 * logger.h
 *
 *  Created on: 18 cze 2019
 *      Author: mk
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "main.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

#define endline "\n\r"

class Logger
{
  const uint32_t CLOCK = 84000000;
  const uint32_t BAUD = 115200;
  const uint32_t AF7 = 7UL;
  static constexpr uint32_t RECEIVE_BUFFER = 512;
  char* dataToSend = nullptr;
  char dataReceived[RECEIVE_BUFFER];
  volatile bool flag = false;
  std::stringstream oss;

  template<typename T>
  friend Logger& operator<<(Logger&l, T data);

public:
  Logger();
  void sendData(const std::string& data);
  void setFlag(bool val);
  bool getFlag();
  void clearFlag();
  uint32_t getReceivedBuffer();
  std::string getReceivedData();
  template <typename T>
  std::vector<T> getNumberFromReceived();
};

template<typename T>
Logger& operator<<(Logger& l, T data)
{
  l.oss<<data;
  if(!l.oss.str().empty())
  {
    l.sendData(l.oss.str());
    l.oss.str("");
  }
  return l;
}

template<typename T>
std::vector<T> Logger::getNumberFromReceived()
{
  if(flag ==  true)
  {
    std::stringstream ss;
    std::string tmp;
    T found;
    std::vector<T> resVec;
    ss<<getReceivedData();
    while(!ss.eof())
    {
      ss>>tmp;
      if(std::stringstream(tmp) >> found)
      {
        resVec.push_back(found);
        tmp = "";
      }
    }
    flag = false;
    return resVec;
  }
}


#endif /* LOGGER_H_ */
