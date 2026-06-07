
#include "System.h"


cBnMsgProcessSystem::cBnMsgProcessSystem(cBotNet* lcBotNet)
  : cBotNet_MsgSysProcess(lcBotNet)
{
}

bool cBnMsgProcessSystem::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;
  UNUSED(lcMsg);

  return lbConsumed;
}


cSystem mcSys;

