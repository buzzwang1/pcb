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
  // Timout für eine Kommunikation
  u16 mu16TimeoutCounter_ms;
  u16 mu16TimeoutCounterReload_ms;

  cBotNet_ComLinkUsartMpHd(u8* lpu8RxComBuf, u32 lu32RxComBufSize, u8* lpu8TxComBuf, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
   : cBotNet_SyncedLinkBase(lpu8RxComBuf, lu32RxComBufSize, lpu8TxComBuf, lu32TxComBufSize, lenType)
  {
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);
    return True;
  }

  void vSetTimeoutReload(u32 lu32Baud)
  {
    // Max Übertragung: ~70B + 70B = ~140B = ~1400Bit
    // Bei 400khz:  3,5ms
    // Bei 300khz:  4,6ms
    // Bei 200khz:  7,0ms
    // Bei 100khz: 14,0ms
    // Bei  50khz: 28,0ms
    // Bei  40khz: 35,0ms
    // Bei  20khz: 70,0ms
    mu16TimeoutCounterReload_ms = 20;
    if (lu32Baud <= 300000) mu16TimeoutCounterReload_ms = 20;
    if (lu32Baud <= 200000) mu16TimeoutCounterReload_ms = 20;
    if (lu32Baud <= 100000) mu16TimeoutCounterReload_ms = 40;
    if (lu32Baud <= 50000)  mu16TimeoutCounterReload_ms = 50;
    if (lu32Baud <= 40000)  mu16TimeoutCounterReload_ms = 60;
    if (lu32Baud <= 20000)  mu16TimeoutCounterReload_ms = 100;
  }
};


class cBotNet_DownLinkUsartMpHd: public cBotNet_ComLinkUsartMpHd
{
  public:
  u8 mpu8ComBufRx[cBotNet_DownLinkComBufSize];
  u8 mpu8ComBufTx[cBotNet_DownLinkComBufSize];

