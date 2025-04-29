#ifndef __BOTNET_LINK_I2C_H__
#define __BOTNET_LINK_I2C_H__

#include "Typedef.h"

#include "cI2C.h"
#include "cStrT.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"


class cBotNet_ComLinkI2c: public cBotNet_SyncedLinkBase
{
public:
  u16 mu16TimeoutCounter_ms;
  u16 mu16TimeoutCounterReload_ms;

  cBotNet_ComLinkI2c(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
    : cBotNet_SyncedLinkBase(lu32RxComBufSize, lu32TxComBufSize, lenType)
  {
    mu16TimeoutCounterReload_ms = 100;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);
    mAdr <<= 1; // für I2C
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


class cBotNet_DownLinkI2c: public cBotNet_ComLinkI2c
{
  public:
  cI2cMaster*  mcDownLink;

  cComDatMsg mpcMsgSyncR;
  cComDatMsg mpcMsgSyncT;
  cComDatMsg mpcMsgDataR;
  cComDatMsg mpcMsgDataT;

  u8   mu8WaitForSlave;
  bool mbSBusy;

  cBotNet_SyncedLinkBase::tenStates   menSm;

  cBotNet_DownLinkI2c(cI2cMaster* lcDownLink)
    : cBotNet_ComLinkI2c(cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize, cBotNet_LinkBase::enDownLink)
  {
    mcDownLink = lcDownLink;

    mpcMsgSyncR.vMemAlloc(0, 2);
    mpcMsgSyncT.vMemAlloc(2, 0);
    mpcMsgDataR.vMemAlloc(0, enCnstMaxDataPlusCheckSum);
    mpcMsgDataT.vMemAlloc(enCnstMaxDataPlusCheckSum, 0);

    vOnResetCom();

    vSetTimeoutReload(lcDownLink->mu32Baud);
  }

  //~cBotNet_DownLinkI2c()
  //{
  //}

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkI2c::bAddedToBn(lu16Adr);
    mcDownLink->vAddSlave((cComNode*)this);
    return lbRet;
  }

  virtual uint32 SizeOf()
  {
    return 0;
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

  // Aufgerufen nach 300ms Offline
  void vOnResetCom() override
  {
    cBotNet_SyncedLinkBase::vOnResetCom();

    mStatus.IsInit        = 1;
    mu16TimeoutCounter_ms = 0;
    mbSBusy               = False;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    vSm(cComNode::tenEvent::enEvDummy);
  }

  // vComstart wird von I2C Treiber aufgerufen, sobalt man an der Reihe ist.
  void vComStart(cComNode::tenEvent lenEvent) // override __attribute__((optimize("-O0")))
  {
    vSm(lenEvent);
  }

  void vComDone() override  // __attribute__((optimize("-O0")))
  {
    vSm(cComNode::tenEvent::enEvDone);
  }

  void vSm(cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

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
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx;
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
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

            // Die ganze Kommunikation darf nicht mehr als wie 4ms dauern
            mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;
            mcDownLink->SetLock(mAdr);

            // Slave-Adressierung starten
            // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
            mpcMsgSyncT.vStart();
            mcDownLink->vStartMsg(&mpcMsgSyncT);

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          }
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
            lbLoop = True;
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
            // Slave-Adressierung starten
            // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
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
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartRx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
          // Slave-Adressierung starten
          // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
          mpcMsgSyncR.vStart();
          mcDownLink->vStartMsg(&mpcMsgSyncR);
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
          lbLoop = True;
          mu16TimeoutCounter_ms = 0;
          break;
        case enStEnd:
          mcDownLink->ResetLock();
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          lbLoop = True;
          mu16TimeoutCounter_ms = 0;
          break;
        default:
          break;
      }
    } while (lbLoop);
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

  // Diese Methode wird vom BotCom nur für den ersten Link
  // aufgerufen, weil es an die HW weitergeleitet wird
  void vTick1ms() override
  {
    mcDownLink->vTick1ms();
    mcDownLink->bStartNext();
  }

  void vSync() override
  {
    mStatus.IsStartRequested = 1;
  }
};

// -------------------------------------------------------------------------------------------


class cBotNet_UpLinkI2c:public cBotNet_ComLinkI2c
{
  public:
  cI2cSlave *mcUpLink;

  cComDatMsg mpcMsgSyncR;
  cComDatMsg mpcMsgSyncT;
  cComDatMsg mpcMsgDataR;
  cComDatMsg mpcMsgDataT;

  cBotNet_SyncedLinkBase::tenStates   menSm;

  bool mbMBusy;

  cBotNet_UpLinkI2c(cI2cSlave* lcUpLink)
  : cBotNet_ComLinkI2c(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink)
  {
    vSet(lcUpLink);

    mpcMsgSyncR.vMemAlloc(0, 2);
    mpcMsgSyncT.vMemAlloc(2, 0);
    mpcMsgDataR.vMemAlloc(0, enCnstMaxDataPlusCheckSum);
    mpcMsgDataT.vMemAlloc(enCnstMaxDataPlusCheckSum, 0);

    vOnResetCom();

    vSetTimeoutReload(lcUpLink->mu32Baud);
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinkI2c::bAddedToBn(lu16Adr);
    mcUpLink->vSetNode((cComNode*)this);
    return lbRet;
  }

  //~cBotNet_I2c_MaSl_Slave_Slave()
  //{
  //}


  virtual uint32 SizeOf()
  {
    return 0;
  }

  void vSet(cI2cSlave *lcUpLink)
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

    mStatus.IsInit        = 1;
    mbMBusy               = False;
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
        mcUpLink->vSetMsg(&mpcMsgSyncR);
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
          menSm = cBotNet_SyncedLinkBase::tenStates::enStEndErrorNack;
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
        mcUpLink->vSetMsg(&mpcMsgDataR);
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
          //mcUpLink->vWaitWhileBusy();
          //mcUpLink->vAdrEnable();
          ////GPIOB->BSRR = (1 << (16 + 8));

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitForTx:
          if (lenEvent == cComNode::tenEvent::enEvPrepareForTx)
          {
            // Clock Strech started hier
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;
            lbLoop = True;
          }
          else
            if (lenEvent == cComNode::tenEvent::enEvPrepareForRx)
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
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
            mcUpLink->vSetMsg(&mpcMsgSyncT);
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
          if (lenEvent == cComNode::tenEvent::enEvPrepareForTx)
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
            lbLoop = True;
          }
          else
          if (lenEvent == cComNode::tenEvent::enEvPrepareForRx)
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
          mcUpLink->vSetMsg(&mpcMsgDataT);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Nun auf Sync vom Master warten
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Ende -----------------------------------
        //
        case enStEndErrorNack:
          mcUpLink->vActivateNack();
          mcUpLink->vAdrDisable();
          ///GPIOB->BSRR = (1 << 8);
          // Nothing todo: Wait for Timeout
          break;
        case enStEndError:
          mcUpLink->vResetCom();
          mu16TimeoutCounter_ms = 0;
          //mcUpLink->vAdrEnable();
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          //mcUpLink->vAdrEnable();
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
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
  }
};


#endif // __BOTNET_LINK_I2C_H__
