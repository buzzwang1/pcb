#ifndef __BOTNET_MESSAGE_SYS_BTR_H__
#define __BOTNET_MESSAGE_SYS_BTR_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

class cBotNetMsgPortBtr : public cBotNet_MsgSysProcess
{
public:

  u8    mu8Btr[cBnMsgSysBtrCfgRamSize] __attribute__((aligned(8)));
  u8*   mu8BtrIdx;
  u32   mu32Index2;
  u32   mu32IndexMax;
  u32   mu32Size;
  u32   mu32Checksum;

  bool  mbIsReadInProgress;
  u8    mu8PackageCounter;

  u8 mu8MsgTx_Buf[cBotNet_MsgSize];
  cBarryPtrT<u16> mcTxMsgTx;
  cBotNetAdress mcTxDAdr;

  cBotNetMsgPortBtr(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet),
      mcTxMsgTx(mu8MsgTx_Buf, cBotNet_MsgSize)
  {
    mu8BtrIdx      = mu8Btr;
    mu32Index2     = 0;
    mu32IndexMax   = 0;
    mu32Checksum   = 1;
    mu32Size       = cBnMsgSysBtrCfgRamSize;
    mbIsReadInProgress = 0;
  }


  uint32 ui32ChkSum(uint8* lpui8Src, uint32 lui32Len)
  {
    uint32 lui32Checksum;

    lui32Checksum = 0;
    while (lui32Len > 0)
    {
      lui32Checksum += *((uint8*)lpui8Src);
      (uint8*)lpui8Src++;
      lui32Len--;
    }
    return (lui32Checksum + 1);
  };

  u32 vMemCopy(const uint8* lpvSrc, uint8* lpvDst, uint32 lui32Len)
  {
    u32 lu32Checksum;

    lu32Checksum = 0;
    while (lui32Len > 0)
    {
      *((uint8*)lpvDst) = *((uint8*)lpvSrc);
      lu32Checksum += *((uint8*)lpvSrc);
      (uint8*)lpvDst++;
      (uint8*)lpvSrc++;
      lui32Len--;
    }
    return lu32Checksum;
  };

  inline u8* u8U32toU8(u32 lu32Val, u8* lu8Array)
  {
    lu8Array[3] = (lu32Val & 0xFF); lu32Val >>= 8;
    lu8Array[2] = (lu32Val & 0xFF); lu32Val >>= 8;
    lu8Array[1] = (lu32Val & 0xFF); lu32Val >>= 8;
    lu8Array[0] = (lu32Val & 0xFF);
    return lu8Array;
  }

  inline u32 u32U8toU32(u8* lu8Array)
  {
    return (lu8Array[0] << 24) + (lu8Array[1] << 16) + (lu8Array[2] << 8) + (lu8Array[3]);
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
  void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_BTR_H__
