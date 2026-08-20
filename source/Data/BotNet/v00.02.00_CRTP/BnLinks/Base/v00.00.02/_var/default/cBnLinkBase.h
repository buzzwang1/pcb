#ifndef __BOTNET_LINK_BASE_H__
#define __BOTNET_LINK_BASE_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cBnMsgPool.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"
#include "cBnErrCnt.h"

#include "ComDat.h"

#include <concepts>


typedef enum
{
  enUpLink = 0,
  enDownLink,
  enSideLink,
  enOpenLink
}tenBotNet_LinkBase2_Type;

template <tenBotNet_LinkBase2_Type tyenType, bool tybNoCheck>
class cBotNet_LinkBase2 : public cComNode, public cBnErrCnt
{
public:

  typedef enum
  {
    enCnstMaxHeader = 9,
    enCnstMaxData   = cBotNet_MsgSize + 1, // 64 (Max Message Lenght) + 1 (Protokoll Byte)
    enCnstMaxDataPlusCheckSum = enCnstMaxData
  }tenConsts;

  typedef enum
  {
    // Start
    enStIdle = 0,
    enStDisabled,

    enStWaitStart,

    // Sync
      // -- TX
      enStSyncPrepareTx,
      enStSyncWaitTx,
      enStSyncStartTx,
      enStSyncDoneTx,

      // -- RX
      enStSyncPrepareRx,
      enStSyncWaitRx,
      enStSyncStartRx,
      enStSyncDoneRx,

    // Data
      // -- TX
      enStDataStartTx,
      enStDataWaitTx,
      enStDataPrepareTx,
      enStDataDoneTx,

      // -- RX
      enStDataStartRx,
      enStDataWaitRx,
      enStDataPrepareRx,
      enStDataDoneRx,

    enStEnd,
    enStEndError,
  }tenStates;

  u8 mpu8ComBufRx[16];
  u8 mpu8ComBufTx[16];
  cRingBufT<u8, u16>  mcRxComBuf;
  cRingBufT<u8, u16>  mcTxComBuf;

  cBotNetAdress mcAdr;   // My Adress
                         // Bei Uplink, d.h. es ist ein Slave, steht hier die meine Adresse drin
                         // Bei Downlink steht die Dest-Adresse drin, also die Slave-Adresse

  // Online Counter/Überwachung
  // 200ms kein vOnSync aufgerufen, dann wird vOnEnterOffline
  u16  mu16NoSyncCnt_ms;
  u16  mu16NoSyncTimeout_ms;

  u8 mu8MsgCntTx;
  u8 mu8MsgCntRx;
  u8 mu8MsgCntRx_Last;

  u8 mu8PoolIdxRx;
  u8 mu8PoolIdxTx;

  tenBotNet_LinkBase2_Type menType;

  cBotNet_LinkBase2()
    : mcRxComBuf(mpu8ComBufRx, sizeof(mpu8ComBufRx)), mcTxComBuf(mpu8ComBufTx, sizeof(mpu8ComBufTx))
  {
    menType          = tyenType;
    vResetCom(True);
  }

  void vInit()
  {
    mu16NoSyncCnt_ms = 0;
    mStatus.IsInit = 1;
    vSetOnlineTimeout(200);  // 200 ms;
  }

  bool bAddedToBn(u16 lu16Adr)
  {
    mcAdr.Set(lu16Adr);
    mAdr = mcAdr.GetIdx();
    return True;
  }

  void vUpdateBusy()
  {
    // Schauen, ob noch Platz ist
    if (mcRxComBuf.space_left())
    {
      mStatus.IsBusy = 0;
    }
    else
    {
      // Kein Platz mehr, dann nur senden
      mStatus.IsBusy = 1;
    }
  }

  bool isUpLink()   { return (menType == enUpLink); }
  bool isDownLink() { return (menType == enDownLink); }
  bool isSideLink() { return (menType == enSideLink); }

