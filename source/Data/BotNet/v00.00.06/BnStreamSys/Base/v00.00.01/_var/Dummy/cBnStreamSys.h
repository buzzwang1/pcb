#ifndef __BOTNET_STREAM_SYS_H__
#define __BOTNET_STREAM_SYS_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
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

  bool bPut(cbArrayExtT<uint16> *lcEntry)
  {
    UNUSED(lcEntry);
    return True;
  }

  void get(cbArrayExtT<uint16> *lcEntry)
  {
    UNUSED(lcEntry);
  }

  void vTick10ms()
  {

  }

  void vProcess()
  {
  }
};


#endif // __BOTNET_STREAM_SYS_H__
