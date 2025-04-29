#include "cBnMsgSysSpop.h"
#include "cBotnet.h"

bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn)
{
  bool lbConsumed = False;

  if ((mbIsReadInProgress) || (mbIsChecksumInProgress) || (mbIsResetInProgress)) return False;

  if ((lcMsg.mu16Idx == 3) &&
      ((mbEnable) || (lcMsg.mcPayload[0] == 1)))
  {
    u16 lu16SubIdx = lcMsg.mcPayload[0];
    mu32Exitcounter = 1000 * 60 * 5;
    switch (lu16SubIdx)
    {
      case 0x00: // Exit
        lbConsumed = True;
        {
          mbEnable = False;
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x80);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);
        }
        break;
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lcMsg.mcPayload.mpu8Data[1] == 0x12) &&
              (lcMsg.mcPayload.mpu8Data[2] == 0x34))
          {
            u8 lu8Temp[4];
            mbEnable = True;
            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x81);
            mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index RomConst
            mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)ROMCONST_PARTITION_START_ADRESS_EEP, lu8Temp), 4); // Adresse RomConst.
            mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index BTR
            mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Adress BTR
            mcTxMsgTx.mcPayload.Add(u8U32toU8(cBnMsgSysBtrCfgRamSize, lu8Temp), 4); // Btr Größe in Byte
            mcTxMsgTx.mcPayload.Add(u8U32toU8(cBotNet_UpLinkComBufSize, lu8Temp), 4); // Combuf Größe in Bytes
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
          }
          else
          {
            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x81);
            mcTxMsgTx.mcPayload.Add(0xFE);
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
          }
        }
        break;

      case 0x03: // Config
        lbConsumed = True;
        {
          u8 lu8Reserve;
          
          mu8DefaultMemorySort = lcMsg.mcPayload.mpu8Data[1];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[2];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[3];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[4];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[5];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[6];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[7];
          lu8Reserve = lcMsg.mcPayload.mpu8Data[8];

          UNUSED(lu8Reserve);

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x83);
          mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index RomConst
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);
        }
        break;

      case 0x04: // EEPROM/Flash Sektor löschen
        lbConsumed = True;
        {
          uint8* lpui8Adr;
          u32    lu32Adress;
          lu32Adress = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
          lpui8Adr = (uint8*)lu32Adress;
          switch (u16GetMemorySort(lpui8Adr))
          {
            case RomConst_Sort_Flash:
            {
              u8 lu8Temp[4];
              mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
              mcTxMsgTx.mcPayload.Set(0x84);
              mcTxMsgTx.mcPayload.Add(0x00);
              mcTxMsgTx.mcPayload.Add(u8U32toU8(100, lu8Temp), 4);
              mcTxMsgTx.vEncode();
              lpcBn->bSendMsg(&mcTxMsgTx);
              cBnSpop_vFlash_Unlock();
              cBnSpop_vFlash_EreasePage(lpui8Adr);
              cBnSpop_vFlash_Lock();
              mcTxMsgTx.mcPayload.Set(0x84);
              mcTxMsgTx.mcPayload.Add(0x01);
              mcTxMsgTx.vEncode();
              lpcBn->bSendMsg(&mcTxMsgTx);
            }
            break;
            default:
            {
              mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
              mcTxMsgTx.mcPayload.Set(0x84);
              mcTxMsgTx.mcPayload.Add(0xFE);
              mcTxMsgTx.vEncode();
              lpcBn->bSendMsg(&mcTxMsgTx);
            }
            break;
          }
        }
        break;

      case 0x05: // Speicher lesen
        lbConsumed = True;
        // BTR Read
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
        mu8DataPointer = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        mu32DataCnt    = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2 + 4));
        mu8PackageCounter = 0;
        mu32DataChecksum  = 1;
        if (mu32DataCnt > 0)
        {
          mbIsReadInProgress = True;
          mcBn = lpcBn;
        }
        break;

      case 0x06: // Speicher schreiben
        lbConsumed = True;
        {
          u8    lu8PackageCnt;
          lu8PackageCnt = lcMsg.mcPayload.mpu8Data[1];

          if ((lu8PackageCnt <= 200) || (lu8PackageCnt == 0xFF))
          {
            u8 lu8Size;
            lu8Size = lcMsg.mcPayload.Len() - 2;

            switch (u16GetMemorySort(mu8DataPointer))
            {
              case RomConst_Sort_Flash:
              {
                cMemTools::vMemCpy(mu8FlashWriteBuf, (u8*)(&lcMsg.mcPayload.mpu8Data[2]), lu8Size);
                mu32DataChecksum += cBnSpop_vFlash_Program(mu8DataPointer, mu8FlashWriteBuf, lu8Size);
              }
              break;
              case RomConst_Sort_Ram:
              {
                mu32DataChecksum += vMemCopy((u8*)(&lcMsg.mcPayload.mpu8Data[2]), mu8DataPointer, lu8Size);
              }
              break;
              default:
              break;
            }

            if (lu8PackageCnt == 0xFF)
            {
              u8 lu8Temp[4];

              if (u16GetMemorySort(mu8DataPointer) == RomConst_Sort_Flash)
              {
                cBnSpop_vFlash_Lock();
              }

              mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
              mcTxMsgTx.mcPayload.Set(0x86);
              mcTxMsgTx.mcPayload.Add(0xFC);
              mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
              mcTxMsgTx.vEncode();
              lpcBn->bSendMsg(&mcTxMsgTx);
            }

            mu8DataPointer += lu8Size;
            mu32DataCnt    += lu8Size;
          }
          else if (lu8PackageCnt == 0xF0)
          {
            mu32DataChecksum = 1;
            mu8DataPointer = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 3));
            mu32DataCnt = u32U8toU32((lcMsg.mcPayload.mpu8Data + 3 + 4));

            if (u16GetMemorySort(mu8DataPointer) == RomConst_Sort_Flash)
            {
              cBnSpop_vFlash_Unlock();
            }

            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x86);
            mcTxMsgTx.mcPayload.Add(0x00);
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
          }
        }
        break;

      case 0x08: // Checksumme erstellen
        lbConsumed = True;
        // BTR Read
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
        mu8DataPointer = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        mu32DataCnt = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2 + 4));
        mu32DataChecksum = 1;
        if (mu32DataCnt > 0)
        {
          u8 lu8Temp[4];
          mbIsChecksumInProgress = True;
          mcBn = lpcBn;
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x88);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.mcPayload.Add(u8U32toU8((mu32DataCnt / (1024*16) + 1) * 10, lu8Temp), 4);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);
        }
        break;

      case 0x10: // Call Kommando
        lbConsumed = True;
        {
          uint8* lpui8Adr;
          lpui8Adr = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));
          u32 l32Param[4];
          l32Param[0] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+4));
          l32Param[1] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+8));
          l32Param[2] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+12));
          l32Param[3] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+16));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x90);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);

          if (lpui8Adr != 0)
          {
            cBnSpop_vCall(lpui8Adr, l32Param);
          }
          else
          {
            mbIsResetInProgress = True;
            mbIsResetInProgressDelay = 1000 /*ms*/ / 10 /*Process10ms Ticks*/;
            cBnSpop_vWriteBackupData(0, (u32)(0));
            cBnSpop_vWriteBackupData(1, (u32)(0));
          }

        }
        break;

      case 0x11: // Jump Kommando
        lbConsumed = True;
        {
          uint8* lpui8Adr;
          lpui8Adr = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x91);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);

          if (lpui8Adr != 0)
          {
            cBnSpop_vJumpToApp(lpui8Adr);
          }
          else
          {
            mbIsResetInProgress = True;
            mbIsResetInProgressDelay = 1000; /*ms*/
            cBnSpop_vWriteBackupData(0, (u32)(0));
            cBnSpop_vWriteBackupData(1, (u32)(0));
          }
        }
        break;
      case 0x12: // Call Bootloader
        lbConsumed = True;
        {
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x92);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);

          mbIsResetInProgress = True;
          mbIsResetInProgressDelay = 1000; /*ms*/
          cBnSpop_vWriteBackupData(0, (u32)(('S' << 24) + ('P' << 16) + ('O' << 8) + 'P'));
          cBnSpop_vWriteBackupData(1, (u32)(((u32)lcMsg.mcFrame.mcDAdr.Get() << 16) + (u32)lcMsg.mcFrame.mcSAdr.Get()));
        }
        break;

    }
  }
  return lbConsumed;
}



