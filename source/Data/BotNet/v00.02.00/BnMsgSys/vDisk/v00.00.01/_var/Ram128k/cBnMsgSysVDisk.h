#ifndef __BOTNET_MESSAGE_SYS_VDISK_H__
#define __BOTNET_MESSAGE_SYS_VDISK_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cJobHdl.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

class cBotNetMsgPortVDisk : public cBotNet_MsgSysProcess
{
public:
  static u8 mau8VDisk[1024 * 128];

  bool mbOpen;
  cBotNetMsgPortVDisk(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet)
  {
    mbOpen   = False;
  }

  bool isOpen() {return mbOpen;}

  bool bMsg(cBotNetMsg_MsgProt& lcMsg) override;
  void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_VDISK_H__
