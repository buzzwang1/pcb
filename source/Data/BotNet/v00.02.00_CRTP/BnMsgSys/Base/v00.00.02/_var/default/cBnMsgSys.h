#ifndef __BOTNET_MSG_SYS_H__
#define __BOTNET_MSG_SYS_H__

#include "Typedef.h"
#include "cBnCfg.h"
#include "cBnMsg.h"
#include "cBnMsgPool.h"

class cBotNet_MsgSysProcess
{
  public:

  cBotNet* mcBn;
  cBotNet_MsgSysProcess* mcMsgSysNext;

  u8       mcTxComBufBuf[16];
  cRingBufT<u8, u16>  mcTxComBuf;

  cBotNet_MsgSysProcess(cBotNet* lpcBn)
    : mcTxComBuf(mcTxComBufBuf, sizeof(mcTxComBufBuf))
  {
    mcMsgSysNext = null;
    mcBn = lpcBn;
  }

  //Extern
  u8 u8Get()
  {
    u8 lu8PoolIdx = mcTxComBuf.get();
    return lu8PoolIdx;
  }

  bool IsSpace()
  {
    return mcTxComBuf.space_left();
  }

  u8 u8PutInt(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u16 liIdx, u8* lpuData, u8 luSize);  // __attribute__((optimize("-O0")));

  // Intern
  u8 u8PutInt(u8 lu8PoolIdx)
  {
    if (mcTxComBuf.space_left())
    {
      mcTxComBuf.put(lu8PoolIdx);
      cBnMsgPool::vPutMsg(lu8PoolIdx);
    }
    return mcTxComBuf.space_left();
  }

  virtual bool bMsg(cBotNetMsg_MsgProt& lcMsg) = 0;


  virtual void vTick10ms() {};

  void vAddMsgSys();
};


#endif // __BOTNET_MSG_SYS_H__