  void vClearRingBuf(cRingBufT<u8, u16>* lpcRingBuf)
  {
    while (1)
    {
      u8 lu8PoolIdx = lpcRingBuf->get();
      if (lu8PoolIdx)
      {
        cBnMsgPool::vReleaseMsg(lu8PoolIdx);
      }
      else
      {
        break;
      }
    }
  }

  // Reset Buffer of Offline Links
  void vResetCom(bool bAlways)
  {
    if ((!IsOnline()) || (bAlways))
    {
      vResetStatusFlags();

      mu8MsgCntTx      = 0;
      mu8MsgCntRx      = 0;
      mu8MsgCntRx_Last = 0xFF;

      cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
      cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);

      vClearRingBuf(&mcRxComBuf);
      vClearRingBuf(&mcTxComBuf);
    }
  }


  // Sync2 Bytes setzen
  // DDDD DDDD - BARO MMMM - CCCC CCCC
  //  D: 8Bit: Anzahl Daten zu senden (0..64+1) // +1 für Checksumme
  //  B: 1Bit: Busy Flag:
  //              0: kann Daten empfangen
  //              1: kann keine Daten empfangen
  //  A: 1Bit: Acknowledge Flag:
  //              0: kein Acknowlede. Dem Sender mitteilen, die Daten nochmals zu senden
  //              1: Acknowledge für zuletzt empfangene Daten
  //  R: 1Bit: NoCheck:
  //              0: Daten Checksumme ist Quersumme Daten + 1
  //              1: Daten Checksumme ist 0xCC
  //  O: 1Bit: OneWay
  //              0: Slave Antwortet
  //              1: Slave antowrtet nicht
  //  M: 4Bit: MessageCounter: Wenn frische Daten gesendet werden, wird der Counter erhöht
  //              Counter != Counter(n-1): frische Daten
  //              Counter == Counter(n-1): gleiche Daten wie beim letzten Mal
  //  C: 8Bit: Checksumme: Byte1 + Byte2 + 1

  void vCreateSync(u8* lpu8Dest, u8 lu8DataCntTx, u8 lu8NoCheck, u8 lu8OneWay)
  {
    mu8MsgCntTx &= 0x0F;
    u8 lu8Busy = mStatus.IsBusy;
    u8 lu8Ack  = mStatus.IsAckRx;

    lpu8Dest[0] = lu8DataCntTx;
    lpu8Dest[1] = (u8)((lu8Busy << 7) + (lu8Ack << 6) + (lu8NoCheck << 5) + (lu8OneWay << 4) + mu8MsgCntTx);
    lpu8Dest[2] = 1 + lpu8Dest[0] + lpu8Dest[1];
    vSetAckRx(False);
  }


  bool IsSyncCheckOk(u8* lpu8Source)
  {
    u8 lu8Ack        = (lpu8Source[1] >> 6) & 1;
    u8 lu8Checksum   = (lpu8Source[0] + lpu8Source[1] + 1);

    mu8MsgCntRx = lpu8Source[1] & 0x0F;;

    // Letzte Daten wurden acknowledged
    if (lu8Ack) mStatus.IsAckTx = 1;

    return ((lu8Checksum == lpu8Source[2]) && (lpu8Source[0] <= 65));
  }

  bool IsSyncBusy(u8* lpu8Source)    { return ((lpu8Source[1] & 128)); }
  bool IsSyncAck(u8* lpu8Source)     { return ((lpu8Source[1] & 64)); }
  bool IsSyncNoCheck(u8* lpu8Source) { return ((lpu8Source[1] & 32)); }
  bool IsSyncOneWay(u8* lpu8Source)  { return ((lpu8Source[1] & 16)); }
  bool IsDataNoCheck(u8* lpu8Source) { return (*((cBotNetMsg_Base::tstControl*)lpu8Source)).NoCheck; }
  bool IsDataOneWay(u8* lpu8Source)  { return (*((cBotNetMsg_Base::tstControl*)lpu8Source)).OneWay;}

  void vSetDataNoCheck(u8* lpu8Source) {(*((cBotNetMsg_Base::tstControl*)lpu8Source)).NoCheck = 1;}
  void vAckRx() { vSetAckRx(True); mu8MsgCntRx_Last = mu8MsgCntRx; }

  u8 u8SyncGetMsgLen(u8* lpu8Source) {return lpu8Source[0]; }

  bool IsSyncNewData()
  {
    if (mu8MsgCntRx_Last != mu8MsgCntRx)
    {
      return True;
    }
    return False;
  }

  void vAddChecksum(u8 lu8PoolIdx)
  {
    if (tybNoCheck)
    {
      cBnMsgPool::vAddByte(lu8PoolIdx, 0xCC);
    }
    else
    {
      cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);
      cBnMsgPool::vAddByte(lu8PoolIdx, lcMsg.u8Sum() + 1);
    }
  }

  bool bPut(u8 lu8PoolIdx)
  {
    if (IsOnline())
    {
      if (mcTxComBuf.space_left())
      {
        cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);

        // Ggf. Adresse entfernen
        if (isDownLink())
        {
          // Überprüfen, ob der Empfänger direkter Slave ist, wenn ja, dann kann Adressinformation entfernt werden
          if (lcMsg.cGetSAdr().isDirectMasterOf(lcMsg.cGetDAdr().Get()))
          {
            lcMsg.vAdressRemove();
            cBnMsgPool::vSetLen(lu8PoolIdx, lcMsg.muLen);
          }
        }
        else // isUpLink() // isSideLink()
        {
          // Überprüfen, ob der Empfänger direkter Master ist, wenn ja, dann kann Adressinformation entfernt werden
          if (lcMsg.cGetDAdr().isDirectMasterOf(lcMsg.cGetSAdr().Get()))
          {
            lcMsg.vAdressRemove();
            cBnMsgPool::vSetLen(lu8PoolIdx, lcMsg.muLen);
          }
        }

        vAddChecksum(lu8PoolIdx);
        cBnMsgPool::vPutMsg(lu8PoolIdx);
        mcTxComBuf.put(lu8PoolIdx);
        return True;
      }
      else
      {
        return False;
      }
    }
    // if offline, just drop the message
    return True;
  }

  u8 u8Get()
  {
    u8 lu8PoolIdx = mcRxComBuf.get();

    if (lu8PoolIdx == 0) return 0;

    cBotNetMsg_Base lcMsg;  cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);

    // Überprüfen, ob es eine Adressinformation gibt.
    if (!lcMsg.bHasAdress())
    {
      if (isDownLink())
      {
        // Wenn nicht, dann wurde die Nachricht von eigenem Slave geschickt
        //               lcSAdr     , lcDAdr
        lcMsg.vAdressAdd(mcAdr.Get(), mcAdr.GetMasterAdr());
        cBnMsgPool::vSetLen(lu8PoolIdx, lcMsg.muLen);
      }
      else // isUpLink() // isSideLink()
      {
        // Wenn nicht, dann wurde die Nachricht von eigenem Master geschickt
        //               lcSAdr              , lcDAdr
        lcMsg.vAdressAdd(mcAdr.GetMasterAdr(), mcAdr);
        cBnMsgPool::vSetLen(lu8PoolIdx, lcMsg.muLen);
      }
    }
    return lu8PoolIdx;
  }

  void vSetOnlineTimeout(u16 lu16Timeout_ms)
  {
    mu16NoSyncTimeout_ms = lu16Timeout_ms;
  }

  void vOnSync()
  {
    mStatus.IsOnline = True;
    mu16NoSyncCnt_ms = 0;
  }

  void vOnEnterOffline()
  {
    // Offline setzen
    mStatus.IsOnline = False;
  }

  void vOnOfflineLong()
  {
    // Daten verwerfen
    vResetCom(True);
  }

  void vTick10ms()
  {
    if (IsOnline())
    {
      mu16NoSyncCnt_ms += 10;

      if (mu16NoSyncCnt_ms > mu16NoSyncTimeout_ms)
      {
        vOnEnterOffline();
      }
    }
    else // isOffline
    {
      if (mu16NoSyncCnt_ms < 60000)
      {
        mu16NoSyncCnt_ms += 10;
        if (mu16NoSyncCnt_ms == 60000)
        {
          vOnOfflineLong();
        }
      }
    }
  }
};




