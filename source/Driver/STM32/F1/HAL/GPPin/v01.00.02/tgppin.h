#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

template <uintptr_t mu32Port_BaseAdr>
class tcPort
{
public:
  tcPort()
  {
    /* GPIO Periph clock enable */
    /* Enable USART clock */
    switch (mu32Port_BaseAdr)
    {
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE();break;
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE();break;
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE();break;
      case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE();break;
      //case GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE();break;
      //case GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE();break;
      //case GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE();break;
      //case GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE();break;
    }
  }

  inline volatile GPIO_TypeDef* Port()
  {
    return (volatile GPIO_TypeDef*)mu32Port_BaseAdr;
  }
};

template <const uintptr_t mu32Port_BaseAdr, const uint16 mui16Pin>
class tcGpPin : public tcPort<mu32Port_BaseAdr>
{
public:
  tcGpPin(u32 lenMode,
          u32 lenPuPd,
          u32 lenSpeed,
          u8  lui8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    GPIO_InitTypeDef  gpioinitstruct = {};

    vSet(lui8InitValue);

    /* Configure the GPIO_LED pin */
    gpioinitstruct.Pin   = (1 << mui16Pin);
    gpioinitstruct.Mode  = lenMode;
    gpioinitstruct.Pull  = lenPuPd;
    gpioinitstruct.Speed = lenSpeed;
    HAL_GPIO_Init((GPIO_TypeDef*)this->Port(), &gpioinitstruct);
  }

  // MODE:
  // 00 : Input mode(reset state)
  // 01 : Output mode, max speed 10 MHz.
  // 10 : Output mode, max speed 2 MHz.
  // 11 : Output mode, max speed 50 MHz.

  // CFG:
  //   If MODE = 0
  //   00 : Analog mode
  //   01 : Floating input(reset state)
  //   10 : Input with pull - up / pull - down
  //   11 : Reserved
  // 
  //   If MODE > 0
  //   00 : General purpose output push - pull
  //   01 : General purpose output Open - drain
  //   10 : Alternate function output Push - pull
  //   11 : Alternate function output Open - drain
  void vSetMode2(u8 lu8Mode, u8 lu8Cfg)
  {
    u8 lu8Combined = (lu8Cfg << 2) + lu8Mode;

    if (mui16Pin < 8)
    {
      u32 lu32CRL;
      lu32CRL = ((GPIO_TypeDef*)mu32Port_BaseAdr)->CRL;
      lu32CRL &= ~((3 << 2) << (mui16Pin * 4));
      lu32CRL |= (lu8Combined << (mui16Pin * 4));
      ((GPIO_TypeDef*)mu32Port_BaseAdr)->CRL = lu32CRL;
    }
    else
    {
      u32 lu32CRH;
      lu32CRH = ((GPIO_TypeDef*)mu32Port_BaseAdr)->CRH;
      lu32CRH &= ~((3 << 2) << ((mui16Pin - 8) * 4));
      lu32CRH |= (lu8Combined << ((mui16Pin - 8) * 4));
      ((GPIO_TypeDef*)mu32Port_BaseAdr)->CRH = lu32CRH;
    }
  }

  // GPIO_MODE_OUTPUT_PP
  // GPIO_MODE_OUTPUT_OD
  // GPIO_MODE_AF_PP
  // GPIO_MODE_AF_OD
  // GPIO_MODE_INPUT
  // GPIO_MODE_ANALOG

  // GPIO_SPEED_FREQ_LOW
  // GPIO_SPEED_FREQ_MEDIUM
  // GPIO_SPEED_FREQ_HIGH
  void vSetMode(u16 lu16Mode, u16 lu16Speed)
  {
    switch (lu16Mode)
    {
    case GPIO_MODE_OUTPUT_PP: vSetMode2(lu16Speed, 0); break;
    case GPIO_MODE_OUTPUT_OD: vSetMode2(lu16Speed, 1); break;
    case GPIO_MODE_AF_PP:     vSetMode2(lu16Speed, 2); break;
    case GPIO_MODE_AF_OD:     vSetMode2(lu16Speed, 3); break;
    case GPIO_MODE_INPUT:     vSetMode2(0, 1); break;
    case GPIO_MODE_ANALOG:    vSetMode2(0, 0); break;
    }
  }

  void vSetAF()
  {
    __HAL_RCC_AFIO_CLK_ENABLE();
  }

  inline uint8 ui8Get()
  {
    if (this->Port()->IDR & PinBitMask())
    {
      return 1;
    }
    return 0;
  }

  inline void vSet(uint8 lui8Value)
  {
    if (lui8Value == 0)
    {
      vSet0();
    }
    else
    {
      vSet1();
    }
  }

  inline void vSet1(void)
  {
    this->Port()->BSRR = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->Port()->BRR = PinBitMask();
  }

  inline void vToggle(void)
  {
    this->Port()->ODR ^= PinBitMask();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};


class cGpPin
{
public:
  u32 mu32PortBase;
  u16 mui16Pin;

  cGpPin() {};

