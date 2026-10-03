#include "main.h"
#include <concepts>
#include "cbArrayT.h"
#include "cBnMsgPool.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"
#include "cBnErrCnt.h"

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}

void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  //vDelay_Decrement();
}


class cComNode2
{
  public:

  typedef enum
  {
    enNoError = 0,
    enErUnknown,
    enErStuck,
    enErNACK,
    enErBusError,
    enErArbitrationLost,
    enErStartWhileBusy,
    enErOverrun,
    enErUnderrun,
    enErCrc,
    enErFrame,
    enErMode,
    enErTimeout,
    enErHwTimerTimeout,
    enErComNodeSmTimeout,
    enErComNodeDir,
    enErNoInit,
    enErReInit,
    enErDeviceInitErrorPins,
    enErDeviceInitErrorDevice,
    enErDeviceInitErrorBusy,
  }tenError;

  typedef enum
  {
    enStIdle,
    enStIdle2,
    enStStart,
    enStLock,
    enStAdress,
    enStData,
    enStAdressAndData,
    enStWaitStart,
    enStWaitAdress,
    enStWaitData,
    enStWaitAdressAndData,
    enStTx,
    enStRx,
    enStEnd,
    enStEndRx,
    enStEndTx,
    enStError,
    enStWait
  }tenState;

  typedef enum
  {
    enEvStart = 0,
    enEvStartSkipAdr,
    enEvStartWait,
    enEvDone,
    enEvEnable,
    enEvTick,
    enEvError,
    enEvIrq,
    enEvDummy,

    // Irqs
    enEvAdress,
    enEvDma,
    enEvDmaTc, // Transmission Complete
    enEvDmaRxTc, // Transmission Complete
    enEvDmaRxEr, // Error
    enEvDmaTxTc, // Transmission Complete
    enEvDmaTxEr, // Error
    enEvI2cTc,   // Transmission Complete
    enEvUsartTc, // Transmission Complete
    enEvSpiTc,   // Transmission Complete
    enEvUsartAm, // Adress Match
    enEvUsartErOre, // Error Overrun
    enEvUsartTimer, // Timer Interrupt
    enEvUsartExtiP1, // Externe Interrupts Pin1
    enEvUsartExtiP2, // Externe Interrupts Pin2
    enEvUsartExtiP3, // Externe Interrupts Pin3
    enEvUsartErUnknown, // Error Unkown

    // Start
    enEvPrepareToSendData,
    enEvPrepareToReceiveData,
    enEvPrepareForTx,
    enEvPrepareForRx,
    enEvAfterTxStarted,
    enEvAfterRxStarted,
    enEvTimer,
    enEvMisc,
  }tenEvent;

  typedef enum
  {
    enIsTx,
    enIsRx,
    enIsTxRx,   // Nur für I2C
    enCfgWrite  // Nur für Radio, e.g. nrf905
  }tenDirection;

  typedef struct
  {
    u8 IsEnabled        : 1; // Manually enabled by User
    u8 IsInit           : 1; // 0 = Error in LinkInit, Like HW Error
    u8 IsOnline         : 1; // 1 = There was a communication in the last 200ms. Link online

    u8 IsError          : 1; // 1 = Runtime-Error, like Runtime NACK for I2C
    u8 IsBusy           : 1; // 1 = Rx-Buffer is full

    u8 IsAckTx          : 1; // Last transmission was confirmed
    u8 IsAckRx          : 1; // Confirmation for last receiption
  }tstStatus;

  typedef struct
  {
    u8 StartRequest     : 1; // 1 = Request to action
    u8 EnableRequest    : 1; // 1 = Request to action
  }tstControl;

  u16        mAdr;
  tstStatus  mStatus;
  tstControl mControl;

  cComNode2()
  {
    mStatus.IsInit    = 0;
    mStatus.IsEnabled = 0;
    mControl.EnableRequest = 1;
    vResetStatusFlags();
  }

  void vResetStatusFlags()
  {
     //mStatus.IsInit = 0; // Initialsierung, z.B. durch Hardware
                           // soll bestehen bleiben
     mStatus.IsOnline = 0;
     vResetStatusComFlags();
  }

  void vResetStatusComFlags()
  {
    mStatus.IsError  = 0;
    mStatus.IsBusy   = 0;
    mStatus.IsAckTx  = 1;
    mStatus.IsAckRx  = 0;

    mControl.StartRequest = 0;
  }

  //Called, on Error
  void vComError()
  {
    mStatus.IsError = 1;
  }

  bool IsEnabled()       { return mStatus.IsEnabled; }
  bool IsInit()          { return mStatus.IsInit; }
  bool IsOnline()        { return mStatus.IsOnline; }
  bool IsError()         { return mStatus.IsError; }
  bool IsBusy()          { return mStatus.IsBusy; }
  bool IsAckRx()         { return mStatus.IsAckRx; }
  bool IsAckTx()         { return mStatus.IsAckTx; }
  bool IsInitAndOnline() { return (IsInit() && IsOnline()); }

  void vSetOnline()           { mStatus.IsOnline = 1; }
  void vSetAckRx(bool lbSate) { mStatus.IsAckRx  = lbSate; }
  void vSetAckTx(bool lbSate) { mStatus.IsAckTx  = lbSate; }
};


template <typename T>
concept cptLink = requires(T v, u16 lu16Adr, u16 lu16Time_ms, cComNode2::tenEvent lenEvent, cComNode2::tenError lenError, cComNode2::tenState lenState)
{
  { v.vComStart(lenEvent) }           -> std::same_as<void>;
  { v.vComDone() }                    -> std::same_as<void>;
  { v.vComError(lenError, lenState) } -> std::same_as<void>;

  { v.bAddedToBn(lu16Adr) }          -> std::same_as<bool>;
  { v.vTick(lu16Time_ms) }           -> std::same_as<void>;
};

typedef enum
{
  enUpLink = 0,
  enDownLink,
  enSideLink,
  enOpenLink
}tenBotNet_LinkBase2_Type;

