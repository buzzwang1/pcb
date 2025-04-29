#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "ch641.h"
#include "ch641_rcc.h"
#include "ch641_gpio.h"


class cGpCfg
{
public:
  typedef enum
  {
    //  CNF
    //  Input mode(MODE = 00b) :
    //    00 : Analog input mode;
    //    01: Floating input mode;
    //    10: Mode with pull - up and pull - down
    //    11 : Reserved.
    nAnalog = 0,
    nFloating = 1,
    nPuPd = 2,

    //  CNF
    //  Output mode(MODE > 00b) :
    //    00 : General push - pull output mode;
    //    01: General open - drain output mode;
    //    10: Alternate function push - pull output mode;
    //    11: Alternate function open - drain output mode.
    nPp   = 0,
    nOd   = 1,
    nAfPp = 2,
    nAfOd = 3,
  }tenCnf;

  typedef enum
  {
    //  MODE
    //   00: Input mode;
    //   01: Output mode, maximum speed : 10MHz;
    //   10: Output mode, maximum speed : 2MHz;
    //   11: Output mode, maximum speed : 50MHz;
    nInput = 0,
    nOutput_10MHz = 1,
    nOutput_2MHz  = 2,
    nOutput_50MHz = 3,
  }tenMode;


  static void vClrCnf(GPIO_TypeDef* Port, const u8 Pin)
  {
    if (Pin < 8) Port->CFGLR &= ~((0xF - 0x3) << ((Pin & 7) * 4));
            else Port->CFGHR &= ~((0xF - 0x3) << ((Pin & 7) * 4));
  }


  static void vClrMode(GPIO_TypeDef* Port, const u8 Pin)
  {
    if (Pin < 8) Port->CFGLR &= ~(0x3 << ((Pin & 7) * 4));
           else  Port->CFGHR &= ~(0x3 << ((Pin & 7) * 4));
  }

  static void vClr(GPIO_TypeDef* Port, const u8 Pin)
  {
    if (Pin < 8) Port->CFGLR &= ~(0xF << ((Pin & 7) * 4));
           else  Port->CFGHR &= ~(0xF << ((Pin & 7) * 4));
  }


  static void vSetCnf(GPIO_TypeDef* Port, const u8 Pin, const tenCnf Cnf)
  {
    vClrCnf(Port, Pin);
    if (Pin < 8) Port->CFGLR |= ((Cnf << 2) << ((Pin & 7) * 4));
            else Port->CFGHR |= ((Cnf << 2) << ((Pin & 7) * 4));
  }

  static void vSetMode(GPIO_TypeDef* Port, const u8 Pin, const tenMode Mode)
  {
    vClrMode(Port, Pin);
    if (Pin < 8) {Port->CFGLR |= (Mode << ((Pin & 7) * 4));}
           else  {Port->CFGHR |= (Mode << ((Pin & 7) * 4));}
  }

  static void vSet(GPIO_TypeDef* Port, const u8 Pin, const tenCnf Cnf, const tenMode Mode)
  {
    vClr(Port, Pin);
    if (Pin < 8) Port->CFGLR |= (((Cnf << 2) | Mode) << ((Pin & 7) * 4));
           else  Port->CFGHR |= (((Cnf << 2) | Mode) << ((Pin & 7) * 4));
  }
};



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
  tcGpPin(GPIOMode_TypeDef  lenMode,
          GPIOSpeed_TypeDef lenSpeed,
          uint8 lui8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode  = lenMode;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init((GPIO_TypeDef*)this->Port(), &GPIO_InitStructure);
  }

  /*void vSetType(GPIOOType_TypeDef lui8Type)
  {
    this->Port()->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)mui16Pin));
    this->Port()->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }*/

  void vSetMode(GPIOMode_TypeDef lui8Mode)
  {
    UNUSED(lui8Mode);
    ////u32 lu32Mask = ~(0xC << ((mui16Pin & 7) * 4));
    ////
    ////if (mui16Pin < 8)
    ////{
    ////  this->Port()->CFGLR &= lu32Mask;
    ////  this->Port()->CFGLR |= (((uint32_t)lui8Mode) << (mui16Pin * 4));
    ////}
    ////else
    ////{
    ////  this->Port()->CFGHR &= lu32Mask;
    ////  this->Port()->CFGHR |= (((uint32_t)lui8Mode) << ((mui16Pin & 7) * 4));
    ////}
  }

  void vSetAF(uint8 lui8Af)
  {
    UNUSED(lui8Af);
    //vSetMode(GPIO_Mode_AF);
    //GPIO_PinAFConfig((GPIO_TypeDef*)this->Port(), mui16Pin, lui8Af);
  }

  inline uint8 ui8Get()
  {
    if (this->Port()->INDR & PinBitMask())
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
    this->Port()->BSHR = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->Port()->BCR = PinBitMask();
  }

  inline void vToggle(void)
  {
    this->Port()->OUTDR ^= PinBitMask();
  }

  bool operator==(const u8 rhs) { return ui8Get() == rhs;};

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
      case GPIOA_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); break;
      case GPIOB_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); break;
    }

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin = (1 << mui16Pin);
    GPIO_InitStructure.GPIO_Mode = lenMode;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init(((GPIO_TypeDef*)mu32PortBase), &GPIO_InitStructure);
  }

  /*
  void vSetType(GPIOOType_TypeDef lui8Type)
  {
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)mui16Pin));
    ((GPIO_TypeDef*)mu32PortBase)->OTYPER |= (uint16_t)(((uint16_t)lui8Type) << ((uint16_t)mui16Pin));
  }*/

  void vSetMode(GPIOMode_TypeDef lui8Mode)
  {
    UNUSED(lui8Mode);
    ////u32 lu32Mask = ~(0xC << ((mui16Pin & 7) * 4));
    ////if (mui16Pin < 8)
    ////{
    ////  ((GPIO_TypeDef*)mu32PortBase)->CFGLR &= lu32Mask;
    ////  ((GPIO_TypeDef*)mu32PortBase)->CFGLR |= (((uint32_t)lui8Mode) << (mui16Pin * 4));
    ////}
    ////else
    ////{
    ////  ((GPIO_TypeDef*)mu32PortBase)->CFGHR &= lu32Mask;
    ////  ((GPIO_TypeDef*)mu32PortBase)->CFGHR |= (((uint32_t)lui8Mode) << ((mui16Pin & 7) * 4));
    ////}
  }

  void vSetAF(uint8 lui8Af)
  {
    UNUSED(lui8Af);
    //vSetMode(GPIO_Mode_AF);
    //GPIO_PinAFConfig((GPIO_TypeDef*)((GPIO_TypeDef*)mu32PortBase), mui16Pin, lui8Af);
  }

  inline uint8 ui8Get()
  {
    if (((GPIO_TypeDef*)mu32PortBase)->INDR & PinBitMask())
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
    ((GPIO_TypeDef*)mu32PortBase)->BSHR = PinBitMask();
  }

  inline void vSet0(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->BCR = PinBitMask();
  }

  inline void vToggle(void)
  {
    ((GPIO_TypeDef*)mu32PortBase)->OUTDR ^= PinBitMask();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};

#endif // __TGPPIN_H__
