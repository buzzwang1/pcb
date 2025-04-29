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
  uint8 mAdrSync;
  uint8 mAdrData;
  uint8 mAdrAck;

  cBotNet_ComLinkI2c(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
    : cBotNet_SyncedLinkBase(lu32RxComBufSize, lu32TxComBufSize, lenType)
  {
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);

    mAdr <<= 1; // für I2C

    mAdrSync = mAdr;
    mAdrData = mAdr + (0x20 << 1);
    mAdrAck  = mAdr + (0x40 << 1);

    return True;
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

  u16        mu16TimeoutCounter_ms;


  cBotNet_SyncedLinkBase::tenStates   menSm;

  cBotNet_DownLinkI2c(cI2cMaster* lcDownLink)
    : cBotNet_ComLinkI2c(cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize, cBotNet_LinkBase::enDownLink)
  {
    mcDownLink = lcDownLink;

    mpcMsgSyncR.vMemAlloc(0, 2);
    mpcMsgSyncT.vMemAlloc(2, 0);
    mpcMsgDataR.vMemAlloc(0, enCnstMaxDataPlusCheckSum);
    mpcMsgDataT.vMemAlloc(enCnstMaxDataPlusCheckSum, 0);

    mStatus.IsInit         = 1;

    vOnResetCom();

    mu16TimeoutCounter_ms     = 0;

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
    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    cBotNet_SyncedLinkBase::vOnResetCom();
  }

  // vComstart wird von I2C Treiber aufgerufen, sobalt man an der Reihe ist.
  void vComStart(cComNode::tenEvent lenEvent) override __attribute__((optimize("-O0")))
  {
    if (lenEvent == cComNode::tenEvent::enEvPrepareToSendData)
    {
      if (menSm == cBotNet_SyncedLinkBase::tenStates::enStIdle)
      {
        // zuerst die Sync + Daten vom Slave einlesen
        // Slave beendet die Kommunikation mit I2C.Stop
        // 4ms Timeout: Nach 4ms wir die Kommunikation auch beendet
        vSm(cComNode::tenEvent::enEvStart);
      }
    }
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
          if (lenEvent == cComNode::tenEvent::enEvStart)
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;

            // Die ganze Kommunikation darf nicht mehr als wie 4ms dauern
            mu16TimeoutCounter_ms = 20;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Sync Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
          // Zuerst die Sync-Bytes (2 Bytes) empfangen
          mpcMsgSyncR.cRxData.muiLen = 2;

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;

          // Slave-Adressierung starten
          // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
          mpcMsgSyncR.vStart();
          mcDownLink->vStartMsg(&mpcMsgSyncR);
          break;

        case  enStSyncDoneRx:
          // Daten sind angekommen und können ausgewertet werden
          // 2 Byte Sync
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            mcDownLink->SetLock(mAdr);
            if (IsSyncCheckOk2(mpcMsgSyncR.cRxData.mpu8Data))
            {
              u8   lu8DataLen;
              bool lbSlaveAck;
              vOnSync();

              lbSlaveAck   = IsSyncAck(mpcMsgSyncR.cRxData.mpu8Data);
              lu8DataLen   = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);

              // Letzte Daten wurden acknowledged
              if (lbSlaveAck) vSetAckTx(True);


              // Hat der Slave Daten und habe ich Platz
              if ((lu8DataLen > 0) && (mStatus.IsBusy == 0))
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
                lbLoop = True;
              }
              else
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
                lbLoop = True;
              }
            }
            else
            {
              // Sync Error
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
              lbLoop = True;
            }
          }
          break;

          //
          // ----------------------------------- Daten Rx -----------------------------------
          //
          case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
            {
              u8  lu8DataLen;
              lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);
              mpcMsgDataR.cRxData.muiLen = lu8DataLen; // 1 Byte Checksumme ist hier schon inkludiert

              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;

              // Slave-Adressierung starten
              // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
              mpcMsgDataR.vStart();
              mcDownLink->vStartMsg(&mpcMsgDataR);
            }
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
            {
              // Daten sind angekommen und können ausgewertet und gespeichert werden
              // Daten + 1 Byte Checksumme
              if (lenEvent == cComNode::tenEvent::enEvDone)
              {
                u8 lui8ChkSum;
                mpcMsgDataR.cRxData.muiLen--;
                lui8ChkSum = mpcMsgDataR.u8RxChecksum();

                /* Rx Daten abholen und in den Rx-Buffer schreiben */
                if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
                {
                  if (IsSyncNewData())
                  {
                    // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von einem Sklaven geschickt
                    // Senderadresse == Sklavenadresse Zieladresse == meine Adresse (ich) dazufügen
                    cBotNetMsg_Base lcMsg(&mpcMsgDataR.cRxData);
                    lcMsg.vDecode();

                    if (!lcMsg.bHasAdress())
                    {
                      // DownLink, d.h. Source war Slave, Dest bin ich (Master)
                      // In mcAdr steht die Slave Adresse drin => Source
                      //                 Source           Dest
                      lcMsg.vAdressAdd(mcAdr.Get(), mcAdr.GetMasterAdr());
                      mpcMsgDataR.cRxData.muiLen = lcMsg.muiLen;
                    }

                    mcRxComBuf.put(&mpcMsgDataR.cRxData);
                  }

                  vSetAckRx(True);
                }

                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
                lbLoop = True;
              }
            }
            break;

          //
          // ----------------------------------- Sync Tx -----------------------------------
          //
          case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx:
            // Wurden vorherige Daten Acknowledged ?
            if (IsAckTx())
            {
              // Neue Daten laden
              mcTxComBuf.get(&mpcMsgDataT.cTxData);
              if (mpcMsgDataT.cTxData.muiLen)
              {
                vSetAckTx(False);

                // Ggf. Adresse entfernen
                // Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
                cBotNetMsg_Base lcMsg(&mpcMsgDataT.cTxData);
                lcMsg.vDecode();
                if (lcMsg.mcFrame.mcSAdr.isDirectMasterOf(lcMsg.mcFrame.mcDAdr.Get()))
                {
                  lcMsg.vAdressRemove();
                  mpcMsgDataT.cTxData.muiLen = lcMsg.muiLen;
                }

                u8 lui8ChkSum = mpcMsgDataT.u8TxChecksum();
                mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
                mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme

                mu8MsgCntTx++;
              }
            }

            // Schauen, ob noch Platz ist
            if (mcRxComBuf.space_left() > (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
            {
              mStatus.IsBusy = 0;
            }
            else
            {
              // Kein Platz mehr, dann nur senden
              mStatus.IsBusy = 1;
            }

            vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), 0);
            vSetAckRx(False);
            mpcMsgSyncT.cTxData.muiLen = 2;

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;

            // Slave-Adressierung starten
            // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
            mpcMsgSyncT.vStart();
            mcDownLink->vStartMsg(&mpcMsgSyncT);
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              // Kein Bussy Flag beim Slave gesetzt ?
              bool lbSlaveBusy = IsSyncBusy(mpcMsgSyncR.cRxData.mpu8Data);

              // Sind Daten zu senden ?
              if ((mpcMsgDataT.cTxData.muiLen) && (!lbSlaveBusy))
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx;
                lbLoop = True;
              }
              else
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
                lbLoop = True;
              }
            }
            break;


          //
          // ----------------------------------- Daten Tx -----------------------------------
          //
          case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx:
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;

            // Slave-Adressierung starten
            // Slave sollte dann Clock-Strech machen, bis er seine Daten senden kann
            mpcMsgDataT.vStart();
            mcDownLink->vStartMsg(&mpcMsgDataT);
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              // Nun auf Sync vom Master warten
              menSm  = cBotNet_SyncedLinkBase::tenStates::enStEnd;
              lbLoop = True;
            }
            break;

          //
          // ----------------------------------- Ende -----------------------------------
          //
          case enStEndError:
            mcDownLink->ResetLock();
            mcDownLink->mSm = cComNode::tenState::enStIdle;
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
        mu16TimeoutCounter_ms = 0;
        if (menSm != cBotNet_SyncedLinkBase::tenStates::enStIdle)
        {
          menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
          vSm(cComNode::tenEvent::enEvTimer);
        }
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

  u16        mu16TimeoutCounter_ms;

  bool       mbAckDataRx;
  bool       mbAckDataTx;

  cBotNet_SyncedLinkBase::tenStates   menSm;

  bool mbPrepareChecksum;

  cBotNet_UpLinkI2c(cI2cSlave* lcUpLink)
  : cBotNet_ComLinkI2c(cBotNet_UpLinkComBufSize, cBotNet_UpLinkComBufSize, cBotNet_LinkBase::enUpLink)
  {
    vSet(lcUpLink);

    mpcMsgSyncR.vMemAlloc(0, 2);
    mpcMsgSyncT.vMemAlloc(2, 0);
    mpcMsgDataR.vMemAlloc(0, enCnstMaxDataPlusCheckSum);
    mpcMsgDataT.vMemAlloc(enCnstMaxDataPlusCheckSum, 0);

    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    cBotNet_SyncedLinkBase::vOnResetCom();

    mStatus.IsInit = 1;
    mu16TimeoutCounter_ms = 0;
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
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
    cBotNet_SyncedLinkBase::vOnResetCom();
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


  void  vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        //
        // ----------------------------------- Sync Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncWait:

          // Stimmt die Adresse ?
          if (mAdr == mAdrSync)
          {
            vOnSync();

            switch (lenEvent)
            {
              case cComNode::tenEvent::enEvPrepareForTx:
                {
                  menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
                  // Die ganze Kommunikation darf nicht mehr als wie 4ms dauern
                  mu16TimeoutCounter_ms = 20;
                  lbLoop = True;
                }
                break;
              default:
                break;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx:
          mcUpLink->vSetMsg(&mpcMsgSyncT);

          // Wurden vorherige Daten Acknowledged ?
          if (mbAckDataTx)
          {
            // Neue Daten laden
            mcTxComBuf.get(&mpcMsgDataT.cTxData);
            if (mpcMsgDataT.cTxData.muiLen)
            {
              mbAckDataTx = False;

              // Ggf. Adresse entfernen
              // Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
              cBotNetMsg_Base lcMsg(&mpcMsgDataT.cTxData);
              lcMsg.vDecode();
              if (lcMsg.mcFrame.mcDAdr.isDirectMasterOf(lcMsg.mcFrame.mcSAdr.Get()))
              {
                lcMsg.vAdressRemove();
                mpcMsgDataT.cTxData.muiLen = lcMsg.muiLen;
              }

              u8 lui8ChkSum = mpcMsgDataT.u8TxChecksum();
              mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
              mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme

              mu8MsgCntTx++;
            }
          }

          // Schauen, ob noch Platz ist
          if (mcRxComBuf.space_left() > (enCnstMaxData + 2)) // +2 für Headerbyte in ComBuf
          {
            mStatus.IsBusy = 0;
          }
          else
          {
            // Kein Platz mehr, dann nur senden
            mStatus.IsBusy = 1;
          }

          vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), 0);
          mbAckDataRx = False;
          mpcMsgSyncT.cTxData.muiLen = 2;

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx;
            }
            else
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx;
            }
          }
          break;

        //
        // ----------------------------------- Data Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx:
          switch (lenEvent)
          {
            // Master bereits Daten zu empfangen
            case cComNode::tenEvent::enEvPrepareForTx:
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx;
                lbLoop = True;
              }
              break;
            // Falls Master voll ist, dann versucht er zu senden
            case cComNode::tenEvent::enEvPrepareForRx:
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
                lbLoop = True;
              }
              break;
            default:
              break;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareTx:
          mcUpLink->vSetMsg(&mpcMsgDataT);
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Nun auf Sync vom Master warten
            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx;
          }
          break;

        //
        // ----------------------------------- Sync Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvPrepareForRx:
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
                lbLoop = True;
              }
              break;
            default:
              break;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
          mcUpLink->vSetMsg(&mpcMsgSyncR);

          mpcMsgSyncR.cRxData.muiLen = 2;

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            if (IsSyncCheckOk2(mpcMsgSyncR.cRxData.mpu8Data))
            {
              u8   lu8DataLen;
              bool lbMasterAck;

              lbMasterAck  = IsSyncAck(mpcMsgSyncR.cRxData.mpu8Data);
              lu8DataLen   = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);

              // Letzte Daten wurden acknowledged
              if (lbMasterAck) mbAckDataTx = True;

              // Werden Daten vom Master gesendet ?
              if ((lu8DataLen > 0) && (mStatus.IsBusy == 0))
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx;
              }
              else
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
                lbLoop = True;
              }
            }
            else
            {
              // Sync Error
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
              lbLoop = True;
            }
          }
          break;

        //
        // ----------------------------------- Daten Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvPrepareForRx:
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
                lbLoop = True;
              }
              break;
            default:
              break;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
          mcUpLink->vSetMsg(&mpcMsgDataR);

          u8   lu8DataLen;
          lu8DataLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);
          mpcMsgDataR.cRxData.muiLen = lu8DataLen;

          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            u8 lui8ChkSum;
            mpcMsgDataR.cRxData.muiLen--;
            lui8ChkSum = mpcMsgDataR.u8RxChecksum();

            /* Rx Daten abholen und in den Rx-Buffer schreiben */
            if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
            {
              if (IsSyncNewData())
              {
                // Überprüfen, ob es eine Adressinformation gibt. Wenn nicht dann wurde die Nachricht von einem Sklaven geschickt
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

                mcRxComBuf.put(&mpcMsgDataR.cRxData);
                mbAckDataRx = True;
              }
              else
              {
                mbAckDataRx = True;
              }
            }

            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Ende -----------------------------------
        //
        case enStEndError:
          mcUpLink->mSm = cComNode::tenState::enStIdle;
          mu16TimeoutCounter_ms = 0;
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWait;
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
        if (menSm != cBotNet_SyncedLinkBase::tenStates::enStSyncWait)
        {
          menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
          vSm(cComNode::tenEvent::enEvTimer);
        }
      }
    }
  }

  void vTick1ms() override
  {
    mcUpLink->vTick1ms();
  }
};


#endif // __BOTNET_LINK_I2C_H__