template <tenBotNet_LinkBase2_Type tyenType, bool tybNoCheck>
class cBotNet_LinkBase2: public cComNode2, public cBnErrCnt
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


template <typename T>
concept cptComDriver = requires(T v, cComNode2::tenEvent lenEvent)
{
  { v.enInitHw() }    -> std::same_as<cComNode2::tenError>;
  { v.vResetCom() }   -> std::same_as<void>;
  { v.vComError() }   -> std::same_as<void>;
  { v.bCheckBusy() }  -> std::same_as<bool>;
  { v.vSm(lenEvent) } -> std::same_as<void>;
};

//template <cptComDriver Derived>
template <typename Derived>
class cComNodeBase2
{
public:
  cComNode2*    mpcActiveSlave;
  cComMsg<u16>* mpcActiveMsg;

  u32             mu32Baudrate;

  u16             mu16TickCounter_ms;    // Algemeiner Counter
  u16             mu16ReInitTicks_ms;    // HW-ReInit nach dieser Zeit
  u16             mu16ComTimeoutTicks_ms;  // Stillstandserkennung
  u16             mu16ComTimeoutTicksReload_ms;
  u16             mu16ReInitTicksReload_ms;

  cComNode2::tenState  mSm;
  cComNode2::tenError  mError;

  cComNodeBase2(u16 luInitDelay_ms)
  {
    mError = cComNode2::enErNoInit;
    mSm    = cComNode2::enStError;

    mpcActiveSlave = NULL;
    mpcActiveMsg   = NULL;

    mu16ReInitTicksReload_ms = 500;
    mu16ReInitTicks_ms = luInitDelay_ms;

    mu16ComTimeoutTicksReload_ms = 500;
    mu16ComTimeoutTicks_ms = 0;
  }

  //virtual bool               bCheckBusy() = 0;
  //virtual cComNode2::tenError enInitHw() = 0;
  //virtual void               vResetCom() = 0;
  //virtual void               vStartTimer(u16 luTime_us) { UNUSED(luTime_us); };
  //virtual void               vAddNode(cComNode* lpcNode) = 0;

  void vInit() requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
    mError = pDerived->enInitHw(); // Garantiert durch Concept!

    if (mError != cComNode2::enNoError)
    {
      // Init hat nicht geklappt
      // Etwas später nochmals versuchen
      mu16ReInitTicks_ms = mu16ReInitTicksReload_ms;

      //mError = cComNode2::enErNoInit;
      //mSm = cComNode2::enStError;
    }
    else
    {
      pDerived->vResetCom();
      mu16ComTimeoutTicks_ms = 0;
    }
  }

  void vErrorHdl()
  {
    mSm = cComNode2::enStError;

    ////vComError();

    // Wenn nach SW-Fehlerbehandlung HW immer noch Busy, dann einen HW Reset anstoßen
    ////if (!bCheckBusy())
    ////{
    ////  mError = cComNode2::enErNoInit;
    ////  mSm    = cComNode2::enStError;
    ////}
  }

  void vTick1ms() requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
    mu16TickCounter_ms++;

    pDerived->vSm(cComNode2::tenEvent::enEvTick);

    // Neu Initialisierung, wenn mu16ReInitTicks_ms abgelaufen ist
    if (mSm == cComNode2::enStError)
    {
      // Schauen, ob sich der Fehler selbst repariert
      // Busy kommt z.b., wenn ein der Leitungen nicht Pull Up ist
      if (mError == cComNode2::enErStartWhileBusy)
      ////if ((mError == cComNode2::enErStartWhileBusy) && (bCheckBusy()))
      {
        mSm = cComNode2::enStIdle;
        mError = cComNode2::enNoError;
      }
      else
      {
        if (mu16ReInitTicks_ms)
        {
          mu16ReInitTicks_ms--;

          if (!mu16ReInitTicks_ms)
          {
            vInit();
          }
        }
      }
    }
    else
    {
      mu16ReInitTicks_ms = mu16ReInitTicksReload_ms;
    }

    // Test, ob sich bei der Statemschine noch was tut.
    if (mu16ComTimeoutTicks_ms)
    {
      if ((mSm != cComNode2::tenState::enStIdle) &&
          (mSm != cComNode2::tenState::enStWaitAdress) &&
          (mSm != cComNode2::enStError))
      {
        mu16ComTimeoutTicks_ms--;
        if (mu16ComTimeoutTicks_ms == 0)
        {
          mError = cComNode2::enErComNodeSmTimeout;
          vErrorHdl();
        }
      }
      else
      {
        vResetComTimeout();
      }
    }
  }

  void vResetComTimeout()
  {
    if (mu16ComTimeoutTicksReload_ms > mu16ComTimeoutTicks_ms)
    {
      mu16ComTimeoutTicks_ms = mu16ComTimeoutTicksReload_ms;
    }
  }

  //virtual void vComError() = 0;
  //virtual void vSm(cComNode2::tenEvent lenEvent) = 0;
  //virtual void vStartMsg(cComMsg<u16>* lpcActiveMsg, bool boSkipAdr = False) = 0;
};


//template <cptComDriver Derived>
template <typename Derived>
class cComNodeSlave2 : public cComNodeBase2<Derived>
{
public:

  u16 muAdr;

  cComNodeSlave2(u16 luInitDelay_ms)
    : cComNodeBase2<Derived>(luInitDelay_ms)
  {
    muAdr = 0;
  }

  void vSetReInitTicks(u16 lu16ReInitTicks_ms)
  {
    if (!this->mu16ReInitTicks_ms)
    {
      this->mu16ReInitTicks_ms = lu16ReInitTicks_ms;
    }
  }

  ////void vTick1ms()
  ////{
  ////  cComNodeBase2::vTick1ms();
  ////
  ////  if (mpcActiveSlave)
  ////  {
  ////    mpcActiveSlave->vTick(1);
  ////  }
  ////}

