#include "cBnMsgSysSpop.h"
#include "cBotnet.h"
cBotNetMsgPortSpop::cBotNetMsgPortSpop(cBotNet* lcBotNet)
  : cJobHandler(),
    cBotNet_MsgSysProcess(lcBotNet),
    mcTxMsgTx_BAry(mu8MsgTx_Buf, cBotNet_MsgSize),
    mcTxMsgTx(&mcTxMsgTx_BAry)
{
  mu16Exitcounter_ms = 0;
}



void cBotNetMsgPortSpop::vEnter(uint16 lu16SAdr, uint16 lu16DAdr)
{
  u8 lu8Temp[4];
  mcTxMsgTx.vPrepare(lu16SAdr, lu16DAdr, 3);
  mcTxMsgTx.mcPayload.Set(0x81);
  mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index RomConst
  mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)ROMCONST_PARTITION_START_ADRESS_EEP, lu8Temp), 4); // Adresse RomConst.
  mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index BTR
  mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Adress BTR
  mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Btr Größe in Byte
  mcTxMsgTx.mcPayload.Add(u8U32toU8(cBotNet_UpLinkComBufSize, lu8Temp), 4); // Combuf Größe in Bytes
  mcTxMsgTx.vEncode();
  mcBn->bSendMsg(&mcTxMsgTx);
  vResetExitcounter();
}

bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;
  
  if (isBusy()) return False;

  if (lcMsg.mu16Idx == 3)
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
          mu16Exitcounter_ms = 0;
        }
        break;
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lcMsg.mcPayload.mpu8Data[1] == 0x12) &&
              (lcMsg.mcPayload.mpu8Data[2] == 0x34))
          {
            vEnter(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get());
          }
          //else
          //{
          //  vPrepareMsg(lcMsg, 0x81, 0xFE);
          //  mcTxMsgTx.vEncode();
          //  lpcBn->bSendMsg(&mcTxMsgTx);
          //}
        }
        break;

      case 0x04: // Flash Sektor löschen
        // 04 MI AA AA AA AA
        lbConsumed = True;
        {
          mu8MemmoryIdx  = lcMsg.mcPayload.mpu8Data[1];
          mu8DataPointer = (uint8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
          switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
          {
            case RomConst_Sort_IntFlash:
            {
              u8 lu8Temp[4];
              vPrepareMsgAndFlipAdr(lcMsg, 0x84, 0x00);
              mcTxMsgTx.mcPayload.Add(u8U32toU8(100, lu8Temp), 4);
              mcTxMsgTx.vEncode();
              mcBn->bSendMsg(&mcTxMsgTx);
              cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobEraseIntFlash);
            }
            break;
            default:
            {
              vPrepareMsgAndFlipAdr(lcMsg, 0x84, 0xFE);
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
          cJobHandler::vStart((cJobHandler::cJobs)cJobs::nJobRead);         
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

            mu32DataChecksum += vMemCopy((u8*)(&lcMsg.mcPayload.mpu8Data[2]), mu8FlashWriteBuf, lu8Size);

            // nicht sammel, sondern sofort wegschreiben
            switch (u16GetMemorySortByMemIdx(mu8MemmoryIdx))
            {
              case RomConst_Sort_Ram:
                {
                  cMemTools::vMemCpy(mu8DataPointer, mu8FlashWriteBuf, lu8Size);
                }
                break;

              case RomConst_Sort_IntFlash:
              default:
                {
                  // Während den Flash-Schreibens die Kommunikation mittel Interrupt-Lock sperren
                  // weil Flash schreiben die den Flash blockiert und dann die Interrupt verzögert ausgeführt werden
                  // warte bis Uart in Mute mode, damit nicht eine laufende Kommunikation unterbrochen wird
                  while ((!(USART_STAT(USART0) & USART_STAT_RWU)))    // Nicht im Mute modus
                  {}
                  USART_CTL0(USART0) &= ~USART_CTL0_REN;
                  {
                    cBnSpop_vFlash_Unlock();
                    cBnSpop_vFlash_Program(mu8DataPointer, mu8FlashWriteBuf, lu8Size);
                    cBnSpop_vFlash_Lock();
                  }

                  USART_CMD(USART0)  |= USART_CMD_RXFCMD;
                  USART_CTL0(USART0) |= USART_CTL0_REN;
                  break;
                }
            }
            if (lu8PackageCnt == 0xFF)
            {
              u8 lu8Temp[4];
              vPrepareMsgAndFlipAdr(lcMsg, 0x86, 0xFC);
              mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
              mcTxMsgTx.vEncode();
              mcBn->bSendMsg(&mcTxMsgTx);
              cJobHandler::vFinished();
            }

            mu8DataPointer += lu8Size;
          }
          else if (lu8PackageCnt == 0xF0)
          {
            mu32DataChecksum = 1;
            mu8DataPointer   = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 3));
            //mu32DataCnt      =      u32U8toU32((lcMsg.mcPayload.mpu8Data + 3 + 4));
            mu8MemmoryIdx    = lcMsg.mcPayload.mpu8Data[2];

            vPrepareMsgAndFlipAdr(lcMsg, 0x86, 0x00);

            // Maximale Laufzeit
            u32 lu32Runtime_ms = 1000;
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
        }
        break;

      /*case 0x10: // Call Kommando
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
            mbIsResetInProgressDelay = 100; //ms
          }
        }
        break;*/

      case 0x11: // Jump Kommando
        lbConsumed = True;
        {
          //uint8* lpu8Adr;
          u32    lu32Param;
          //lpu8Adr   = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));
          lu32Param =      u32U8toU32((lcMsg.mcPayload.mpu8Data + 1 + 4));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          vPrepareMsgAndFlipAdr(lcMsg, 0x91, 0x00);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);

          //if (lpu8Adr != 0)
          //{
          //  cBnSpop_vJumpToApp(lpu8Adr);
          //}
          //else
          {
            cBnSpop_vWriteBackupData(0, lu32Param);
            cBnSpop_vWriteBackupData(1, (u32)(((u32)lcMsg.mcFrame.mcDAdr.Get() << 16) + (u32)lcMsg.mcFrame.mcSAdr.Get()));
            vStart((cJobHandler::cJobs)cJobs::nJobReset);
          }
        }
        break;
    }
  }

  if (lbConsumed)
  {
    vResetExitcounter();
  }

  return lbConsumed;
}


