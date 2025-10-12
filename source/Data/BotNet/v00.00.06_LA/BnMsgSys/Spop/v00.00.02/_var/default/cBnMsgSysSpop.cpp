#include "cBnMsgSysSpop.h"
#include "cBotnet.h"
#include "cBuRam.h"

cBotNetMsgPortSpop::cBotNetMsgPortSpop(cBotNet* lcBotNet)
  : cJobHandler(),
    cBotNet_MsgSysProcess(lcBotNet),
    mcTxMsgTx_BAry(mu8MsgTx_Buf, cBotNet_MsgSize),
    mcTxMsgTx(&mcTxMsgTx_BAry)
{
  mu32Exitcounter_ms = 0;
  mu8DefaultMemorySort = RomConst_Sort_IntFlash;
  mcEep = null;
  cBuRam::vEnable();
}


bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;
  bool lbResetExitCounter = False;

  if (isBusy()) return False;

  if ((lcMsg.mu16Idx == 3) &&
      ((mu32Exitcounter_ms) || (lcMsg.mcPayload[0] == 1)))
  {
    u16 lu16SubIdx = lcMsg.mcPayload[0];
    switch (lu16SubIdx)
    {
      case 0x00: // Exit
        lbConsumed = True;
        {
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x80);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);
          lbResetExitCounter = True;
        }
        break;
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lcMsg.mcPayload.mpu8Data[1] == 0x12) &&
              (lcMsg.mcPayload.mpu8Data[2] == 0x34))
          {
            u8 lu8Temp[4];
            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x81);
            mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index RomConst
            mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)ROMCONST_PARTITION_START_ADRESS_EEP, lu8Temp), 4); // Adresse RomConst.
            mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index BTR
            mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Adress BTR
            mcTxMsgTx.mcPayload.Add(u8U32toU8(cBnMsgSysBtrCfgRamSize, lu8Temp), 4); // Btr Größe in Byte
            mcTxMsgTx.mcPayload.Add(u8U32toU8(cBotNet_UpLinkComBufSize, lu8Temp), 4); // Combuf Größe in Bytes
            mcTxMsgTx.vEncode();
            mcBn->bSendMsg(&mcTxMsgTx);
          }
          else
          {
            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x81);
            mcTxMsgTx.mcPayload.Add(0xFE);
            mcTxMsgTx.vEncode();
            mcBn->bSendMsg(&mcTxMsgTx);
            lbResetExitCounter = True;
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
          mcBn->bSendMsg(&mcTxMsgTx);

          //vStart((cJobHandler::cJobs)cJobs::nJobConfig);
          //vFinished();
        }
        break;
      case 0x04: // Flash Sektor löschen
        // 04 MI AA AA AA AA
        lbConsumed = True;
        {
          uint8* lpui8Adr;
          u32    lu32Adress;

          mu8MemmoryIdx = lcMsg.mcPayload.mpu8Data[1];
          lu32Adress    = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
          lpui8Adr      = (uint8*)lu32Adress;
          switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
          {
            case RomConst_Sort_IntFlash:
            {
              u8 lu8Temp[4];
              mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
              mcTxMsgTx.mcPayload.Set(0x84);
              mcTxMsgTx.mcPayload.Add(0x00);
              mcTxMsgTx.mcPayload.Add(u8U32toU8(100, lu8Temp), 4);
              mcTxMsgTx.vEncode();
              mcBn->bSendMsg(&mcTxMsgTx);
              cBnSpop_vFlash_Unlock();
              cBnSpop_vFlash_ErasePage_Start(lpui8Adr);
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobEraseIntFlash);
            }
            break;
            case RomConst_Sort_ExtFlash:
            {
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobEraseExtFlash);
            }
            break;
            default:
            {
              mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
              mcTxMsgTx.mcPayload.Set(0x84);
              mcTxMsgTx.mcPayload.Add(0xFE);
              mcTxMsgTx.vEncode();
              mcBn->bSendMsg(&mcTxMsgTx);
            }
            break;
          }
        }
        break;

      case 0x05: // Speicher lesen
        // 05 MI AA AA AA AA SS SS SS SS
        lbConsumed = True;
        {
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);

          mu8MemmoryIdx  = lcMsg.mcPayload.mpu8Data[1];
          mu8DataPointer = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
          mu32DataCnt    =      u32U8toU32((lcMsg.mcPayload.mpu8Data + 2 + 4));
          mu8PackageCounter = 0;
          mu32DataChecksum  = 1;

          switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
          {
            case RomConst_Sort_Ram:
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobReadIntRam);
              break;

            case RomConst_Sort_IntEep:
            case RomConst_Sort_ExtEep:
              if (mcEep != 0)
              {
                if ((mcEep->IsInit()) && (mcEep->isIdle()))
                {
                  cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobReadExtEeprom);
                }
              }
              break;

            case RomConst_Sort_IntFlash:
            case RomConst_Sort_ExtFlash:
            default:
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobReadIntFlash);
              break;
          }
        }
        break;

      case 0x06: // Speicher schreiben
        // 06 F0 MI AA AA AA AA SS SS SS SS
        lbConsumed = True;
        {
          u8 lu8PackageCnt;

          lu8PackageCnt  = lcMsg.mcPayload.mpu8Data[1];

          if ((lu8PackageCnt <= 200) || (lu8PackageCnt == 0xFF))
          {
            u8 lu8Size;
            lu8Size = lcMsg.mcPayload.Len() - 2;

            mu32DataChecksum += vMemCopy((u8*)(&lcMsg.mcPayload.mpu8Data[2]), mcBn->mcBtr.mu8BtrIdx, lu8Size);
            mcBn->mcBtr.mu8BtrIdx += lu8Size;

            // letztes Packet
            if (lu8PackageCnt == 0xFF)
            {
              // Nachdem die Daten in den BTR geschrieben worden sind,
              // den Schreibjob auslösen
              switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
              {
                case RomConst_Sort_Ram:
                  {
                    u8 lu8Temp[4];
                    cMemTools::vMemCpy(mu8DataPointer, mcBn->mcBtr.mu8Btr, mcBn->mcBtr.mu8BtrIdx - mcBn->mcBtr.mu8Btr);

                    mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
                    mcTxMsgTx.mcPayload.Set(0x86);
                    mcTxMsgTx.mcPayload.Add(0xFC);
                    mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
                    mcTxMsgTx.vEncode();
                    mcBn->bSendMsg(&mcTxMsgTx);
                  }
                  break;

                case RomConst_Sort_IntEep:
                case RomConst_Sort_ExtEep:
                  mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
                  mcBn->mcBtr.mu8BtrIdx = mcBn->mcBtr.mu8Btr;
                  cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobWriteExtEeprom);
                  break;

                case RomConst_Sort_IntFlash:
                case RomConst_Sort_ExtFlash:
                default:
                  {
                    u8 lu8Temp[4];

                    cBnSpop_vFlash_Unlock();
                    cBnSpop_vFlash_Program_Start(mu8DataPointer, mcBn->mcBtr.mu8Btr, mcBn->mcBtr.mu8BtrIdx - mcBn->mcBtr.mu8Btr);
                    cBnSpop_vFlash_Program_Finish();
                    cBnSpop_vFlash_Lock();

                    mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
                    mcTxMsgTx.mcPayload.Set(0x86);
                    mcTxMsgTx.mcPayload.Add(0xFC);
                    mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
                    mcTxMsgTx.vEncode();
                    mcBn->bSendMsg(&mcTxMsgTx);
                    cJobHandler::vFinished();
                    break;
                  }
              }
            }
          }
          else if (lu8PackageCnt == 0xF0)
          {
            mu32DataChecksum = 1;
            mu8DataPointer   = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 3));
            mu32DataCnt      =      u32U8toU32((lcMsg.mcPayload.mpu8Data + 3 + 4));
            mu8MemmoryIdx    = lcMsg.mcPayload.mpu8Data[2];

            // Die Daten werden zuerst im BTR zwischengespeichert
            mcBn->mcBtr.mu8BtrIdx = mcBn->mcBtr.mu8Btr;

            mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
            mcTxMsgTx.mcPayload.Set(0x86);
            mcTxMsgTx.mcPayload.Add(0x00);

            // Maximale Laufzeit
            u32 lu32Runtime_ms = 1000;
            switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
            {
              case RomConst_Sort_IntEep:
              case RomConst_Sort_ExtEep:
                lu32Runtime_ms = mu32DataCnt * 20;
                break;
            }
            u8 lu8Temp[4];
            mcTxMsgTx.mcPayload.Add(u8U32toU8(lu32Runtime_ms, lu8Temp), 4);

            mcTxMsgTx.vEncode();
            mcBn->bSendMsg(&mcTxMsgTx);
          }
        }
        break;

      case 0x08: // Checksumme erstellen
        // 08 MI AAAA AAAA SSSS SSSS
        lbConsumed = True;
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
        mu8DataPointer   = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        mu32DataCnt      = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2 + 4));
        mu8MemmoryIdx    = lcMsg.mcPayload.mpu8Data[1];
        mu32DataChecksum = 1;

        if (mu32DataCnt > 0)
        {
          u8 lu8Temp[4];
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x88);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.mcPayload.Add(u8U32toU8((mu32DataCnt / (1024*16) + 1) * 10, lu8Temp), 4);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);
          vStart((cJobHandler::cJobs)cJobs::nJobCheckSum);

          switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
          {
            case RomConst_Sort_Ram:
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCheckSumIntRam);
              break;

            case RomConst_Sort_IntEep:
            case RomConst_Sort_ExtEep:
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCheckSumExtEeprom);
              break;

            case RomConst_Sort_IntFlash:
            case RomConst_Sort_ExtFlash:
            default:
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobCheckSumIntFlash);
          }
        }
        break;

      case 0x10: // Call Kommando
        lbConsumed = True;
        {
          uint8* lpu8Adr;
          lpu8Adr = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));
          u32 lu32Param[4];
          lu32Param[0] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+4));
          lu32Param[1] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+8));
          lu32Param[2] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+12));
          lu32Param[3] = u32U8toU32((lcMsg.mcPayload.mpu8Data + 1+16));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x90);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);

          if (lpu8Adr != 0)
          {
            cBnSpop_vCall(lpu8Adr, lu32Param);
          }
          else
          {
            cBuRam::mBuRam->u32SpopMp    = lu32Param[0];
            cBuRam::mBuRam->u32SpopBnAdr = (u32)( ((u32)lcMsg.mcFrame.mcDAdr.Get() << 16) + (u32)lcMsg.mcFrame.mcSAdr.Get() );
            vStart((cJobHandler::cJobs)cJobs::nJobReset);
          }
        }
        break;

      case 0x11: // Jump Kommando
        lbConsumed = True;
        {
          uint8* lpu8Adr;
          u32    lu32Param;
          lpu8Adr   = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));
          lu32Param =      u32U8toU32((lcMsg.mcPayload.mpu8Data + 1 + 4));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x91);
          mcTxMsgTx.mcPayload.Add(0x00);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);

          if (lpu8Adr != 0)
          {
            cBnSpop_vJumpToApp(lpu8Adr);
          }
          else
          {
            cBuRam::mBuRam->u32SpopBnAdr = 0;
            cBuRam::mBuRam->u32SpopMp    = lu32Param;
            cBuRam::mBuRam->u32SpopBnAdr = (u32)(((u32)lcMsg.mcFrame.mcDAdr.Get() << 16) + (u32)lcMsg.mcFrame.mcSAdr.Get());
            vStart((cJobHandler::cJobs)cJobs::nJobReset);
          }
        }
        break;
    }
  }

  if (lbResetExitCounter)
  {
    mu32Exitcounter_ms = 0;
  }
  else
  {
    if (lbConsumed)
    {
      mu32Exitcounter_ms = 1000 * 60 * 5; /* 5min Exitcounter */
    }
  }

  return lbConsumed;
}


