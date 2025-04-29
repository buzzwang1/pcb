
#include "System.h"


cBnMsgProcessSystem::cBnMsgProcessSystem(cBotNet* lcBotNet)
  : cBotNet_MsgSysProcess(lcBotNet)
{
}

bool cBnMsgProcessSystem::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  UNUSED(lcMsg);
  bool lbConsumed = False;

  return lbConsumed;
}


cSystem mcSys;

