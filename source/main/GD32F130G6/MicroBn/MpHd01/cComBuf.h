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



class __attribute__((packed)) cComBuf
{
  public:

  u8               cDataBuf[4 * 64];
  cbArrayExtT<u16> cData[4];

  u8 mu8Head;
  u8 mu8Cnt;
  u8 mu8Tail;

  cComBuf()
  {
    reset();
    cData[0].Set(cDataBuf + 0 * 64, 64);
    cData[1].Set(cDataBuf + 1 * 64, 64);
    cData[2].Set(cDataBuf + 2 * 64, 64);
    cData[3].Set(cDataBuf + 4 * 64, 64);
  }

  inline void reset()
  {
    mu8Head = mu8Tail = mu8Cnt = 0;
  }

  inline void inc(u8 &lVal)
  {
    lVal++;
    if (lVal >= 4) lVal = 0;
  }

  inline bool isMsgIn()
  {
    return mu8Cnt > 0;
  }

  bool isFitting()
  {
    return (mu8Cnt < 4);
  }

  inline u8 space_left()
  {
    return (4 - mu8Cnt);
  }


  void put(cbArrayExtT<u16> *lcEntry)
  {
    if (lcEntry->muiLen > 0)
    {
      _dai();
      cData[mu8Head].Set(lcEntry->mpu8Data, lcEntry->muiSize);
      inc(mu8Head);
      mu8Cnt++;
      _eai();
    }
  }


  void get(cbArrayExtT<uint16> *lcEntry)
  {
    _dai();

    if (isMsgIn())
    {
      lcEntry->Set(cData[mu8Tail].mpu8Data, cData[mu8Tail].muiSize);
      inc(mu8Tail);
      mu8Cnt--;
    }
    else
    {
      lcEntry->muiLen = 0;
    }    
    _eai();
  }
};



#endif //__CLASS_COM_BUFFER_H__