// -------------------------------------------------------------------------------------------

template <template <typename> typename tLinkDriver,
          u8   tyu8OftSyncR,
          u16  tyu16SyncWaitTime,
          u16  tyu16DataWaitTime,
          bool tybNoCheck>
class cBnUpLink : public cBotNet_LinkBase2<tenBotNet_LinkBase2_Type::enUpLink, tybNoCheck>
{
  public:
  using Base = cBotNet_LinkBase2<tenBotNet_LinkBase2_Type::enUpLink, tybNoCheck>;
  using typename Base::tenStates;

  tLinkDriver<cBnUpLink> mcDriver;

  cComMsgS<u16, 3 + tyu8OftSyncR> mpcMsgSyncR; // +1 wegen Id/Adressbyte)
  cComMsgS<u16, 3>                mpcMsgSyncT;
  cComMsg<u16> mpcMsgDataR;
  cComMsg<u16> mpcMsgDataT;

  tenStates menSm;

  // Timout für eine Kommunikation
  u16 mu16TimeoutCounter_ms;
  u16 mu16TimeoutCounterReload_ms;
  u16 mu16Counter_ms;

  bool mbMBusy;

  cBnUpLink()
    : Base(),
      mcDriver(0),
      mpcMsgSyncR(cComNode::tenDirection::enIsRx),
      mpcMsgSyncT(cComNode::tenDirection::enIsTx),
      mpcMsgDataR(cComNode::tenDirection::enIsRx),
      mpcMsgDataT(cComNode::tenDirection::enIsTx)
  {
    vSetTimeoutReload(mcDriver.u32GetBaudRate());

    mpcMsgSyncR.muLen = mpcMsgSyncR.muSize;
    mpcMsgSyncT.muLen = mpcMsgSyncT.muSize;

    this->vSetOnlineTimeout(500);
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

    // (1400 [Bit]  * 1000[ms/s] / lu32Baud [Bit/s]) * 3
    // 4.200.000 [Bit*ms/s] / lu32Baud [Bit/s]
    // Bei 400khz:   10ms
    // Bei 300khz:   14ms
    // Bei 200khz:   21ms
    // Bei 100khz:   42ms
    // Bei  50khz:   84ms
    // Bei  40khz:  105ms
    // Bei  20khz:  210ms

    mu16TimeoutCounterReload_ms = (u16)((u32)4200000 / lu32Baud);
  }

