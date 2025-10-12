#ifndef SYS_TICK_H
#define SYS_TICK_H

#include "Typedef.h"
#include "DiffTimer.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cSysTick : public cDiffTimer
{
  public:
  u32 mu32Tick_ms;

  cSysTick(u32 lu32InitCnt_us = 0)
    : cDiffTimer(lu32InitCnt_us, False)
  {
    mu32Temp_us = 0;
    mu32Tick_ms = 0;
  };

  void vDoProcess()
  {
    mu32Temp_us += get();


    if (mu32Temp_us > 1000)
    {
      mu32Tick_ms += mu32Temp_us / 1000;
      mu32Temp_us = mu32Temp_us % 1000;
    }    
  }
  private:
  u32 mu32Temp_us;
};

#ifdef __cplusplus
}
#endif

#endif // SYS_TICK_H
