#ifndef __BOTNET_LINK_NRF905_H__
#define __BOTNET_LINK_NRF905_H__

#include "Typedef.h"
// Driver
#include "cnRF905.h"
#include "cBnLinkBase.h"

class Botnet;

class cBotNet_ComLinknRf905: public cBotNet_SyncedLinkBase
{
  public:
  cNRF905   *mcNRF905;
  cBotNet_SyncedLinkBase::tenStates   menSm;

  u16 mu16DataIdx;

  // Timeout für eine Kommunikation
  u16 mu16TimeoutCounter_ms;
  u16 mu16TimeoutCounterReload_ms;

  cBotNet_ComLinknRf905(u8* lpu8RxComBuf, u32 lu32RxComBufSize, u8* lpu8TxComBuf, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType, cNRF905 *lcNRF905)
    : cBotNet_SyncedLinkBase(lpu8RxComBuf, lu32RxComBufSize, lpu8TxComBuf, lu32TxComBufSize, lenType)
  {
    mcNRF905 = lcNRF905;
    mu16TimeoutCounterReload_ms = 100;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);

    bool mbnRF905 = mcNRF905->bInit();

    if (!mbnRF905)
    {
      mStatus.IsInit = 0;
      mStatus.IsOnline = 0;
      return False;
    }
    mStatus.IsInit = 1;
    return True;
  }
};

