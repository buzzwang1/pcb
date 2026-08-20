#ifndef __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__
#define __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

#define cBotNetMsgPortRRptSlots 8

class cBotNetMsgPortRRpt : public cBotNet_MsgSysProcess
{
public:


  cBotNetMsgPortRRpt(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet)
  {
    UNUSED(lcBotNet);
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    UNUSED(lcMsg);
    return False;
  }

  void vProcess10ms()
  {
  }
};


#endif // __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__