  cUartMpHdMaster*  mcDownLink;

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2];
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR; u8 MsgDataRxRBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataT; u8 MsgDataTxRBuf[enCnstMaxDataPlusCheckSum];

  u16  mu16AdrSave;
  u8   mu8WaitForSlave;
  bool mbSBusy;
  bool mbAlwaysNoCheck;

  cBotNet_SyncedLinkBase::tenStates   menSm;

  cBotNet_DownLinkUsartMpHd(cUartMpHdMaster* lcDownLink, bool lbAlwaysNoCheck = False)
    : cBotNet_ComLinkUsartMpHd(mpu8ComBufRx, sizeof(mpu8ComBufRx), mpu8ComBufTx, sizeof(mpu8ComBufTx), cBotNet_LinkBase::enDownLink),
      mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
      mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
      mpcMsgDataR(null,                              0, MsgDataRxRBuf, sizeof(MsgDataRxRBuf)),
      mpcMsgDataT(MsgDataTxRBuf, sizeof(MsgDataTxRBuf), null,          0)
  {
    mcDownLink      = lcDownLink;
    mbAlwaysNoCheck = lbAlwaysNoCheck;

    mpcMsgSyncR.cRxData.muiLen = sizeof(MsgSyncRxRBuf);
    mpcMsgSyncT.cTxData.muiLen = sizeof(MsgSyncTxRBuf);

    vSetTimeoutReload(lcDownLink->mu32Baudrate);
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    mcDownLink->vAddSlave((cComNode*)this);
    mu16AdrSave = mAdr;
    vOnResetCom();
    return lbRet;
  }


  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);

    switch(lenError)
    {
      case cComNode::tenError::enErStuck:           cBnErrCnt::vInc(cBnErrCnt::tenErr::enStuck);          break;
      case cComNode::tenError::enErNACK:            cBnErrCnt::vInc(cBnErrCnt::tenErr::enNack);           break;
      case cComNode::tenError::enErTimeout:         cBnErrCnt::vInc(cBnErrCnt::tenErr::enTimeout);        break;
      case cComNode::tenError::enErStartWhileBussy: cBnErrCnt::vInc(cBnErrCnt::tenErr::enStartWhileBusy); break;
      default: cBnErrCnt::vInc(cBnErrCnt::tenErr::enMisc); break;
    }

    mcDownLink->vSupressErrorHandling();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
    vSm(cComNode::tenEvent::enEvError);
  }

  // Aufgefrufen, wenn Online und 200ms kein Sync
  //void vOnEnterOffline() override
  //{
  //  cBotNet_SyncedLinkBase::vOnEnterOffline();
  //}

  // Aufgerufen nach 10s Offline
  //void vOnOffline10s() override // __attribute__((optimize("-O0")))
  //{
  //  cBotNet_SyncedLinkBase::vOnOffline10s();
  //}

  void vOnResetCom() override
  {
    cBotNet_SyncedLinkBase::vOnResetCom();

    mStatus.IsInit = 1;
    mu16TimeoutCounter_ms = 0;
    mbSBusy = False;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
  }

  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    //GPIOA->BSRR = 1;
    vSm(lenEvent);
    //GPIOA->BRR = 1;
  }

  void vComDone() override  // __attribute__((optimize("-O0")))
  {
    vSm(cComNode::tenEvent::enEvDone);
  }

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    bool lbLoop;
    // GPIOB->BSRR = (1 << 13); // Test
    //GPIOA->BSRR = 1; // Test

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        case cBotNet_SyncedLinkBase::tenStates::enStIdle:
          mu8WaitForSlave = 0;

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;

          //
          // ----------------------------------- Sync Tx -----------------------------------
          //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx:
        {
          if (lenEvent == cComNode::tenEvent::enEvPrepareToSendData)
          {
           u8 lu8NoCheck = 0;
           u8 lu8OneWay = 0;

            // Wurden vorherige Daten Acknowledged ?
            if ((IsAckTx()) || (mpcMsgDataT.cTxData.muiLen == 0))
            {
              // Neue Daten laden
              ////GPIOA->BSRR = (1 << 15);
              mcTxComBuf.get(&mpcMsgDataT.cTxData);
              ////GPIOA->BSRR = (1 << (16 + 15));

              if (mpcMsgDataT.cTxData.muiLen)
              {
                cBotNetMsg_Base lcMsg(&mpcMsgDataT.cTxData);
                lcMsg.vDecode();

                // Ggf. Adresse entfernen
                // Überprüfen, ob der Empfänger Slave ist, wenn ja, dann kann Adressinformation entfernt werden
                if (lcMsg.mcFrame.mcSAdr.isDirectMasterOf(lcMsg.mcFrame.mcDAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgDataT.cTxData.muiLen = lcMsg.muiLen;
                }

                u8 lui8ChkSum;
                if (lcMsg.isNoCkeck() || (mbAlwaysNoCheck))
                {
                  lu8NoCheck = 1;
                  vSetAckTx(True);
                  lui8ChkSum = 0xCC;
                }
                else
                {
                  vSetAckTx(False);
                  ////GPIOA->BSRR = (1 << 15);
                  lui8ChkSum = mpcMsgDataT.u8TxChecksum();
                  ////GPIOA->BSRR = (1 << (16 + 15));
                }

                if (lcMsg.isOneWay())
                {
                  vSetAckTx(True);
                  lu8OneWay = 1;
                }

                mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
                mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
                mu8MsgCntTx++;
              }
            }
            else
            {
              if (mpcMsgDataT.cTxData.muiLen)
              {
                // !!! Hier ist noch Len+1 wegen Checksumme
                if (IsDataNoCheck(&mpcMsgDataT.cTxData[mpcMsgDataT.cTxData.muiLen - 2])) lu8NoCheck = 1;
                if (IsDataOneWay(&mpcMsgDataT.cTxData[mpcMsgDataT.cTxData.muiLen - 2]))  lu8OneWay  = 1;
              }
            }

            vUpdateBusy();

            vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), lu8NoCheck, lu8OneWay);
 
            // Die ganze Kommunikation darf nicht mehr als wie 4ms dauern
            mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;
          }

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx;
          lbLoop = True;
        }
        break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx:
          // Nichts zuwarten
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;

          //WarteZeit vor Sync kann man sich sparen
          //if (enCnstWaitRSyncDwn)
          //{
          //  mcDownLink->mTimer.vStart(enCnstWaitRSyncDwn);
          //}
          //else
          //{
            lbLoop = True;
          //}
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
          // Slave-Sync starten
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          mAdr = mu16AdrSave;
          mcDownLink->SetLock((u8)mAdr);
          mpcMsgSyncT.vStart();
          mcDownLink->vStartMsg(&mpcMsgSyncT);
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              // NoCheck immer senden
              if ((IsSyncNoCheck(mpcMsgSyncT.cTxData.mpu8Data)) ||
                  (IsSyncOneWay(mpcMsgSyncT.cTxData.mpu8Data)) ||
                  (!mbSBusy))
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx;
                lbLoop = True;
                break;
              }
            }

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
            lbLoop = True;
          }
          break;

          //
          // ----------------------------------- Daten Tx -----------------------------------
          //
          case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx:
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx;
            lbLoop = True;
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx:
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
            // Kurz warten, damit Slave Sync verarbeiten und Empfangen
            // vorbereiten kann
            if (enCnstWaitRDataDwn > 0)
            {
              mcDownLink->mTimer.vStart(enCnstWaitRDataDwn);
            }
            else
            {
              lbLoop = True;
            }
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
            // Slave-Adressierung starten
            mAdr = 0xFF; // keine Adresse schicken
            mpcMsgDataT.vStart();
            mcDownLink->vStartMsg(&mpcMsgDataT);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              // Broadcast ? Dann nicht antworten.
              // !!! Hier ist noch Len+1 wegen Checksumme
              if (IsSyncOneWay(mpcMsgSyncT.cTxData.mpu8Data))
              {
                vOnSync();
                menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
                lbLoop = True;
                break;
              }

              menSm  = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
              lbLoop = True;
            }
            break;

        //
        // ----------------------------------- Sync Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForRx;
          // Slave-Adressierung starten
          // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
          mpcMsgSyncR.vStart();
          mcDownLink->vStartMsg(&mpcMsgSyncR);
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartRx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
          lbLoop = True;
          break;

        case  enStSyncDoneRx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Daten sind angekommen und können ausgewertet werden
            // 2 Byte Sync
            if (IsSyncCheckOk2(mpcMsgSyncR.cRxData.mpu8Data))
            {
              mbSBusy = IsSyncBusy(mpcMsgSyncR.cRxData.mpu8Data);
              if (mbSBusy) cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxBusy);
              vOnSync();
            }
            else
            {
              // Sync Error
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
              cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxSync);
              lbLoop = True;
              break;
            }

            u8   lu8DataLen;
            lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);

            // Hat der Slave Daten und habe ich Platz
            // Werden Daten vom Slave gesendet ?
            if (lu8DataLen > 0)
            {
              // Eine NoCheck Nachricht wird immer genommen
              // auch wenn es keinen Platz gibt (Busy)
              if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) ||
                  (IsSyncOneWay(mpcMsgSyncR.cRxData.mpu8Data)))
              {
                mStatus.IsAckTx = 1;
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
                mpcMsgDataR.cRxData.muiLen = lu8DataLen;
                lbLoop = True;
                break;
              }
              else
              {
                // Es ist eine Check-Nachricht
                // Daten werden nur genommen, wenn es Platz gibt.
                if (!IsBusy())
                {
                  menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
                  mpcMsgDataR.cRxData.muiLen = lu8DataLen;
                  lbLoop = True;
                  break;
                }
              }
            }

            // Es werden keine Daten vom Slave gesendet. Direkt zum Ende.
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
            break;
          }
          break;

        //
        // ----------------------------------- Daten Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartRx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartRx:
          {
            u8  lu8DataLen;
            lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);
            mpcMsgDataR.cRxData.muiLen = lu8DataLen; // 1 Byte Checksumme ist hier schon inkludiert

            // Slave-Adressierung starten
            // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
            mpcMsgDataR.vStart();
            mcDownLink->vStartMsg(&mpcMsgDataR);

            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
          {
            // Daten sind angekommen und können ausgewertet und gespeichert werden
            // Daten + 1 Byte Checksumme
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) || (!IsBusy()))
              {
                if (IsSyncNewData())
                {
                  u8   lui8ChkSum;

                  mpcMsgDataR.cRxData.muiLen--; // -1 wegen CheckSumme

                  /* Rx Daten abholen und in den Rx-Buffer schreiben */
                  if (IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data))
                  {
                    // Rx Daten abholen und in den Rx-Buffer schreiben
                    lui8ChkSum = 0xCC;
                  }
                  else
                  {
                    ////GPIOA->BSRR = (1 << 15);
                    lui8ChkSum = mpcMsgDataR.u8RxChecksum();
                    ////GPIOA->BSRR = (1 << (16 + 15));
                  }

                  if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
                  {

                    // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von eigenem Slave geschickt
                    // Senderadresse == Sklave-Adresse Zieladresse == meine Adresse (ich) dazufügen
                    cBotNetMsg_Base lcMsg(&mpcMsgDataR.cRxData);
                    lcMsg.vDecode();

                    if (!lcMsg.bHasAdress())
                    {
                      // UpLink, d.h. Source war Slave, Dest bin ich (Master)
                      // In mcAdr steht meine Adresse drin => Dest
                      //                 Source                Dest
                      lcMsg.vAdressAdd(mcAdr.Get(), mcAdr.GetMasterAdr());
                      mpcMsgDataR.cRxData.muiLen = lcMsg.muiLen;
                    }

                    if (mbAlwaysNoCheck)
                    {
                      vSetDataNoCheck(&mpcMsgDataR.cTxData[mpcMsgDataT.cTxData.muiLen - 1]);
                    }

                    ////GPIOA->BSRR = (1 << 15);
                    mcRxComBuf.put(&mpcMsgDataR.cRxData);
                    ////GPIOA->BSRR = (1 << (16 + 15));

                    vSetAckRx(True);
                  }
                  else
                  {
                    cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxChkSum);
                    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
                    lbLoop = True;
                    break;
                  }
                }
                else
                {
                  // Rx Daten sind schon bekannt, daher nur Ack
                  vSetAckRx(True);
                }
              }

              menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
              lbLoop = True;
            }
          }
          break;

        //
        // ----------------------------------- End -----------------------------------
        //
        case enStEndError:
          mcDownLink->vResetCom();
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mu16TimeoutCounter_ms = 0;
          break;
        case enStEnd:
          mcDownLink->ResetLock();
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mu16TimeoutCounter_ms = 0;
          break;
        default:
          break;
      }
    } while (lbLoop);
    //GPIOA->BRR = 1;  // Test
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    mcDownLink->vTick10ms();

    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > 10)
      {
        mu16TimeoutCounter_ms -= 10;
      }
      else
      {
        ////GPIOB->BSRR = (1 << 8);
        mu16TimeoutCounter_ms = 0;
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
        vSm(cComNode::tenEvent::enEvTimer);
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
        ////GPIOB->BSRR = (1 << (16 + 8));
      }
    }
  }

  void vTick1ms() override
  {
    mcDownLink->vTick1ms();
    mcDownLink->bStartNext();
  }

  void vSync() override
  {
    mStatus.IsStartRequested = 1;
    vSyncStart();
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkUsartMpHd:public cBotNet_ComLinkUsartMpHd
{
  public:
  u8 mpu8ComBufRx[cBotNet_UpLinkComBufSize];
  u8 mpu8ComBufTx[cBotNet_UpLinkComBufSize];

  cUartMpHdSlave *mcUpLink;

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2 + 1]; // +1 wegen Id/Adressbyte)
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR; u8 MsgDataRxRBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataT; u8 MsgDataTxRBuf[enCnstMaxDataPlusCheckSum];

  cBotNet_SyncedLinkBase::tenStates   menSm;

  bool mbMBusy;

  cBotNet_UpLinkUsartMpHd(cUartMpHdSlave* lcUpLink)
  : cBotNet_ComLinkUsartMpHd(mpu8ComBufRx, sizeof(mpu8ComBufRx), mpu8ComBufTx, sizeof(mpu8ComBufTx), cBotNet_LinkBase::enUpLink),
    mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
    mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
    mpcMsgDataR(null,                              0, MsgDataRxRBuf, sizeof(MsgDataRxRBuf)),
    mpcMsgDataT(MsgDataTxRBuf, sizeof(MsgDataTxRBuf), null,          0)
  {
    vSet(lcUpLink);

    mpcMsgSyncR.cRxData.muiLen = sizeof(MsgSyncRxRBuf);
    mpcMsgSyncT.cTxData.muiLen = sizeof(MsgSyncTxRBuf);

    vSetTimeoutReload(lcUpLink->mu32Baudrate);

    vSetOnlineTimeout(500);
  }


  bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    mcUpLink->vSetNode((cComNode*)this);
    vOnResetCom();
    return lbRet;
  }


  void vSet(cUartMpHdSlave* lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);
    mcUpLink->vSupressErrorHandling();
    vResetStatusComFlags();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
    vSm(cComNode::tenEvent::enEvError);
  }

  // Aufgefrufen, wenn Online und 200ms kein Sync
  //void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  //{
  //  cBotNet_SyncedLinkBase::vOnEnterOffline();
  //}

  // Aufgerufen nach 10s Offline
  // void vOnOffline10s() override // __attribute__((optimize("-O0")))
  // {
  //  cBotNet_SyncedLinkBase::vOnOffline10s();
  // }

  // Aufgerufen nach 300ms Offline
  void vOnResetCom() override
  {
    mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwDma);
    cBotNet_SyncedLinkBase::vOnResetCom();

    mStatus.IsInit = 1;
    mbMBusy = False;
    mu16TimeoutCounter_ms = 0;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
    vSm(cComNode::tenEvent::enEvDummy);
  }



  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    vSm(lenEvent);
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    vSm(cComNode::tenEvent::enEvDone);
  }

  void vSmPutDataRx()
  {
    u8   lui8ChkSum;

    if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) || (!IsBusy()))
    {
      if (IsSyncNewData())
      {
        mpcMsgDataR.cRxData.muiLen--; // -1 wegen CheckSumme

        if (IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data))
        {
          lui8ChkSum = 0xCC;
        }
        else
        {
          ////GPIOA->BSRR = (1 << 15);
          lui8ChkSum = mpcMsgDataR.u8RxChecksum();
          ////GPIOA->BSRR = (1 << (16 + 15));
        }

        if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
        {
          // Rx Daten abholen und in den Rx-Buffer schreiben
          // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von eigenem Master geschickt
          // Senderadresse == Sklavenadresse Zieladresse == meine Adresse (ich) dazufügen
          cBotNetMsg_Base lcMsg(&mpcMsgDataR.cRxData);
          lcMsg.vDecode();

          if (!lcMsg.bHasAdress())
          {
            // UpLink, d.h. Source war Master, Dest bin ich (Slave)
            // In mcAdr steht meine Adresse drin => Dest
            //                 Source                Dest
            lcMsg.vAdressAdd(mcAdr.GetMasterAdr(), mcAdr.Get());
            mpcMsgDataR.cRxData.muiLen = lcMsg.muiLen;
          }

          ////GPIOA->BSRR = (1 << 15);
          mcRxComBuf.put(&mpcMsgDataR.cRxData);
          ////GPIOA->BSRR = (1 << (16 + 15));

          vSetAckRx(True);
        }
        else
        {
          cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxChkSum);
        }
      }
      else
      {
        // Rx Daten sind schon bekannt, daher nur Ack
        vSetAckRx(True);
      }
    }
  }

  void  vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        //
        // ----------------------------------- Sync: Rx Master:Tx  Slave:Rx -----------------------------------
        //
      case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
        mcUpLink->vWaitMsg(&mpcMsgSyncR);
        menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx;
        break;

      case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvDone:
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
              lbLoop = True;
            }
            break;
          default:
            break;
        }
        break;

      case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx:
        mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;

        // Im ersten Byte steht die Adresse
        // Von daher alles um ein Byte nach links schieben
        mpcMsgSyncR.cRxData.mpu8Data[0] = mpcMsgSyncR.cRxData.mpu8Data[1];
        mpcMsgSyncR.cRxData.mpu8Data[1] = mpcMsgSyncR.cRxData.mpu8Data[2];

        if (IsSyncCheckOk2(mpcMsgSyncR.cRxData.mpu8Data))
        {
          mbMBusy = IsSyncBusy(mpcMsgSyncR.cRxData.mpu8Data);
          if (mbMBusy) cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxBusy);
          vOnSync();
        }
        else
        {
          // Sync Error
          // On Sync-Error, Nack next transmission
          cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxSync);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
          lbLoop = True;
          break;
        }

        u8 lu8DataLen;
        lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);

        // Werden Daten vom Master gesendet ?
        if (lu8DataLen > 0)
        {
          // Eine NoCheck Nachricht wird immer gesendet
          // auch wenn es keinen Platz gibt (Busy)
          if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) ||
              (IsSyncOneWay(mpcMsgSyncR.cRxData.mpu8Data)))
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
            mpcMsgDataR.cRxData.muiLen = lu8DataLen;
            lbLoop = True;
            break;
          }
          else
          {
            // Es ist eine Check-Nachricht
            // Daten werden nur gesendet, wenn es Platz gibt.
            if (!IsBusy())
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
              mpcMsgDataR.cRxData.muiLen = lu8DataLen;
              lbLoop = True;
              break;
            }
          }
        }

        // Es werden keine Daten vom Master gesendet. Direkt zum Senden.
        menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
        lbLoop = True;
        break;

        //
        // ----------------------------------- Data: Rx Master:Tx  Slave:Rx -----------------------------------
        //
      case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
        mcUpLink->vStartMsg(&mpcMsgDataR);
        menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx;
        break;


      case cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvAfterRxStarted:
            //cClockInfo::Delay_us(2);
            //mcUpLink->vAdrDisable();
            ////GPIOB->BSRR = (1 << 8);
            break;
          case cComNode::tenEvent::enEvDone:
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
              lbLoop = True;
            }
            break;
          default:
            break;
        }
        break;

      case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
        {

          // Das Übernehmen kann einige Zeit dauern
          // Das kann dem Master nur während eines Sync mitgeteilt werden
          // Aber der Master setzt nach dem Senden direkt mit der Abfrage fort
          // Daher Daten erst im nächsten Sync übernehmen

          if (IsSyncOneWay(mpcMsgSyncR.cRxData.mpu8Data))
          {
            // Bei OneWay Daten direkt übernehmen
            vSmPutDataRx();
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
            break;
          }

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;
        }

        //
        // ----------------------------------- Sync: Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;
          if (enCnstWaitRSyncUp > 0)
          {
            mcUpLink->mTimer.vStart(enCnstWaitRSyncUp);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
          {
            u8 lu8NoCheck = 0;
            u8 lu8OneWay  = 0;

            if (u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data))
            {
              // Gegebenenfalls noch die Daten aus der letzten Übertragung übernehmen
              vSmPutDataRx();
            }

            // Wurden vorherige Daten Acknowledged ?
            if ((IsAckTx()) || (mpcMsgDataT.cTxData.muiLen == 0))
            {
              // Neue Daten laden
              ////GPIOA->BSRR = (1 << 15);
              mcTxComBuf.get(&mpcMsgDataT.cTxData);
              ////GPIOA->BSRR = (1 << (16 + 15));

              if (mpcMsgDataT.cTxData.muiLen)
              {
                cBotNetMsg_Base lcMsg(&mpcMsgDataT.cTxData);
                lcMsg.vDecode();

                // Ggf. Adresse entfernen
                // Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
                if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgDataT.cTxData.muiLen = lcMsg.muiLen;
                }

                u8 lui8ChkSum;
                if (lcMsg.isNoCkeck())
                {
                  lu8NoCheck = 1;
                  vSetAckTx(True);
                  lui8ChkSum = 0xCC;
                }
                else
                {
                  vSetAckTx(False);
                  ////GPIOA->BSRR = (1 << 15);
                  lui8ChkSum = mpcMsgDataT.u8TxChecksum();
                  ////GPIOA->BSRR = (1 << (16 + 15));
                }

                if (lcMsg.isOneWay())
                {
                  vSetAckTx(True);
                  lu8OneWay = 1;
                }

                mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
                mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
                mu8MsgCntTx++;
              }
            }
            else
            {
              if (mpcMsgDataT.cTxData.muiLen)
              {
                // !!! Hier ist noch Len+1 wegen Checksumme
                if (IsDataNoCheck(&mpcMsgDataT.cTxData[mpcMsgDataT.cTxData.muiLen - 2])) lu8NoCheck = 1;
                if (IsDataOneWay(&mpcMsgDataT.cTxData[mpcMsgDataT.cTxData.muiLen - 2]))  lu8OneWay  = 1;
              }
            }

            vUpdateBusy();

            vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), lu8NoCheck, lu8OneWay);
            mcUpLink->vStartMsg(&mpcMsgSyncT);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
            break;
          }

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              // NoCheck immer senden
              if ((IsSyncNoCheck(mpcMsgSyncT.cTxData.mpu8Data)) ||
                  (IsSyncOneWay(mpcMsgSyncT.cTxData.mpu8Data)) ||
                  (!mbMBusy))
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx;
                lbLoop = True;
                break;
              }
            }

            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Data: Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
          if (enCnstWaitRDataUp > 0)
          {
            mcUpLink->mTimer.vStart(enCnstWaitRDataUp);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
          mcUpLink->vStartMsg(&mpcMsgDataT);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Ende -----------------------------------
        //
        case enStEndError:
          mcUpLink->vResetCom();
          mu16TimeoutCounter_ms = 0;
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
          lbLoop = True;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
          lbLoop = True;
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

    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > 10)
      {
        mu16TimeoutCounter_ms -= 10;
      }
      else
      {
        mu16TimeoutCounter_ms = 0;
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
        vSm(cComNode::tenEvent::enEvTimer);
      }
    }
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
    vSyncStart();
  }
};

