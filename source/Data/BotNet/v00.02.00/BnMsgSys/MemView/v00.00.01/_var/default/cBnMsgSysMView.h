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
  u8    mu8MsgTx_Buf[cBotNet_MsgSize];
  cBarryPtrT<u16> mcTxMsgTx;

  u32 pau32EntryAdr[cBotNetMsgPortMViewEntries];
  u8  pau8EntrySize[cBotNetMsgPortMViewEntries];

  bool mbOpen;

  cBotNetMsgPortMView(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet),
      mcTxMsgTx(mu8MsgTx_Buf, cBotNet_MsgSize)
  {
    mbOpen = False;
  }

  bool isOpen() {return mbOpen;}

  bool bMsg(cBotNetMsg_MsgProt& lcMsg) override;
  void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_MEMVIEW_H__
