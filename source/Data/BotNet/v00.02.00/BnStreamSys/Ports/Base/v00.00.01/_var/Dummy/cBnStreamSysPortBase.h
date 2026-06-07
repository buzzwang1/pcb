#ifndef __BOTNET_STREAM_SYS_PORT_BASE_H__
#define __BOTNET_STREAM_SYS_PORT_BASE_H__

#include "Typedef.h"

#include "cRingBufT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

class cBotnet;

class cBotNetStreamPort
{
  public:


  cBotNetStreamPort()
  {
  }

  virtual void vAddedToBn(cBotNet* lcBotNet)
  {
    UNUSED(lcBotNet);
  }

  virtual void vTick10ms() {}; // 20250609: Scheint keiner zu nutzen
  virtual void vProcess()    = 0;
};


#endif // __BOTNET_STREAM_SYS_PORT_BASE_H__
