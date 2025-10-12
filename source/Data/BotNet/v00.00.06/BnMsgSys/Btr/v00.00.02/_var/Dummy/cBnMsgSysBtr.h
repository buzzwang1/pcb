#ifndef __BOTNET_MESSAGE_SYS_BTR_H__
#define __BOTNET_MESSAGE_SYS_BTR_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

class cBotNetMsgPortBtr : public cBotNet_MsgSysProcess
{
public:

  cBotNetMsgPortBtr(cBotNet* lcBotNet)
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


#endif // __BOTNET_MESSAGE_SYS_BTR_H__
