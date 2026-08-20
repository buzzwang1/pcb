#include "cBnMsgSys.h"
#include "cBotnet.h"


void cBotNet_MsgSysProcess::vAddMsgSys()
{
  this->mcMsgSysNext = mcBn->mcpMsgProcess;
  mcBn->mcpMsgProcess = this;
}


u8 cBotNet_MsgSysProcess::u8PutInt(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u16 liIdx, u8* lpuData, u8 luSize)
{
  u8 lu8PoolIdx;
  cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, luSize + 10);
  if (lu8PoolIdx == 0) return 0;
  lcMsgTx.cPrepare(liIdx).Set(lpuData, luSize);
  lcMsgTx.vEncode(lcSAdr, lcDAdr);
  cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.muLen);
  u8 lu8SpaceLeft = u8PutInt(lu8PoolIdx);
  cBnMsgPool::vReleaseMsg(lu8PoolIdx);
  return lu8SpaceLeft;
}
