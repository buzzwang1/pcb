#include "cBnStreamSys.h"
#include "cBotnet.h"

cBotNetStreamSystem::cBotNetStreamSystem(cBotNet* lcBn)
  : mcRxComBuf(mcRxComBufBuf, sizeof(mcRxComBufBuf)), mcTxComBuf(mcTxComBufBuf, sizeof(mcTxComBufBuf)),
    mcMsgRx(mcMsgRxBuf, sizeof(mcMsgRxBuf)), mcMsgStreamRx(&mcMsgRx),
    mcMsgTx(mcMsgTxBuf, sizeof(mcMsgTxBuf)), mcMsgStreamTx(&mcMsgTx),
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




