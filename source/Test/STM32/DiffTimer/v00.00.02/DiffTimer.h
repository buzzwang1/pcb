
#ifndef __DIFFTIMER_H__
#define __DIFFTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "cStringTools.h"
#include "DiffTimerHw.h"

#ifdef __GNUC__
class __attribute__((packed)) cDiffTimer : public cDiffTimerHw
#else
class cDiffTimer: public cDiffTimerHw
#endif
{
  public:
  static u8  mu8Init;
  char mszString[10];

  u32  mu32LastTimerValue;

  cDiffTimer(u32 lu32InitCnt = 0)
  {
    if (mu8Init == 0)
    {
      mu8Init = 1;
      InitTimer(lu32InitCnt);
    }

    start();
  };

  void start()
  {
    mu32LastTimerValue = u32GetTimer();
  };

  // Gibt die seit dem letzen Aufruf vergange Zeit zurück
  // us
  u32 get()
  {
    u32 lu32Diff;
    u32 lu32Temp;

    lu32Temp = u32GetTimer();
    lu32Diff = diff(mu32LastTimerValue, lu32Temp);
    mu32LastTimerValue = lu32Temp;

    return lu32Diff;
  };

  // Überprüft, ob die gegebene Zeit [us] erreicht wurde
  bool isValue(u32 lu32Value)
  {
    u32 lu32Diff;
    u32 lu32Temp;

    lu32Temp = u32GetTimer();
    lu32Diff = diff(mu32LastTimerValue, lu32Temp);

    if (lu32Diff > lu32Value)
    {
      mu32LastTimerValue = lu32Temp;
      return True;
    }
    return False;
  }

  char* toString()
  {
    cStrTools::uixItoan(get(), mszString, 10, 10);
    return mszString;
  };

private:
  u32 diff(u32 lu32ValueOld, u32 lu32ValueNew)
  {
    u32 lu32Diff;

    if (lu32ValueNew >= lu32ValueOld)
    {
      return (lu32ValueNew - lu32ValueOld);
    }
    else
    {
      if (lu32ValueOld > U16MAX)
      {
        // Wahrscheinlich 32 Bit Timer
        lu32Diff = (U32MAX - (lu32ValueOld + 1)) + lu32ValueNew;
      }
      else
      {
        // Wahrscheinlich 16 Bit Timer
        lu32Diff = ((u32)U16MAX - (lu32ValueOld + 1)) + lu32ValueNew;
      }
    }
    return lu32Diff;
  }
};

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_H__
