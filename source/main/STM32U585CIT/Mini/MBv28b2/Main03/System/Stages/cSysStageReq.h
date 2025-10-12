#ifndef __SYSTEM_STAGE_REQUEST_H__
#define __SYSTEM_STAGE_REQUEST_H__

#include "TypeDef.h"

#ifdef __cplusplus
  extern "C" {
#endif


class cSysStageReq
{
  public:

  u8 mu8Requests;

  cSysStageReq()
  {
    vReset();
  }

  void vReset()
  {
    mu8Requests = 0;
  }

  void vStart()
  {
    mu8Requests++;
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_REQUEST_H__
