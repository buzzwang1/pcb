
#ifndef __SYS_PKG_CLOCK_H__
#define __SYS_PKG_CLOCK_H__

#include "TypeDef.h"
#include "cClock.h"



class cSysPkgClock
{
  public:
  u32        mu32ClockResyncTimeout_s;

  cClock     mClock; // Clock muss vor Powermanager sein, weil der Powermanager die
                     // WUF- zurücksetzt.

  cSysPkgClock();

  void vInit1(void);
  void vInit2(void);

  bool isReadyForSleep(cStr& lcStatus);

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};


#endif // __SYS_PKG_CLOCK_H__
