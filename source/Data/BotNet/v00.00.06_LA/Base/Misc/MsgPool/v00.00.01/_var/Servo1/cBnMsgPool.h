#ifndef __BOTNET_MSG_POOL_H__
#define __BOTNET_MSG_POOL_H__

#include "Typedef.h"
#include "cChr8.h"
#include "cBnCfg.h"
#include "ComDat.h"
#include "cbArrayExtT.h"
#include "cRingBufT.h"
#include "cBnMsg.h"


class cRingBuf816
  //#endif
{
public:
  u8 mau8Buf[16];
  u8 mu8Size;
  u8 mu8Head;
  u8 mu8Cnt;
  u8 mu8Tail;

  cRingBuf816()
  {
    reset();
    mu8Size = 16;
  }

  inline void reset() { mu8Head = mu8Tail = mu8Cnt = 0; }

  bool isEmpty() { return mu8Cnt == 0; }
  bool isFull() { return (mu8Cnt == mu8Size); }
  u8 cnt() { return mu8Cnt; }
  u8 space_left() { return (mu8Size - mu8Cnt); }

  void inc(u8& lu8Val)
  {
    lu8Val = (lu8Val + 1) & 15;
  }

  void put(u8 lu8Item)
  {
    if (!isFull())
    {
      mau8Buf[mu8Head] = lu8Item;
      inc(mu8Head);
      mu8Cnt++;
    }
  }


  u8 get(void)
  {
    u8 lu8Zwerg;

    if (isEmpty())
    {
      return 0;
    }

    //Read data and advance the tail (we now have a free space)
    lu8Zwerg = mau8Buf[mu8Tail];
    inc(mu8Tail);
    mu8Cnt--;

    return lu8Zwerg;
  }
};


class cBnMsgPool
{
public:

  enum MsgConst
  {
    nCnt16B = cBotNet_MsgPool16B,
    nCnt32B = cBotNet_MsgPool32B,
    nCnt64B = cBotNet_MsgPool64B,
  };

  struct MsgStatus
  {
    u8 Decoded  : 1;
    u8 CheckSum : 1;
  };

  typedef union
  {
    u8 u8Stat;
    MsgStatus stStat;
  }tunStat;

  class cMsg_Base
  {
    public:
      cBotNetMsg_Base mcMsgBase;
      u8 muCnt;
      tunStat munStat;

      cMsg_Base(u8* lu8Buf, u16 lu16Size)
        : mcMsgBase(lu8Buf, lu16Size)
      {
        muCnt = 0;
        //muLen = 0;
        munStat.u8Stat = 0;
      }
  };

  template <u16 mu16MsgBufSize>
  class cMsg: public cMsg_Base
  {
    public:
      u8 mu8Buf[mu16MsgBufSize];

      cMsg()
        : cMsg_Base(mu8Buf, sizeof(mu8Buf))
      {
      }
  };

  class cMsgPoolBase
  {
    public:
      cRingBufT<uint8, uint16> mcMsgRingBuf;

      cMsgPoolBase(u8* lu8RingBufBuf, u16 lu16RingBufSize)
        : mcMsgRingBuf(lu8RingBufBuf, lu16RingBufSize)
      {
      }

      void vInit(u8 lu8Offset, u8 lu8MsgPoolBufSize)
      {
        u8 lu8t;
        for (lu8t = lu8Offset; lu8t < lu8Offset + lu8MsgPoolBufSize; lu8t++) mcMsgRingBuf.put_unsafe(lu8t);
      }

      virtual cMsg_Base* cGetMsg(u8 lu8Idx) = 0;

      u8 u8GetIdx()
      {
        return mcMsgRingBuf.get();
      }
  };


  template <u16 mu16MsgBufSize, u8 mu8MsgPoolBufSize, u8 lu8Offset>
  class cMsgPool: public cMsgPoolBase
  {
    public:
      cMsg<mu16MsgBufSize> mcMsgPool[mu8MsgPoolBufSize];
      u8 mcRingBufBuf[mu8MsgPoolBufSize];

      cMsgPool()
        : cMsgPoolBase((u8*)mcRingBufBuf, sizeof(mcRingBufBuf))
      {}

      void vInit()
      {
        cMsgPoolBase::vInit(lu8Offset, mu8MsgPoolBufSize);
      }

      cMsg_Base* cGetMsg(u8 lu8Idx) override
      {
        return &mcMsgPool[lu8Idx];
      }

      u8 vOffset()
      {
        return lu8Offset;
      }

