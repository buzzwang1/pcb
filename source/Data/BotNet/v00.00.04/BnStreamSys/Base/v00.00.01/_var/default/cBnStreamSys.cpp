#include "cBnStreamSys.h"
#include "cBotnet.h"

cBotNetStreamSystem::cBotNetStreamSystem(cBotNet* lcBn)
  : mcRxComBuf(cBotNet_StreamSysComBufSize), mcTxComBuf(cBotNet_StreamSysComBufSize),
    mcMsgRx(cBotNet_MsgSize), mcMsgStreamRx(&mcMsgRx),
    mcMsgTx(cBotNet_MsgSize), mcMsgStreamTx(&mcMsgTx),
    mcCmdPort(lcBn)
{
  mcBotNet = lcBn;
  mcAdr.Set(lcBn->mcAdr.Get());

  for (int i = 0; i < cBotNet_StreamSysPortsCnt; i++)
  {
    mcPorts[i] = null;
    mu8PortCnt = 0;
  }

  AddPort(&mcCmdPort);
}




