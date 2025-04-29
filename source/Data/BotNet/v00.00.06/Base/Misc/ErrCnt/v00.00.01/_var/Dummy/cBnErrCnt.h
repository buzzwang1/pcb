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
    enStuck,
    enNack,
    enTimeout,
    enStartWhileBusy,
    enMisc,
    enRxSync,
    enRxChkSum,
    enRxTimeout,
    enRxBusy,
    enErrCnt
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
