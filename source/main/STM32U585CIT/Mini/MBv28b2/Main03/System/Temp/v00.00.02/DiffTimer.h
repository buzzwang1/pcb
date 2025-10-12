
#ifndef __DIFFTIMER_H__
#define __DIFFTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "cStringTools.h"
#include "cSysTick.h"

class cDiffTimer
{
  public:

  u32  mu32LastTimerValue;

  cDiffTimer()
  {
    vInit();    
  };

  void vInit()
  {
    cSysTick::vInit(0);
    start();
  }

  void start()
  {
    mu32LastTimerValue = cSysTick::u32Get_us();
  };

  // Gibt die seit dem letzen Aufruf vergange Zeit zurück
  // us
  u32 get()
  {
    u32 lu32Diff;
    u32 lu32Temp;

    lu32Temp = cSysTick::u32Get_us();
    lu32Diff = cTimer32Tools::u32Diff(mu32LastTimerValue, lu32Temp);
    mu32LastTimerValue = lu32Temp;

    return lu32Diff;
  };

  // Überprüft, ob die gegebene Zeit [us] erreicht wurde
  bool isValue(u32 lu32Value)
  {
    u32 lu32Diff;
    u32 lu32Temp;

    lu32Temp = cSysTick::u32Get_us();
    lu32Diff = cTimer32Tools::u32Diff(mu32LastTimerValue, lu32Temp);

    if (lu32Diff > lu32Value)
    {
      mu32LastTimerValue = lu32Temp;
      return True;
    }
    return False;
  }

  void vDelay_us(u16 lu16Delay_us)
  {
    start();
    while (!(isValue(lu16Delay_us)))
    {
    }
  }

  char* toString()
  {
    static char mszString[10];
    cStrTools::uixItoan(get(), mszString, 10, 10);
    return mszString;
  };
};

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_H__
