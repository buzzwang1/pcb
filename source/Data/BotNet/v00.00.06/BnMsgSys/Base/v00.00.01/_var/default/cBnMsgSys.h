#ifndef __BOTNET_MSG_SYS_H__
#define __BOTNET_MSG_SYS_H__

#include "Typedef.h"
#include "cBnCfg.h"
#include "cBnMsg.h"

class cBotNet_MsgSysProcess
{
  public:

  virtual bool bMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn) = 0;
};


#endif // __BOTNET_MSG_SYS_H__
