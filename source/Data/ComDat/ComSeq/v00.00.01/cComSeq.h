#pragma once

#include "Typedef.h"

#define cComSeqDelay(u16Delay_ms)    cEntry(nDELAY), cEntry((u16Delay_ms) >> 8), cEntry((u16Delay_ms) & 0xFF)
#define cComSeqBlockWr(u8Len, ...)   cWrite(nBLOCK), cEntry(u8Len), __VA_ARGS__
#define cComSeqPtrWr(u16Len, pu8Ptr) cWrite(nPTR),   cEntry((u16Len) >> 8), cEntry((u16Len) & 0xFF), cEntry((pu8Ptr) >> 24), cEntry((pu8Ptr) >> 16), cEntry((pu8Ptr) >> 8), cEntry((pu8Ptr) & 0xFF)

class cComSeq
{
  public:

  enum tenDefaultSequences : u8
  {
    nSEQ00Reset = 0,
    nSEQ01Init1,  // - Read All
    nSEQ02Init2,  // - Set Init values
    nSEQ04Enable,
    nSEQ05Disable,
    nSEQ06DeInit,
  };

  class cEntry
  {
    public:
    union tunEntry
    {
      u8 u8Value;
      struct
      {
        u8 Idx   : 7; // unteren 7 Bits, Bit: 0..6
        u8 Write : 1; // Bit 7, 0 =  read: 1 = write
      } Bits;

      tunEntry(const u8 lu8Value)
      {
        u8Value = lu8Value;
      }

      tunEntry(const u8 lu8Idx, const bool bWrite)
      {
        Bits.Write = bWrite;
        Bits.Idx = lu8Idx;
      }
    };

    /*const*/ tunEntry munEntry;

    cEntry(const u8 lu8Value)
      : munEntry(lu8Value)
    {}

    cEntry(const u8 lu8Idx, const bool bWrite)
      : munEntry(lu8Idx, bWrite)
    {}

    cEntry(const cEntry &lcCopy)
      : munEntry(lcCopy.munEntry.u8Value)
    {}

    u8 u8GetReg()
    {
      return munEntry.Bits.Idx;
    }

    bool isReg(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx < lu8RegCnt);
    }

    bool isRegAll(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt);
    }

    bool isRead()
    {
      return (!munEntry.Bits.Write);
    }

    bool isWrite()
    {
      return (munEntry.Bits.Write);
    }

    bool isPtr(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt + 3);
    }

    bool isBlock(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt + 2);
    }

    bool isDelay(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt + 1);
    }

    cEntry& operator=(const cEntry &lcCopy)
    {
      munEntry.u8Value = lcCopy.munEntry.u8Value;
      return *this;
    }
  };

  class cWrite : public cEntry
  {
    public:
    cWrite(const u8 lu8Idx)
      : cEntry(lu8Idx, True)
    {}
  };

  class cRead : public cEntry
  {
    public:
     cRead(const u8 lu8Idx)
      : cEntry(lu8Idx, False)
    {}
  };

  virtual cEntry* pacGet() { return null; }
  virtual u8      u8GetLen() { return 0; }
};

