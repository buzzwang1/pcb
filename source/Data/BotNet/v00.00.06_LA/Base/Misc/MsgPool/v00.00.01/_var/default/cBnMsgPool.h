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

  class cMsg
  {
    public:
      u8 muCnt;
      tunStat munStat;

      cMsg()
      {
        muCnt = 0;
        //muLen = 0;
        munStat.u8Stat = 0;
      }
  };

  class Msg16B : public cMsg
  {
    public:
      u8 mu8Buf[(cBotNet_MsgSize / 4)];
      cMsg mcMsg;                // 3/4 Byte
      cBotNetMsg_Base mcMsgBase; // Base 2/4 Frame 5/8

      Msg16B()
        : cMsg(),
          mcMsgBase(mu8Buf, sizeof(mu8Buf))
      {
      }
  };

  class Msg32B : public cMsg
  {
    public:
      u8 mu8Buf[(cBotNet_MsgSize / 2)];
      cMsg mcMsg;
      cBotNetMsg_Base mcMsgBase;

      Msg32B()
        : cMsg(),
          mcMsgBase(mu8Buf, sizeof(mu8Buf))
      {
      }
  };

  class Msg64B : public cMsg
  {
    public:
      u8 mu8Buf[(cBotNet_MsgSize + 1)]; // + 1 für Checksumme
      cMsg mcMsg;
      cBotNetMsg_Base mcMsgBase;

      Msg64B()
        : cMsg(),
          mcMsgBase(mu8Buf, sizeof(mu8Buf))
      {
      }
  };

  static Msg16B mcMsg16B_Pool[16];
  static Msg32B mcMsg32B_Pool[8];
  static Msg64B mcMsg64B_Pool[8];

  static u8 mcMsg16BBuf[16];
  static u8 mcMsg32BBuf[8];
  static u8 mcMsg64BBuf[8];

  static cRingBufT<uint8, uint16> mcMsg16B;
  static cRingBufT<uint8, uint16> mcMsg32B;
  static cRingBufT<uint8, uint16> mcMsg64B;

  cBnMsgPool()
  {
  }

  static void vInit()
  {
    u8 lu8t;
    // Alle RingBuffer initialisieren
    //  - Idx.  32..47:  16x 16 Byte Memory
    //  - Idx.  64..71:   8x  32 Byte Memory
    //  - Idx. 128..137:  8x  64 Byte Memory
    for (lu8t = 32;  lu8t <  32 + 16; lu8t++) mcMsg16B.put_unsafe(lu8t);
    for (lu8t = 64;  lu8t <  64 +  8; lu8t++) mcMsg32B.put_unsafe(lu8t);
    for (lu8t = 128; lu8t < 128 +  8; lu8t++) mcMsg64B.put_unsafe(lu8t);
  }

  static u8 DecodeIdx(u8 lu8PoolIdx, u8 &lu8IdxMsg)
  {
    if (lu8PoolIdx >= 128)
    {
      lu8IdxMsg  = lu8PoolIdx - 128;
      return 3;
    }

    if (lu8PoolIdx >= 64)
    {
      lu8IdxMsg = lu8PoolIdx - 64;
      return 2;
    }

    if (lu8PoolIdx >= 32)
    {
      lu8IdxMsg = lu8PoolIdx - 32;
      return 1;
    }
    return 0;
  }

  static u8 u8ReqMsg(u16 lu16ReqLen)
  {
    u8 lu8PoolIdx = 0;
    if (lu16ReqLen <= (cBotNet_MsgSize / 4))
    {
      lu8PoolIdx = mcMsg16B.get();
      if (lu8PoolIdx)
      {
        mcMsg16B_Pool[lu8PoolIdx - 32].muCnt++;
        mcMsg16B_Pool[lu8PoolIdx - 32].mcMsgBase.muiLen = 0;
        mcMsg16B_Pool[lu8PoolIdx - 32].mcMsgBase.vClearAge();
        mcMsg16B_Pool[lu8PoolIdx - 32].munStat.u8Stat = 0;
        return lu8PoolIdx;
      }
    }

    if (lu16ReqLen <= (cBotNet_MsgSize / 2))
    {
      lu8PoolIdx = mcMsg32B.get();
      if (lu8PoolIdx)
      {
        mcMsg32B_Pool[lu8PoolIdx - 64].muCnt++;
        mcMsg32B_Pool[lu8PoolIdx - 64].mcMsgBase.muiLen = 0;
        mcMsg32B_Pool[lu8PoolIdx - 64].mcMsgBase.vClearAge();
        mcMsg32B_Pool[lu8PoolIdx - 64].munStat.u8Stat = 0;
        return lu8PoolIdx;
      }
    }

    if (lu16ReqLen <= (cBotNet_MsgSize + 1))
    {
      lu8PoolIdx = mcMsg64B.get();
      if (lu8PoolIdx)
      {
        mcMsg64B_Pool[lu8PoolIdx - 128].muCnt++;
        mcMsg64B_Pool[lu8PoolIdx - 128].mcMsgBase.muiLen = 0;
        mcMsg64B_Pool[lu8PoolIdx - 128].mcMsgBase.vClearAge();
        mcMsg64B_Pool[lu8PoolIdx - 128].munStat.u8Stat = 0;
      }
    }
    return lu8PoolIdx;
  }

  static void vReleaseMsg(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        if (mcMsg16B_Pool[lu8IdxMsg].muCnt)
        {
          mcMsg16B_Pool[lu8IdxMsg].muCnt--;
          if (mcMsg16B_Pool[lu8IdxMsg].muCnt == 0)
          {
            mcMsg16B.put(lu8IdxMsg);
          }
        }
        break;
      case 2:  // 32 Byte
        if (mcMsg32B_Pool[lu8IdxMsg].muCnt)
        {
          mcMsg32B_Pool[lu8IdxMsg].muCnt--;
          if (mcMsg32B_Pool[lu8IdxMsg].muCnt == 0)
          {
            mcMsg32B.put(lu8IdxMsg);
          }
        }
        break;
      case 3:  // 65 Byte
        if (mcMsg64B_Pool[lu8IdxMsg].muCnt)
        {
          mcMsg64B_Pool[lu8IdxMsg].muCnt--;
          if (mcMsg64B_Pool[lu8IdxMsg].muCnt == 0)
          {
            mcMsg64B.put(lu8IdxMsg);
          }
        }
        break;
    }
  }

  static u8 u8Len(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        return mcMsg16B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        break;
      case 2:  // 32 Byte
        return mcMsg32B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        break;
      case 3:  // 65 Byte
        return mcMsg64B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        break;
    }

    return 0;
  }

  static void vPutMsg(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        mcMsg16B_Pool[lu8IdxMsg].muCnt++;
        break;
      case 2:  // 32 Byte
        mcMsg32B_Pool[lu8IdxMsg].muCnt++;
        break;
      case 3:  // 65 Byte
        mcMsg64B_Pool[lu8IdxMsg].muCnt++;
        break;
    }
  }


  static void vPutMsgChk(u8 lu8PoolIdx)
  {
    vAddChecksum(lu8PoolIdx);
    vPutMsg(lu8PoolIdx);
  }


  static void vGetMsgBAry(u8 lu8PoolIdx, cbArrayExtT<uint16>* lcMsg)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        lcMsg->muiLen   = mcMsg16B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        lcMsg->mpu8Data = mcMsg16B_Pool[lu8PoolIdx].mcMsgBase.mpu8Data;
        lcMsg->muiSize  = mcMsg16B_Pool[lu8PoolIdx].mcMsgBase.muiSize;
        break;
      case 2:  // 32 Byte
        lcMsg->muiLen   = mcMsg32B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        lcMsg->mpu8Data = mcMsg32B_Pool[lu8PoolIdx].mcMsgBase.mpu8Data;
        lcMsg->muiSize  = mcMsg32B_Pool[lu8PoolIdx].mcMsgBase.muiSize;
        break;
      case 3:  // 65 Byte
        lcMsg->muiLen   = mcMsg64B_Pool[lu8PoolIdx].mcMsgBase.muiLen;
        lcMsg->mpu8Data = mcMsg64B_Pool[lu8PoolIdx].mcMsgBase.mpu8Data;
        lcMsg->muiSize  = mcMsg64B_Pool[lu8PoolIdx].mcMsgBase.muiSize;
        break;
    }

    lcMsg->muiLen   = 0;
    lcMsg->mpu8Data = null;
    lcMsg->muiSize  = 0;
  }

  static bool isDecoded(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        return mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.Decoded;
        break;
      case 2:  // 32 Byte
        return mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.Decoded;
        break;
      case 3:  // 65 Byte
        return mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.Decoded;
        break;
    }
    return False;
  }

  static void vSetDecoded(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
    case 1:  // 16 Byte
      mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
      break;
    case 2:  // 32 Byte
      mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
      break;
    case 3:  // 65 Byte
      mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
      break;
    }
  }

  static void vDecode(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;

    if (isDecoded(lu8PoolIdx)) return;

    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        mcMsg16B_Pool[lu8IdxMsg].mcMsgBase.vDecode();
        mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
        break;
      case 2:  // 32 Byte
        mcMsg32B_Pool[lu8IdxMsg].mcMsgBase.vDecode();
        mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
        break;
      case 3:  // 65 Byte
        mcMsg64B_Pool[lu8IdxMsg].mcMsgBase.vDecode();
        mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.Decoded = 1;
        break;
    }
  }


  static void vGetMsg(u8 lu8PoolIdx, cBotNetMsg_Base** lcMsg)
  {
    u8 lu8IdxMsg;

    vRemoveChecksum(lu8PoolIdx);

    vDecode(lu8PoolIdx);

    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        *lcMsg = &mcMsg16B_Pool[lu8IdxMsg].mcMsgBase;
        break;
      case 2:  // 32 Byte
        *lcMsg = &mcMsg32B_Pool[lu8IdxMsg].mcMsgBase;
        break;
      case 3:  // 65 Byte
        *lcMsg = &mcMsg64B_Pool[lu8IdxMsg].mcMsgBase;
        break;
    }
    *lcMsg = null;
  }

  static bool HasChecksum(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;
    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        return mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.CheckSum;
        break;
      case 2:  // 32 Byte
        return mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.CheckSum;
        break;
      case 3:  // 65 Byte
        return mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.CheckSum;
        break;
    }
    return False;
  }


  static void vRemoveChecksum(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;

    if (!HasChecksum(lu8PoolIdx)) return;

    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        mcMsg16B_Pool[lu8IdxMsg].mcMsgBase.muiLen--;
        mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 0;
        break;
      case 2:  // 32 Byte
        mcMsg32B_Pool[lu8IdxMsg].mcMsgBase.muiLen--;
        mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 0;
        break;
      case 3:  // 65 Byte
        mcMsg64B_Pool[lu8IdxMsg].mcMsgBase.muiLen--;
        mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 0;
        break;
    }
  }


  static void vAddChecksum(u8 lu8PoolIdx)
  {
    u8 lu8IdxMsg;

    if (HasChecksum(lu8PoolIdx)) return;

    switch (DecodeIdx(lu8PoolIdx, lu8IdxMsg))
    {
      case 1:  // 16 Byte
        mcMsg16B_Pool[lu8IdxMsg].mu8Buf[mcMsg16B_Pool[lu8PoolIdx].mcMsgBase.muiLen] = 0xCC;
        mcMsg16B_Pool[lu8IdxMsg].mcMsgBase.muiLen++;
        mcMsg16B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 1;
        break;
      case 2:  // 32 Byte
        mcMsg32B_Pool[lu8IdxMsg].mu8Buf[mcMsg32B_Pool[lu8PoolIdx].mcMsgBase.muiLen] = 0xCC;
        mcMsg32B_Pool[lu8IdxMsg].mcMsgBase.muiLen++;
        mcMsg32B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 1;
        break;
      case 3:  // 65 Byte
        mcMsg64B_Pool[lu8IdxMsg].mu8Buf[mcMsg64B_Pool[lu8PoolIdx].mcMsgBase.muiLen] = 0xCC;
        mcMsg64B_Pool[lu8IdxMsg].mcMsgBase.muiLen++;
        mcMsg64B_Pool[lu8IdxMsg].munStat.stStat.CheckSum = 1;
        break;
    }
  }
};

#endif // __BOTNET_MSG_POOL_H__