void cBotNetMsgPortSpop::vProcess10ms()
{
  if ((mbIsReadInProgress) || (mbIsChecksumInProgress) || (mbIsResetInProgress)) {}
  else
  {
    if (mu32Exitcounter > 9)
    {
      mu32Exitcounter -= 10;
    }
    else
    {
      mu32Exitcounter = 0;
      mbEnable = False;
    }
  }

  if (mbIsReadInProgress)
  {
    if (mu32DataCnt > 0)
    {
      u8 lu8Temp[48];
      mcTxMsgTx.mcPayload.Set(0x85);
      if (mu32DataCnt > 48)
      {
        mcTxMsgTx.mcPayload.Add(mu8PackageCounter);
        mu32DataChecksum += vMemCopy(mu8DataPointer, lu8Temp, 48);
        mcTxMsgTx.mcPayload.Add(lu8Temp, 48);
        mu8DataPointer += 48;
        mu32DataCnt -= 48;
      }
      else
      {
        mcTxMsgTx.mcPayload.Add(0xFF);
        mu32DataChecksum += vMemCopy(mu8DataPointer, lu8Temp, mu32DataCnt);
        mcTxMsgTx.mcPayload.Add(lu8Temp, mu32DataCnt);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
        mu32DataCnt = 0;
      }
      mu8PackageCounter++;
      mcTxMsgTx.vEncode();
      mcBn->bSendMsg(&mcTxMsgTx);
    }
    if (mu32DataCnt == 0) mbIsReadInProgress = False;
  }
  else
  if (mbIsChecksumInProgress)
  {
    if (mu32DataCnt > 0)
    {
      if (mu32DataCnt > (1024*16))
      {
        mu32DataChecksum += ui32ChkSum(mu8DataPointer, (1024 * 16));
        mu8DataPointer   += (1024 * 16);
        mu32DataCnt      -= (1024 * 16);
      }
      else
      {
        u8 lu8Temp[4];
        mu32DataChecksum += ui32ChkSum(mu8DataPointer, mu32DataCnt);
        mu32DataCnt       = 0;
        mbIsChecksumInProgress = False;
        mcTxMsgTx.mcPayload.Set(0x88);
        mcTxMsgTx.mcPayload.Add(0x01);
        mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
        mcTxMsgTx.vEncode();
        mcBn->bSendMsg(&mcTxMsgTx);
      }
    }
  }
  else
  if (mbIsResetInProgress)
  {
    if (mbIsResetInProgressDelay > 10)
    {
      mbIsResetInProgressDelay -= 10;
    }
    else
    {
      mbIsResetInProgress = False;
      cBnSpop_vJumpToApp(0);
    }
  }
}