  ////void vAddNode(cComNode2* lpcNode)
  ////{
  ////  mpcActiveSlave = lpcNode;
  ////  muAdr = lpcNode->mAdr;
  ////  lpcNode->mStatus.IsEnabled = True;
  ////  cComNodeBase2::vInit();
  ////  lpcNode->vInit(True);
  ////}

  void vWaitMsg(cComMsg<u16>* lpcActiveMsg) requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);

    // Beim Slave kann kein StartWhileBusy kommen
    // weil er durch interrupt gesteuert wird
    // und nicht alle 1ms wie bei Master	
    //if (bCheckBusy())
    {
      this->vResetComTimeout();
      this->mpcActiveMsg = lpcActiveMsg;
      pDerived->vSm(cComNode2::tenEvent::enEvStartWait);
    }
    //else
    //{
    //  this->mError = cComNode2::enErStartWhileBusy;
    //  this->vErrorHdl();
    //}
  }

  void vStartMsg(cComMsg<u16>* lpcActiveMsg, bool boSkipAdr = False) requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
	
    // Beim Slave kann kein StartWhileBusy kommen
    // weil er durch interrupt gesteuert wird
    // und nicht alle 1ms wie bei Master	
    //if (bCheckBusy())
    {
      this->vResetComTimeout();
      this->mpcActiveMsg = lpcActiveMsg;
      if (boSkipAdr)
      {
        pDerived->vSm(cComNode2::tenEvent::enEvStartSkipAdr);
      }
      else
      {
        pDerived->vSm(cComNode2::tenEvent::enEvStart);
      }
    }
    //else
    //{
    //  mError = cComNode2::enErStartWhileBusy;
    //  vErrorHdl();
    //}
  }

  //virtual void vHwStop() = 0;
  //virtual void vHwContinue() = 0;
};

class cUsartMpHd_HwTimer
{
  public:

  cUsartMpHd_HwTimer()
  {
    vInit();
  }

  void vStart(u16 lui16Time_us)
  {
    // In upcounting mode, the counter counts from 0 to the auto-reload value (content of the TIMx_ARR register), then restarts from 0 and generates a counter overflow event
    TIM4->CNT = 0;
    TIM4->ARR = lui16Time_us;
    /* auto-reload preload enable */
    TIM4->CR1 |= TIM_CR1_CEN;
  }

  void vStop()
  {
    TIM4->SR &= ~TIM_SR_UIF;
    TIM4->CR1 &= ~TIM_CR1_CEN;
  }

  void vInit(void)
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
    // Braucht aber so 90Byte weniger
    TIM4->PSC = 23;    //  23 für 24Mhz, 15 für 16Mhz und 7 für 8Mhz

    TIM4->EGR |= TIM_EGR_UG;
    TIM4->SR &= ~TIM_SR_UIF;

    /*NVIC config*/
    TIM4->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM4_IRQn);
  }
};

class cUartMpHd
{
  public:
  cUsartMpHd_HwTimer     mTimer;
  u16                    mu16DmaCounter2ms_Old;
  u8                     mu8SelfTimer;
  //Single Wire, only Tx Pin
  //cGpPin               mPinTx;

  cUartMpHd()
  {
    mu8SelfTimer = 0;
  }

  void  vStopDMA()
  {
    //usart_dma_receive_config(USART1,  USART_DENT_DISABLE);
    //usart_dma_transmit_config(USART1, USART_DENR_DISABLE);
    USART_CTL2(USART1) &= ~(USART_CTL2_DENR | USART_CTL2_DENT);
    dma_channel_disable(DMA_CH1);
    dma_channel_disable(DMA_CH2);
  }