// -------------------------------------------------------------------------------------------


// Reduzierte Slave Link
//  - Behandelt jede Nachricht wie eine NoCheck
//  - Es gigt kein Busy behalndlung
class cBotNet_UpLinkUsartMpHdNoCheck :public cBotNet_ComLinkUsartMpHd
{
public:
  u8 mpu8ComBufRx[cBotNet_UpLinkComBufSize];
  u8 mpu8ComBufTx[cBotNet_UpLinkComBufSize];

  cUartMpHdSlave* mcUpLink;

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2 + 1]; // +1 wegen Id/Adressbyte)
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR; u8 MsgDataRxRBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataT; u8 MsgDataTxRBuf[enCnstMaxDataPlusCheckSum];

  cBotNet_SyncedLinkBase::tenStates menSm;

  cBotNet_UpLinkUsartMpHdNoCheck(cUartMpHdSlave* lcUpLink)
    : cBotNet_ComLinkUsartMpHd(mpu8ComBufRx, sizeof(mpu8ComBufRx), mpu8ComBufTx, sizeof(mpu8ComBufTx), cBotNet_LinkBase::enUpLink),
      mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
      mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
      mpcMsgDataR(null,                              0, MsgDataRxRBuf, sizeof(MsgDataRxRBuf)),
      mpcMsgDataT(MsgDataTxRBuf, sizeof(MsgDataTxRBuf), null,          0)
  {
    vSet(lcUpLink);

    mpcMsgSyncR.cRxData.muiLen = sizeof(MsgSyncRxRBuf);
    mpcMsgSyncT.cTxData.muiLen = sizeof(MsgSyncTxRBuf);

    vSetTimeoutReload(lcUpLink->mu32Baudrate);

    vSetOnlineTimeout(500);
  }


  bool bAddedToBn(u16 lu16Adr) override
  {
    bool lbRet = cBotNet_ComLinkUsartMpHd::bAddedToBn(lu16Adr);
    mcUpLink->vSetNode((cComNode*)this);
    vOnResetCom();
    return lbRet;
  }


  void vSet(cUartMpHdSlave* lcUpLink)
  {
    mcUpLink = lcUpLink;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);
    mcUpLink->vSupressErrorHandling();
    vResetStatusComFlags();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
    vSm(cComNode::tenEvent::enEvError);
  }

  // Aufgefrufen, wenn Online und 200ms kein Sync
  //void vOnEnterOffline() override // __attribute__((optimize("-O0")))
  //{
  //  cBotNet_SyncedLinkBase::vOnEnterOffline();
  //}

  // Aufgerufen nach 10s Offline
  // void vOnOffline10s() override // __attribute__((optimize("-O0")))
  // {
  //  cBotNet_SyncedLinkBase::vOnOffline10s();
  // }

  // Aufgerufen nach 300ms Offline
  void vOnResetCom() override
  {
    mcUpLink->vReInitHw(cComNode::tenConsts::enResetHwDma);
    cBotNet_SyncedLinkBase::vOnResetCom();

    mStatus.IsInit = 1;
    mu16TimeoutCounter_ms = 0;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
    vSm(cComNode::tenEvent::enEvDummy);
  }



  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    vSm(lenEvent);
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    vSm(cComNode::tenEvent::enEvDone);
  }

  void vSmPutDataRx()
  {
    if (IsSyncNewData())
    {
      bool lbDataOk;
      mpcMsgDataR.cRxData.muiLen--; // -1 wegen CheckSumme

      lbDataOk = True;
      if (IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data))
      {
        if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] != 0xCC)
        {
          lbDataOk = False;
        }
      }

      if (lbDataOk)
      {
        // Rx Daten abholen und in den Rx-Buffer schreiben
        // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von eigenem Master geschickt
        // Senderadresse == Sklavenadresse Zieladresse == meine Adresse (ich) dazufügen
        cBotNetMsg_Base lcMsg(&mpcMsgDataR.cRxData);
        lcMsg.vDecode();

        if (!lcMsg.bHasAdress())
        {
          // UpLink, d.h. Source war Master, Dest bin ich (Slave)
          // In mcAdr steht meine Adresse drin => Dest
          //                 Source                Dest
          lcMsg.vAdressAdd(mcAdr.GetMasterAdr(), mcAdr.Get());
          mpcMsgDataR.cRxData.muiLen = lcMsg.muiLen;
        }

        ////GPIOA->BSRR = (1 << 15);
        mcRxComBuf.put(&mpcMsgDataR.cRxData);
        ////GPIOA->BSRR = (1 << (16 + 15));

        vSetAckRx(True);
      }
      else
      {
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxChkSum);
      }
    }
    else
    {
      // Rx Daten sind schon bekannt, daher nur Ack
      vSetAckRx(True);
    }
  }

  void vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        //
        // ----------------------------------- Sync: Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
          mcUpLink->vWaitMsg(&mpcMsgSyncR);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvDone:
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
              lbLoop = True;
            }
            break;
            default:
              break;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx:
          mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;

          // Im ersten Byte steht die Adresse
          // Von daher alles um ein Byte nach links schieben
          mpcMsgSyncR.cRxData.mpu8Data[0] = mpcMsgSyncR.cRxData.mpu8Data[1];
          mpcMsgSyncR.cRxData.mpu8Data[1] = mpcMsgSyncR.cRxData.mpu8Data[2];

          if (IsSyncCheckOk2(mpcMsgSyncR.cRxData.mpu8Data))
          {
            vOnSync();
          }
          else
          {
            // Sync Error
            // On Sync-Error, Nack next transmission
            cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxSync);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
            lbLoop = True;
            break;
          }

          u8 lu8DataLen;
          lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);

          // Werden Daten vom Master gesendet ?
          if (lu8DataLen > 0)
          {
            // Eine NoCheck Nachricht wird immer gesendet
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
            mpcMsgDataR.cRxData.muiLen = lu8DataLen;
            lbLoop = True;
            break;
          }

          // Es werden keine Daten vom Master gesendet. Direkt zum Senden.
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;

          //
          // ----------------------------------- Data: Rx Master:Tx  Slave:Rx -----------------------------------
          //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
          mcUpLink->vStartMsg(&mpcMsgDataR);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvDone:
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
              lbLoop = True;
            }
            break;
            default:
              break;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
        {
          if (IsSyncOneWay(mpcMsgSyncR.cRxData.mpu8Data))
          {
            // Bei OneWay Daten direkt übernehmen
            vSmPutDataRx();
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
            break;
          }

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;
        }

        //
        // ----------------------------------- Sync: Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx:
        //  menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx;
        //  lbLoop = True;
        //  break;
        //
        //case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;
          if (enCnstWaitRSyncUp > 0)
          {
            mcUpLink->mTimer.vStart(enCnstWaitRSyncUp);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
        {
          if (u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data))
          {
            // Gegebenenfalls noch die Daten aus der letzten Übertragung übernehmen
            vSmPutDataRx();
          }

          // Wurden vorherige Daten Acknowledged ?
          if ((IsAckTx()) || (mpcMsgDataT.cTxData.muiLen == 0))
          {
            // Neue Daten laden
            ////GPIOA->BSRR = (1 << 15);
            mcTxComBuf.get(&mpcMsgDataT.cTxData);
            ////GPIOA->BSRR = (1 << (16 + 15));

            if (mpcMsgDataT.cTxData.muiLen)
            {
              cBotNetMsg_Base lcMsg(&mpcMsgDataT.cTxData);
              lcMsg.vDecode();

              // Ggf. Adresse entfernen
              // Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
              if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
              {
                lcMsg.vAdressRemove();
                mpcMsgDataT.cTxData.muiLen = lcMsg.muiLen;
              }


              vSetAckTx(True);

              mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = 0xCC;
              mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
              mu8MsgCntTx++;
            }
          }

          vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), 1, 0);
          mcUpLink->vStartMsg(&mpcMsgSyncT);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          break;
        }

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              // NoCheck immer senden
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx;
              lbLoop = True;
              break;
            }

            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

          //
          // ----------------------------------- Data: Tx Master:Rx  Slave:Tx -----------------------------------
          //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx:
        //  menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx;
        //  lbLoop = True;
        //  break;
        //
        //case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
          if (enCnstWaitRDataUp > 0)
          {
            mcUpLink->mTimer.vStart(enCnstWaitRDataUp);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
          mcUpLink->vStartMsg(&mpcMsgDataT);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

          //
          // ----------------------------------- Ende -----------------------------------
          //
        case enStEndError:
          mcUpLink->vResetCom();
          mu16TimeoutCounter_ms = 0;
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
          lbLoop = True;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
          lbLoop = True;
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

    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > 10)
      {
        mu16TimeoutCounter_ms -= 10;
      }
      else
      {
        mu16TimeoutCounter_ms = 0;
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
        vSm(cComNode::tenEvent::enEvTimer);
      }
    }
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
    vSyncStart();
  }
};

#endif // __BOTNET_LINK_USART_MP_HD_H__