void cBotNetMsgPortSpop::vProcess(u32 lu32TimeDiff_us)
{
  cJobHandler::vProcess(lu32TimeDiff_us);

  if (!isBusy())
  {
    mu32Exitcounter_us += lu32TimeDiff_us;
    if (mu32Exitcounter_us > 1000)
    {
      if (mu32Exitcounter_ms > (mu32Exitcounter_us / 1000))
      {
        mu32Exitcounter_ms -= (mu32Exitcounter_us / 1000);
      }
      else
      {
        mu32Exitcounter_ms = 0;
      }
      mu32Exitcounter_us %= 1000;
    }
  }


  /*


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
  }*/

  else
  {
    switch ((cBotNetMsgPortSpop::cJobs)mcJob)
    {
      case cJobs::nJobExit:
        break;
      case cJobs::nJobEnter:
        break;
      case cJobs::nJobConfig:
        break;
      case cJobs::nJobErase:
        break;
      case cJobs::nJobEraseIntRam:
        break;
      case cJobs::nJobEraseExtRam:
        break;
      case cJobs::nJobEraseIntEeprom:
        break;
      case cJobs::nJobEraseExtEeprom:
        break;

      case cJobs::nJobEraseIntFlash:
        if (!cBnSpop_isFlash_Busy() || cBnSpop_isFlash_Error())
        {
          cBnSpop_vFlash_ErasePage_Finish();
          cBnSpop_vFlash_Lock();
          mcTxMsgTx.mcPayload.Set(0x84);
          mcTxMsgTx.mcPayload.Add(0x01);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);
          vFinished();
        }
        break;
      case cJobs::nJobEraseExtFlash:
        vFinished();
        break;

      case cJobs::nJobRead:
      case cJobs::nJobReadIntRam:
      case cJobs::nJobReadIntFlash:
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
          if (mu32DataCnt == 0) cJobHandler::vFinished();
          break;

      case cJobs::nJobReadExtRam:
        break;

      case cJobs::nJobReadIntEeprom:
      case cJobs::nJobReadExtEeprom:
          // Der EEPROM-Mirrow Ram liegt intern
          // Von daher den copy job darauf umbiegen
           mu8DataPointer = (u8*)mcEep->mu8RamMirrow;
           cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobReadIntRam, True);
        break;

      case cJobs::nJobReadExtFlash:
        break;

      case cJobs::nJobWrite:
        break;
      case cJobs::nJobWriteIntRam:
        break;
      case cJobs::nJobWriteExtRam:
        break;

      case cJobs::nJobWriteIntEeprom:
      case cJobs::nJobWriteExtEeprom:
        if ((mu32DataCnt) && (mcEep->IsInit()))
        {
          if (mcEep->isIdle())
          {
            mcEep->vCmdWriteByte((u16)((u32)mu8DataPointer), *mcBn->mcBtr.mu8BtrIdx);
            mcBn->mcBtr.mu8BtrIdx++;
            mu8DataPointer++;
            mu32DataCnt--;
          }
        }
        else
        {
          if ((mcEep->isIdle()) && (mcEep->IsInit()))
          {
            u8 lu8Temp[4];
            mcTxMsgTx.mcPayload.Set(0x86);
            mcTxMsgTx.mcPayload.Add(0xFC);
            mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
            mcTxMsgTx.vEncode();
            mcBn->bSendMsg(&mcTxMsgTx);
            cJobHandler::vFinished();
          }
        }
        break;


      case cJobs::nJobWriteIntFlash:
        break;
      case cJobs::nJobWriteExtFlash:
        break;

      case cJobs::nJobCheckSumExtRam:
        break;
      case cJobs::nJobCheckSumIntEeprom:
        break;
      case cJobs::nJobCheckSumExtEeprom:
        break;

      case cJobs::nJobCheckSum:
      case cJobs::nJobCheckSumIntRam:
      case cJobs::nJobCheckSumIntFlash:
        if (mu32DataCnt > 0)
        {
          if (mu32DataCnt > (1024*4))
          {
            mu32DataChecksum += ui32ChkSum(mu8DataPointer, (1024 * 4));
            mu8DataPointer   += (1024 * 4);
            mu32DataCnt      -= (1024 * 4);
          }
          else
          {
            u8 lu8Temp[4];
            mu32DataChecksum += ui32ChkSum(mu8DataPointer, mu32DataCnt);
            mu32DataCnt       = 0;
            mcTxMsgTx.mcPayload.Set(0x88);
            mcTxMsgTx.mcPayload.Add(0x01);
            mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
            mcTxMsgTx.vEncode();
            mcBn->bSendMsg(&mcTxMsgTx);
            cJobHandler::vFinished();
          }
        }
        break;
      case cJobs::nJobCheckSumExtFlash:
        break;

      case cJobs::nJobCall:
        break;
      case cJobs::nJobJump:
        break;
      case cJobs::nJobReset:
        if (cJobHandler::u32GetTime_us() > 100*1000)
        {
          cBnSpop_vJumpToApp(0);
        }
        break;
      default:
        vFinished();
    }
  }
}


void cBotNetMsgPortSpop::vProcess10ms()
{
  vProcess(10000);
}

