#include "cBnMsgSysSpop.h"
#include "cBotnet.h"
#include "cBuRam.h"

cBotNetMsgPortSpop::cBotNetMsgPortSpop(cBotNet* lcBotNet)
  : cJobHandler(),
    cBotNet_MsgSysProcess(lcBotNet),
    mcTxMsgTx(mu8MsgTx_Buf, cBotNet_MsgSize)
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

  if ((lcMsg.u16GetIdx() == 3) &&
      ((mu32Exitcounter_ms) || (lcMsg.GetPayload()[0] == 1)))
  {
    u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
    mcTxDAdr = lcMsg.cGetSAdr();

    switch (lpu8PayloadRx[0])
    {
      case 0x00: // Exit
        lbConsumed = True;
        {
          mcTxMsgTx.Set(0x80);
          u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          lbResetExitCounter = True;
        }
        break;
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lpu8PayloadRx[1] == 0x12) &&
              (lpu8PayloadRx[2] == 0x34))
          {
            u8 lu8Temp[4];
            mcTxMsgTx.Set(0x81);
            mcTxMsgTx.Add(0x00);  // Memory Index RomConst
            mcTxMsgTx.Add(u8U32toU8((u32)ROMCONST_PARTITION_START_ADRESS_EEP, lu8Temp), 4); // Adresse RomConst.
            mcTxMsgTx.Add(0x00);  // Memory Index BTR
            mcTxMsgTx.Add(u8U32toU8(0, lu8Temp), 4); // Adress BTR
            mcTxMsgTx.Add(u8U32toU8(cBnMsgSysBtrCfgRamSize, lu8Temp), 4); // Btr Größe in Byte
            mcTxMsgTx.Add(u8U32toU8(cBotNet_UpLinkComBufSize, lu8Temp), 4); // Combuf Größe in Bytes
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
          else
          {
            mcTxMsgTx.Set(0x81);
            mcTxMsgTx.Add(0xFE);
            u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
        }
        break;
      case 0x03: // Config
        lbConsumed = True;
        {
          u8 lu8Reserve;

          mu8DefaultMemorySort = lpu8PayloadRx[1];
          lu8Reserve = lpu8PayloadRx[2];
          lu8Reserve = lpu8PayloadRx[3];
          lu8Reserve = lpu8PayloadRx[4];
          lu8Reserve = lpu8PayloadRx[5];
          lu8Reserve = lpu8PayloadRx[6];
          lu8Reserve = lpu8PayloadRx[7];
          lu8Reserve = lpu8PayloadRx[8];

          UNUSED(lu8Reserve);

          mcTxMsgTx.Set(0x83);
          mcTxMsgTx.Add(0x00);  // Memory Index RomConst
          u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());

          //vStart((cJobHandler::cJobs)cJobs::nJobConfig);
          //vFinished();
        }
        break;
      case 0x04: // Flash Sektor löschen
        // 04 MI AA AA AA AA
        lbConsumed = True;
        {
          u32    lu32Adress;

          mu8MemmoryIdx  = lpu8PayloadRx[1];
          lu32Adress     = u32U8toU32((lpu8PayloadRx + 2));
          mu8DataPointer = (uint8*)lu32Adress;
          switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
          {
            case RomConst_Sort_IntFlash:
            {
              u8 lu8Temp[4];

              mcTxMsgTx.Set(0x84);
              mcTxMsgTx.Add(0x00);
              mcTxMsgTx.Add(u8U32toU8(100, lu8Temp), 4);
              u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());

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
              mcTxMsgTx.Set(0x84);
              mcTxMsgTx.Add(0xFE);
              u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
            }
            break;
          }
        }
        break;

      case 0x05: // Speicher lesen
        // 05 MI AA AA AA AA SS SS SS SS
        lbConsumed = True;
        {
          mu8MemmoryIdx  = lpu8PayloadRx[1];
          mu8DataPointer = (u8*)u32U8toU32((lpu8PayloadRx + 2));
          mu32DataCnt    =      u32U8toU32((lpu8PayloadRx + 2 + 4));
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

          lu8PackageCnt  = lpu8PayloadRx[1];

          if ((lu8PackageCnt <= 200) || (lu8PackageCnt == 0xFF))
          {
            u8 lu8Size;
            lu8Size = lcMsg.mcPayload.Len() - 2;

            mu32DataChecksum += vMemCopy((u8*)(&lpu8PayloadRx[2]), mcBn->mcBtr.mu8BtrIdx, lu8Size);
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

                    mcTxMsgTx.Set(0x86);
                    mcTxMsgTx.Add(0xFC);
                    mcTxMsgTx.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);

                    u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
                  }
                  break;

                case RomConst_Sort_IntEep:
                case RomConst_Sort_ExtEep:
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

                    mcTxMsgTx.Set(0x86);
                    mcTxMsgTx.Add(0xFC);
                    mcTxMsgTx.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
                    u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
                    cJobHandler::vFinished();
                    break;
                  }
              }
            }
          }
          else if (lu8PackageCnt == 0xF0)
          {
            mu32DataChecksum = 1;
            mu8DataPointer   = (u8*)u32U8toU32((lpu8PayloadRx + 3));
            mu32DataCnt      =      u32U8toU32((lpu8PayloadRx + 3 + 4));
            mu8MemmoryIdx    = lpu8PayloadRx[2];

            // Die Daten werden zuerst im BTR zwischengespeichert
            mcBn->mcBtr.mu8BtrIdx = mcBn->mcBtr.mu8Btr;

            mcTxMsgTx.Set(0x86);
            mcTxMsgTx.Add(0x00);

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
            mcTxMsgTx.Add(u8U32toU8(lu32Runtime_ms, lu8Temp), 4);

            u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
        }
        break;

      case 0x08: // Checksumme erstellen
        // 08 MI AAAA AAAA SSSS SSSS
        lbConsumed = True;
        mu8DataPointer   = (u8*)u32U8toU32((lpu8PayloadRx + 2));
        mu32DataCnt      = u32U8toU32((lpu8PayloadRx + 2 + 4));
        mu8MemmoryIdx    = lpu8PayloadRx[1];
        mu32DataChecksum = 1;

        if (mu32DataCnt > 0)
        {
          u8 lu8Temp[4];

          mcTxMsgTx.Set(0x88);
          mcTxMsgTx.Add(0x00);
          mcTxMsgTx.Add(u8U32toU8((mu32DataCnt / (1024*16) + 1) * 10, lu8Temp), 4);

          u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
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
          lpu8Adr = (u8*)u32U8toU32((lpu8PayloadRx + 1));
          u32 lu32Param[4];
          lu32Param[0] = u32U8toU32((lpu8PayloadRx + 1+4));
          lu32Param[1] = u32U8toU32((lpu8PayloadRx + 1+8));
          lu32Param[2] = u32U8toU32((lpu8PayloadRx + 1+12));
          lu32Param[3] = u32U8toU32((lpu8PayloadRx + 1+16));

          mcTxMsgTx.Set(0x90);
          mcTxMsgTx.Add(0x00);

          u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());

          if (lpu8Adr != 0)
          {
            cBnSpop_vCall(lpu8Adr, lu32Param);
          }
          else
          {
            cBuRam::mBuRam->u32SpopMp    = lu32Param[0];
            cBuRam::mBuRam->u32SpopBnAdr = (u32)( ((u32)lcMsg.cGetDAdr().Get() << 16) + (u32)lcMsg.cGetSAdr().Get() );
            vStart((cJobHandler::cJobs)cJobs::nJobReset);
          }
        }
        break;

      case 0x11: // Jump Kommando
        lbConsumed = True;
        {
          uint8* lpu8Adr;
          u32    lu32Param;
          lpu8Adr   = (u8*)u32U8toU32((lpu8PayloadRx + 1));
          lu32Param =      u32U8toU32((lpu8PayloadRx + 1 + 4));

          mcTxMsgTx.Set(0x91);
          mcTxMsgTx.Add(0x00);

          u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());

          if (lpu8Adr != 0)
          {
            cBnSpop_vJumpToApp(lpu8Adr);
          }
          else
          {
            cBuRam::mBuRam->u32SpopBnAdr = 0;
            cBuRam::mBuRam->u32SpopMp    = lu32Param;
            cBuRam::mBuRam->u32SpopBnAdr = (u32)(((u32)lcMsg.cGetDAdr().Get() << 16) + (u32)lcMsg.cGetSAdr().Get());
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


void cBotNetMsgPortSpop::vFlashErase()
{
  // tERASE Page(2 KB) erase time - 22.02 24.47
  cBnSpop_vFlash_Unlock();
  cBnSpop_vFlash_ErasePage_Start(mu8DataPointer);

  mcTxMsgTx.Set(0x84);
  if (cBnSpop_bFlash_ErasePage(mu8DataPointer))
  {
    mcTxMsgTx.Add(0x01);
  }
  else
  {
    mcTxMsgTx.Add(0xFE);
  }
  u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
  cBnSpop_vFlash_Lock();
}

void cBotNetMsgPortSpop::vProcess(u16 lu16TimeDiff_ms)
{
  cJobHandler::vProcess(lu16TimeDiff_ms);

  if (!isBusy())
  {
    if (lu16TimeDiff_ms <= mu32Exitcounter_ms)
    {
      mu32Exitcounter_ms -= lu16TimeDiff_ms;
    }
    else
    {
      mu32Exitcounter_ms = 0;
    }
  }
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
        {
          switch(cJobHandler::mcState)
          {
            case cJobHandler::cJobStates::stJobProcessStart:
              // This time is used to wait for transmission of the response
              if (cJobHandler::u16GetTime_ms() > 10)
              {
                // This time is used to wait for stop of communication
                cBotNet_LinkBase* lpcLink = (cBotNet_LinkBase*)mcBn->mcUpLink;

                if (lpcLink != null)
                {
                  // Uplink anhalten
                  lpcLink->mControl.EnableRequest = 0;
                  cJobHandler::mcState = cJobHandler::cJobStates::stJobProcess2;
                }
                else
                {
                  vFlashErase();
                  vFinished();
                }
              }
              break;
            case cJobHandler::cJobStates::stJobProcess2:
              {
                cBotNet_LinkBase* lpcLink = (cBotNet_LinkBase*)mcBn->mcUpLink;
                // Uplink angehalten ?
                if (!lpcLink->IsEnabled())
                {
                  vFlashErase();
                  vFinished();
                  lpcLink->mControl.EnableRequest = 1;
                }
              }
              break;
            default: break;
          }
        }
        break;
      case cJobs::nJobEraseExtFlash:
        vFinished();
        break;

      case cJobs::nJobRead:
      case cJobs::nJobReadIntRam:
      case cJobs::nJobReadIntFlash:
          {
            bool lbLoop = True;
            while ((mu32DataCnt > 0) && (lbLoop))
            {
              u8 lu8Temp[48];
              mcTxMsgTx.Set(0x85);
              if (mu32DataCnt > 48)
              {
                mcTxMsgTx.Add(mu8PackageCounter);
                mu32DataChecksum += vMemCopy(mu8DataPointer, lu8Temp, 48);
                mcTxMsgTx.Add(lu8Temp, 48);
                mu8DataPointer += 48;
                mu32DataCnt -= 48;
              }
              else
              {
                mcTxMsgTx.Add(0xFF);
                mu32DataChecksum += vMemCopy(mu8DataPointer, lu8Temp, mu32DataCnt);
                mcTxMsgTx.Add(lu8Temp, mu32DataCnt);
                mcTxMsgTx.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
                mu32DataCnt = 0;
              }
              mu8PackageCounter++;
              if (u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len()) >= 15)
              {
                lbLoop = True;
              }
              else
              {
                lbLoop = False;
              }
            }
            if (mu32DataCnt == 0) cJobHandler::vFinished();
          }
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
            mcTxMsgTx.Set(0x86);
            mcTxMsgTx.Add(0xFC);
            mcTxMsgTx.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);

            u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
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
            mcTxMsgTx.Set(0x88);
            mcTxMsgTx.Add(0x01);
            mcTxMsgTx.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);

            u8PutInt(mcBn->mcAdr, mcTxDAdr, 3, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
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
        if (cJobHandler::u16GetTime_ms() > 100)
        {
          cBnSpop_vJumpToApp(0);
        }
        break;
      default:
        vFinished();
    }
  }
}


void cBotNetMsgPortSpop::vTick10ms()
{
  vProcess(10);
}

