#ifndef __BOTNET_MSG_POOL_H__
#define __BOTNET_MSG_POOL_H__

#include "Typedef.h"
#include "cChr8.h"
#include "cBnCfg.h"
#include "ComDat.h"
#include "cRingBufT.h"
#include "cBnMsg.h"


class cBnMsgPool
{
public:

  enum MsgConst
  {
    nCnt16B = cBotNet_MsgPool16B,
    nCnt32B = cBotNet_MsgPool32B,
    nCnt64B = cBotNet_MsgPool64B,
  };

  class cPoolMsgAttrib
  {
    public:
    u8 muCnt;
    u8 muLen;

    u8* Data()
    {
      return (u8*)this + sizeof(cPoolMsgAttrib);
    }

    //cPoolMsgAttrib()
    //{
    //  muCnt = 0;
    //}
  };

  template <u16 mu16MsgBufSize>
  class cPoolMsg: public cPoolMsgAttrib
  {
    public:
      u8 mu8Buf[mu16MsgBufSize];

      //cPoolMsg()
      //  : cPoolMsgAttrib()
      //{
      //}
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

      virtual cPoolMsgAttrib* cGetMsg(u8 lu8Idx) = 0;

      u8 u8GetIdx()
      {
        return mcMsgRingBuf.get();
      }
  };


  template <u16 mu16MsgBufSize, u8 mu8MsgPoolBufSize, u8 lu8Offset>
  class cMsgPool: public cMsgPoolBase
  {
    public:
      cPoolMsg<mu16MsgBufSize> mcMsgPool[mu8MsgPoolBufSize];
      u8 mcRingBufBuf[mu8MsgPoolBufSize];

      cMsgPool()
        : cMsgPoolBase((u8*)mcRingBufBuf, sizeof(mcRingBufBuf))
      {}

      void vInit()
      {
        cMsgPoolBase::vInit(lu8Offset, mu8MsgPoolBufSize);
      }

      cPoolMsgAttrib* cGetMsg(u8 lu8Idx) override
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

  static u8 mu8DummyBuf[cBotNet_MsgSize + 1];

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

    //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    //GPIOA->MODER &= ~(3 << (2 * 1));
    //GPIOA->MODER |= (1 << (2 * 1));
    //GPIOA->BSRR = (1 << (16 + 1)); // PA01 = 0
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

    lu8IdxMsg = 0;

    return 0;
  }

  static u8 vGetPoolMsgSize(u8 lu8PoolIdx)
  {
    #if cBotNet_MsgPool64B > 0
      if (lu8PoolIdx >= mcMsgPool64B.vOffset())
      {
        return mcMsgPool64B.vMsgSize();
      }
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu8PoolIdx >= mcMsgPool32B.vOffset())
      {
        return mcMsgPool32B.vMsgSize();
      }
    #endif

    #if cBotNet_MsgPool16B > 0
      if (lu8PoolIdx >= mcMsgPool16B.vOffset())
      {
        return mcMsgPool16B.vMsgSize();
      }
      else
    #endif
    return 0;
  }

  static cPoolMsgAttrib* vGetPoolMsg(u8 lu8PoolIdx)
  {
    #if cBotNet_MsgPool64B > 0
      if (lu8PoolIdx >= mcMsgPool64B.vOffset())
      {
        return mcMsgPool64B.cGetMsg(lu8PoolIdx - mcMsgPool64B.vOffset());
      }
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu8PoolIdx >= mcMsgPool32B.vOffset())
      {
        return mcMsgPool32B.cGetMsg(lu8PoolIdx - mcMsgPool32B.vOffset());
      }
    #endif

    #if cBotNet_MsgPool16B > 0
      if (lu8PoolIdx >= mcMsgPool16B.vOffset())
      {
        return mcMsgPool16B.cGetMsg(lu8PoolIdx - mcMsgPool16B.vOffset());
      }
      else
    #endif
    return 0;
  }

  static void vReqMsg(cBarryPtrT<u16>& lcBnMsg, u8& lu8PoolIdx, u16 lu16ReqLen)  // __attribute__((optimize("-O0")))
  {
    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
    cMsgPoolBase* lcMsgPool;

    #if cBotNet_MsgPool16B > 0
      if (lu16ReqLen <= mcMsgPool16B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool16B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();
      }
      else
    #endif

    #if cBotNet_MsgPool32B > 0
      if (lu16ReqLen <= mcMsgPool32B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool32B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();
      }
      else
    #endif

    #if cBotNet_MsgPool64B > 0
      if (lu16ReqLen <= mcMsgPool64B.vMsgSize())
      {
        lcMsgPool = &mcMsgPool64B;
        lu8PoolIdx = lcMsgPool->u8GetIdx();
      }
      else
    #endif
    {
      lu8PoolIdx = 0;
    }

    if (lu8PoolIdx)
    {
      cPoolMsgAttrib* lcMsgAttr = vGetPoolMsg(lu8PoolIdx);
      lcMsgAttr->muCnt++;
      lcMsgAttr->muLen = 0;
    }

    vGetMsg(lcBnMsg, lu8PoolIdx);

    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
  }

  static void vReleaseMsg(u8 &lu8PoolIdx)
  {
    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
    if (lu8PoolIdx == 0) return;

    u8 lu8IdxMsg;
    u8 lu8Idx = DecodeIdx(lu8PoolIdx, lu8IdxMsg);

    if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt)
    {
      mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt--;
      if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt == 0)
      {
        mcMsgPool[lu8Idx]->mcMsgRingBuf.put(lu8PoolIdx);
      }
    }
    lu8PoolIdx = 0;
    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
  }

  static void vDeleteMsg(u8 &lu8PoolIdx)
  {
    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
    u8 lu8IdxMsg;
    u8 lu8Idx = DecodeIdx(lu8PoolIdx, lu8IdxMsg);
    if (mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt)
    {
      mcMsgPool[lu8Idx]->cGetMsg(lu8IdxMsg)->muCnt = 0;
      mcMsgPool[lu8Idx]->mcMsgRingBuf.put(lu8PoolIdx);
    }
    lu8PoolIdx = 0;
    //GPIOA->ODR ^= (1 << 1); // Toggle PA1
  }


  static void vSetLen(u8 lu8PoolIdx, u16 lu16ReqLen)
  {
    vGetPoolMsg(lu8PoolIdx)->muLen = lu16ReqLen;
  }

  static u8 u8Len(u8 lu8PoolIdx)
  {
    return vGetPoolMsg(lu8PoolIdx)->muLen;
  }

  static void vPutMsg(u8 lu8PoolIdx)
  {
    vGetPoolMsg(lu8PoolIdx)->muCnt++;
  }

  static void vAddByte(u8 lu8PoolIdx, u8 lu8Byte)
  {
    cPoolMsgAttrib* lcMsgAttr = vGetPoolMsg(lu8PoolIdx);
    lcMsgAttr->Data()[lcMsgAttr->muLen] = lu8Byte;
    lcMsgAttr->muLen++;
  }

  static void vGetMsg(cBarryPtrT<u16>& lcMsg, u8 lu8PoolIdx)
  {
    cPoolMsgAttrib* lcMsgAttr = vGetPoolMsg(lu8PoolIdx);
    if (lcMsgAttr)
    {
      //lpaArray, luLen, luSize
      lcMsg.From(lcMsgAttr->Data(), lcMsgAttr->muLen, vGetPoolMsgSize(lu8PoolIdx));
    }
    else
    {
      //Dummy
      lcMsg.From(mu8DummyBuf, 0, sizeof(mu8DummyBuf));
    }
  }
};

#endif // __BOTNET_MSG_POOL_H__
