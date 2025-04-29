
#ifndef __SYS_PKG_BASE_H__
#define __SYS_PKG_BASE_H__

#include "TypeDef.h"
#include "cStrT.h"


class SysPkgBase
{
  public:

  virtual void vInit() {}
  virtual void vTick1msHp() {}
  virtual bool isError(cStr& lszErrorInfo) { UNUSED(lszErrorInfo); return False; }
  virtual bool isReadyForSleep(cStr& lcStatus) { UNUSED(lcStatus); return True; }
  virtual void vProcess(u32 lu32DiffTime) { UNUSED(lu32DiffTime); }
};



#endif // __SYS_PKG_BASE_H__
