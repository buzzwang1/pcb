#ifndef __BOTNET_LINK_USART_MP_HD_H__
#define __BOTNET_LINK_USART_MP_HD_H__

#include "Typedef.h"

#include "cUsart_MpHd.h"
#include "cStrT.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"


class cBotNet_ComLinkUsartMpHd: public cBotNet_SyncedLinkBase, public cBotNet_ComLinkUsartMpHdCfg
{
public:
  cBotNet_ComLinkUsartMpHd(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
   : cBotNet_SyncedLinkBase(lu32RxComBufSize, lu32TxComBufSize, lenType)
  {
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);

    return True;
  }
};


class cBotNet_DownLinkUsartMpHd: public cBotNet_ComLinkUsartMpHd
{
  public:
  cUartMpHdMaster*  mcDownLink;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgDataTx;
  cComDatMsg mpcMsgDataRx;
  cComDatMsg mpcMsgAckTx;

  u8 mu8Sync;
  u8 mu8SyncRetrys;
  u8 isNormalMessage;
  u8 isReducedMsgReqMode;

  u16 mu16AdrSave;

  cBotNet_SyncedLinkBase::tenStates menSm;
  cBotNet_SyncedLinkBase::tenStates menSmLast;

  cBotNet_DownLinkUsartMpHd(cUartMpHdMaster* lcDownLink)
    : cBotNet_ComLinkUsartMpHd(cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize, cBotNet_LinkBase::enDownLink)
  {
    mcDownLink   = lcDownLink;

    mpcMsgSync.vMemAlloc(2,  2);
    mpcMsgDataTx.vMemAlloc(enCnstMaxDataPlusCheckSum,  1); // 1 Byte Acknowledge
    mpcMsgDataRx.vMemAlloc(0,  enCnstMaxDataPlusCheckSum);

    mpcMsgAckTx.vMemAlloc(1,  0);

    lcDownLink->vAddSlave((cComNode*)this);

    vInit();
  }

