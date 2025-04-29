#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

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
      case GPIOA_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);break;
      case GPIOB_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);break;
      case GPIOC_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);break;
      case GPIOD_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);break;
      case GPIOE_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);break;
      case GPIOF_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);break;
      case GPIOG_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);break;
      case GPIOH_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);break;
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
  tcGpPin(GPIOMode_TypeDef lenMode,
          GPIOOType_TypeDef lenOType,
          GPIOPuPd_TypeDef lenPuPd,
          GPIOSpeed_TypeDef lenSpeed,
          uint8 lui8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode = lenMode;
    GPIO_InitStructure.GPIO_OType = lenOType;
    GPIO_InitStructure.GPIO_PuPd = lenPuPd;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init((GPIO_TypeDef*)this->Port(), &GPIO_InitStructure);
  }

  void vSetType(GPIOOType_TypeDef lui8Type)
  {
    this->Port()->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)mui16Pin));
    this->Port()->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }

  void vSetMode(GPIOMode_TypeDef lui8Mode)
  {
    this->Port()->MODER &= ~(GPIO_MODER_MODER0 << (mui16Pin * 2));
    this->Port()->MODER |= (((uint32_t)lui8Mode) << (mui16Pin * 2));
  }

  void vSetAF(uint8 lui8Af)
  {
    vSetMode(GPIO_Mode_AF);
    GPIO_PinAFConfig((GPIO_TypeDef*)this->Port(), mui16Pin, lui8Af);
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
    this->Port()->BSRRL = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->Port()->BSRRH = PinBitMask();
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
         GPIOOType_TypeDef lenOType,
         GPIOPuPd_TypeDef lenPuPd,
         GPIOSpeed_TypeDef lenSpeed,
         uint8 lui8InitValue)

  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    mu32PortBase = lu32PortBase;
    mui16Pin     = lui16Pin;

    switch (lu32PortBase)
    {
      case GPIOA_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);break;
      case GPIOB_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);break;
      case GPIOC_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);break;
      case GPIOD_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);break;
      case GPIOE_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);break;
      case GPIOF_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);break;
      case GPIOG_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);break;
      case GPIOH_BASE: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);break;
    }

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode = lenMode;
    GPIO_InitStructure.GPIO_OType = lenOType;
    GPIO_InitStructure.GPIO_PuPd = lenPuPd;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init(((GPIO_TypeDef*)mu32PortBase), &GPIO_InitStructure);
  }

  void vSetType(GPIOOType_TypeDef lui8Type)
  {
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)mui16Pin));
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }

  void vSetMode(GPIOMode_TypeDef lui8Mode)
  {
    ((GPIO_TypeDef*)mu32PortBase)->MODER &= ~(GPIO_MODER_MODER0 << (mui16Pin * 2));
    ((GPIO_TypeDef*)mu32PortBase)->MODER |= (((uint32_t)lui8Mode) << (mui16Pin * 2));
  }

  void vSetAF(uint8 lui8Af)
  {
    vSetMode(GPIO_Mode_AF);
    GPIO_PinAFConfig((GPIO_TypeDef*)((GPIO_TypeDef*)mu32PortBase), mui16Pin, lui8Af);
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
    ((GPIO_TypeDef*)mu32PortBase)->BSRRL = PinBitMask();
  }

  inline void vSet0(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->BSRRH = PinBitMask();
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
