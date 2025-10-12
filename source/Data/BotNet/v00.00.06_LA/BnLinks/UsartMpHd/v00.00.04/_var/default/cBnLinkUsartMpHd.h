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

  cBotNet_ComLinkUsartMpHd(cBotNet_LinkBase::tenType lenType)
   : cBotNet_SyncedLinkBase(lenType)
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



// Reduzierte Slave Link
//  - Behandelt jede Nachricht wie eine NoCheck
//  - Es gigt kein Busy behalndlung
class cBotNet_UpLinkUsartMpHdNoCheck :public cBotNet_ComLinkUsartMpHd
{
public:
  cUartMpHdSlave* mcUpLink;

  cComDatMsg mpcMsgSyncR; u8 MsgSyncRxRBuf[2 + 1]; // +1 wegen Id/Adressbyte)
  cComDatMsg mpcMsgSyncT; u8 MsgSyncTxRBuf[2];
  cComDatMsg mpcMsgDataR;
  cComDatMsg mpcMsgDataT;

  cBotNet_SyncedLinkBase::tenStates menSm;

  cBotNet_UpLinkUsartMpHdNoCheck(cUartMpHdSlave* lcUpLink)
    : cBotNet_ComLinkUsartMpHd(cBotNet_LinkBase::enUpLink),
      mpcMsgSyncR(null,                              0, MsgSyncRxRBuf, sizeof(MsgSyncRxRBuf)),
      mpcMsgSyncT(MsgSyncTxRBuf, sizeof(MsgSyncTxRBuf), null,          0),
      mpcMsgDataR(null,                              0, null,          0),
      mpcMsgDataT(null,                              0, null,          0)
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

        ////GPIOA->BSRR = (1 << 15);
        mcRxComBuf.put(mu8PoolIdxRx);
        cBnMsgPool::vPutMsg(mu8PoolIdxRx);
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

    // Nachticht wurde verarbeitet. Speicher freigeben.
    cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
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

          // Wurden vorherige Daten Acknowledged ?
          if (IsAckTx())
          {
            cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);
            mu8PoolIdxTx = 0;
          }

          // Werden Daten vom Master gesendet ?
          if (u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data) > 0)
          {
            // Eine NoCheck Nachricht wird immer gesendet
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataPrepareRx;
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
          {
            u8 lu8MsgLen = u8SyncGetMsgLen(mpcMsgSyncR.cRxData.mpu8Data);
            mu8PoolIdxRx = cBnMsgPool::u8ReqMsg(lu8MsgLen);
            cBnMsgPool::vSetLen(mu8PoolIdxRx, lu8MsgLen);
            cBnMsgPool::vSetChecksum(mu8PoolIdxRx);
            cBnMsgPool::vGetMsgBAry(mu8PoolIdxRx, &mpcMsgDataR.cRxData);

            mcUpLink->vStartMsg(&mpcMsgDataR);
            menSm = cBotNet_SyncedLinkBase::tenStates::enStDataWaitRx;
          }
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
          if ((IsAckTx()) || (mu8PoolIdxTx == 0))
          {
            // Neue Daten laden
            ////GPIOA->BSRR = (1 << 15);
            mu8PoolIdxTx = mcTxComBuf.get();
            ////GPIOA->BSRR = (1 << (16 + 15));
            cBnMsgPool::vGetMsgBAry(mu8PoolIdxTx, &mpcMsgDataT.cTxData);
            if (mu8PoolIdxTx > 0)
            {
              vSetAckTx(True);

              //Checksumme ist schon vorberechnet
              //mpcMsgDataT.cTxData.mpu8Data[mpcMsgDataT.cTxData.muiLen] = 0xCC;
              //mpcMsgDataT.cTxData.muiLen++; // + 1 für Checksumme
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

          // NoCheck immer Releasen
          cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);

          lbLoop = True;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          ////GPIOB->BSRR = (1 << (16 + 8));
          menSm = cBotNet_SyncedLinkBase::tenStates::enStSyncPrepareRx;

          // NoCheck immer Releasen
          cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);

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