  cGpPin(u32 lu32PortBase,
         u16 lui16Pin,
         u32 lenMode,
         u32 lenPuPd,
         u32 lenSpeed,
         u8  lui8InitValue)
  {
    vInit(lu32PortBase, lui16Pin, lenMode, lenPuPd, lenSpeed, lui8InitValue);
  }

  void vInit(u32 lu32PortBase,
             u16 lui16Pin,
             u32 lenMode,
             u32 lenPuPd,
             u32 lenSpeed,
             u8  lui8InitValue)
  {
    GPIO_InitTypeDef  gpioinitstruct = {};

    mu32PortBase = lu32PortBase;
    mui16Pin     = lui16Pin;

    switch (mu32PortBase)
    {
      case (u32)GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE();break;
      case (u32)GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE();break;
      case (u32)GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE();break;
      case (u32)GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE();break;
      //case (u32)GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE();break;
      //case (u32)GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE();break;
      //case (u32)GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE();break;
      //case (u32)GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE();break;
    }

    if ((lenMode == GPIO_MODE_OUTPUT_PP) ||
        (lenMode == GPIO_MODE_OUTPUT_OD))
    {
      vSet(lui8InitValue);
    }

    /* Configure the GPIO_LED pin */
    gpioinitstruct.Pin   = (1 << mui16Pin);
    gpioinitstruct.Mode  = lenMode;
    gpioinitstruct.Pull  = lenPuPd;
    gpioinitstruct.Speed = lenSpeed;
    HAL_GPIO_Init(((GPIO_TypeDef*)mu32PortBase), &gpioinitstruct);
  }


  // MODE:
  // 00 : Input mode(reset state)
  // 01 : Output mode, max speed 10 MHz.
  // 10 : Output mode, max speed 2 MHz.
  // 11 : Output mode, max speed 50 MHz.

  // CFG:
  //   If MODE = 0
  //   00 : Analog mode
  //   01 : Floating input(reset state)
  //   10 : Input with pull - up / pull - down
  //   11 : Reserved
  // 
  //   If MODE > 0
  //   00 : General purpose output push - pull
  //   01 : General purpose output Open - drain
  //   10 : Alternate function output Push - pull
  //   11 : Alternate function output Open - drain
  void vSetMode2(u8 lu8Mode, u8 lu8Cfg)
  {
    u8 lu8Combined = (lu8Cfg << 2) + lu8Mode;

    if (mui16Pin < 8)
    {
      u32 lu32CRL;
      lu32CRL = ((GPIO_TypeDef*)mu32PortBase)->CRL;
      lu32CRL &= ~((3 << 2) << (mui16Pin * 4));
      lu32CRL |= (lu8Combined << (mui16Pin * 4));
      ((GPIO_TypeDef*)mu32PortBase)->CRL = lu32CRL;
    }
    else
    {
      u32 lu32CRH;
      lu32CRH = ((GPIO_TypeDef*)mu32PortBase)->CRH;
      lu32CRH &= ~((3 << 2) << ((mui16Pin - 8) * 4));
      lu32CRH |= (lu8Combined << ((mui16Pin - 8) * 4));
      ((GPIO_TypeDef*)mu32PortBase)->CRH = lu32CRH;
    }
  }

  // GPIO_MODE_OUTPUT_PP
  // GPIO_MODE_OUTPUT_OD
  // GPIO_MODE_AF_PP
  // GPIO_MODE_AF_OD
  // GPIO_MODE_INPUT
  // GPIO_MODE_ANALOG

  // GPIO_SPEED_FREQ_LOW
  // GPIO_SPEED_FREQ_MEDIUM
  // GPIO_SPEED_FREQ_HIGH
  void vSetMode(u16 lu16Mode, u16 lu16Speed)
  {
    switch (lu16Mode)
    {
      case GPIO_MODE_OUTPUT_PP: vSetMode2(lu16Speed, 0); break;
      case GPIO_MODE_OUTPUT_OD: vSetMode2(lu16Speed, 1); break;
      case GPIO_MODE_AF_PP:     vSetMode2(lu16Speed, 2); break;
      case GPIO_MODE_AF_OD:     vSetMode2(lu16Speed, 3); break;
      case GPIO_MODE_INPUT:     vSetMode2(0, 1); break;
      case GPIO_MODE_ANALOG:    vSetMode2(0, 0); break;
    }
  }

  void vSetAF()
  {
    __HAL_RCC_AFIO_CLK_ENABLE();
  }

  inline uint8 ui8Get()
  {
    if (((GPIO_TypeDef*)mu32PortBase)->IDR & PinBitMask())
    {
      return 1;
    }
    return 0;
  }

  inline void vSet(uint8 lui8Value)
  {
    if (lui8Value == 0)
    {
      vSet0();
    }
    else
    {
      vSet1();
    }
  }

  inline void vSet1(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->BSRR = PinBitMask();
  }

  inline void vSet0(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->BRR = PinBitMask();
  }

  inline void vToggle(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->ODR ^= PinBitMask();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};

#endif // __TGPPIN_H__
