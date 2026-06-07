#ifndef __BOTNET_STREAM_SYS_H__
#define __BOTNET_STREAM_SYS_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

#include "cBnStreamSysPortBase.h"


class cBotNet;

class cBotNetStreamSystem
{
  public:

  cBotNetStreamSystem(cBotNet* lcBn)
  {
    UNUSED(lcBn);
  }

  void ConnectPort(u8 lu8SourcePortIdx, u16 lu8DestPortAdr,  u8 lu8DestPortIdx)
  {
    UNUSED(lu8SourcePortIdx);
    UNUSED(lu8DestPortAdr);
    UNUSED(lu8DestPortIdx);
  }

  void ConnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    UNUSED(lu8SourcePortIdx);
    UNUSED(lu8DestPortIdx);
  }

  u16 AddPort(cBotNetStreamPort* lcPort)
  {
    UNUSED(lcPort);
    return 0xFFFF;
  }

  //Extern
  bool bPut(u8 lu8PoolIdx)
  {
    UNUSED(lu8PoolIdx);
    return True;
  }

  u8 u8Get()
  {
    return 0;
  }

  void vTick10ms()
  {

  }

  void vProcess()
  {
  }
};


#endif // __BOTNET_STREAM_SYS_H__
