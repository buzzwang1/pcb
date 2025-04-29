#include "cBnMsgSysBtr.h"
#include "cBotnet.h"



bool cBotNetMsgPortBtr::bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn)
{
  bool lbConsumed = False;

  if (mbIsReadInProgress) return False;

  switch (lcMsg.mu16Idx)
  {
  case 0:
  {
    u16 lu16SubIdx = (lcMsg.mcPayload[0] << 8) + lcMsg.mcPayload[1];
    switch (lu16SubIdx)
    {
    case 0x0100: // BTR Info
      lbConsumed = True;
      {
        u8 lu8Temp[4];
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x00);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32Size, lu8Temp), 4);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32CombufSize, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0101: // ECU Info1
      lbConsumed = True;
      {
        u8 lu8Temp[4];
        mu8BtrIdx = mu8Btr;
        mu32Index2 = 0;
        mu32IndexMax = 5 * 16;
        vMemCopy(ROMCONST_PARTITION_START_ADRESS_EEP, mu8BtrIdx, mu32IndexMax);
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x01);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32IndexMax, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0102: // ECU Info2
      lbConsumed = True;
      {
        u8 lu8Temp[4];
        mu8BtrIdx = mu8Btr;
        mu32Index2 = 0;
        mu32IndexMax = RomConst_stDevice_Info->u8PartitionCnt * 32;
        vMemCopy(ROMCONST_PARTITION_START_ADRESS_EEP + 5 * 16, mu8BtrIdx, mu32IndexMax);
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x02);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32IndexMax, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0108: // BTR direkt lesen (optional)
      lbConsumed = True;
      {
        u32   lu32ReadIdx;
        u8    lu8Size;

        lu32ReadIdx = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        lu8Size = (lcMsg.mcPayload.mpu8Data[6] & 0xF) + 1;

        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x08);
        mcTxMsgTx.mcPayload.Add((u8*)&mu8Btr[lu32ReadIdx], lu8Size);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0110: // BTR Index lesen
      lbConsumed = True;
      {
        u8 lu8Temp[4];
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x10);
        mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)(mu8BtrIdx - mu8Btr), lu8Temp), 4);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32Checksum, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0208: // BTR direkt schreiben (optional)
      lbConsumed = True;
      {
        u32   lu32WriteIdx;
        u8    lu8Size;

        lu32WriteIdx = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        lu8Size = (lcMsg.mcPayload.mpu8Data[6] & 0xF) + 1;

        if ((lu32WriteIdx + lu8Size) < mu32Size)
        {
          cMemTools::vMemCpy(&mu8Btr[lu32WriteIdx], &(lcMsg.mcPayload.mpu8Data[7]), lu8Size);
        }
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x82);
        mcTxMsgTx.mcPayload.Add(0x08);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    case 0x0210: // BTR Index setzen
      lbConsumed = True;
      {
        u32   lu32BtrIdx;
        u8    lu8ResetChkSum;

        lu32BtrIdx = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        lu8ResetChkSum = lcMsg.mcPayload.mpu8Data[6];

        if (lu8ResetChkSum) mu32Checksum = 1;
        mu8BtrIdx = &mu8Btr[lu32BtrIdx];
        mu32Index2 = 0;
        mu32IndexMax = 0;

        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x82);
        mcTxMsgTx.mcPayload.Add(0x10);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
      break;
    }
  }


  break;
  case 1:
    lbConsumed = True;
    {
      // BTR Write
      u8    lu8Size;
      u8    lu8PackageCnt;
      lu8PackageCnt = lcMsg.mcPayload.mpu8Data[0];
      lu8Size = lcMsg.mcPayload.Len() - 1;
      if (((u32)(mu8BtrIdx - mu8Btr) + lu8Size) < mu32Size)
      {
        mu32Checksum += vMemCopy((u8*)(&lcMsg.mcPayload.mpu8Data[1]), mu8BtrIdx, lu8Size);
        mu8BtrIdx += lu8Size;
        mu32Index2 += lu8Size;
        mu32IndexMax += lu8Size;
      }

      if (lu8PackageCnt == 0XFF)
      {
        u8 lu8Temp[4];
        mu32Index2 = 0;
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
        mcTxMsgTx.mcPayload.Set(0x81);
        mcTxMsgTx.mcPayload.Add(0x10);
        mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)(mu8BtrIdx - mu8Btr), lu8Temp), 4);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32Checksum, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        lpcBn->bSendMsg(&mcTxMsgTx);
      }
    }
    break;
  case 2:
    lbConsumed = True;
    // BTR Read
    mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 0);
    mu32Index2 = u32U8toU32((lcMsg.mcPayload.mpu8Data));
    mu8PackageCounter = 0;
    if (mu32Index2 > 0)
    {
      mbIsReadInProgress = True;
      mcBn = lpcBn;
    }
    break;
  }
  return lbConsumed;
}

void cBotNetMsgPortBtr::vProcess10ms()
{
  if (mbIsReadInProgress)
  {
    if (mu32Index2 > 0)
    {
      u8 lu8Temp[48];
      if (mu32Index2 > 48)
      {
        mcTxMsgTx.mcPayload.Set(mu8PackageCounter);
        mu32Checksum += vMemCopy(mu8BtrIdx, lu8Temp, 48);
        mcTxMsgTx.mcPayload.Add(lu8Temp, 48);
        mu8BtrIdx += 48;
        mu32Index2 -= 48;
      }
      else
      {
        mcTxMsgTx.mcPayload.Set(0xFF);
        mu32Checksum += vMemCopy(mu8BtrIdx, lu8Temp, mu32Index2);
        mcTxMsgTx.mcPayload.Add(lu8Temp, mu32Index2);
        mu8BtrIdx += mu32Index2;
        mu32Index2 = 0;
      }
      mu8PackageCounter++;
      mcTxMsgTx.vEncode();
      mcBn->bSendMsg(&mcTxMsgTx);
    }
    if (mu32Index2 == 0) mbIsReadInProgress = False;
  }
}