      u8 vMsgSize()
      {
        return mu16MsgBufSize;
      }
  };

  #if cBotNet_MsgPool16B > 0
    static cMsgPool<(cBotNet_MsgSize / 4), nCnt16B, 1>                     mcMsgPool16B;
  #endif
  #if cBotNet_MsgPool32B > 0
    static cMsgPool<(cBotNet_MsgSize / 2), nCnt32B, 1 + nCnt16B>           mcMsgPool32B;
  #endif
  #if cBotNet_MsgPool64B > 0
    static cMsgPool<(cBotNet_MsgSize + 1), nCnt64B, 1 + nCnt16B + nCnt32B> mcMsgPool64B;
  #endif

  static cMsgPoolBase* mcMsgPool[1 + 3];

  cBnMsgPool()
  {
  }

  static void vInit()
  {
    #if cBotNet_MsgPool16B > 0
      mcMsgPool16B.vInit();
    #endif

    #if cBotNet_MsgPool32B > 0
      mcMsgPool32B.vInit();
    #endif

    #if cBotNet_MsgPool64B > 0
      mcMsgPool64B.vInit();
    #endif
  }

  static u8 DecodeIdx(u8 lu8PoolIdx, u8 &lu8IdxMsg)
  {
    #if cBotNet_MsgPool64B > 0
      if (lu8PoolIdx >= mcMsgPool64B.vOffset())
      {
        lu8IdxMsg = lu8PoolIdx - mcMsgPool64B.vOffset();
        return 3;
      }
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu8PoolIdx >= mcMsgPool32B.vOffset())
      {
        lu8IdxMsg = lu8PoolIdx - mcMsgPool32B.vOffset();
        return 2;
      }
    #endif

    #if cBotNet_MsgPool16B > 0
      if (lu8PoolIdx >= mcMsgPool16B.vOffset())
      {
        lu8IdxMsg = lu8PoolIdx - mcMsgPool16B.vOffset();
        return 1;
      }
    #endif

