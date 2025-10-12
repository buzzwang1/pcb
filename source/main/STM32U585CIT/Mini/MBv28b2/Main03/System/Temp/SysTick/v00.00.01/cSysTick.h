#ifndef SYS_TICK_H
#define SYS_TICK_H

#include "Typedef.h"
#include "cSysTickHw.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cTimer32Tools
{
  public: 

  static u32 u32Diff(u32 lu32ValueOld, u32 lu32ValueNew)
  {
    u32 lu32Diff;

    if (lu32ValueNew >= lu32ValueOld)
    {
      return (lu32ValueNew - lu32ValueOld);
    }
    else
    {
      lu32Diff = (U32MAX - (lu32ValueOld + 1)) + lu32ValueNew;
    }
    return lu32Diff;
  }

  // Überprüft, ob die gegebene Zeit erreicht wurde
  static bool isValue(u32 u32Timer, u32 lu32Value)
  {
    if (u32Timer >= lu32Value)
    {
      return True;
    }
    return False;
  }
};


class cSysTick : public cSysTickHw
{
  private:
  static u32 mu32Temp_us;

public:
  static u32 mu32Tick_ms;
  static u8  mu8Init;

  static void vInit(u32 lu32InitCnt = 0)
  {
    if (mu8Init == 0)
    {
      mu8Init = 1;
      InitTimer(lu32InitCnt);
    }

    start();
  }

  static u32 u32Get_ms()
  {
    return mu32Tick_ms;
  }

  static u32 u32Get_us()
  {
    return u32GetTimer();
  }

  static void vDoProcess()
  {
    mu32Temp_us += get();

    if (mu32Temp_us > 1000)
    {
      mu32Tick_ms += mu32Temp_us / 1000;
      mu32Temp_us = mu32Temp_us % 1000;
    }    
  }

  private:
    static void start()
    {
      mu32Temp_us = u32GetTimer();
    };

    // Gibt die seit dem letzen Aufruf vergange Zeit zurück
    // us
    static u32 get()
    {
      u32 lu32Diff;
      u32 lu32Temp;

      lu32Temp = u32GetTimer();
      lu32Diff = cTimer32Tools::u32Diff(mu32Temp_us, lu32Temp);
      mu32Temp_us = lu32Temp;

      return lu32Diff;
    };
};

#ifdef __cplusplus
}
#endif

#endif // SYS_TICK_H