  // Überpruft ob sich der Rx DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return DMA_CH2CNT;
    return 0;
  }

  u16 u16GetDmaCounterTx()
  {
    return DMA_CH1CNT;
    return 0;
  }


  inline void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mu16DmaCounter2ms_Old = BufferSize;
    DMA_INTC            = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH2);
    DMA_CH2CNT          = BufferSize;
    DMA_CH2MADDR        = (uint32)pBuffer;
    DMA_CH2CTL         |=  (DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
    USART_CTL2(USART1) |= USART_DENR_ENABLE;
  }

  inline void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    USART1->SR         &= ~USART_SR_TC;
    DMA_INTC            = DMA_FLAG_ADD(DMA_FLAG_FTF, DMA_CH1);
    DMA_CH1CNT          = BufferSize;
    DMA_CH1MADDR        = (uint32)pBuffer;
    // Ende der Übertragung wird durch  U0.TC interrupt erledigt
    // Tx DMA Interrupt wird nicht benötigt
    DMA_CH1CTL         |=  (DMA_CHXCTL_CHEN /* | DMA_CHXCTL_FTFIE */);
    USART_CTL2(USART1) |= USART_DENT_ENABLE;
  }

  void vInitHwUart(u8 lui8Adr)
  {
      // Usart Periph clock enable
    rcu_periph_clock_enable(RCU_USART1);

    // Usart Peripheral Disable
    // usart_deinit(USART1);
    rcu_periph_reset_enable(RCU_USART1RST);
    rcu_periph_reset_disable(RCU_USART1RST);

    /*
    usart_oversample_config(USART1, USART_OVSMOD_8);
    usart_baudrate_set(USART1,    19200);
    usart_word_length_set(USART1, USART_WL_9BIT);

    usart_halfduplex_enable(USART1);
    usart_address_config(USART1, mui8Adr);
    usart_address_detection_mode_config(USART1, USART_ADDM_4BIT);

    usart_mute_mode_enable(USART1);
    usart_mute_mode_wakeup_config(USART1, USART_WM_ADDR);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    */

    //Das Gleiche wie oben, spart aber 600 Byte Code
    USART_CTL0(USART1) = 0x0000b848;
    USART_CTL1(USART1) = 0x01000000;
    usart_address_config(USART1, lui8Adr);
    USART_CTL2(USART1) = 0x00000009;
    //USART_BAUD(USART1) = 0x00000340; //  19200 Baud @8Mhz  // 1.0Mhz/52.0 =  19230,77
    USART_BAUD(USART1) = 0x000000F0;   // 200000 Baud @8Mhz  // 1.0Mhz/ 5.0 = 200000,00 0x00000050
                                       // 200000 Baud @12Mhz // 1.5Mhz/ 7.5 = 200000,00 0x00000078
                                       // 200000 Baud @16Mhz // 2.0Mhz/10.0 = 200000,00 0x000000A0
                                       // 200000 Baud @24Mhz // 3.0Mhz/15.0 = 200000,00 0x000000F0

    // Usart Peripheral Enable
    usart_enable(USART1);
    
    vClearAllFlags()
    nvic_irq_enable(USART1_IRQn, 1, 1);

    //Ist oben schon drin
    //usart_interrupt_enable(USART1, USART_INT_TC);
    //usart_interrupt_enable(USART1, USART_INT_ERR);
  }

  void  vInitHw(u8 lui8Adr)
  {
    // Setup Pins
    //PA09
    rcu_periph_clock_enable(RCU_GPIOA);
    //
    gpio_bit_set(GPIOA, GPIO_PIN_9);
    GPIO_OMODE(GPIOA) |= 1 << (9);      // Open drain
    GPIO_OSPD(GPIOA)  |= 1 << (9 * 2);  // 10MHz
    GPIO_PUD(GPIOA)   |= 1 << (9 * 2);  // Pull Up
    GPIO_CTL(GPIOA)   |= 2 << (9 * 2);  // Af Mode
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    //
    ///* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    vStopDMA();

    vInitHwUart(lui8Adr);

    // ----------- DMA ---------
    /* enable the DMA clock */
    rcu_periph_clock_enable(RCU_DMA);

    /* dma_parameter_struct dma_init_struct;

    // configure the USART TX DMA channel
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_addr  = USART1+0x28;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA_CH1, dma_init_struct);

    // configure the USART RX DMA channel
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.periph_addr  = USART1+0x24;
    dma_init(DMA_CH2, dma_init_struct);*/

    //Das Gleiche wie oben, spart aber 200 Byte Code
    DMA_CH1CTL   = 0x00003190;
    DMA_CH1PADDR = 0x40013828;
    
    DMA_CH2CTL   = 0x00003180;
    DMA_CH2PADDR = 0x40013824;
    
    nvic_irq_enable(DMA_Channel1_2_IRQn, 1, 1);
  }

  void vEnableIrq()
  {
    nvic_irq_enable(USART1_IRQn, 1, 1);
    nvic_irq_enable(DMA_Channel1_2_IRQn, 1, 1);
  }

  void vDisableIrq()
  {
    nvic_irq_disable(USART1_IRQn);
    nvic_irq_disable(DMA_Channel1_2_IRQn);
  }


  bool bCheckBusy()
  {
    return (True);
  }

  bool bCheckPins()
  {
    return True;
  }
};


template <typename tcLink>
class cUartMpHdSlave2 : public cUartMpHd, public cComNodeSlave2<cUartMpHdSlave2<tcLink>>
{
  tcLink* mcLink = nullptr;
  
  using BaseNode = cComNodeSlave2<cUartMpHdSlave2<tcLink>>;


public:
  cUartMpHdSlave2(u16 luInitDelay_ms)
    : cUartMpHd(),
      BaseNode(luInitDelay_ms)
  {
    this->mSm = cComNode2::tenState::enStIdle;
    this->mu32Baudrate = 200000;
  }

  void vAddNode(tcLink& c) requires cptLink<tcLink>
  {
    mcLink = &c;
  }

  cComNode2::tenError enInitHw()
  {
    cUartMpHd::vInitHw(this->muAdr);
    return cComNode2::tenError::enNoError;
  }

  void vClearAllFlags()
  {
    // It is cleared by a software sequence(an read to the USART_SR register followed by a read to the USART_DR register)
    volatile uint32_t dummy_sr = 0;
    volatile uint8_t  dummy_dr = 0;
    dummy_sr = USART1->SR; // 1. Read SR
    dummy_dr = (uint8_t)(USART1->DR & 0xFF); // 2. Read DR -> Fehler-Flags zurückgesetzt
    (void)dummy_sr;
    (void)dummy_dr;
  }

  void vHwStop()
  {
    vDisableIrq();
    vClearAllFlags()
    usart_disable(USART1);
  }

  void vHwContinue()
  {
    usart_enable(USART1);
    vClearAllFlags()
    vEnableIrq();
  }

  void vResetCom()
  {
    if (this->mSm != cComNode2::tenState::enStIdle)
    {
      // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
      USART_CTL0(USART1) &= ~USART_CTL0_REN; // Rx ausschalten
      USART_CMD(USART1)  |= USART_CMD_RXFCMD;
      vStopDMA();

      mTimer.vStop();
      vClearAllFlags()

      this->mSm    = cComNode2::tenState::enStIdle;
      this->mError = cComNode2::enNoError;
    }
  }

  void vComError()
  {
    cComNode2::tenState lSm = this->mSm;
    cComNode2::tenError lError = this->mError;

    vResetCom();

    if (this->mpcActiveSlave != NULL)
    {
      mcLink->vComError(lError, lSm);
    }
  }

  u32 u32GetBaudRate()
  {
    return this->mu32Baudrate;
  }

  void TIM_EV_IRQHandler()
  {
    if (this->mpcActiveSlave)
    {
      if (mu8SelfTimer)
      {
        u16 lu16DmaCounter = u16GetDmaCounterRx();
        if (mu16DmaCounter2ms_Old == lu16DmaCounter) // Counter hat sich nicht geändert
        {
          mu8SelfTimer = 0;
          this->mError = cComNode2::enErHwTimerTimeout;
        }
        else  // Timer neu starten
        {
          mu8SelfTimer = 1;
          mu16DmaCounter2ms_Old = lu16DmaCounter;
          mTimer.vStart(250);
        }
      }
      else
      {
        mcLink->vComStart(cComNode2::tenEvent::enEvTimer);
      }
    }
  }

