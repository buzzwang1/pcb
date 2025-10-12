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


  // Intern
  bool bPutInt(u8 lu8PoolIdx)
  {
    if (mcTxComBuf.space_left())
    {
      mcTxComBuf.put(lu8PoolIdx);
      cBnMsgPool::vPutMsg(lu8PoolIdx);
      cBnMsgPool::vSetDecoded(lu8PoolIdx);
      return True;
    }
    else
    {
      return False;
    }
  }

  virtual bool bMsg(cBotNetMsg_MsgProt& lcMsg) = 0;


  virtual void vTick10ms() {};

  void vAddMsgSys();
};


#endif // __BOTNET_MSG_SYS_H__
