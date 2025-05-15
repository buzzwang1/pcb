/*
 * MyTestClass.h
 *
 *  Created on: May 6, 2025
 *      Author: ms
 */

#ifndef SRC_MYTESTCLASS_H_
#define SRC_MYTESTCLASS_H_

// https://github.com/sgbush/cppcon2022/blob/release/code/sections/1_gpio_configuration/snippets.hpp

// https ://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP11.md
// https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP14.md
// https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP17.md
// https://github.com/AnthonyCalandra/modern-cpp-features/blob/master/CPP20.md#bit-operations

#include <cstdint>
#include <array>
#include <span>
#include <concepts>


#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_gpio.h"

#ifdef __cplusplus
}
#endif



class MyTestClass {
public:
  MyTestClass() {}
};



// a type that will completely describe the function of the IO pin
using GPIODEF = \
struct IODefStuct {
  GPIO_TypeDef* GPIO;
  uint32_t PinNumber;
  enum IOFUNCTION :uint32_t { INPUT = 0, OUTPUT = 1, ALT = 2, ANALOG = 3 } Function;
  uint32_t AltFunc;
  enum IOTYPE :uint32_t { NORMAL = 0, OPENDRAIN = 1 } Type;
  enum IOSPEED :uint32_t { LOW = 0, MEDIUM = 1, HIGH = 2, VERYHIGH = 3 } Speed;
  enum IOPULL :uint32_t { NONE = 0, PULLUP = 1, PULLDOWN = 2 } Bias;
  enum IOSTATE :uint32_t { LOGIC_LOW, LOGIC_HIGH, DONT_CARE } InitialState;
};

// declarative GPIO configuration
// this will be located in flash, and the configure function will consume the data in pace, without the need to populate structs in stack memory
static const std::array<GPIODEF, 2> gpiodefs = { {
    // green LED
    {GPIOB, 0,
    GPIODEF::IOFUNCTION::OUTPUT, 1,
    GPIODEF::IOTYPE::NORMAL,
    GPIODEF::IOSPEED::LOW,
    GPIODEF::IOPULL::NONE,
    GPIODEF::IOSTATE::LOGIC_LOW},
    // red LED
    {GPIOB, 2,
    GPIODEF::IOFUNCTION::OUTPUT, 1,
    GPIODEF::IOTYPE::NORMAL,
    GPIODEF::IOSPEED::LOW,
    GPIODEF::IOPULL::NONE,
    GPIODEF::IOSTATE::LOGIC_LOW},
    // ... and so on
} };

#endif /* SRC_MYTESTCLASS_H_ */
