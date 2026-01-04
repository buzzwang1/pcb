#ifndef _BURAM_H
#define _BURAM_H

#include "typedef.h"
#include "BuRamDef.h"

#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

class cBuRam
{
  public:

  cBuRam();

  static tstBuRamDef* mBuRam;
  static u8 mu8ValueLastPos;
  static u8 mu8ValueLastSys;

  static void vEnable();

  // Auf BURAM kann man nur 32Bit Breit schreiben.
  static u8 u8ReadByte(u8 BytePos)
  {
    tun4Byte lunData;
    lunData.ui32Data = ((u32*)(RTC_BASE + 0x50))[BytePos >> 2];
    return lunData.ui8Byte[BytePos & 3];
  }

  static void vWriteByte(u8 BytePos, u8 lu8Value)
  {
    tun4Byte lunData;
    lunData.ui32Data = ((u32*)(RTC_BASE + 0x50))[BytePos >> 2];
    lunData.ui8Byte[BytePos & 3] = lu8Value;
    ((u32*)(RTC_BASE + 0x50))[BytePos >> 2] = lunData.ui32Data;
  }

  static void vAddLogPos(u8 lu8Value)
  {
    u8 lu8PosStart = (u8) ( (u8*)&mBuRam->au8LogPos - (u8*)(RTC_BASE + 0x50) );
    u8 lu8Pos = u8ReadByte(lu8PosStart + 15);
    if (lu8Pos > 14)
    {
      lu8Pos = 0;
    }

    if (mu8ValueLastPos != lu8Value)
    {
      mu8ValueLastPos = lu8Value;
      vWriteByte(lu8PosStart + lu8Pos, lu8Value);
      lu8Pos++;
      vWriteByte(lu8PosStart + 15, lu8Pos);
    }
  }


  static void vAddLogSys(u8 lu8Value)
  {
    u8 lu8PosStart = (u8) ( (u8*)&mBuRam->au8LogSys - (u8*)(RTC_BASE + 0x50) );
    u8 lu8Pos = u8ReadByte(lu8PosStart + 15);
    if (lu8Pos > 14)
    {
      lu8Pos = 0;
    }

    if (mu8ValueLastSys != lu8Value)
    {
      mu8ValueLastSys = lu8Value;
      vWriteByte(lu8PosStart + lu8Pos, lu8Value);
      lu8Pos++;
      vWriteByte(lu8PosStart + 15, lu8Pos);
    }
  }
};



#endif /* _BURAM_H */
