#ifndef __BOTNET_MSG_SYS_H__
#define __BOTNET_MSG_SYS_H__

#include "Typedef.h"
#include "cBnCfg.h"
#include "cBnMsg.h"

class cBotNet_MsgSysProcess
{
  public:

  cBotNet* mcBn;
  cBotNet_MsgSysProcess* mcMsgSysNext;

  cBotNet_MsgSysProcess(cBotNet* lpcBn)
  {
    mcMsgSysNext = null;
    mcBn = lpcBn;
  }

  virtual bool bMsg(cBotNetMsg_MsgProt &lcMsg) = 0;
  void vAddMsgSys();  
};


#endif // __BOTNET_MSG_SYS_H__
