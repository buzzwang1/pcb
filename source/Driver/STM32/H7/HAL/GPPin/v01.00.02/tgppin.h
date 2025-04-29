#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "stm32H7xx.h"
#include "stm32H7xx_hal.h"

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
      case GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE();break;
      //case GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE();break;
      //case GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE();break;
      case GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE();break;
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
    gpioinitstruct.Alternate = 0;
    HAL_GPIO_Init((GPIO_TypeDef*)this->Port(), &gpioinitstruct);
  }

  void vSetType(u32 lui8Type)
  {
    this->Port()->OTYPER &= ~((GPIO_OTYPER_OT0) << ((uint16_t)mui16Pin));
    this->Port()->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }

  void vSetMode(u32 lui8Mode)
  {
    if (lui8Mode & 0x10)
    {
      vSetType(1);
      lui8Mode &= 0x0F;
    }
    else
    {
      vSetType(0);
    }
    this->Port()->MODER &= ~(GPIO_MODER_MODE0 << (mui16Pin * 2));
    this->Port()->MODER |= (((uint32_t)lui8Mode) << (mui16Pin * 2));
  }

  void vSetAF(u32 lenMode, uint8 lui8Af)
  {
    uint32 temp;

    if((lenMode == GPIO_MODE_AF_PP) || (lenMode == GPIO_MODE_AF_OD))
    {
      vSetMode(lenMode); // GPIO_MODE_AF_PP, GPIO_MODE_AF_OD

      /* Check the Alternate function parameters */
      assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
      assert_param(IS_GPIO_AF(GPIO_Init->Alternate));
        
      /* Configure Alternate function mapped with the current IO */
      temp = this->Port()->AFR[mui16Pin >> 3];
      temp &= ~((uint32)0xF << ((uint32)(mui16Pin & (uint32)0x07) * 4)) ;
      temp |= ((uint32)(lui8Af) << (((uint32)mui16Pin & (uint32)0x07) * 4));
      this->Port()->AFR[mui16Pin >> 3] = temp;
    }
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
    this->Port()->BSRR = PinBitMask() << 16;
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



class tcGpPinDummy
{
public:
  tcGpPinDummy(u32 lenMode,
               u32 lenPuPd,
               u32 lenSpeed,
               u8  lui8InitValue)
  {
    UNUSED(lenMode);
    UNUSED(lenPuPd);
    UNUSED(lenSpeed);
    UNUSED(lui8InitValue);
  }

  void vSetType(u32 lui8Type)
  {
    UNUSED(lui8Type);
  }

  void vSetMode(u32 lui8Mode)
  {
    UNUSED(lui8Mode);
  }

  void vSetAF(u32 lenMode, uint8 lui8Af)
  {
    UNUSED(lenMode);
    UNUSED(lui8Af);
  }

  inline uint8 ui8Get()
  {
    return 0;
  }

  inline void vSet(uint8 lui8Value)
  {
    UNUSED(lui8Value);
  }

  inline void vSet1(void)
  {
  }

  inline void vSet0(void)
  {
  }

  inline void vToggle(void)
  {
  }
};



class cGpPin
{
public:
  u32 mu32PortBase;
  u16 mui16Pin;

  cGpPin(u32 lu32PortBase,
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
      case (u32)GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE();break;
      //case (u32)GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE();break;
      //case (u32)GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE();break;
      case (u32)GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE();break;
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
    gpioinitstruct.Alternate = 0;
    HAL_GPIO_Init(((GPIO_TypeDef*)mu32PortBase), &gpioinitstruct);
  }

  void vSetType(u32 lui8Type)
  {
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER &= ~((GPIO_OTYPER_OT0) << ((uint16_t)mui16Pin));
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }

  void vSetMode(u32 lui8Mode)
  {
    if (lui8Mode & 0x10)
    {
      vSetType(1);
      lui8Mode &= 0x0F;
    }
    else
    {
      vSetType(0);
    }
    ((GPIO_TypeDef*)mu32PortBase)->MODER &= ~(GPIO_MODER_MODE0 << (mui16Pin * 2));
    ((GPIO_TypeDef*)mu32PortBase)->MODER |= (((uint32_t)lui8Mode) << (mui16Pin * 2));
  }

  void vSetAF(u32 lenMode, uint8 lui8Af)
  {
    uint32 temp;

    if((lenMode == GPIO_MODE_AF_PP) || (lenMode == GPIO_MODE_AF_OD))
    {
      /* Check the Alternate function parameters */
      assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
      assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

      /* Configure Alternate function mapped with the current IO */
      temp = ((GPIO_TypeDef*)mu32PortBase)->AFR[mui16Pin >> 3];
      temp &= ~((uint32)0xF << ((uint32)(mui16Pin & (uint32)0x07) * 4)) ;
      temp |= ((uint32)(lui8Af) << (((uint32)mui16Pin & (uint32)0x07) * 4));
      ((GPIO_TypeDef*)mu32PortBase)->AFR[mui16Pin >> 3] = temp;

      vSetMode(lenMode); // GPIO_MODE_AF_PP, GPIO_MODE_AF_OD
    }
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
    ((GPIO_TypeDef*)mu32PortBase)->BSRR = PinBitMask() << 16;
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
