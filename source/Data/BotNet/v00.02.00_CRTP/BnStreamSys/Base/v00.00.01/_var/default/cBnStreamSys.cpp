#include "cBnStreamSys.h"
#include "cBotnet.h"

cBotNetStreamSystem::cBotNetStreamSystem(cBotNet* lcBn)
  : mcRxComBuf(mcRxComBufBuf, sizeof(mcRxComBufBuf)), mcTxComBuf(mcTxComBufBuf, sizeof(mcTxComBufBuf)),
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

u8 cBotNetStreamSystem::u8PutIntCmd(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u8 luCmd, u8 luStream, u8* lpuData, u8 luSize)
{
  u8 lu8PoolIdx;
  cBotNetMsg_StreamProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, luSize + 10);
  if (lu8PoolIdx == 0) return 0;
  lcMsgTx.cPrepareCmd(luCmd, luStream).Set(lpuData, luSize);
  lcMsgTx.vEncode(lcSAdr, lcDAdr);
  cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.muLen);
  u8 lu8SpaceLeft = u8PutInt(lu8PoolIdx);
  cBnMsgPool::vReleaseMsg(lu8PoolIdx);
  return lu8SpaceLeft;
}

u8 cBotNetStreamSystem::u8PutIntData(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u8 luStream, u8* lpuData, u8 luSize)
{
  u8 lu8PoolIdx;
  cBotNetMsg_StreamProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, luSize + 10);
  if (lu8PoolIdx == 0) return 0;
  lcMsgTx.cPrepareData(luStream).Set(lpuData, luSize);
  lcMsgTx.vEncode(lcSAdr, lcDAdr);
  cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.muLen);
  u8 lu8SpaceLeft = u8PutInt(lu8PoolIdx);
  cBnMsgPool::vReleaseMsg(lu8PoolIdx);
  return lu8SpaceLeft;
}