void cBotNetMsgPortSpop::vProcess(u16 lu16TimeDiff_ms)
{
  cJobHandler::vProcess(lu16TimeDiff_ms);

  if (!isBusy())
  {
    if (mu16Exitcounter_ms >= lu16TimeDiff_ms)
    {
      mu16Exitcounter_ms -= lu16TimeDiff_ms;
    }
    else
    {
      mu16Exitcounter_ms = 0;
    }


    if (mu16Exitcounter_ms == 0)
    {
      cBnSpop_vWriteBackupData(0, 0);
      cBnSpop_vWriteBackupData(1, 0);
      cBnSpop_vJumpToApp(0);
    }
  }
  else
  {
    switch ((cBotNetMsgPortSpop::cJobs)mcJob)
    {
      case cJobs::nJobEraseIntFlash:
        // Noch kurz warten damit die Anwort raus kann
        if (cJobHandler::u16GetTime_ms() > 20)
        {
          // Während den Flash-Schreibens die Kommunikation mittel Interrupt-Lock sperren
          // weil Flash schreiben die den Flash blockiert und dann die Interrupt verzögert ausgeführt werden
          // warte bis Uart in Mute mode, damit nicht eine laufende Kommunikation unterbrochen wird
          while ((!(USART_STAT(USART0) & USART_STAT_RWU))) {}   // Nicht im Mute modus
          USART_CTL0(USART0) &= ~USART_CTL0_REN;

            cBnSpop_vFlash_Unlock();
            cBnSpop_vFlash_ErasePage(mu8DataPointer);
            cBnSpop_vFlash_Lock();

          USART_CMD(USART0) |= USART_CMD_RXFCMD;
          USART_CTL0(USART0) |= USART_CTL0_REN;
            
          mcTxMsgTx.mcPayload.Set(0x84);
          mcTxMsgTx.mcPayload.Add(0x01);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);
          vFinished();
        }
        break;

      case cJobs::nJobRead:
      //case cJobs::nJobReadIntRam:
      //case cJobs::nJobReadIntFlash:
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


void cBotNetMsgPortSpop::vProcess10ms()
{
  vProcess(10);
}

