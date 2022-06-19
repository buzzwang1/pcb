#include "cBotNet.h"
#include "cBnStreamSysBlu.h"

cBotNetStreamSystemBlu::cBotNetStreamSystemBlu(cBotNet* lcBn)
  : mcRxComBuf(cBotNet_StreamSysComBufSize), mcTxComBuf(cBotNet_StreamSysComBufSize),
    mcMsgRx(cBotNet_MsgSize), mcMsgStreamRx(&mcMsgRx),
    mcMsgTx(cBotNet_MsgSize), mcMsgStreamTx(&mcMsgTx),
    mcCmdEcho()
{
  mcBotNet = lcBn;
  mcAdr.Set(lcBn->mcAdr.Get());

  for (int i = 0; i < cBotNet_StreamSysPortsCnt; i++)
  {
    mcPorts[i] = null;
    mu8PortCnt = 0;
  }

  AddPort(&mcCmdEcho);
}




