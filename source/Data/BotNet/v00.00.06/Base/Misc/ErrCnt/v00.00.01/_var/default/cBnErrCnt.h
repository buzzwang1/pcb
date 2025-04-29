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
    enErrCnt          // 9
  }tenErr;

  u16 muErrCnt[enErrCnt];

  cBnErrCnt()
  {
    vReset();
  }

  void vReset()
  {
    for (u8 lu8Err = 0; lu8Err < (u8)enErrCnt; lu8Err++)
    {
      muErrCnt[lu8Err] = 0;
    }
  }

  void vInc(tenErr lenErr)
  {
    muErrCnt[(u8)lenErr]++;
  }

  void vGet(tenErr lenErr, cStr& lcParam)
  {
    switch (lenErr)
    {
      case enStuck:          lcParam.Setf("%s%d", (rsz)"Stuck: ",     muErrCnt[(u8)lenErr]); break;
      case enNack:           lcParam.Setf("%s%d", (rsz)"Nack: ",      muErrCnt[(u8)lenErr]); break;
      case enTimeout:        lcParam.Setf("%s%d", (rsz)"Timeout: ",   muErrCnt[(u8)lenErr]); break;
      case enStartWhileBusy: lcParam.Setf("%s%d", (rsz)"SWB: ",       muErrCnt[(u8)lenErr]); break;
      case enMisc:           lcParam.Setf("%s%d", (rsz)"Misc: ",      muErrCnt[(u8)lenErr]); break;
      case enRxSync:         lcParam.Setf("%s%d", (rsz)"RxSync: ",    muErrCnt[(u8)lenErr]); break;
      case enRxChkSum:       lcParam.Setf("%s%d", (rsz)"RxChkSum: ",  muErrCnt[(u8)lenErr]); break;
      case enRxTimeout:      lcParam.Setf("%s%d", (rsz)"RxTimeout: ", muErrCnt[(u8)lenErr]); break;
      case enRxBusy:         lcParam.Setf("%s%d", (rsz)"RxBusyv: ",   muErrCnt[(u8)lenErr]); break;
      default:               lcParam = ""; break;
    }
  }
};



#endif // __BOTNET_ERRORCOUNTER_H__
