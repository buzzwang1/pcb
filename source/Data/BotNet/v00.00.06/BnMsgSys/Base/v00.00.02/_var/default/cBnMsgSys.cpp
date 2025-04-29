#include "cBnMsgSys.h"
#include "cBotnet.h"


void cBotNet_MsgSysProcess::vAddMsgSys()
{
  this->mcMsgSysNext = mcBn->mcpMsgProcess;
  mcBn->mcpMsgProcess = this;
}



