#ifndef __SYS_ERR_HANDLER_H__
#define __SYS_ERR_HANDLER_H__

#include "typedef.h"
#include "ErrDef.h"
#include "cBuRam.h"

class cErr
{
public:

  static tunErrSources *munErr;

  cErr()
  {
    vInit();
  }

  static void vInit()
  {
    cBuRam::vEnable();
    munErr = (tunErrSources*)(&cBuRam::mBuRam->u32Err);
  }

};

#endif /* __SYS_ERR_HANDLER_H__ */
