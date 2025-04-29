#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "samd10.h"
#include "drivers.h"

template <uintptr_t mu32Port_BaseAdr>
class tcPort
{
public:
  tcPort()
  {
    if (mu32Port_BaseAdr == (uintptr_t)&PORTA)
    {
      //case PORTA: /*__HAL_RCC_GPIOA_CLK_ENABLE();*/ break;
    }
  }

  inline volatile PortGroup* mPort()
  {
    return (volatile PortGroup*)mu32Port_BaseAdr;
  }
};


template <const uintptr_t mu32Port_BaseAdr, const uint16 mui16Pin>
class tcGpPin : public tcPort<mu32Port_BaseAdr>
{
public:
  tcGpPin(u8 lu8Direction,
          u8 lu8PuPd,
          u8 lu8DrvStr,
          u8 lui8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    vDirection(lu8Direction);
    vPuPd(lu8PuPd);
    vDrvStr(lu8PuPd);
  }

  void vDirection(u8 lu8Direction)
  {
    if (lu8Direction) this->mPort()->DIRSET.reg = PinBitMask();
                 else this->mPort()->DIRCLR.reg = PinBitMask();
  }

  void vPuPd(u8 lu8PuPd)
  {
    this->mPort()->PINCFG[mui16Pin].bit.PULLEN = lu8PuPd;
  }

  void vDrvStr(u8 lu8DrvStr)
  {
    this->mPort()->PINCFG[mui16Pin].bit.DRVSTR = lu8DrvStr;
  }

  void vSetAF(uint8 lui8Af)
  {
    if (lui8Af < 8)
    {
      this->mPort()->PINCFG[mui16Pin].bit.PMUXEN = 1;
      if (mui16Pin & 1)
      {
        this->mPort()->PMUX[mui16Pin / 2].bit.PMUXO = lui8Af;
      }
      else
      {
        this->mPort()->PMUX[mui16Pin / 2].bit.PMUXE = lui8Af;
      }
    }
    else
    {
      this->mPort()->PINCFG[mui16Pin].bit.PMUXEN = 0;
    }
  }

  inline uint8 ui8Get()
  {
    if (this->mPort()->IN.reg & PinBitMask())
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
    this->mPort()->OUTSET.reg = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->mPort()->OUTCLR.reg = PinBitMask();
  }

  inline void vToggle(void)
  {
    this->mPort()->OUTTGL.reg = PinBitMask();
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
  uintptr_t mu32Port_BaseAdr;
  u16  mui16Pin;

  cGpPin() {};

  cGpPin(uintptr_t lu32Port_BaseAdr,
    u16 lui16Pin,
    u8 lu8Direction,
    u8 lu8PuPd,
    u8 lu8DrvStr,
    u8 lui8InitValue)
  {
    vInit(lu32Port_BaseAdr, lui16Pin, lu8Direction, lu8PuPd, lu8DrvStr, lui8InitValue);
  }

  void vInit(uintptr_t lu32Port_BaseAdr,
    u16 lui16Pin,
    u8 lu8Direction,
    u8 lu8PuPd,
    u8 lu8DrvStr,
    u8 lui8InitValue)
  {
    mu32Port_BaseAdr = lu32Port_BaseAdr;
    mui16Pin = lui16Pin;

    if (mu32Port_BaseAdr == (uintptr_t)&PORTA)
    {
      //case PORTA: /*__HAL_RCC_GPIOA_CLK_ENABLE();*/ break;
    }

    vDirection(lu8Direction);
    vPuPd(lu8PuPd);
    vDrvStr(lu8PuPd);
  }

  inline volatile PortGroup* mPort()
  {
    return (volatile PortGroup*)mu32Port_BaseAdr;
  }

  void vDirection(u8 lu8Direction)
  {
    if (lu8Direction) this->mPort()->DIRSET.reg = PinBitMask();
    else this->mPort()->DIRCLR.reg = PinBitMask();
  }

  void vPuPd(u8 lu8PuPd)
  {
    this->mPort()->PINCFG[mui16Pin].bit.PULLEN = lu8PuPd;
  }

  void vDrvStr(u8 lu8DrvStr)
  {
    this->mPort()->PINCFG[mui16Pin].bit.DRVSTR = lu8DrvStr;
  }

  void vSetAF(uint8 lui8Af)
  {
    if (lui8Af < 8)
    {
      this->mPort()->PINCFG[mui16Pin].bit.PMUXEN = 1;
      if (mui16Pin & 1)
      {
        this->mPort()->PMUX[mui16Pin / 2].bit.PMUXO = lui8Af;
      }
      else
      {
        this->mPort()->PMUX[mui16Pin / 2].bit.PMUXE = lui8Af;
      }
    }
    else
    {
      this->mPort()->PINCFG[mui16Pin].bit.PMUXEN = 0;
    }
  }

  inline uint8 ui8Get()
  {
    if (this->mPort()->IN.reg & PinBitMask())
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
    this->mPort()->OUTSET.reg = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->mPort()->OUTCLR.reg = PinBitMask();
  }

  inline void vToggle(void)
  {
    this->mPort()->OUTTGL.reg = PinBitMask();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};

#endif // __TGPPIN_H__