  void IrqHandler(cComNode2::tenEvent lenEvent)
  {
    if (USART1->SR & 0xF)
    {
      this->mError = cComNode2::enErOverrun;
      vComError();
    }
    else
    {
      switch (lenEvent)
      {
        case cComNode2::tenEvent::enEvDmaRxTc:
          DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
          DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
          vSm(lenEvent);
          break;
        case cComNode2::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          DMA_CHCTL(DMA_CH1) &= ~DMA_CHXCTL_CHEN;
          USART1->SR         &= ~USART_SR_TC;
          vSm(lenEvent);
          break;
        default:
          break;
      }
    }
  }

  inline void vMute()
  {
    //USART_CMD(USART1) |= USART_CMD_MMCMD;
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    USART_CTL0(USART1) &= ~USART_CTL0_REN;  // Rx ausschalten
    this->mSm = cComNode2::tenState::enStIdle;
    ////mpcActiveSlave->vComDone();
  }

  bool bCheckBusy()
  {
    return True;
  }

  void vStartTimer(u16 luTime_us)
  {
    mTimer.vStart(luTime_us);
  }

  void vSm(cComNode2::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    switch (this->mSm)
    {
      case cComNode2::tenState::enStIdle:
      {
        if (!(USART1->SR & USART_STAT_RWU))    // Nicht im Mute modus
        {
          switch (lenEvent)
          {
            case cComNode2::tenEvent::enEvStart:
            case cComNode2::tenEvent::enEvStartSkipAdr:
              // Rx
              if (this->mpcActiveMsg->isRx())
              {
                USART_CMD(USART1)  |= USART_CMD_RXFCMD;
                USART_CTL0(USART1) |= USART_CTL0_REN; // Alles einschalten
                USART_INTC(USART1) = USART_INTC_AMC;
                vStartDMARx(this->mpcActiveMsg->mpu8Data, this->mpcActiveMsg->muLen);
                mu8SelfTimer = 1;
                mTimer.vStart(250);
                this->mSm = cComNode2::tenState::enStWait;
              }
              else
              {
                // Tx
                if (this->mpcActiveMsg->isTx())
                {
                  USART_CTL0(USART1) &= ~USART_CTL0_REN; // Rx ausschalten
                  vStartDMATx(this->mpcActiveMsg->mpu8Data, this->mpcActiveMsg->muLen);
                  this->mSm = cComNode2::tenState::enStEnd;
                }
                else
                {
                  vDone();
                }
              }
              break;

            case cComNode2::tenEvent::enEvStartWait:
              // Rx
              if (this->mpcActiveMsg->isRx())
              {
                vMute();
                USART_CMD(USART1)  |= USART_CMD_RXFCMD;
                USART_CTL0(USART1) |= USART_CTL0_REN; // Alles einschalten
                USART_INTC(USART1) = USART_INTC_AMC;
                vStartDMARx(this->mpcActiveMsg->mpu8Data, this->mpcActiveMsg->muLen);
                this->mSm = cComNode2::tenState::enStWaitAdress;
              }
              else
              {
                vDone();
              }
              break;
            default:
              break;
          }
        }
      }
      break;

      case cComNode2::tenState::enStWaitAdress:
        // HW is Muted
        // Wait for adress match and resulting wake up
        // and DMA is finished
        // Before that, PrepareRx has to be called
        if (USART1->SR & USART_STAT_AMF)
        {
          USART_INTC(USART1) = USART_INTC_AMC;
        }

        if (lenEvent == cComNode2::tenEvent::enEvDmaRxTc)
        {
          vDone();
        }
        break;

      case cComNode2::tenState::enStWait:
        {
          if (lenEvent == cComNode2::tenEvent::enEvDmaRxTc)
          {
            vDone();
          }
        }
        break;
      case cComNode2::tenState::enStEnd:
        {
          if (lenEvent == cComNode2::tenEvent::enEvUsartTc)
          {
            vDone();
          }
        }
        break;
      default:
        break;
    }
  }
};


template <template <typename> typename tLinkDriver,
          u8   tyu8OftSyncR,
          u16  tyu16SyncWaitTime,
          u16  tyu16DataWaitTime,
          bool tybNoCheck>
class cBnUpLink : public cBotNet_LinkBase2<tenBotNet_LinkBase2_Type::enUpLink, False>
{
  public:
  tLinkDriver<cBnUpLink> mcDriver;

  cComMsgS<u16, 3 + tyu8OftSyncR> mpcMsgSyncR; // +1 wegen Id/Adressbyte)
  cComMsgS<u16, 3>                mpcMsgSyncT;
  cComMsg<u16> mpcMsgDataR;
  cComMsg<u16> mpcMsgDataT;

  cBotNet_LinkBase2::tenStates menSm;

  // Timout für eine Kommunikation
  u16 mu16TimeoutCounter_ms;
  u16 mu16TimeoutCounterReload_ms;
  u16 mu16Counter_ms;

  bool mbMBusy;

  cBnUpLink()
    : cBotNet_LinkBase2(),
      mcDriver(0),
      mpcMsgSyncR(cComNode::tenDirection::enIsRx),
      mpcMsgSyncT(cComNode::tenDirection::enIsTx),
      mpcMsgDataR(cComNode::tenDirection::enIsRx),
      mpcMsgDataT(cComNode::tenDirection::enIsTx)
  {
    vSetTimeoutReload(mcDriver.u32GetBaudRate());

    mpcMsgSyncR.muLen = mpcMsgSyncR.muSize;
    mpcMsgSyncT.muLen = mpcMsgSyncT.muSize;

    vSetOnlineTimeout(500);
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

  bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_LinkBase2::bAddedToBn(lu16Adr);
    mcDriver.vAddNode(*this);
    return lbRet;
  }

