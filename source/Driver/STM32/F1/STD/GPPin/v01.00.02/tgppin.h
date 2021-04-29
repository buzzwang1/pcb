#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


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
      case GPIOA_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);break;
      case GPIOB_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);break;
      case GPIOC_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);break;
      case GPIOD_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);break;
      case GPIOE_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);break;
      case GPIOF_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);break;
      case GPIOG_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);break;
    }
  }

  inline volatile GPIO_TypeDef* Port()
  {
    return (volatile GPIO_TypeDef*)mu32Port_BaseAdr;
  }
};

template <const uintptr_t mu32Port_BaseAdr, const uint16 mui16Pin>
class tcGPin : public tcPort<mu32Port_BaseAdr>
{
public:
  tcGPin(GPIOMode_TypeDef lenMode,
         GPIOSpeed_TypeDef lenSpeed,
         uint8 lui8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode = lenMode;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init((GPIO_TypeDef*)this->Port(), &GPIO_InitStructure);
  }

  /*void vSetMode(GPIOMode_TypeDef lui8Mode)
  {
    this->Port()->MODER &= ~(GPIO_MODER_MODER0 << (mui16Pin * 2));
    this->Port()->MODER |= (((uint32_t)lui8Mode) << (mui16Pin * 2));
  }*/


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

  cGpPin(u32 lu32PortBase,
         u16 lui16Pin,
         GPIOMode_TypeDef lenMode,
         GPIOSpeed_TypeDef lenSpeed,
         uint8 lui8InitValue)

  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    mu32PortBase = lu32PortBase;
    mui16Pin     = lui16Pin;

    switch (lu32PortBase)
    {
      case GPIOA_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);break;
      case GPIOB_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);break;
      case GPIOC_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);break;
      case GPIOD_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);break;
      case GPIOE_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);break;
      case GPIOF_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);break;
      case GPIOG_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);break;
    }

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode = lenMode;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init(((GPIO_TypeDef*)mu32PortBase), &GPIO_InitStructure);
  }

  
  // GPIO_Mode_AIN = 0x0,
  // GPIO_Mode_IN_FLOATING = 0x04,
  // GPIO_Mode_IPD = 0x28,
  // GPIO_Mode_IPU = 0x48,
  // GPIO_Mode_Out_OD = 0x14,
  // GPIO_Mode_Out_PP = 0x10,
  // GPIO_Mode_AF_OD = 0x1C,
  // GPIO_Mode_AF_PP = 0x18
  void vSetMode(GPIOMode_TypeDef lenMode)
  {
    if (mui16Pin < 8)
    {
      u32 lu32CRL;
      lu32CRL = ((GPIO_TypeDef*)mu32PortBase)->CRL;
      lu32CRL &= ~((3 << 2) << (mui16Pin * 4));
      lu32CRL |=  (lenMode << (mui16Pin * 4));
    }
    else
    {
      u32 lu32CRH;
      lu32CRH = ((GPIO_TypeDef*)mu32PortBase)->CRL;
      lu32CRH &= ~((3 << 2) << ((mui16Pin - 8) * 4));
      lu32CRH |= (lenMode << ((mui16Pin - 8) * 4));
    }
  }

  void vSetAF(GPIOMode_TypeDef lenMode)
  {
    vSetMode(lenMode);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
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
