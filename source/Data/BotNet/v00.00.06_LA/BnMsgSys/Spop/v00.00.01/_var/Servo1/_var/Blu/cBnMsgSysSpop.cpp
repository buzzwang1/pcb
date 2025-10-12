#include "cBnMsgSysSpop.h"
#include "cBotnet.h"

void cBotNetMsgPortSpop::vPrepareMsg(cBotNetMsg_MsgProt& lcMsg, uint8 lu8D0, uint16 lu8D1)
{
  mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
  mcTxMsgTx.mcPayload.Set(lu8D0);
  mcTxMsgTx.mcPayload.Add(lu8D1);
}


void cBotNetMsgPortSpop::vEnter(uint16 lu16SAdr, uint16 lu16DAdr, cBotNet* lpcBn)
{
  u8 lu8Temp[4];
  mbEnable = True;
  mcTxMsgTx.vPrepare(lu16SAdr, lu16DAdr, 3);
  mcTxMsgTx.mcPayload.Set(0x81);
  mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index RomConst
  mcTxMsgTx.mcPayload.Add(u8U32toU8((u32)ROMCONST_PARTITION_START_ADRESS_EEP, lu8Temp), 4); // Adresse RomConst.
  mcTxMsgTx.mcPayload.Add(0x00);  // Memory Index BTR
  mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Adress BTR
  mcTxMsgTx.mcPayload.Add(u8U32toU8(0, lu8Temp), 4); // Btr Größe in Byte
  mcTxMsgTx.mcPayload.Add(u8U32toU8(cBotNet_UpLinkComBufSize, lu8Temp), 4); // Combuf Größe in Bytes
  mcTxMsgTx.vEncode();
  lpcBn->bSendMsg(&mcTxMsgTx);
}

bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn)
{
  bool lbConsumed = False;

  if ((mbIsReadInProgress) || 
      (mbIsSectorEreaseInProgress) ||
      (mbIsResetInProgress)) return False;

  if ((lcMsg.mu16Idx == 3) &&
      ((mbEnable) || (lcMsg.mcPayload[0] == 1)))
  {
    u16 lu16SubIdx = lcMsg.mcPayload[0];
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
          mbIsResetInProgress = True;
          mbIsResetInProgressDelay = 100; //ms
        }
        break;
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lcMsg.mcPayload.mpu8Data[1] == 0x12) &&
              (lcMsg.mcPayload.mpu8Data[2] == 0x34))
          {
            vEnter(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), lpcBn);
          }
          //else
          //{
          //  vPrepareMsg(lcMsg, 0x81, 0xFE);
          //  mcTxMsgTx.vEncode();
          //  lpcBn->bSendMsg(&mcTxMsgTx);
          //}
        }
        break;

      case 0x04: // EEPROM/Flash Sektor löschen
        lbConsumed = True;
        {
          u32    lu32Adress;
          lu32Adress = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
          mu8DataPointer = (uint8*)lu32Adress;
          if (cBnSpop_u16GetMemorySort(mu8DataPointer) != RomConst_Sort_Ram)
          {
            u8 lu8Temp[4];
            vPrepareMsg(lcMsg, 0x84, 0x00);
            mcTxMsgTx.mcPayload.Add(u8U32toU8(100, lu8Temp), 4);
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
            mbIsSectorEreaseInProgress = True;
            mbIsResetInProgressDelay   = 20;
          }
          else
          {
            vPrepareMsg(lcMsg, 0x84, 0xFE);
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
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

            if (cBnSpop_u16GetMemorySort(mu8DataPointer) != RomConst_Sort_Ram)
            {
              cMemTools::vMemCpy(mu8FlashWriteBuf, (u8*)(&lcMsg.mcPayload.mpu8Data[2]), lu8Size);

              // Während den Flash-Schreibens die Kommunikation mittel Interrupt-Lock sperren
              // weil Flash schreiben die den Flash blockiert und dann die Interrupt verzögert ausgeführt werden
              // warte bis Uart in Mute mode, damit nicht eine laufende Kommunikation unterbrochen wird
              while ((!(USART_STAT(USART0) & USART_STAT_RWU)))    // Nicht im Mute modus
              {}
              USART_CTL0(USART0) &= ~USART_CTL0_REN;
              mu32DataChecksum += cBnSpop_vFlash_Program(mu8DataPointer, mu8FlashWriteBuf, lu8Size);
              USART_CMD(USART0)  |= USART_CMD_RXFCMD;
              USART_CTL0(USART0) |= USART_CTL0_REN;
            }
            else
            {
              mu32DataChecksum += vMemCopy((u8*)(&lcMsg.mcPayload.mpu8Data[2]), mu8DataPointer, lu8Size);
            }

            if (lu8PackageCnt == 0xFF)
            {
              u8 lu8Temp[4];

              if (cBnSpop_u16GetMemorySort(mu8DataPointer) != RomConst_Sort_Ram)
              {
                cBnSpop_vFlash_Lock();
              }

              vPrepareMsg(lcMsg, 0x86, 0xFC);
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

            if (cBnSpop_u16GetMemorySort(mu8DataPointer) != RomConst_Sort_Ram)
            {
              cBnSpop_vFlash_Unlock();
            }

            vPrepareMsg(lcMsg, 0x86, 0x00);
            mcTxMsgTx.vEncode();
            lpcBn->bSendMsg(&mcTxMsgTx);
          }
        }
        break;

      case 0x08: // Checksumme erstellen
        lbConsumed = True;
        mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
        mu8DataPointer = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 2));
        mu32DataCnt = u32U8toU32((lcMsg.mcPayload.mpu8Data + 2 + 4));
        mu32DataChecksum = 1;
        if (mu32DataCnt > 0)
        {
          u8 lu8Temp[4];

          mcBn = lpcBn;
          vPrepareMsg(lcMsg, 0x88, 0x00);
          mcTxMsgTx.mcPayload.Add(u8U32toU8((mu32DataCnt / (1024*16) + 1) * 10, lu8Temp), 4);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);

          mu32DataChecksum += ui32ChkSum(mu8DataPointer, mu32DataCnt);
          vPrepareMsg(lcMsg, 0x86, 0x01);
          mcTxMsgTx.mcPayload.Add(u8U32toU8(mu32DataChecksum, lu8Temp), 4);
          mcTxMsgTx.vEncode();
          mcBn->bSendMsg(&mcTxMsgTx);
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
          uint8* lpui8Adr;
          lpui8Adr = (u8*)u32U8toU32((lcMsg.mcPayload.mpu8Data + 1));

          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          vPrepareMsg(lcMsg, 0x91, 0x00);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);

          if (lpui8Adr != 0)
          {
            cBnSpop_vJumpToApp(lpui8Adr);
          }
          else
          {
            mbIsResetInProgress = True;
            mbIsResetInProgressDelay = 100; /*ms*/
          }
        }
        break;
    }
  }
  return lbConsumed;
}



void cBotNetMsgPortSpop::vProcess10ms()
{
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
  else
  {
    if (mbIsSectorEreaseInProgress)
    {
      if (mbIsResetInProgressDelay > 10)
      {
        mbIsResetInProgressDelay -= 10;
      }
      else
      {
        cBnSpop_vFlash_Unlock();
        // Während den Flash-Schreibens die Kommunikation mittel Interrupt-Lock sperren
        // weil Flash schreiben die den Flash blockiert und dann die Interrupt verzögert ausgeführt werden
        // warte bis Uart in Mute mode, damit nicht eine laufende Kommunikation unterbrochen wird
        while ((!(USART_STAT(USART0) & USART_STAT_RWU)))    // Nicht im Mute modus
        {
        }
        USART_CTL0(USART0) &= ~USART_CTL0_REN;
        cBnSpop_vFlash_EreasePage(mu8DataPointer);
        USART_CMD(USART0)  |= USART_CMD_RXFCMD;
        USART_CTL0(USART0) |= USART_CTL0_REN;
        cBnSpop_vFlash_Lock();

        vPrepareMsg(mcTxMsgTx, 0x84, 0x01);
        mcTxMsgTx.vFlipAdress();
        mcTxMsgTx.vEncode();
        mcBn->bSendMsg(&mcTxMsgTx);
        mbIsSectorEreaseInProgress = False;
      }
    }
  }
}