  void vInit(bool lbStart)
  {
    cBotNet_LinkBase2::vInit();

    mu16TimeoutCounter_ms = 0;
    mu16Counter_ms = 0;

    if (lbStart)
    {
      menSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareRx;
    }
    else
    {
      menSm = cBotNet_LinkBase2::tenStates::enStIdle;
    }
    vSm(cComNode2::tenEvent::enEvDummy);
  }

  void vComError(cComNode2::tenError lenError, cComNode2::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);
    vResetStatusComFlags();
    menSm = cBotNet_LinkBase2::tenStates::enStEndError;
    vSm(cComNode2::tenEvent::enEvError);
  }

  void vComStart(cComNode2::tenEvent lenEvent)
  {
    vSm(lenEvent);
  }

  void vComDone()
  {
    vSm(cComNode2::tenEvent::enEvDone);
  }

  void vSmPrepareDataTx()
  {
    vUpdateBusy();

    // Wurden vorherige Daten Acknowledged ?
    if ((IsAckTx()) || (mu8PoolIdxTx == 0))
    {
      // vorher genutzen Speicher freigeben ?
      cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);

      // Neue Daten laden
      mu8PoolIdxTx = mcTxComBuf.get();

      if (mu8PoolIdxTx > 0)
      {
        cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, mu8PoolIdxTx);
        mpcMsgDataT.From(lcMsg);
        mu8MsgCntTx++;

        // Checksumme ist schon vorberechnet. Daher Länge -1
        lcMsg.muLen--;

        if (tybNoCheck) lcMsg.vSetNoCheck(tybNoCheck);

        if (lcMsg.isNoCheck())
        {
          vSetAckTx(True);
        }
        else
        {
          vSetAckTx(False);
        }

        // Oneway geht nur von Master zum Slave, von der 0
        vCreateSync(mpcMsgSyncT.mpu8Data, (uint8)mpcMsgDataT.Len(), lcMsg.isNoCheck(), 0);
      }
      else
      {
        mpcMsgDataT.muLen = 0;
        vCreateSync(mpcMsgSyncT.mpu8Data, (uint8)mpcMsgDataT.Len(), 0, 0);
      }
    }
  }

  void vSmPutDataRx()
  {
    bool lbAckRx = False;

    // NoCheck wird immer Acknowleged
    if ((IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
        (tybNoCheck))
    {
      lbAckRx = True;
    }

    // Daten nur übernehmen, wenn neu und wenn Platz ist
    if (mpcMsgDataR.Len() == 0) return;
    if (!IsBusy() && mu8PoolIdxRx)
    {
      if (IsSyncNewData())
      {
        u8 lui8ChkSum;

        mpcMsgDataR.muLen--; // -1 wegen CheckSumme
        if ((IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
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
          mcRxComBuf.put(mu8PoolIdxRx);
          cBnMsgPool::vPutMsg(mu8PoolIdxRx);
          cBnMsgPool::vSetLen(mu8PoolIdxRx, mpcMsgDataR.Len());
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
      vAckRx();
    }

    cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
  }

  void vDataRxPrepare(bool& lbLoop, cBotNet_LinkBase2::tenStates& lenSm)
  {
    UNUSED(lbLoop);
    u8 lu8MsgLen = u8SyncGetMsgLen(mpcMsgSyncR.mpu8Data);

    //Vorherige Daten freigeben
    cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);

    // Speicher anfordern für neue Nachricht
    cBnMsgPool::vReqMsg(mpcMsgDataR, mu8PoolIdxRx, lu8MsgLen);
    mpcMsgDataR.muLen = lu8MsgLen;
    if (mu8PoolIdxRx == 0)
    {
      // No memory
      cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxNoMem);
    }

    lenSm = cBotNet_LinkBase2::tenStates::enStDataWaitRx;
    mcDriver.vStartMsg(&mpcMsgDataR);
  }

  void vDataRxDone(bool& lbLoop, cBotNet_LinkBase2::tenStates& lenSm)
  {
    if (IsSyncOneWay(mpcMsgSyncR.mpu8Data))
    {
      lenSm = cBotNet_LinkBase2::tenStates::enStEnd;
    }
    else
    {
      lenSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareTx;
    }
    lbLoop = True;
  }

  void vDataTxPrepare(bool& lbLoop, cBotNet_LinkBase2::tenStates& lenSm)
  {
    lenSm = cBotNet_LinkBase2::tenStates::enStDataWaitTx;
    lbLoop = True;
  }

  void vDataTxStart(bool& lbLoop, cBotNet_LinkBase2::tenStates& lenSm)
  {
    UNUSED(lbLoop);
    lenSm = cBotNet_LinkBase2::tenStates::enStDataDoneTx;
    mcDriver.vStartMsg(&mpcMsgDataT);
  }

  void vDataTxDone(bool& lbLoop, cBotNet_LinkBase2::tenStates& lenSm)
  {
    lenSm = cBotNet_LinkBase2::tenStates::enStEnd;
    lbLoop = True;
  }

  void vSm(cComNode2::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    bool lbLoop;

    do
    {
      lbLoop = False;
      switch (menSm)
      {
        case enStDisabled:
          mStatus.IsEnabled = False;
          mcDriver.vHwStop();
          break;
        //
        // ----------------------------------- Sync: Rx Master:Tx  Slave:Rx -----------------------------------
        //
        case cBotNet_LinkBase2::tenStates::enStSyncPrepareRx:
          if (mControl.EnableRequest)
          {
            mStatus.IsEnabled = True;
            mcDriver.vWaitMsg(&mpcMsgSyncR);
            menSm = cBotNet_LinkBase2::tenStates::enStSyncWaitRx;
          }
          else
          {
            menSm = cBotNet_LinkBase2::tenStates::enStDisabled;
            lbLoop = True;
          }
          break;

        case cBotNet_LinkBase2::tenStates::enStSyncWaitRx:
          switch (lenEvent)
          {
            case cComNode2::tenEvent::enEvDone:
            {
              menSm = cBotNet_LinkBase2::tenStates::enStSyncDoneRx;
              lbLoop = True;
            }
            break;
          default:
            break;
          }
          break;

        case cBotNet_LinkBase2::tenStates::enStSyncDoneRx:
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

          if (IsSyncCheckOk(mpcMsgSyncR.mpu8Data))
          {
            mbMBusy = IsSyncBusy(mpcMsgSyncR.mpu8Data);
            if (mbMBusy) cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxBusy);
            vOnSync();
          }
          else
          {
            // Sync Error
            // On Sync-Error, Nack next transmission
            cBnErrCnt::vInc(cBnErrCnt::tenErr::enRxSync);
            menSm = cBotNet_LinkBase2::tenStates::enStEndError;
            lbLoop = True;
            break;
          }

          mpcMsgDataR.muLen = u8SyncGetMsgLen(mpcMsgSyncR.mpu8Data);

          // Werden Daten vom Master gesendet ?
          if (mpcMsgDataR.Len() > 0)
          {
            // Eine NoCheck Nachricht wird immer gesendet
            // auch wenn es keinen Platz gibt (Busy)
            if ((IsSyncNoCheck(mpcMsgSyncR.mpu8Data)) ||
                (IsSyncOneWay(mpcMsgSyncR.mpu8Data)) ||
                (tybNoCheck) ||
                (!IsBusy()))
            {
              menSm = cBotNet_LinkBase2::tenStates::enStDataPrepareRx;
              lbLoop = True;
              break;
            }
          }

          // Es werden keine Daten vom Master gesendet. Direkt zum Senden.
          menSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareTx;
          lbLoop = True;
          break;

          //
          // ----------------------------------- Data: Rx Master:Tx  Slave:Rx -----------------------------------
          //
        case cBotNet_LinkBase2::tenStates::enStDataPrepareRx:
        {
          vDataRxPrepare(lbLoop, menSm);
        }
        break;

        case cBotNet_LinkBase2::tenStates::enStDataWaitRx:
        {
          switch (lenEvent)
          {
            case cComNode2::tenEvent::enEvDone:
            {
              menSm = cBotNet_LinkBase2::tenStates::enStDataDoneRx;
              lbLoop = True;
            }
            break;
            default:
              break;
          }
        }
        break;

        case cBotNet_LinkBase2::tenStates::enStDataDoneRx:
        {
          vDataRxDone(lbLoop, menSm);
          if (menSm == cBotNet_LinkBase2::tenStates::enStEnd)
          {
            // Bei OneWay, ist hier zuende. Daten direkt übernehmen
            vSmPutDataRx();
          }
        }
        break;

        //
        // ----------------------------------- Sync: Tx Master:Rx  Slave:Tx -----------------------------------
        //
        case cBotNet_LinkBase2::tenStates::enStSyncPrepareTx:
          //  menSm = cBotNet_LinkBase2::tenStates::enStSyncWaitForTx;
          //  lbLoop = True;
          //  break;
          //
          //case cBotNet_LinkBase2::tenStates::enStSyncWaitForTx:
          menSm = cBotNet_LinkBase2::tenStates::enStSyncStartTx;
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

        case cBotNet_LinkBase2::tenStates::enStSyncStartTx:
          {
            vSmPrepareDataTx();
            menSm = cBotNet_LinkBase2::tenStates::enStSyncDoneTx;
            mcDriver.vStartMsg(&mpcMsgSyncT);
          }
          break;		

        case cBotNet_LinkBase2::tenStates::enStSyncDoneTx:
          if (lenEvent == cComNode2::tenEvent::enEvDone)
          {
            // Sind Daten zu senden ?
            if (mpcMsgDataT.Len())
            {
              // NoCheck schlägt Busy
              // OneWay gibt es nicht beim Slave
              if ((IsSyncNoCheck(mpcMsgSyncT.mpu8Data)) ||
                  (tybNoCheck) ||
                  (!mbMBusy))
              {
                menSm = cBotNet_LinkBase2::tenStates::enStDataPrepareTx;
                lbLoop = True;
                break;
              }
            }

            menSm = cBotNet_LinkBase2::tenStates::enStEnd;
            lbLoop = True;
          }
          break;

          //
          // ----------------------------------- Data: Tx Master:Rx  Slave:Tx -----------------------------------
          //
        case cBotNet_LinkBase2::tenStates::enStDataPrepareTx:
            vDataTxPrepare(lbLoop, menSm);
            break;

        case cBotNet_LinkBase2::tenStates::enStDataWaitTx:
          menSm = cBotNet_LinkBase2::tenStates::enStDataStartTx;
          if (tyu16DataWaitTime > 0)
          {
            mcDriver.vStartTimer(tyu16DataWaitTime);
          }
          else
          {
            lbLoop = True;
          }
          break;

        case cBotNet_LinkBase2::tenStates::enStDataStartTx:
          vDataTxStart(lbLoop, menSm);
          break;

        case cBotNet_LinkBase2::tenStates::enStDataDoneTx:
          if (lenEvent == cComNode2::tenEvent::enEvDone)
          {
            vDataTxDone(lbLoop, menSm);
          }
          break;

          //
          // ----------------------------------- Ende -----------------------------------
          //
        case enStEndError:
          mcDriver.vResetCom();
          mu16TimeoutCounter_ms = 0;
          menSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareRx;
          lbLoop = True;
          break;
        case enStEnd:
          mu16TimeoutCounter_ms = 0;
          menSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareRx;
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
      cBotNet_LinkBase2::vTick10ms();
      mu16Counter_ms -= 10;
    }

    if (mControl.EnableRequest)
    {
      if (menSm == enStDisabled)
      {
        mcDriver.vHwContinue();

        menSm = cBotNet_LinkBase2::tenStates::enStSyncPrepareRx;
        vSm(cComNode2::tenEvent::enEvDummy);
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
        mcDriver.mError = cComNode2::tenError::enErTimeout;
        mcDriver.vComError();
      }
    }
    _eai(); //enable all interrupts
  }

  // -------------------------------------------

  bool bPut(u8 lu8PoolIdx)
  {
    return cBotNet_LinkBase2::bPut(lu8PoolIdx);
  }

  u8 u8Get()
  {
    return cBotNet_LinkBase2::u8Get();
  }
};



void vLedInit()
{
  // PA0
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  // PC13 liegt im CRH-Register (Control Register High, da Pin 8-15)
  // Bit-Maske für Pin 13 löschen (Bits 20..23)
  GPIOC->CRH &= ~(0xF << 20);

  // Bits setzen: MODE13 = 10 (Output 2 MHz), CNF13 = 00 (General Purpose Push-Pull) -> 0x2
  GPIOC->CRH |= (0x2 << 20);
}

void vLedToggle()
{
  GPIOC->ODR ^= (1 << 13);
}


//cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
//
//
//cUartMpHdSlave gcUartMpHdU0;
////cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);
//
cBnUpLink<cUartMpHdSlave2, 1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, True> gcUplink;
//
//cBotNet gcBn(&mcMyBotNetCfg);
//
////cBotNetMsgPortBtr   gcBtr(&gcBn);
//cBotNetMsgPortSpop  gcSpop(&gcBn);
////cBotNetMsgPortRRpt  gcRRpt(&gcBn);





void DMA_Channel1_2_IRQHandler(void)
{
  // USART1 RX
  //if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    gcUplink.mcDriver.IrqHandler(cComNode2::tenEvent::enEvDmaRxTc);
  }
}

