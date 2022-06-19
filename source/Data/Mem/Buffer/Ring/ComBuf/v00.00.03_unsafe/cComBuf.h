#ifndef __CLASS_COM_BUFFER_H__
#define __CLASS_COM_BUFFER_H__


#include "typedef.h"
#include  "cRingBufT.h"
#include  "cBArrayT.h"
#include  "cBArrayExtT.h"

#define cComBufMsgT_pui8GETMEM(x)  (new uint8[x])
#define cComBufMsgT_vFREEMEM(x)    (delete[] x)
#define cComBufMsgT_memcpy         MEMTOOLS_vMemCpy
#define cComBufMsgT_memcmp         MEMTOOLS_iMemCmp

#define cComBufMsgT_UINTMAX        ((1 << (sizeof(tybArrayIdxrT) * 8)) - 1)
// uint8 => (1 << (sizeof(uint8) * 8)) - 1
// uint8 => (1 << 8) - 1 => 255



class cComBufEntry : public cbArrayT<uint16>
{};


#ifdef __GNUC__
class __attribute__((packed)) cComBuf : public cRingBufDynT<uint8, uint16>
#else
class cComBuf : public cRingBufDynT<uint8, uint16>
#endif
{
  public:

  uint16 mui16MsgCnt;

  cComBuf(uint16 luSize)
    : cRingBufDynT<uint8, uint16>(luSize)
  {
    mui16MsgCnt = 0;
  }

  inline uint32 SizeOf()
  {
    return sizeof(cComBuf) + cRingBufDynT<uint8, uint16>::mtySize * sizeof(uint8);
  }

  inline bool isMsgIn()
  {
    return mui16MsgCnt > 0;
  }

  inline cComBufEntry* GetPtrNextMsg()
  {
    if (isMsgIn())
    {
      return (cComBufEntry*)(&mtyBuf[mtyTail]);
    }
    return null;
  }

  bool isFitting(cbArrayExtT<uint16> *lcEntry)
  {
    return (cRingBufDynT<uint8, uint16>::space_left() >= (lcEntry->muiLen + 2));  
  }


  void put(cbArrayExtT<uint16> *lcEntry)
  {
    if (lcEntry->muiLen > 0)
    {
      _dai();
      cRingBufDynT<uint8, uint16>::put_unsafe((uint8*)&lcEntry->muiLen,   sizeof(uint16),  False);
      cRingBufDynT<uint8, uint16>::put_unsafe((uint8*)lcEntry->mpu8Data , lcEntry->muiLen, False);
      mui16MsgCnt++;
      _eai();
    }
  }

  uint16 readLenght()
  {
    _dai();
    uint16 luLen;
    cRingBufDynT<uint8, uint16>::get_unsafe((uint8*)&luLen, sizeof(uint16));
    _eai();
    return luLen;
  }

  /*
  u32 getwithsum(cbArrayExtT<uint16>* lcEntry)
  {
    uint32 luSum = 0;
    _dai();

    if (isMsgIn())
    {
      uint16 luLen;

      cRingBufDynT<uint8, uint16>::get_unsafe((uint8*)&luLen, sizeof(uint16));

      lcEntry->muiLen = luLen;
      if (lcEntry->muiLen > lcEntry->muiSize)
      {
        // Not good! Not enough space for the message
        lcEntry->muiLen = lcEntry->muiSize;
        luSum = cRingBufDynT<uint8, uint16>::getwithsum_unsafe((uint8*)lcEntry->mpu8Data, lcEntry->muiLen);
        luLen -= lcEntry->muiLen;
        while (luLen)
        {
          cRingBufDynT<uint8, uint16>::get_unsafe();
          luLen--;
        }
      }
      else
      {
        luSum = cRingBufDynT<uint8, uint16>::getwithsum_unsafe((uint8*)lcEntry->mpu8Data, lcEntry->muiLen);
      }
      mui16MsgCnt--;
    }
    else
    {
      lcEntry->muiLen = 0;
    }
    _eai();
    return luSum;
  }*/

  void get(cbArrayExtT<uint16> *lcEntry)
  {
    _dai();

    if (isMsgIn())
    {
      uint16 luLen;

      cRingBufDynT<uint8, uint16>::get_unsafe((uint8*)&luLen, sizeof(uint16));

      lcEntry->muiLen = luLen;

      // !!!Unsave: Commented to save some bytes. 50Byte less
      /*if (lcEntry->muiLen > lcEntry->muiSize)
      {
        // Not good! Not enough space for the message
        lcEntry->muiLen = lcEntry->muiSize;
        cRingBufDynT<uint8, uint16>::get_unsafe((uint8*)lcEntry->mpu8Data, lcEntry->muiLen);
        luLen -= lcEntry->muiLen;
        while (luLen)
        {
          cRingBufDynT<uint8, uint16>::get_unsafe();
          luLen--;
        }
      }
      else*/
      {
        cRingBufDynT<uint8, uint16>::get_unsafe((uint8*)lcEntry->mpu8Data, lcEntry->muiLen);
      }
      mui16MsgCnt--;
    }
    else
    {
      lcEntry->muiLen = 0;
    }
    
    _eai();
  }
};



#endif //__CLASS_COM_BUFFER_H__