class cBotNet_DownLinknRf905:public cBotNet_ComLinknRf905
{
  public:
  u8 mpu8ComBufRx[cBotNet_DownLinkComBufSize];
  u8 mpu8ComBufTx[cBotNet_DownLinkComBufSize];

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2];
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR; u8 MsgDataRxRBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataT; u8 MsgDataTxRBuf[enCnstMaxDataPlusCheckSum];

  u8   mu8WaitForSlave;
  bool mbSBusy;

  cBotNet_DownLinknRf905(cNRF905 *lcNRF905)
    : cBotNet_ComLinknRf905(mpu8ComBufRx, sizeof(mpu8ComBufRx), mpu8ComBufTx, sizeof(mpu8ComBufTx), cBotNet_LinkBase::enDownLink, lcNRF905),
      mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
      mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
      mpcMsgDataR(null,                              0, MsgDataRxRBuf, sizeof(MsgDataRxRBuf)),
      mpcMsgDataT(MsgDataTxRBuf, sizeof(MsgDataTxRBuf), null,          0)
  {
    mpcMsgSyncR.cRxData.muiLen = sizeof(MsgSyncRxRBuf);
    mpcMsgSyncT.cTxData.muiLen = sizeof(MsgSyncTxRBuf);
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinknRf905::bAddedToBn(lu16Adr);
    vOnResetCom();
    return lbRet;
  }

  virtual uint32 SizeOf()
  {
    return 0;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenState);

    switch(lenError)
    {
      case cComNode::tenError::enErStuck:           cBnErrCnt::vInc(cBnErrCnt::tenErr::enStuck);          break;
      case cComNode::tenError::enErNACK:            cBnErrCnt::vInc(cBnErrCnt::tenErr::enNack);           break;
      case cComNode::tenError::enErTimeout:         cBnErrCnt::vInc(cBnErrCnt::tenErr::enTimeout);        break;
      case cComNode::tenError::enErStartWhileBussy: cBnErrCnt::vInc(cBnErrCnt::tenErr::enStartWhileBusy); break;
      default: cBnErrCnt::vInc(cBnErrCnt::tenErr::enMisc); break;
    }

    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
    bEventHandler(cNRF905::NRF905_EvDummy);
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

    mu16TimeoutCounter_ms = 0;
    mbSBusy               = False;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  void vComStart(tenEvent lenEvent)
  {
    UNUSED(lenEvent);
  }

  void vComDone()
  {
  }

  void vMasterStartSync()
  {
    if (menSm == cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx)
    {
      bEventHandler(cNRF905::NRF905_EvStart);
    }
  }

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent) // __attribute__((optimize("-O0")))
  {
    bool lbLoop = True;

    // Unterliegende Statemaschine wartet noch auf was/ist  noch nicht fertig
    if (mcNRF905->mSm != cNRF905::NRF905_StIdle)
    {
      mcNRF905->bEventHandler(lenEvent);
      lbLoop = (mcNRF905->mSm == cNRF905::NRF905_StIdle);
    }

    while (lbLoop)
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
          {
           u8 lu8NoCheck = 0;
           u8 lu8OneWay = 0;

            // Wurden vorherige Daten Acknowledged ?
            if ((IsAckTx()) || (mpcMsgDataT.cTxData.muiLen == 0))
            {
              // Neue Daten laden
              mcTxComBuf.get(&mpcMsgDataT.cTxData);

              if (mpcMsgDataT.cTxData.muiLen)
              {
                u8 lui8ChkSum;
                lui8ChkSum = mpcMsgDataT.u8TxChecksum();

                mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
                mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
                mu8MsgCntTx++;
              }
            }

            vUpdateBusy();

            vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), lu8NoCheck, lu8OneWay);
          }

          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx;
          lbLoop = True;
        }
        break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitTx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
          if (lenEvent == cNRF905::NRF905_EvStart)
          {
            // Timeout für die ganze Kommunikation setzen
            mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;

            // Daten übernehmen
            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 2;
            mcNRF905->mstNRF905.mui8TxPayLoad[0] = mpcMsgSyncT.cTxData.mpu8Data[0];
            mcNRF905->mstNRF905.mui8TxPayLoad[1] = mpcMsgSyncT.cTxData.mpu8Data[1];
            mcNRF905->vStartTransmit();

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              if ((!mbSBusy))
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
            mu16DataIdx = 0;
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx;
            lbLoop = True;
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx:
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
            // Kurz warten, damit Slave Sync verarbeiten und Empfangen
            // vorbereiten kann
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
            {
              u8 lui8Data2Copy;

              lui8Data2Copy = mpcMsgDataT.cTxData.muiLen - mu16DataIdx;  // 1 Byte Checksumme ist hier schon inkludiert
              if (lui8Data2Copy > 32) lui8Data2Copy = 32;

              for (u8 lu8t = 0; lu8t < lui8Data2Copy; lu8t++)
              {
                mcNRF905->mstNRF905.mui8TxPayLoad[lu8t] = mpcMsgDataT.cTxData.mpu8Data[mu16DataIdx + lu8t];
              }
              mu16DataIdx += lui8Data2Copy;

              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = lui8Data2Copy;
              mcNRF905->vStartTransmit();

              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx;
            }
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx:
            {
              if (mu16DataIdx < mpcMsgDataT.cTxData.muiLen)
              {
                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
              }
              else
              {
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
                lbLoop = True;
              }
            }
            break;

          case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
            {
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

          mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 2;
          mcNRF905->vStartReceive();

          //Timeout setzen
          mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
          lbLoop = True;
          break;

        case  enStSyncDoneRx:
          mcNRF905->mTimer.vStop();
          if (lenEvent != cNRF905::NRF905_EvTimer)
          {
            mpcMsgSyncR.cRxData.mpu8Data[0] = mcNRF905->mstNRF905.mui8RxPayLoad[0];
            mpcMsgSyncR.cRxData.mpu8Data[1] = mcNRF905->mstNRF905.mui8RxPayLoad[1];
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

            // Es werden keine Daten vom Slave gesendet. Direkt zum Ende.
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
            break;
          }
          else
          {
            // Timeout Error
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
            cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
            lbLoop = True;
            break;
          }
          break;

        //
        // ----------------------------------- Daten Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
          {
            mu16DataIdx = 0;

            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartRx;
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartRx:
          {
            u8 lui8Data2Copy;

            lui8Data2Copy = mpcMsgDataR.cRxData.muiLen - mu16DataIdx;   // Len+1 für Checksumme
            if (lui8Data2Copy > 32) lui8Data2Copy = 32;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui8Data2Copy;

            mcNRF905->vStartReceive();
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx:
          {
            mcNRF905->mTimer.vStop();
            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              u8 lui8Data2Copy = mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width;

              // Daten holen
              for (u8 lu8Idx = 0; lu8Idx < lui8Data2Copy; lu8Idx++)
              {
                mpcMsgDataR.cRxData.mpu8Data[mu16DataIdx + lu8Idx] = mcNRF905->mstNRF905.mui8RxPayLoad[lu8Idx];
              }

              mu16DataIdx += lui8Data2Copy;

              if (mu16DataIdx < mpcMsgDataR.cRxData.muiLen)
              {
                // Es gibt noch was zu empfangen
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartRx;
                lbLoop = True;
                break;
              }
              else
              {
                // Alle Daten empfangen
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
                lbLoop = True;
                break;
              }
            }
            else
            {
              // Timeout Error
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
              cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
              lbLoop = True;
              break;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
          {
            // Daten sind angekommen und können ausgewertet und gespeichert werden
            // Daten + 1 Byte Checksumme
            {
              if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) || (!IsBusy()))
              {
                if (IsSyncNewData())
                {
                  u8   lui8ChkSum;

                  mpcMsgDataR.cRxData.muiLen--; // -1 wegen CheckSumme
                  lui8ChkSum = mpcMsgDataR.u8RxChecksum();

                  /* Rx Daten abholen und in den Rx-Buffer schreiben */
                  if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
                  {
                    mcRxComBuf.put(&mpcMsgDataR.cRxData);
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
          mcNRF905->mSm = cNRF905::NRF905_StIdle;
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mu16TimeoutCounter_ms = 0;
          lbLoop = True;
          break;
        case enStEnd:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStIdle;
          mu16TimeoutCounter_ms = 0;
          lbLoop = True;
          break;
        default:
          break;
      }
    };
    return False;
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();

    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > 10)
      {
        mu16TimeoutCounter_ms -= 10;
      }
      else
      {
        mu16TimeoutCounter_ms = 0;
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
        bEventHandler(cNRF905::NRF905_EvDummy);
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
      }
    }
  }

  void vTick1ms() override
  {
    //mcNRF905->vTick1ms();
    //vSyncStart();
  }
};

class cBotNet_UpLinknRf905:public cBotNet_ComLinknRf905
{
  public:
  u8 mpu8ComBufRx[cBotNet_UpLinkComBufSize];
  u8 mpu8ComBufTx[cBotNet_UpLinkComBufSize];

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2 + 1]; // +1 wegen Id/Adressbyte)
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR; u8 MsgDataRxRBuf[enCnstMaxDataPlusCheckSum];
  cComDatMsg mpcMsgDataT; u8 MsgDataTxRBuf[enCnstMaxDataPlusCheckSum];

  bool mbMBusy;

  cBotNet_UpLinknRf905(cNRF905 *lcNRF905)
    : cBotNet_ComLinknRf905(mpu8ComBufRx, sizeof(mpu8ComBufRx), mpu8ComBufTx, sizeof(mpu8ComBufTx), cBotNet_LinkBase::enUpLink, lcNRF905),
      mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
      mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
      mpcMsgDataR(null,                              0, MsgDataRxRBuf, sizeof(MsgDataRxRBuf)),
      mpcMsgDataT(MsgDataTxRBuf, sizeof(MsgDataTxRBuf), null,          0)
  {
    mpcMsgSyncR.cRxData.muiLen = sizeof(MsgSyncRxRBuf);
    mpcMsgSyncT.cTxData.muiLen = sizeof(MsgSyncTxRBuf);

    vSetOnlineTimeout(500);
  }

  bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_ComLinknRf905::bAddedToBn(lu16Adr);
    vOnResetCom();
    return lbRet;
  }


  void vSet(cNRF905* lcNRF905)
  {
    mcNRF905 = lcNRF905;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    UNUSED(lenError);
    UNUSED(lenState);
    vResetStatusComFlags();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
    bEventHandler(cNRF905::tenCmdEvents::NRF905_EvError);
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
    //mcNRF905->vReInitHw(cComNode::tenConsts::enResetHwDma);
    cBotNet_SyncedLinkBase::vOnResetCom();

    mbMBusy = False;
    mu16TimeoutCounter_ms = 0;

    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  void vComStart(tenEvent lenEvent) override
  {
    (void)lenEvent;
  }

  void  vComDone() override
  {
  }

  void vSlaveStartSync()
  {
    vSyncStart();
    menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  void vSmPutDataRx()
  {
    u8   lui8ChkSum;

    if ((IsSyncNoCheck(mpcMsgSyncR.cRxData.mpu8Data)) || (!IsBusy()))
    {
      if (IsSyncNewData())
      {
        mpcMsgDataR.cRxData.muiLen--; // -1 wegen CheckSumme

        lui8ChkSum = mpcMsgDataR.u8RxChecksum();

        if (mpcMsgDataR.cRxData.mpu8Data[mpcMsgDataR.cRxData.Len()] == lui8ChkSum)
        {
          // Rx Daten abholen und in den Rx-Buffer schreiben
          mcRxComBuf.put(&mpcMsgDataR.cRxData);
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

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop = True;

    // Unterliegende Statemaschine wartet noch auf was/ist  noch nicht fertig
    if (mcNRF905->mSm != cNRF905::NRF905_StIdle)
    {
      mcNRF905->bEventHandler(lenEvent);
      lbLoop = (mcNRF905->mSm == cNRF905::NRF905_StIdle);
    }

    while (lbLoop)
    {
      lbLoop = False;
      switch (menSm)
      {
        //
        // ----------------------------------- Sync: Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx:
          mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 2;
          mcNRF905->vStartReceive();
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncWaitRx:
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneRx:
          mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;

          // Im ersten Byte steht die Adresse
          // Von daher alles um ein Byte nach links schieben
          mpcMsgSyncR.cRxData.mpu8Data[0] = mcNRF905->mstNRF905.mui8RxPayLoad[0];
          mpcMsgSyncR.cRxData.mpu8Data[1] = mcNRF905->mstNRF905.mui8RxPayLoad[1];

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

          // Es werden keine Daten vom Master gesendet. Direkt zum Senden.
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;

          //
          // ----------------------------------- Data: Rx Master:Tx  Slave:Rx -----------------------------------
          //
        case cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx:
          {
            mu16DataIdx = 0;
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartRx;
            lbLoop = True;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartRx:
          {
            u8 lui8Data2Copy;

            lui8Data2Copy = mpcMsgDataR.cRxData.muiLen - mu16DataIdx;   // Len+1 für Checksumme
            if (lui8Data2Copy > 32) lui8Data2Copy = 32;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = lui8Data2Copy;

            mcNRF905->vStartReceive();
            mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForRx:
          {
            mcNRF905->mTimer.vStop();
            if (lenEvent != cNRF905::NRF905_EvTimer)
            {
              u8 lui8Data2Copy = mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width;

              // Daten holen
              for (u8 lu8Idx = 0; lu8Idx < lui8Data2Copy; lu8Idx++)
              {
                mpcMsgDataR.cRxData.mpu8Data[mu16DataIdx + lu8Idx] = mcNRF905->mstNRF905.mui8RxPayLoad[lu8Idx];
              }

               mu16DataIdx += lui8Data2Copy;

              if (mu16DataIdx < mpcMsgDataR.cRxData.muiLen)
              {
                // Es gibt noch was zu empfangen
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartRx;
                lbLoop = True;
                break;
              }
              else
              {
                // Alle Daten empfangen
                menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx;
                lbLoop = True;
                break;
              }
            }
            else
            {
              // Timeout Error
              menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
              cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
              lbLoop = True;
              break;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneRx:
          {

            // Das Übernehmen kann einige Zeit dauern
            // Das kann dem Master nur während eines Sync mitgeteilt werden
            // Aber der Master setzt nach dem Senden direkt mit der Abfrage fort
            // Daher Daten erst im nächsten Sync übernehmen
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
          mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
          if (u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data))
          {
            // Die Wartezeit nutzen und gegebenenfalls
            // noch die Daten aus der letzten Übertragung übernehmen
            vSmPutDataRx();
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStSyncStartTx:
          {
            u8 lu8NoCheck = 0;
            u8 lu8OneWay  = 0;

            // Wurden vorherige Daten Acknowledged ?
            if ((IsAckTx()) || (mpcMsgDataT.cTxData.muiLen == 0))
            {
              // Neue Daten laden
              mcTxComBuf.get(&mpcMsgDataT.cTxData);

              if (mpcMsgDataT.cTxData.muiLen)
              {
                u8 lui8ChkSum = mpcMsgDataT.u8TxChecksum();

                mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = lui8ChkSum;
                mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
                mu8MsgCntTx++;
              }
            }

            vUpdateBusy();

            vCreateSync2(mpcMsgSyncT.cTxData.mpu8Data, (uint8)mpcMsgDataT.cTxData.Len(), lu8NoCheck, lu8OneWay);

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 2;
            mcNRF905->mstNRF905.mui8TxPayLoad[0] = mpcMsgSyncT.cTxData.mpu8Data[0];
            mcNRF905->mstNRF905.mui8TxPayLoad[1] = mpcMsgSyncT.cTxData.mpu8Data[1];
            mcNRF905->vStartTransmit();

            menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx;
            break;
          }

        case cBotNet_SyncedLinkBase::tenStates::enStSyncDoneTx:
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.cTxData.muiLen)
            {
              if (!mbMBusy)
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
          mu16DataIdx = 0;
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx;
          lbLoop = True;
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitForTx:
          // Kurz warten, damit Master Sync verarbeiten und Empfangen
          // vorbereiten kann
          menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
          mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataStartTx:
          {
            u8 lui8Data2Copy;

            lui8Data2Copy = mpcMsgDataT.cTxData.muiLen - mu16DataIdx;  // 1 Byte Checksumme ist hier schon inkludiert
            if (lui8Data2Copy > 32) lui8Data2Copy = 32;

            for (u8 lu8t = 0; lu8t < lui8Data2Copy; lu8t++)
            {
              mcNRF905->mstNRF905.mui8TxPayLoad[lu8t] = mpcMsgDataT.cTxData.mpu8Data[mu16DataIdx + lu8t];
            }
            mu16DataIdx += lui8Data2Copy;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = lui8Data2Copy;
            mcNRF905->vStartTransmit();

            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx;
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataWaitTx:
          {
            if (mu16DataIdx < mpcMsgDataT.cTxData.muiLen)
            {
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TX_DELAY);
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataStartTx;
            }
            else
            {
              menSm = cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx;
              lbLoop = True;
            }
          }
          break;

        case cBotNet_SyncedLinkBase::tenStates::enStDataDoneTx:
          {
            menSm = cBotNet_SyncedLinkBase::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

        //
        // ----------------------------------- Ende -----------------------------------
        //
        case enStEndError:
          mcNRF905->mSm = cNRF905::NRF905_StIdle;
          //mcNRF905->vResetCom();
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
    }

    return False;
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    //mcNRF905->vTick10ms();

    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > 10)
      {
        mu16TimeoutCounter_ms -= 10;
      }
      else
      {
        mu16TimeoutCounter_ms = 0;
        menSm = cBotNet_SyncedLinkBase::tenStates::enStEndError;
        bEventHandler(cNRF905::NRF905_EvDummy);
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxTimeout);
      }
    }
  }

  void vTick1ms() override
  {
    //mcNRF905->vTick1ms();
    //vSyncStart();
  }
};

#endif // __BOTNET_LINK_NRF905_H__