void USART1_IRQHandler(void)
{
  if (USART1->SR & ~USART_SR_TC)
  {
    gcUplink.mcDriver.IrqHandler(cComNode2::tenEvent::enEvUsartTc);
  }
  else
  {
    vClearAllFlags()
  }
}

void TIM4_IRQHandler(void)
{
  if (TIM16->SR & TIM_SR_UIF)
  {
    TIM16->SR &= ~TIM_SR_UIF;
    TIM16->CR1 &= ~TIM_CR1_CEN;
    gcUplink.mcDriver.TIM_EV_IRQHandler();
  }
}

u16 MAIN_u8Counter_ms;
u16 MAIN_u8Counter_old_ms;

void TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_SR_UIF)
  {
    TIM2->SR &= ~TIM_SR_UIF;

    MAIN_u8Counter_ms++;
  }
}



void vInitTim2()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  MAIN_u8Counter_old_ms = MAIN_u8Counter_ms = 0;

  // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
  // Braucht aber so 90Byte weniger
  TIM2->PSC = 23;    //  23 für 24Mhz, 15 für 16Mhz und 7 für 8Mhz
  TIM2->ARR = 999;

  TIM2->EGR |= TIM_EGR_UG;
  TIM2->SR &= ~TIM_SR_UIF;

  /*NVIC config*/
  TIM2->DIER |= TIM_DIER_UIE;
  NVIC_EnableIRQ(TIM2_IRQn);
  TIM2->CR1 |= TIM_CR1_CEN;
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

  vLedInit();
  cBnMsgPool::vInit();

  // Add MsgSys
  //gcBtr.vAddMsgSys();
  //gcSpop.vAddMsgSys();
  //gcRRpt.vAddMsgSys();

  // Add Uplink
  //gcBn.bAddLink((cBotNet_LinkBase*)&gcUpLink);

  vInitTim2();
}

