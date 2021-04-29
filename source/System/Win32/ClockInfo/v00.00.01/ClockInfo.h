
#ifndef __ClockInfo_H__
#define __ClockInfo_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "ClockInfo1.h"

class cClockInfo
{
  public:

  //static RCC_ClocksTypeDef mstClocks;

  cClockInfo()
  {
    Update();
  };

  static void Update()
  {
    //RCC_GetClocksFreq(&mstClocks);
  };

  static void Delay_us(volatile uint32 lui32Delay)
  {

  }

  static u32 u32GetClockCore() { return ClockInfo_Clock_Core; }
  static u32 u32GetClockSys() { return ClockInfo_Clock_Sys; }
  static u32 u32GetClockPort() { return ClockInfo_Clock_Port; }

  static void Delay_ms(volatile uint32 lui32Delay)
  {
  }
};

#ifdef __cplusplus
}
#endif

#endif // __ClockInfo_H__