  void vInit()
  {
    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    menSmLast = cBotNet_SyncedLinkBase::tenStates::enStIdle;

    mStatus.IsInit = 1;
    mu8SyncRetrys = 0;
    isReducedMsgReqMode = 0;

    vSetOnlineTimeout(200);
  }


  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet;
    lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    mu16AdrSave = mAdr;
    return lbRet;
  }


  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);

    // Wird auch nach 100ms von cBotNet_SyncedLinkBase.vTick10ms offline gesetzt
    if ((menSmLast == cBotNet_SyncedLinkBase::tenStates::enStSync) && (mStatus.IsLastTxOk == 0))
    {
      if (mu8SyncRetrys > 0)
      {
        mu8SyncRetrys--;
        vOnSync();
      }
      else
      {
        mStatus.IsOnline    = 0;
        isReducedMsgReqMode = 0;
      }
    }
    menSm         = cBotNet_SyncedLinkBase::tenStates::enStIdle;
  }

  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    cBotNet_SyncedLinkBase::vOnEnterOffline();
    mStatus.IsFirstTx  = 0;
  }

  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    mStatus.IsFirstTx  = 1;
    // Daten verwerfen
    mpcMsgDataTx.cTxData.muiLen = 0;
    mcTxComBuf.reset();
  }

  void vOnResetCom() override
  {
    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
  }

  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOA->BSRR = 1;
    if (lenEvent == cComNode::tenEvent::enEvPrepareToSendData)
    {
      if (menSm == cBotNet_SyncedLinkBase::tenStates::enStIdle)
      {
        if (mu8Sync)
        {
          mu8Sync = 0;
          mAdr = mu16AdrSave;
          vSyncStart();
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSync;
          vComDone();
        }
      }
      mStatus.IsStartRequested = mu8Sync;
    }
    else if (lenEvent == cComNode::tenEvent::enEvMisc)
    {
      // Misc ist der Timer Interrupt
      if ((menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStEndWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStAckWaitForTimer))
      {
        vComDone();
      }
    }
    //GPIOA->BRR = 1;
  }

  void vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    // GPIOB->BSRR = (1 << 13); // Test
    //GPIOA->BSRR = 1; // Test

    menSmLast = menSm;
    do
    {
      lbLoop     = False;
      switch (menSm)
      {
        case cBotNet_SyncedLinkBase::tenStates::enStIdle:
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSync:
          {
            // Schauen, ob noch Platz ist
            if (mcRxComBuf.space_left() > (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
            {
              mStatus.IsBusy = 0;
            }
            else
            {
              mStatus.IsBusy = 1;
            }

            // Falls vorherige Daten übertragen werden konnten
            // dann nächste Daten laden
            // sonst vorherige Daten beibehalten
            mStatus.IsLastTxOk = mStatus.IsThisTxOk;

            if ((mpcMsgDataTx.cTxData.Len() == 0) || (mStatus.IsLastTxOk) || (mStatus.IsFirstTx))
            {
              // Überprüfen, obes Daten zu senden gibt
              mcTxComBuf.get(&mpcMsgDataTx.cTxData);
              isNormalMessage = 1;
              if (mpcMsgDataTx.cTxData.Len())
              {
                mStatus.IsThisTxOk = 0;
                mStatus.IsFirstTx  = 0;
                mu8MsgCntTx++;
              }

              // Ggf. Adresse entfernen
              if (mpcMsgDataTx.cTxData.Len())
              {
                //Überprüfen, ob der Empfänger direkter Slave ist, wenn ja, dann kann Adressinformation entfernt werden
                cBotNetMsg_Base lcMsg(&mpcMsgDataTx.cTxData);
                lcMsg.vDecode();
                if (lcMsg.mcFrame.mcSAdr.isDirectMasterOf(lcMsg.mcFrame.mcDAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgDataTx.cTxData.muiLen = lcMsg.muiLen;
                }

                //if ((lcMsg.mcFrame.mstControl.NoCheck == cBotNetMsgFrame::tenType::enRMessage) ||
                if ((lcMsg.mcFrame.mstControl.NoCheck) ||
                    (isReducedMsgReqMode))
                {
                  // Bei reduzierter Message, für Low Level gedacht
                  // Busy-Flag wird ignoriert, keine Checksummen, kein Ackowledge
                  isNormalMessage = 0;
                }
              }

              vCreateSync(mpcMsgSync.cTxData.mpu8Data, (uint8)mpcMsgDataTx.cTxData.Len(), 0, mAdr);

              // Ggf. Checksumme berechnen und anfügen
              if (mpcMsgDataTx.cTxData.Len())
              {
                if (isNormalMessage)
                {
                  mpcMsgDataTx.cTxData.Add(mpcMsgDataTx.u8TxChecksum());
                }
                else
                {
                  //Dummy Checksumme
                  mpcMsgDataTx.cTxData.Add(0xCC);
                }
              }
            }

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForSlave;
            mpcMsgSync.vStart();
            mcDownLink->SetLock();
            mcDownLink->vStartMsg(&mpcMsgSync);
          }
          break;
        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForSlave:
          // Timeout wird von cUsart_MpHd.h gemacht
          // Bei Timeout wird vComError aufgerufen
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDone;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDone:
          menSm = enStEndWaitForTimer;

          if (IsSyncNoCheck(mpcMsgSync.cRxData.mpu8Data))
          {
            isNormalMessage     = 0;
            isReducedMsgReqMode = 1;
          }

          //Sync Checksumme ?
          if (IsSyncCheckOk(mpcMsgSync.cRxData.mpu8Data) && (isNormalMessage))
          {
            vOnSync();
            mu8SyncRetrys = 50;

            u8 lu8Busy = mStatus.IsBusy;
            if (mpcMsgSync.cRxData[1] & 128) lu8Busy+=2;

            // Bei normaler Message kommt ein Ack zurück
            mpcMsgDataTx.cRxData.muiLen = 1;

            switch (lu8Busy)
            {
              case 0:  // Master und Slave haben genug Platz
                {
                  mpcMsgDataRx.cRxData.muiLen = mpcMsgSync.cRxData[0];

                  if ((mpcMsgDataRx.cRxData.Len()) || (mpcMsgDataTx.cTxData.Len()))
                  {
                    // Gibt es Daten zu empfangen
                    if (mpcMsgDataRx.cRxData.Len())
                    {
                      mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
                    }

                    if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
                    {
                      // Kurz Warten, damit Slave vorbereiten kann
                      menSm = enStDataWaitForTimer;
                      mcDownLink->mTimer.vStart(enCnstWaitSyncDwn);
                    }
                    else if (mpcMsgDataRx.cRxData.Len()) // Gibt es nur was zu empfangen
                    {
                      menSm = enStDataWait;
                      mpcMsgDataRx.vStart();
                      mcDownLink->vStartMsg(&mpcMsgDataRx);
                    }
                  }
                }
                break;
              case 1:  // Nur Slave hat genug Platz
                {
                  if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
                  {
                    // Kurz Warten, damit Slave vorbereiten kann
                    menSm = enStDataWaitForTimer;
                    mcDownLink->mTimer.vStart(enCnstWaitSyncDwn);
                  }
                }
                break;
              case 2: // Nur Master hat genug Platz
                {
                  mpcMsgDataRx.cRxData.muiLen = mpcMsgSync.cRxData[0];

                  if (mpcMsgDataRx.cRxData.Len())
                  {
                    mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
                    mpcMsgDataRx.vStart();
                    mcDownLink->vStartMsg(&mpcMsgDataRx);
                  }
                }
                break;
              case 3: // Keiner hat genug Platz
                {
                }
                break;
            }
          }
          else
          {
            // Reduzierte Nachricht geht immer
            vOnSync();
            mu8SyncRetrys = 50;

            mpcMsgDataRx.cRxData.muiLen = mpcMsgSync.cRxData[0];

            // Bei reduzierte Message kommt kein Ack zurück
            mpcMsgDataTx.cRxData.muiLen = 0;

            if ((mpcMsgDataRx.cRxData.Len()) || (mpcMsgDataTx.cTxData.Len()))
            {
              // Gibt es Daten zu empfangen
              if (mpcMsgDataRx.cRxData.Len())
              {
                mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
              }

              if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
              {
                // Kurz Warten, damit Slave vorbereiten kann
                menSm = enStDataWaitForTimer;
                mcDownLink->mTimer.vStart(enCnstWaitRSyncDwn);
              }
              else if (mpcMsgDataRx.cRxData.Len()) // Gibt es nur was zu empfangen
              {
                menSm = enStDataWait;
                mpcMsgDataRx.vStart();
                mcDownLink->vStartMsg(&mpcMsgDataRx);
              }
            }
          }

          // Kurz Warten, damit Slave vorbereiten kann
          if (menSm == enStEndWaitForTimer)
          {
             mcDownLink->mTimer.vStart(enCnstWaitEndDwn);
          }

          break;


        // ---------------- Master Tx and Slave Ack

        case enStDataWaitForTimer:
          menSm = enStAckWait;
          if (isNormalMessage)
          {
            mpcMsgDataTx.cRxData.mpu8Data[0] = 0xFF;
          }

          mAdr = 0xFF; // keine Adresse schicken
          // Daten senden und auf Ack warten
          // Ack daten stehen in Rx von der DataMsgTx
          mpcMsgDataTx.vStart();
          mcDownLink->vStartMsg(&mpcMsgDataTx);
          break;

        case enStAckWait:
          if (isNormalMessage)
          {
            if (mpcMsgDataTx.cRxData.mpu8Data[0] == 0xA)
            {
              mStatus.IsThisTxOk = 1;
            }
          }
          else
          {
            mStatus.IsThisTxOk = 1;
          }

          //          Bei normaler Message                               Bei reduzierter Message
          // Master hat genug Platz und kann empfangen                        geht immer
          if (((!mStatus.IsBusy) || (!isNormalMessage)) && (mpcMsgDataRx.cRxData.muiLen > 0))
          {
            menSm = enStDataWait;
            mpcMsgDataRx.vStart();
            mcDownLink->vStartMsg(&mpcMsgDataRx);
          }
          else
          {
            // Kurz Warten, damit Slave die Daten verarbeiten kann
            menSm = enStEndWaitForTimer;
            mcDownLink->mTimer.vStart(enCnstWaitEndDwn);
          }
          break;

        // ----------------- Slave Tx and Master Ack

        case enStDataWait:
          mpcMsgDataRx.cRxData.muiLen--;
          if (isNormalMessage)
          {
            // Kurz Warten, damit Slave vorbereiten kann
            menSm = enStAckWaitForTimer;
            mcDownLink->mTimer.vStart(enCnstWaitComDwn);

            //Checksumme überprüfen
            uint8 lui8ChkSum;

            lui8ChkSum = mpcMsgDataRx.u8RxChecksum();

            if (mpcMsgDataRx.cRxData.mpu8Data[mpcMsgDataRx.cRxData.Len()] != lui8ChkSum)
            {
              mpcMsgAckTx.cTxData[0] = 0xFF;
            }
            else
            {
              mpcMsgAckTx.cTxData[0] = 0x0A;
            }
          }
          else
          {
            // Es gibt kein Ack
            menSm = enStEndWaitForTimer;
            mcDownLink->mTimer.vStart(enCnstWaitEndDwn);
          }

          // Rx Daten abholen und in den Rx-Buffer schreiben
          if (mpcMsgDataRx.cRxData.Len())
          {
            if (IsSyncNewData() || (!isNormalMessage))
            {
              // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von einem Sklaven geschickt
              // Senderadresse == Sklavenadresse Zieladresse == meine Adresse (ich) dazufügen
              cBotNetMsg_Base lcMsg(&mpcMsgDataRx.cRxData);
              lcMsg.vDecode();

              if (!lcMsg.bHasAdress())
              {
                // DownLink, d.h. Source war Slave, Dest bin ich (Master)
                // In mcAdr steht die Slave Adresse drin => Source
                //                 Source                Dest
                lcMsg.vAdressAdd(mcAdr.Get(), mcAdr.GetMasterAdr());
                mpcMsgDataRx.cRxData.muiLen = lcMsg.muiLen;
              }

              mcRxComBuf.put(&mpcMsgDataRx.cRxData);
            }
          }
          break;


        case enStAckWaitForTimer:
          menSm = enStAckDone;
          mAdr = 0xFF; // keine Adresse schicken
          mpcMsgAckTx.vStart();
          mcDownLink->vStartMsg(&mpcMsgAckTx);
          break;


        case enStAckDone:
          menSm = enStEndWaitForTimer;
          mcDownLink->mTimer.vStart(enCnstWaitEndDwn);
          break;


        case enStEndWaitForTimer:
        case enStEnd:
        default:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mcDownLink->ResetLock();
          break;
      }
    } while (lbLoop);
    //GPIOA->BRR = 1;  // Test
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    mcDownLink->vTick10ms();
  }

  void vTick1ms() override
  {
    mcDownLink->vTick1ms();
    mcDownLink->bStartNext();
  }

  void vSync() override
  {
    mu8Sync           = 1;
    mStatus.IsStartRequested = 1;
    vSyncStart();
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkUsartMpHd:public cBotNet_ComLinkUsartMpHd
{
  public:
  cUartMpHdSlave *mcUpLink;

  cComDatMsg mpcMsgSyncRx;
  cComDatMsg mpcMsgSyncTx;

  cComDatMsg mpcMsgDataRx;
  cComDatMsg mpcMsgDataTx;

  cComDatMsg mpcMsgAckRx;
  cComDatMsg mpcMsgAckTx;

  cBotNet_SyncedLinkBase::tenStates  menSm;

  cBotNet_UpLinkUsartMpHd(cUartMpHdSlave* lcUpLink)
  : cBotNet_ComLinkUsartMpHd(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink)
  {
    vSet(lcUpLink);

    mpcMsgSyncTx.vMemAlloc(2,  0);
    mpcMsgSyncRx.vMemAlloc(0,  2 + 1); // +1 wegen Id/Adressbyte

    mpcMsgDataTx.vMemAlloc(enCnstMaxDataPlusCheckSum,  0);
    mpcMsgDataRx.vMemAlloc(0,  enCnstMaxDataPlusCheckSum);

    mpcMsgAckTx.vMemAlloc(1,  0);
    mpcMsgAckRx.vMemAlloc(0,  1);

    vInit();
  }

  void vInit()
  {
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    mStatus.IsInit = 1;
    vSetOnlineTimeout(500);
  }


  bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    menSm = enStSyncWait;
    mcUpLink->vSetNode((cComNode*)this);
    return lbRet;
  }


  void vSet(cUartMpHdSlave* lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    (void)lenError; // avoid warning
    (void)lenState; // avoid warning
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    vComPrepareRx();
  }

  // Eine Art von Watchdog. Kommt von cBotNet_SyncedLinkBase
  // Wurde seit 200ms kein vOnSync aufgerufen, wird der Link deaktiviert
  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    cBotNet_SyncedLinkBase::vOnEnterOffline();
    mStatus.IsFirstTx  = 0;
  }

  // Kommt von cBotNet_SyncedLinkBase
  // Nach 10s offline wird das hier aufgerufen
  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    mStatus.IsFirstTx  = 1;
    // Daten verwerfen
    mpcMsgDataTx.cTxData.muiLen = 0;
    mcTxComBuf.reset();
  }

  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOB->BSRR = (1 << 4);  // Test
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait) //Sync
    {
      vComPrepareRx();
    }
    else if (lenEvent == cComNode::tenEvent::enEvMisc)
    {
      // Misc ist der Timer Interrupt
      if ((menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStEndWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStAckWaitForTimer))
      {
        vComDone();
      }
    }
  }

  void vComPrepareRx() // __attribute__((optimize("-O0")))
  {
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait)
    {
      mcUpLink->vMute();
      // Master schickt Aresse + 2 Byte Sync Nachricht (wird vom DMA erledigt)
      mcUpLink->vStartMsg(&mpcMsgSyncRx);

      mStatus.IsLastTxOk = mStatus.IsThisTxOk;
      if ((mStatus.IsLastTxOk) || (mStatus.IsFirstTx) || (mpcMsgDataTx.cTxData.muiLen == 0))
      {
        // Daten holen
        mcTxComBuf.get(&mpcMsgDataTx.cTxData);
        if (mpcMsgDataTx.cTxData.muiLen)
        {
          mu8MsgCntTx++; // Nachrichtenzähler
          mStatus.IsThisTxOk = 0;
          mStatus.IsFirstTx  = 0;
        }

        // Ggf. Adresse entfernen
        if (mpcMsgDataTx.cTxData.Len())
        {
          //Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
          cBotNetMsg_Base lcMsg(&mpcMsgDataTx.cTxData);
          lcMsg.vDecode();
          if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
          {
            lcMsg.vAdressRemove();
            mpcMsgDataTx.cTxData.muiLen = lcMsg.muiLen;
          }
        }

        // Ggf. Checksumme berechnen und anfügen
        if (mpcMsgDataTx.cTxData.Len())
        {
          mpcMsgDataTx.cTxData.Add(mpcMsgDataTx.u8TxChecksum());
        }
      }


      // Schauen, ob noch Platz ist, ansonsten Busy setzen
      if (mcRxComBuf.space_left() < (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
      {
        mStatus.IsBusy = 1;
      }
      else
      {
        mStatus.IsBusy = 0;
      }

      if (mpcMsgDataTx.cTxData.Len())
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, (uint8)mpcMsgDataTx.cTxData.Len() - 1, 0); // -1 Wegen checksumme
      }
      else
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, 0, 0);
      }
    }
  }


  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    // GPIOB->BSRR = (1 << 5);  // Test

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        case cBotNet_SyncedLinkBase::tenStates::enStSyncWait:
          {
            u8 lu8Error = 1;

            if (mpcMsgSyncRx.cRxData.mpu8Data[0] == mAdr)
            {
              // Sync
              vOnSync();
              mpcMsgDataRx.cRxData.muiLen = mpcMsgSyncRx.cRxData.mpu8Data[1];

              // Sync Checksumme ?
              if (IsSyncCheckOk((mpcMsgSyncRx.cRxData.mpu8Data+1), mAdr))
              {
                if (mpcMsgDataRx.cRxData.Len())
                {
                  mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
                }
                lu8Error = 0;

                // Direkt senden, auf Master muss nicht gewartet werden
                mcUpLink->vStartMsg(&mpcMsgSyncTx);
                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDone;
              }
            }

            if (lu8Error)
            {
              menSm = enStEnd;
              lbLoop = True;
            }
          }
        break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDone:
          {
            u8 lu8Busy = mStatus.IsBusy;
            if (mpcMsgSyncRx.cRxData.mpu8Data[2] & 128) lu8Busy+=2;

            switch (lu8Busy)
            {
              case 0:  // Master und Slave haben genug Platz
                {
                  if (mpcMsgDataRx.cRxData.Len()) // Gibt es was zu empfangen
                  {
                    menSm = enStDataWait;
                    mcUpLink->vStartMsg(&mpcMsgDataRx);
                  }
                  else if (mpcMsgDataTx.cTxData.Len()) // Gibt es nur was zu senden
                  {
                    // Kurz Warten, damit Master vorbereiten kann
                    menSm = enStDataWaitForTimer;
                    mcUpLink->mTimer.vStart(enCnstWaitSyncUp);
                  }
                  else // Es werden keine Daten transferiert
                  {
                    menSm = enStEnd;
                    lbLoop = True;
                  }
                }
                break;
              case 1:  // Nur Master hat genug Platz
                {
                  if (mpcMsgDataTx.cTxData.Len()) // Gibt es nur was zu senden
                  {
                    // Kurz Warten, damit Master vorbereiten kann
                    menSm = enStDataWaitForTimer;
                    mcUpLink->mTimer.vStart(enCnstWaitSyncUp);
                  }
                  else // Es werden keine Daten transferiert
                  {
                    menSm = enStEnd;
                    lbLoop = True;
                  }
                }
                break;
              case 2: //  Nur Slave hat genug Platz
                {
                  if (mpcMsgDataRx.cRxData.Len()) // Gibt es was zu empfangen
                  {
                    menSm = enStDataWait;
                    mcUpLink->vStartMsg(&mpcMsgDataRx);
                  }
                  else // Es werden keine Daten transferiert
                  {
                    menSm = enStEnd;
                    lbLoop = True;
                  }
                }
                break;

              case 3: // Keiner hat genug Platz
                {
                  menSm = enStEnd;
                  lbLoop = True;
                }
                break;
            }
          }
          break;

        // ----------------- Master Tx Slave Ack

        case enStDataWait:
          {
            // Direkt senden, auf Master muss nicht gewartet werden
            mcUpLink->vStartMsg(&mpcMsgAckTx);
            menSm = enStAckDone;

            // Checksumme der empfangenen Daten überprüfen
            // Falls ok, dann wird die Ack Nachricht geACK't
            // ansonsten geNACK't
            {
              uint8 lui8ChkSum;

              mpcMsgDataRx.cRxData.muiLen--; //Checksumme abschneiden
              lui8ChkSum = mpcMsgDataRx.u8RxChecksum();

              if (mpcMsgDataRx.cRxData.mpu8Data[mpcMsgDataRx.cRxData.Len()] == lui8ChkSum)
              {
                // Slave Ack
                mpcMsgAckTx.cTxData.mpu8Data[0] = 0xA; //Acknowledge
              }
              else
              {
                // Slave Nack
                mpcMsgAckTx.cTxData.mpu8Data[0] = 0xF; // Fail
              }
            }

            // Wenn Daten empfangen wurden, dann
            // Rx Daten abholen und in den Rx-Buffer schreiben
            //
            if (IsSyncNewData())
            {
              // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht vonm direkten Master
              // Senderadresse == Masteradresse  Zieladresse == meine Adresse (ich) dazufügen
              cBotNetMsg_Base lcMsg(&mpcMsgDataRx.cRxData);
              lcMsg.vDecode();

              if (!lcMsg.bHasAdress())
              {
                // UpLink, d.h. Source war Master, Dest bin ich (Slave)
                // In mcAdr steht meine Adresse drin => Dest
                //                 Source                Dest
                lcMsg.vAdressAdd(mcAdr.GetMasterAdr(), mcAdr.Get());
                mpcMsgDataRx.cRxData.muiLen = lcMsg.muiLen;
              }

              mcRxComBuf.put(&mpcMsgDataRx.cRxData);
            }
          }
          break;

        case enStAckDone:
          {
            u8 lu8Busy = (mpcMsgSyncRx.cRxData.mpu8Data[2] & 128);

            if (!lu8Busy)
            {
              if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
              {
                // Kurz Warten, damit Master vorbereiten kann
                menSm = enStDataWaitForTimer;
                mcUpLink->mTimer.vStart(enCnstWaitComUp);
              }
              else // Es werden keine Daten transferiert
              {
                menSm = enStEnd;
                lbLoop = True;
              }
            }
            else
            {
              menSm = enStEnd;
              lbLoop = True;
            }
          }
          break;

        // ----------------- Slave Tx Master Ack

        case enStDataWaitForTimer:
          {
            mcUpLink->vStartMsg(&mpcMsgDataTx);
            menSm = enStDataDone;
          }
          break;

        case enStDataDone:
          {
            menSm = enStAckWait;
            mcUpLink->vStartMsg(&mpcMsgAckRx);
          }
          break;

        case enStAckWait:
          {
            menSm = enStEnd;
            lbLoop = True;

            if (mpcMsgAckRx.cRxData.mpu8Data[0] == 0xA)
            {
              mStatus.IsThisTxOk = 1;
            }
          }
          break;

        case enStEnd:
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
            vComPrepareRx();
          }
        break;

        default:
          break;
      }
    } while (lbLoop);
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    mcUpLink->vTick10ms();
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
    mStatus.IsStartRequested = 1;
    vSyncStart();
    mcUpLink->bStartNext();
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkUsartMpHd_RMsg :public cBotNet_ComLinkUsartMpHd
{
public:
  cUartMpHdSlave* mcUpLink;

  cComDatMsg mpcMsgSyncRx;
  cComDatMsg mpcMsgSyncTx;

  cComDatMsg mpcMsgDataRx;
  cComDatMsg mpcMsgDataTx;

  cBotNet_SyncedLinkBase::tenStates  menSm;

  cBotNet_UpLinkUsartMpHd_RMsg(cUartMpHdSlave* lcUpLink)
    : cBotNet_ComLinkUsartMpHd(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink)
  {
    mStatus.IsInit = 1;

    vSet(lcUpLink);

    mpcMsgSyncTx.vMemAlloc(2, 0);
    mpcMsgSyncRx.vMemAlloc(0, 2 + 1); // +1 wegen Id/Adressbyte

    mpcMsgDataTx.vMemAlloc(enCnstMaxDataPlusCheckSum, 0);
    mpcMsgDataRx.vMemAlloc(0, enCnstMaxDataPlusCheckSum);

    //menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;

    vSetOnlineTimeout(500);
  }

  bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    mcUpLink->vSetNode((cComNode*)this);
    return lbRet;
  }


  void vSet(cUartMpHdSlave* lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    (void)lenError; // avoid warning
    (void)lenState; // avoid warning
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    vComPrepareRx();
  }

  void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  {
    cBotNet_SyncedLinkBase::vOnEnterOffline();
  }

  void vOnOffline10s() override // __attribute__((optimize("-O0")))
  {
    // Daten verwerfen
    mpcMsgDataTx.cTxData.muiLen = 0;
    mcTxComBuf.reset();
  }

  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOB->BSRR = (1 << 4);  // Test
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait) //Sync
    {
      vComPrepareRx();
    }
    else if (lenEvent == cComNode::tenEvent::enEvMisc)
    {
      // Misc ist der Timer Interrupt
      if ((menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTimer) ||
          (menSm == cBotNet_SyncedLinkBase::tenStates::enStEndWaitForTimer))
      {
        vComDone();
      }
    }
  }

  void vComPrepareRx() // __attribute__((optimize("-O0")))
  {
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncWait)
    {
      mcUpLink->vMute();
      // Master schickt Aresse + 2 Byte Sync Nachricht (wird vom DMA erledigt)
      mcUpLink->vStartMsg(&mpcMsgSyncRx);
      {
        // Daten holen
        mcTxComBuf.get(&mpcMsgDataTx.cTxData);
        if (mpcMsgDataTx.cTxData.muiLen)
        {
          mu8MsgCntTx++; // Nachrichtenzähler
        }

        // Ggf. Adresse entfernen
        if (mpcMsgDataTx.cTxData.Len())
        {
          //Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
          cBotNetMsg_Base lcMsg(&mpcMsgDataTx.cTxData);
          lcMsg.vDecode();
          if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
          {
            lcMsg.vAdressRemove();
            mpcMsgDataTx.cTxData.muiLen = lcMsg.muiLen;
          }
        }

        // Ggf. Checksumme berechnen und anfügen
        if (mpcMsgDataTx.cTxData.Len())
        {
          // Dummy Checksumme
          mpcMsgDataTx.cTxData.mpu8Data[mpcMsgDataTx.cTxData.muiLen] = 0xCC;
          mpcMsgDataTx.cTxData.muiLen++;
        }
      }

      mStatus.IsBusy = 0;

      if (mpcMsgDataTx.cTxData.Len())
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, (uint8)mpcMsgDataTx.cTxData.Len() - 1, 1); // -1 Wegen checksumme
      }
      else
      {
        vCreateSync(mpcMsgSyncTx.cTxData.mpu8Data, 0, 1);
      }
    }
  }


  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    // GPIOB->BSRR = (1 << 5);  // Test

    do
    {
      lbLoop = False;
      switch (menSm)
      {
      case cBotNet_SyncedLinkBase::tenStates::enStSyncWait:
      {
        u8 lu8Error = 1;

        if (mpcMsgSyncRx.cRxData[0] == mAdr)
        {
          // Sync
          vOnSync();
          mpcMsgDataRx.cRxData.muiLen = mpcMsgSyncRx.cRxData[1];

          // Sync Checksumme ?
          if (IsSyncCheckOk((mpcMsgSyncRx.cRxData.mpu8Data + 1), mAdr))
          {
            if (mpcMsgDataRx.cRxData.Len())
            {
              mpcMsgDataRx.cRxData.muiLen++; // +1 für Checksumme
            }
            lu8Error = 0;

            // Kurz Warten, damit Master vorbereiten kann
            if (((u8)enCnstWaitRSyncUp) != 0)
            {
              menSm = enStSyncWaitForTimer;
              mcUpLink->mTimer.vStart(enCnstWaitRSyncUp);
            }
            else
            {
              // Direkt senden, auf Master muss nicht gewartet werden
              mcUpLink->vStartMsg(&mpcMsgSyncTx);
              menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDone;
            }
          }
        }

        if (lu8Error)
        {
          menSm = enStEnd;
          lbLoop = True;
        }
      }
      break;

      case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTimer:
      {
        mcUpLink->vStartMsg(&mpcMsgSyncTx);
        menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDone;
        break;
      }

      case cBotNet_SyncedLinkBase::tenStates::enStSyncDone:
      {
        if (mpcMsgDataRx.cRxData.Len()) // Gibt es was zu empfangen
        {
          menSm = enStDataWait;
          mcUpLink->vStartMsg(&mpcMsgDataRx);
        }
        else if (mpcMsgDataTx.cTxData.Len()) // Gibt es nur was zu senden
        {
          // Kurz Warten, damit Master vorbereiten kann
          if (((u8)enCnstWaitRDataUp) != 0)
          {
            menSm = enStDataWaitForTimer;
            mcUpLink->mTimer.vStart(enCnstWaitRDataUp);
          }
          else
          {
            for (u8 lu8t = 0; lu8t < 40; lu8t++)
            {
              __asm volatile ("nop");
            }
            mcUpLink->vStartMsg(&mpcMsgDataTx);
            menSm = enStDataDone;
          }
        }
        else // Es werden keine Daten transferiert
        {
          menSm = enStEnd;
          lbLoop = True;
        }
      }
      break;

      // ----------------- Master Tx Slave Rx

      case enStDataWait:
      {

        menSm = enStAckDone;
        mpcMsgDataRx.cRxData.muiLen--;

        // Wenn Daten empfangen wurden, dann
        // Rx Daten abholen und in den Rx-Buffer schreiben
        //
        if (IsSyncNewData())
        {
          // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht vonm direkten Master
          // Senderadresse == Masteradresse  Zieladresse == meine Adresse (ich) dazufügen
          cBotNetMsg_Base lcMsg(&mpcMsgDataRx.cRxData);
          lcMsg.vDecode();

          if (!lcMsg.bHasAdress())
          {
            // UpLink, d.h. Source war Master, Dest bin ich (Slave)
            // In mcAdr steht meine Adresse drin => Dest
            //                 Source                Dest
            lcMsg.vAdressAdd(mcAdr.GetMasterAdr(), mcAdr.Get());
            mpcMsgDataRx.cRxData.muiLen = lcMsg.muiLen;
          }

          mcRxComBuf.put(&mpcMsgDataRx.cRxData);
        }

        if (mpcMsgDataTx.cTxData.Len()) // Gibt es was zu senden
        {
          // Kurz Warten, damit Master vorbereiten kann
          if (((u8)enCnstWaitRDataUp) != 0)
          {
            menSm = enStDataWaitForTimer;
            mcUpLink->mTimer.vStart(enCnstWaitRDataUp);
          }
          else
          {
            for (u8 lu8t = 0; lu8t < 40; lu8t++)
            {
              __asm volatile ("nop");
            }
            mcUpLink->vStartMsg(&mpcMsgDataTx);
            menSm = enStDataDone;
          }
        }
        else // Es werden keine Daten transferiert
        {
          menSm = enStEnd;
          lbLoop = True;
        }
      }
      break;

      // ----------------- Slave Tx Master Ack

      case enStDataWaitForTimer:
      {
        mcUpLink->vStartMsg(&mpcMsgDataTx);
        menSm = enStDataDone;
      }
      break;

      case enStDataDone:
      {
        menSm = enStEnd;
        lbLoop = True;
      }
      break;

      case enStEnd:
      {
        menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
        vComPrepareRx();
      }
      break;

      default:
        break;
      }
    } while (lbLoop);
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
    mStatus.IsStartRequested = 1;
    vSyncStart();
    mcUpLink->bStartNext();
  }
};

#endif // __BOTNET_LINK_USART_MP_HD_H__
