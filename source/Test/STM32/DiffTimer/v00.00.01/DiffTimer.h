
#ifndef __DIFFTIMER_H__
#define __DIFFTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "cStringTools.h"
#include "DiffTimerHw.h"

#ifdef __GNUC__
class __attribute__((packed)) cDiffTimer
#else
class cDiffTimer
#endif
{
  public:
  static u8  mu8Init;
  char mszString[10];

  u32       mu32LastTimerValue;

  cDiffTimer()
  {
    if (mu8Init == 0)
    {
      mu8Init = 0;
      InitTimer();
    }

    mu32LastTimerValue = u32GetTimer();
  };

  void start()
  {
    mu32LastTimerValue = u32GetTimer();
  };

  u32 get()
  {
    u32 lu32Diff;
    u32 lu32Temp;

    lu32Temp = u32GetTimer();
    lu32Diff = lu32Temp - mu32LastTimerValue;
    mu32LastTimerValue = lu32Temp;

    return lu32Diff;
  };

  char* toString()
  {
    cStrTools::uixItoan(get(), mszString, 10, 10);
    return mszString;
  };

};

#ifdef __cplusplus
}
#endif

#endif // __DIFFTIMER_H__