  bool bAddedToBn123(u16 lu16Adr)
  {
    bool lbRet = Base::bAddedToBn(lu16Adr);
    mcDriver.vAddNode(*this);
    return lbRet;
  }

  void vInit(bool lbStart)
  {
    Base::vInit();

    mu16TimeoutCounter_ms = 0;
    mu16Counter_ms = 0;

    if (lbStart)
    {
      menSm = Base::tenStates::enStSyncPrepareRx;
    }
    else
    {
      menSm = tenStates::enStIdle;
    }
    vSm(cComNode::tenEvent::enEvDummy);
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);
    this->vResetStatusComFlags();
    menSm = tenStates::enStEndError;
    vSm(cComNode::tenEvent::enEvError);
  }

  void vComStart(cComNode::tenEvent lenEvent)
  {
    vSm(lenEvent);
  }

  void vComDone()
  {
    vSm(cComNode::tenEvent::enEvDone);
  }

  void vSmPrepareDataTx()
  {
    this->vUpdateBusy();

    // Wurden vorherige Daten Acknowledged ?
    if ((this->IsAckTx()) || (this->mu8PoolIdxTx == 0))
    {
      // vorher genutzen Speicher freigeben ?
      cBnMsgPool::vReleaseMsg(this->mu8PoolIdxTx);

      // Neue Daten laden
      this->mu8PoolIdxTx = this->mcTxComBuf.get();

      if (this->mu8PoolIdxTx > 0)
      {
        cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, this->mu8PoolIdxTx);
        mpcMsgDataT.From(lcMsg);
        this->mu8MsgCntTx++;

        // Checksumme ist schon vorberechnet. Daher Länge -1
        lcMsg.muLen--;

        if (tybNoCheck) lcMsg.vSetNoCheck(tybNoCheck);

        if (lcMsg.isNoCheck())
        {
          this->vSetAckTx(True);
        }
        else
        {
          this->vSetAckTx(False);
        }

        // Oneway geht nur von Master zum Slave, von der 0
        this->vCreateSync(mpcMsgSyncT.mpu8Data, (uint8)mpcMsgDataT.Len(), lcMsg.isNoCheck(), 0);
      }
      else
      {
        mpcMsgDataT.muLen = 0;
        this->vCreateSync(mpcMsgSyncT.mpu8Data, (uint8)mpcMsgDataT.Len(), 0, 0);
      }
    }
  }

  void vSmPutDataRx()
  {
    bool lbAckRx = False;

    // NoCheck wird immer Acknowleged
    if ((this->IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
        (tybNoCheck))
    {
      lbAckRx = True;
    }

    // Daten nur übernehmen, wenn neu und wenn Platz ist
    if (mpcMsgDataR.Len() == 0) return;
    if (!this->IsBusy() && this->mu8PoolIdxRx)
    {
      if (this->IsSyncNewData())
      {
        u8 lui8ChkSum;

        mpcMsgDataR.muLen--; // -1 wegen CheckSumme
        if ((this->IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
            (tybNoCheck))
        {
          lui8ChkSum = 0xCC;
        }
        else
        {
          lui8ChkSum = mpcMsgDataR.u8Sum() + 1;
        }

        if (mpcMsgDataR.mpu8Data[mpcMsgDataR.Len()] == lui8ChkSum)
        {
          // Rx Daten abholen und in den Rx-Buffer schreiben
          this->mcRxComBuf.put(this->mu8PoolIdxRx);
          cBnMsgPool::vPutMsg(this->mu8PoolIdxRx);
          cBnMsgPool::vSetLen(this->mu8PoolIdxRx, mpcMsgDataR.Len());
          lbAckRx = True;
        }
        else
        {
          cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxChkSum);
        }
      }
      else
      {
        // Nachricht schon bekannt -> Ack
        lbAckRx = True;
      }
    }
    else
    {
      cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxBusy);
    }

    if (lbAckRx)
    {
      this->vAckRx();
    }

    cBnMsgPool::vReleaseMsg(this->mu8PoolIdxRx);
  }

  void vDataRxPrepare(bool& lbLoop, tenStates& lenSm)
  {
    UNUSED(lbLoop);
    u8 lu8MsgLen = this->u8SyncGetMsgLen(mpcMsgSyncR.mpu8Data);

    //Vorherige Daten freigeben
    cBnMsgPool::vReleaseMsg(this->mu8PoolIdxRx);

    // Speicher anfordern für neue Nachricht
    cBnMsgPool::vReqMsg(mpcMsgDataR, this->mu8PoolIdxRx, lu8MsgLen);
    mpcMsgDataR.muLen = lu8MsgLen;
    if (this->mu8PoolIdxRx == 0)
    {
      // No memory
      cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxNoMem);
    }

    lenSm = tenStates::enStDataWaitRx;
    mcDriver.vStartMsg(&mpcMsgDataR);
  }

  void vDataRxDone(bool& lbLoop, tenStates& lenSm)
  {
    if (this->IsSyncOneWay(mpcMsgSyncR.mpu8Data))
    {
      lenSm = tenStates::enStEnd;
    }
    else
    {
      lenSm = tenStates::enStSyncPrepareTx;
    }
    lbLoop = True;
  }

  void vDataTxPrepare(bool& lbLoop, tenStates& lenSm)
  {
    lenSm = tenStates::enStDataWaitTx;
    lbLoop = True;
  }

  void vDataTxStart(bool& lbLoop, tenStates& lenSm)
  {
    UNUSED(lbLoop);
    lenSm = tenStates::enStDataDoneTx;
    mcDriver.vStartMsg(&mpcMsgDataT);
  }

  void vDataTxDone(bool& lbLoop, tenStates& lenSm)
  {
    lenSm = tenStates::enStEnd;
    lbLoop = True;
  }

  void vSm(cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        case tenStates::enStDisabled:
          this->mStatus.IsEnabled = False;
          mcDriver.vHwStop();
          break;
        //
        // ----------------------------------- Sync: Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case tenStates::enStSyncPrepareRx:
          if (this->mControl.EnableRequest)
          {
            this->mStatus.IsEnabled = True;
            mcDriver.vWaitMsg(&mpcMsgSyncR);
            menSm = tenStates::enStSyncWaitRx;
          }
          else
          {
            menSm = tenStates::enStDisabled;
            lbLoop = True;
          }
          break;

        case tenStates::enStSyncWaitRx:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvDone:
            {
              menSm = tenStates::enStSyncDoneRx;
              lbLoop = True;
            }
            break;
          default:
            break;
          }
          break;

        case tenStates::enStSyncDoneRx:
          mu16TimeoutCounter_ms = mu16TimeoutCounterReload_ms;

          // Im ersten Byte steht die Adresse
          // Von daher alles um ein Byte nach links schieben
          if (tyu8OftSyncR > 0)
          {
            for (u8 lu8t = 0; lu8t < 3; lu8t++)
            {
              mpcMsgSyncR.mpu8Data[lu8t] = mpcMsgSyncR.mpu8Data[lu8t + tyu8OftSyncR];
            }
          }

          if (this->IsSyncCheckOk(mpcMsgSyncR.mpu8Data))
          {
            mbMBusy = this->IsSyncBusy(mpcMsgSyncR.mpu8Data);
            if (mbMBusy) cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxBusy);
            this->vOnSync();
          }
          else
          {
            // Sync Error
            // On Sync-Error, Nack next transmission
            cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxSync);
            menSm = tenStates::enStEndError;
            lbLoop = True;
            break;
          }

          mpcMsgDataR.muLen = this->u8SyncGetMsgLen(mpcMsgSyncR.mpu8Data);

          // Werden Daten vom Master gesendet ?
          if (mpcMsgDataR.Len() > 0)
          {
            // Eine NoCheck Nachricht wird immer gesendet
            // auch wenn es keinen Platz gibt (Busy)
            if ((this->IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
                (this->IsSyncOneWay(mpcMsgSyncR.mpu8Data)) ||
                (tybNoCheck) ||
                (!this->IsBusy()))
            {
              menSm = tenStates::enStDataPrepareRx;
              lbLoop = True;
              break;
            }
          }

          // Es werden keine Daten vom Master gesendet. Direkt zum Senden.
          menSm = tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;

          //
          // ----------------------------------- Data: Rx Master:Tx  Slave:Rx -----------------------------------
          //
        case tenStates::enStDataPrepareRx:
        {
          vDataRxPrepare(lbLoop, menSm);
        }
        break;

        case tenStates::enStDataWaitRx:
        {
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvDone:
            {
              menSm = tenStates::enStDataDoneRx;
              lbLoop = True;
            }
            break;
            default:
              break;
          }
        }
        break;

        case tenStates::enStDataDoneRx:
        {
          vDataRxDone(lbLoop, menSm);
          if (menSm == tenStates::enStEnd)
          {
            // Bei OneWay, ist hier zuende. Daten direkt übernehmen
            vSmPutDataRx();
          }
        }
        break;

        //
        // ----------------------------------- Sync: Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case tenStates::enStSyncPrepareTx:
          //  menSm = tenStates::enStSyncWaitForTx;
          //  lbLoop = True;
          //  break;
          //
          //case tenStates::enStSyncWaitForTx:
          menSm = tenStates::enStSyncStartTx;
          if (tyu16SyncWaitTime > 0)
          {
            mcDriver.vStartTimer(tyu16SyncWaitTime);
          }
          else
          {
            lbLoop = True;
          }
          vSmPutDataRx();
          break;

        case tenStates::enStSyncStartTx:
          {
            vSmPrepareDataTx();
            menSm = tenStates::enStSyncDoneTx;
            mcDriver.vStartMsg(&mpcMsgSyncT);
          }
          break;

        case tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.Len())
            {
              // NoCheck schlägt Busy
              // OneWay gibt es nicht beim Slave
              if ((this->IsSyncNoCheck(mpcMsgSyncT.mpu8Data)) ||
                  (tybNoCheck) ||
                  (!mbMBusy))
              {
                menSm = tenStates::enStDataPrepareTx;
                lbLoop = True;
                break;
              }
            }

            menSm = tenStates::enStEnd;
            lbLoop = True;
          }
          break;

          //
          // ----------------------------------- Data: Tx Master:Rx  Slave:Tx -----------------------------------
          //
        case tenStates::enStDataPrepareTx:
            vDataTxPrepare(lbLoop, menSm);
            break;

        case tenStates::enStDataWaitTx:
          menSm = tenStates::enStDataStartTx;
          if (tyu16DataWaitTime > 0)
          {
            mcDriver.vStartTimer(tyu16DataWaitTime);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case tenStates::enStDataStartTx:
          vDataTxStart(lbLoop, menSm);
          break;

        case tenStates::enStDataDoneTx:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            vDataTxDone(lbLoop, menSm);
          }
          break;

          //
          // ----------------------------------- Ende -----------------------------------
          //
        case tenStates::enStEndError:
          mcDriver.vResetCom();
          mu16TimeoutCounter_ms = 0;
          menSm = tenStates::enStSyncPrepareRx;
          lbLoop = True;
          break;
        case tenStates::enStEnd:
          mu16TimeoutCounter_ms = 0;
          menSm = tenStates::enStSyncPrepareRx;
          lbLoop = True;
          break;
        default:
          break;
      }
    } while (lbLoop);
  }

  void vTick(u16 lu16Time_ms)
  {
    mu16Counter_ms += lu16Time_ms;
    if (mu16Counter_ms >= 10)
    {
      Base::vTick10ms();
      mu16Counter_ms -= 10;
    }

    if (this->mControl.EnableRequest)
    {
      if (menSm == tenStates::enStDisabled)
      {
        mcDriver.vHwContinue();

        menSm = tenStates::enStSyncPrepareRx;
        vSm(cComNode::tenEvent::enEvDummy);
      }
    }


    // mu16Counter_ms kann auch durch den Interrupt-Kontext geändert werden
    // Wenn z.B. der vDone/vError über interrupt reinknallt,
    // und dadurch in enStEnd 'mu16TimeoutCounter_ms = 0' gesetzt wird,
    // kann es zu Problemen führen. Daher Interruptsperre darumlegen
    _dai(); //disable all interrupts
    if (mu16TimeoutCounter_ms > 0)
    {
      if (mu16TimeoutCounter_ms > lu16Time_ms)
      {
        mu16TimeoutCounter_ms -= lu16Time_ms;
      }
      else
      {
        mu16TimeoutCounter_ms = 0;
        cBnErrCnt::vInc(cBnErrCnt::tenErr::enTimeout);
        mcDriver.mError = cComNode::tenError::enErTimeout;
        mcDriver.vComError();
      }
    }
    _eai(); //enable all interrupts
  }
};


#endif // __BOTNET_LINK_BASE_H__
