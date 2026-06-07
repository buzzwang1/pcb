#ifndef __BOTNET_ERRORCOUNTER_H__
#define __BOTNET_ERRORCOUNTER_H__

#include "Typedef.h"
#include "cStrT.h"


#ifdef __GNUC__
class __attribute__((packed)) cBnErrCnt
#else
class cBnErrCnt
#endif
{
public:

  typedef enum
  {
    enStuck,          // 0
    enNack,           // 1
    enTimeout,        // 2
    enStartWhileBusy, // 3
    enMisc,           // 4
    enRxSync,         // 5
    enRxChkSum,       // 6
    enRxTimeout,      // 7
    enRxBusy,         // 8
    enRxNoMem,        // 9
    enErrCnt          // 10
  }tenErr;

  cBnErrCnt()
  {
    vReset();
  }

  void vReset()
  {
  }

  void vInc(tenErr lenErr)
  {
    UNUSED(lenErr);
  }

  void vGet(tenErr lenErr, cStr& lcParam)
  {
    UNUSED(lenErr);
    UNUSED(lcParam);
  }
};



#endif // __BOTNET_ERRORCOUNTER_H__