    return 0;
  }

  static cMsg_Base* cGetMsg(u8 lu8PoolIdx)
  {
    #if cBotNet_MsgPool64B > 0
      if (lu8PoolIdx >= mcMsgPool64B.vOffset())
      {
        u8 lu8IdxMsg = lu8PoolIdx - mcMsgPool64B.vOffset();
        return (cMsg_Base*)&mcMsgPool64B.mcMsgPool[lu8IdxMsg];
      }
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu8PoolIdx >= mcMsgPool32B.vOffset())
      {
        u8 lu8IdxMsg = lu8PoolIdx - mcMsgPool32B.vOffset();
        return (cMsg_Base*)&mcMsgPool32B.mcMsgPool[lu8IdxMsg];
      }
    #endif

    #if cBotNet_MsgPool16B > 0
      if (lu8PoolIdx >= mcMsgPool16B.vOffset())
      {
        u8 lu8IdxMsg = lu8PoolIdx - mcMsgPool16B.vOffset();
        return (cMsg_Base*)&mcMsgPool16B.mcMsgPool[lu8IdxMsg];
      }
    #endif

    return null;
  }

  static u8 u8ReqMsg(u16 lu16ReqLen)  // __attribute__((optimize("-O0")))
  {
    u8 lu8PoolIdx  = 0;
    u8 lu8PoolIdx2;
    cMsgPoolBase* lcMsgPool;

    #if cBotNet_MsgPool16B > 0
      if (lu16ReqLen <= mcMsgPool16B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool16B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();

          if (lu8PoolIdx)
          {
            lu8PoolIdx2 = lu8PoolIdx - mcMsgPool16B.vOffset();

            cMsg_Base* lcMsg = lcMsgPool->cGetMsg(lu8PoolIdx2);
            lcMsg->muCnt++;
            lcMsg->mcMsgBase.muiLen = 0;
            lcMsg->mcMsgBase.vClearAge();
            lcMsg->munStat.u8Stat = 0;
            return lu8PoolIdx;
          }
        }
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu16ReqLen <= mcMsgPool32B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool32B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();

        if (lu8PoolIdx)
        {
          lu8PoolIdx2 = lu8PoolIdx - mcMsgPool32B.vOffset();

          cMsg_Base* lcMsg = lcMsgPool->cGetMsg(lu8PoolIdx2);
          lcMsg->muCnt++;
          lcMsg->mcMsgBase.muiLen = 0;
          lcMsg->mcMsgBase.vClearAge();
          lcMsg->munStat.u8Stat = 0;
          return lu8PoolIdx;
        }
      }
    #endif

   #if cBotNet_MsgPool64B > 0
      if (lu16ReqLen <= mcMsgPool64B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool64B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();

        if (lu8PoolIdx)
        {
          lu8PoolIdx2 = lu8PoolIdx - mcMsgPool64B.vOffset();

          cMsg_Base* lcMsg = lcMsgPool->cGetMsg(lu8PoolIdx2);
          lcMsg->muCnt++;
          lcMsg->mcMsgBase.muiLen = 0;
          lcMsg->mcMsgBase.vClearAge();
          lcMsg->munStat.u8Stat = 0;
          return lu8PoolIdx;
        }
      }
    #endif

    return lu8PoolIdx;
  }

  static void vReleaseMsg(u8 &lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    if (lu8PoolIdx)
    {
      u8 lu8Idx = DecodeIdx(lu8PoolIdx, lu8IdxMsg);

      if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt)
      {
        mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt--;
        if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt == 0)
        {
          mcMsgPool[lu8Idx]->mcMsgRingBuf.put(lu8PoolIdx);
          lu8PoolIdx = 0;
        }
      }
    }
  }

  static void vDeleteMsg(u8 &lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    if (lu8PoolIdx)
    {
      u8 lu8Idx = DecodeIdx(lu8PoolIdx, lu8IdxMsg);
      if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt)
      {
        mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt = 0;
        mcMsgPool[lu8Idx]->mcMsgRingBuf.put(lu8PoolIdx);
      }
    }
    lu8PoolIdx = 0;
  }


  static void vSetLen(u8 lu8PoolIdx, u16 lu16ReqLen)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->mcMsgBase.muiLen = lu16ReqLen;
    }
  }

  static u8 u8Len(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      return lcMsg->mcMsgBase.muiLen;
    }
    return 0;
  }

  static void vPutMsg(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->muCnt++;
    }
  }


  static void vPutMsgChk(u8 lu8PoolIdx)
  {
    vAddChecksum(lu8PoolIdx);
    vPutMsg(lu8PoolIdx);
  }


  static void vGetMsgBAry(u8 lu8PoolIdx, cbArrayExtT<uint16>* lcMsgRet)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsgRet->muiLen   = lcMsg->mcMsgBase.muiLen;
      lcMsgRet->mpu8Data = lcMsg->mcMsgBase.mpu8Data;
      lcMsgRet->muiSize  = lcMsg->mcMsgBase.muiSize;
    }
    else
    {
      lcMsgRet->muiLen = 0;
      lcMsgRet->mpu8Data = null;
      lcMsgRet->muiSize = 0;
    }
  }

  static bool isDecoded(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      return lcMsg->munStat.stStat.Decoded;
    }
    return False;
  }

  static void vSetDecoded(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->munStat.stStat.Decoded = 1;
    }
  }

  static void vDecode(u8 lu8PoolIdx)
  {
    if (isDecoded(lu8PoolIdx)) return;

    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->mcMsgBase.vDecode();
      lcMsg->munStat.stStat.Decoded = 1;
    }
  }


  static void vGetMsg(u8 lu8PoolIdx, cBotNetMsg_Base** lcMsgRet)
  {
    vRemoveChecksum(lu8PoolIdx);
    vDecode(lu8PoolIdx);

    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      *lcMsgRet = &lcMsg->mcMsgBase;
    }
    else
    {
      *lcMsgRet = null;
    }
  }

  static bool HasChecksum(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      return lcMsg->munStat.stStat.CheckSum;
    }
    else
    {
      return False;
    }
  }


  static void vRemoveChecksum(u8 lu8PoolIdx)
  {
    if (!HasChecksum(lu8PoolIdx)) return;

    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->mcMsgBase.muiLen--;
      lcMsg->munStat.stStat.CheckSum = 0;
    }
  }


  static void vAddChecksum(u8 lu8PoolIdx)
  {
    if (HasChecksum(lu8PoolIdx)) return;

    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->mcMsgBase.mpu8Data[lcMsg->mcMsgBase.muiLen] = 0xCC;
      lcMsg->mcMsgBase.muiLen++;
      lcMsg->munStat.stStat.CheckSum = 1;
    }
  }

  static void vSetChecksum(u8 lu8PoolIdx)
  {
    if (lu8PoolIdx)
    {
      cMsg_Base* lcMsg = cGetMsg(lu8PoolIdx);
      lcMsg->munStat.stStat.CheckSum = 1;
    }
  }
};

#endif // __BOTNET_MSG_POOL_H__