int main(void)
{
  MAIN_vInitSystem();
 

  while (1)
  {
    //fwdgt_counter_reload();

    if (MAIN_u8Counter_old_ms != MAIN_u8Counter_ms)
    {
      MAIN_u8Counter_old_ms = MAIN_u8Counter_ms;
      ////gcBn.vProcess(1000);
      gcUplink.mcDriver.vTick1ms();

      if ((MAIN_u8Counter_ms % 1000) == 0)
      {
        vLedToggle();
      }
    }

    //fwdgt_counter_reload();

    __asm("wfi");
  }
}


void SystemClock_24MHz_Init(void) {
  // 1. RCC auf Standardwerte zurücksetzen
  RCC_DeInit();

  // 2. Externen Quarz (HSE) einschalten
  RCC_HSEConfig(RCC_HSE_ON);

  // 3. Warten, bis HSE stabil läuft
  ErrorStatus hseStatus = RCC_WaitForHSEStartUp();

  if (hseStatus == SUCCESS) 
  {
    // Flash Latency auf 0 Wait States setzen (reicht bis 24 MHz)
    FLASH_SetLatency(FLASH_Latency_0);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    // HCLK (AHB) = SYSCLK (24 MHz)
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    // PCLK2 (APB2) = HCLK (24 MHz)
    RCC_PCLK2Config(RCC_HCLK_Div1);

    // PCLK1 (APB1) = HCLK (24 MHz) -> Max. 36 MHz erlaubt
    RCC_PCLK1Config(RCC_HCLK_Div1);

    // PLL-Quelle: HSE ungeteilt (8 MHz), Multiplikator: 3x -> 8 MHz * 3 = 24 MHz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);

    // PLL aktivieren
    RCC_PLLCmd(ENABLE);

    // Warten, bis PLL bereit/eingelockt ist
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    // PLL als System-Taktquelle auswählen
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    // Warten, bis PLL tatsächlich als Systemtakt aktiv ist (0x08 = PLL)
    while (RCC_GetSYSCLKSource() != 0x08);
  }
  else {
    // Fehlerbehandlung: Falls der Quarz nicht startet (Fallback auf HSI/Intern)
    while (1)
    {
      __asm("nop");
    }
  }
}


void MainSystemInit(void)
{
  /* FPU settings */
  #if (__FPU_PRESENT == 1U) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
  #endif

  /* configure system clock */
  // Externe 24Mhz
  SystemClock_24MHz_Init();

  #ifdef  VECT_TAB_OFFSET
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, VECT_TAB_OFFSET);
  #endif
}
