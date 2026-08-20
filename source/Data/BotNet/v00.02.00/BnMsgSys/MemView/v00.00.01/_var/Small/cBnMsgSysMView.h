#ifndef __BOTNET_MESSAGE_SYS_MEMVIEW_H__
#define __BOTNET_MESSAGE_SYS_MEMVIEW_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cJobHdl.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

#define cBotNetMsgPortMViewEntries 8

class cBotNetMsgPortMView : public cBotNet_MsgSysProcess
{
public:
  bool mbOpen;
  cBotNetMsgPortMView(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet)
  {
    mbOpen = False;
  }

  bool isOpen() {return mbOpen;}

  bool bMsg(cBotNetMsg_MsgProt& lcMsg) override;
  void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_MEMVIEW_H__
