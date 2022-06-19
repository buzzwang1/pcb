
#ifndef __ClockInfo_H__
#define __ClockInfo_H__

#include "TypeDef.h"
#include "ClockInfo1.h"


class cClockInfo
{
  public:

  static RCC_ClocksTypeDef mstClocks;

  cClockInfo()
  {
    Update();
  };

  static void Update()
  {
    RCC_GetClocksFreq(&mstClocks);
  };

  static void Delay_us(volatile uint32 lui32Delay)
  {
    lui32Delay *= (u32GetClockCore() / 1000000);
    lui32Delay /= 8;  // ca. 3 für while + 5 Nop

    while (lui32Delay > 0)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      asm("NOP");
      lui32Delay--;
    }
  }

  static u32 u32GetClockCore()  {return ClockInfo_Clock_Core;}
  static u32 u32GetClockSys()   {return ClockInfo_Clock_Sys;}
  static u32 u32GetClockPort()  {return ClockInfo_Clock_Port;}

  static void Delay_ms(volatile uint32 lui32Delay)
  {
    while (lui32Delay > 0)
    {
      Delay_us(1000);
      lui32Delay--;
    }
  }
};


#endif // __ClockInfo_H__
