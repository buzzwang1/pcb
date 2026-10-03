
#include "main.h"
#include <concepts>

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz
//
//
//  PB09  -> Status Led
//
//  OutPuts:
//    PA01: High side switch for 3V3 
//
//  BotNet UpLink
//    U1
//      PA09  -> TX
//
//    I2C1
//      PB06  -> I2C1 SCL AF4
//      PB07  -> I2C1 SDA AF4
//
//  DMA Usage:
//    DMA1:
//      0:
//      1: 
//      2: 
//      3: 
//      4: 
//      5: 
//      6: I2C1 Tx: UpLink   CS:3 
//      7: I2C1 Rx: UpLink   CS:3 

//    DMA2:
//      6: U1 Tx: UpLink   CS:2 
//      7: U1 Rx: UpLink   CS:2 

__IO uint32_t TimingDelay = 0;
u32   mu32SpopCounter;

LED<GPIOB_BASE, 9> mcLed;
cGpPin lcS_3V3(GPIOA_BASE, 1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);



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

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif



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
    Derived* pDerived = static_cast<Derived*>(this);

    mSm = cComNode2::enStError;

    pDerived->vComError();

    // Wenn nach SW-Fehlerbehandlung HW immer noch Busy, dann einen HW Reset anstoßen
    if (!pDerived->bCheckBusy())
    {
      mError = cComNode2::enErNoInit;
      mSm    = cComNode2::enStError;
    }
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

  //void vTick1ms()
  //{
  //  cComNodeBase2::vTick1ms();
  //
  //  if (this->mpcActiveSlave)
  //  {
  //    this->mpcActiveSlave->vTick(1);
  //  }
  //}

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
    if (pDerived->bCheckBusy())
    {
      this->vResetComTimeout();
      this->mpcActiveMsg = lpcActiveMsg;
      pDerived->vSm(cComNode2::tenEvent::enEvStartWait);
    }
    else
    {
      this->mError = cComNode2::enErStartWhileBusy;
      this->vErrorHdl();
    }
  }

  void vStartMsg(cComMsg<u16>* lpcActiveMsg, bool boSkipAdr = False) requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
	
    // Beim Slave kann kein StartWhileBusy kommen
    // weil er durch interrupt gesteuert wird
    // und nicht alle 1ms wie bei Master
    if (pDerived->bCheckBusy())
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
    else
    {
      this->mError = cComNode2::enErStartWhileBusy;
      this->vErrorHdl();
    }
  }

  //virtual void vHwStop() = 0;
  //virtual void vHwContinue() = 0;
};


class cUartMpHd2
{
  public:
  u16                    mu16DmaCounter2ms_Old;
  u8                     mu8SelfTimer;

  //Single Wire, only Tx Pin
  USART_TypeDef*        mUsart;
  cGpPin                mPinTx;
  DMA_TypeDef*          mDma;
  DMA_Channel_TypeDef*  mChnTx;
  DMA_Channel_TypeDef*  mChnRx;
  u32                   mChnTxFlagTc;
  u32                   mChnRxFlagTc;
  IRQn_Type             mIrqChannelUsart;
  IRQn_Type             mIrqChannelDMA;



  cUartMpHd2(USART_TypeDef *lstUsart)
  {
    mUsart = lstUsart;
    mu8SelfTimer = 0;

    if (mUsart == USART1)
    {
      mDma = DMA2;
      mChnTx = DMA2_Channel6;
      mChnRx = DMA2_Channel7;
      mChnTxFlagTc = DMA_FLAG_TC6;
      mChnRxFlagTc = DMA_FLAG_TC7;

      mIrqChannelDMA =   DMA2_Channel7_IRQn; //USART1 connect to channel 7 of DMA2
      mIrqChannelUsart = USART1_IRQn;
    }
    else
    if (mUsart == USART2)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel7;
      mChnRx = DMA1_Channel6;
      mChnTxFlagTc = DMA_FLAG_TC7;
      mChnRxFlagTc = DMA_FLAG_TC6;

      mIrqChannelDMA   = DMA1_Channel6_IRQn;
      mIrqChannelUsart = USART2_IRQn;
    }
    else
    if (mUsart == USART3)
    {
      mDma = DMA1;
      mChnTx = DMA1_Channel2;
      mChnRx = DMA1_Channel3;
      mChnTxFlagTc = DMA_FLAG_TC2;
      mChnRxFlagTc = DMA_FLAG_TC3;

      mIrqChannelDMA = DMA1_Channel3_IRQn;
      mIrqChannelUsart = USART3_IRQn;
    }
  }

  void  vStopDMA()
  {
    LL_USART_DisableDMAReq_RX(mUsart);
    LL_USART_DisableDMAReq_TX(mUsart);
    mChnTx->CCR &=  ~DMA_CCR_EN; // USART1_TX
    mChnRx->CCR &=  ~DMA_CCR_EN; // USART1_RX
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounterRx()
  {
    return mChnRx->CNDTR;
  }

  void  vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mu16DmaCounter2ms_Old = BufferSize;
    mDma->IFCR = mChnRxFlagTc;
    mChnRx->CNDTR = BufferSize;
    mChnRx->CMAR = (uint32)pBuffer;
    mChnRx->CCR |= (DMA_CCR_EN | DMA_CCR_TCIE);
    LL_USART_EnableDMAReq_RX(mUsart);
  }

  void  vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    mDma->IFCR = mChnTxFlagTc;
    mChnTx->CNDTR = BufferSize;
    mChnTx->CMAR  = (uint32)pBuffer;
    // Ende der Übertragung wird durch  Usart.TC interrupt erledigt
    // Tx DMA Interrupt wird nicht benötigt
    mChnTx->CCR  |=  (DMA_CCR_EN /*| DMA_CCR_TCIE*/);
    LL_USART_EnableDMAReq_TX(mUsart);
  }

  //void  vInitHw(u8 lui8Adr, u32 lu32Baudrate, bool lbMaster)
  void  vInitHw(u8 lui8Adr)
  {
    u32 lu32Baudrate = 200000;
    bool lbMaster = false;

    // Setup Pins
    if (mUsart == USART1) mPinTx.vInit(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART2) mPinTx.vInit(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);
    else if (mUsart == USART3) mPinTx.vInit(GPIOB_BASE,10, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_LOW, 1);


    /* Restart USART clock */
    if (mUsart == USART1)      { __HAL_RCC_USART1_FORCE_RESET(); __HAL_RCC_USART1_RELEASE_RESET(); __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_SYSCLK); }
    else if (mUsart == USART2) { __HAL_RCC_USART2_FORCE_RESET(); __HAL_RCC_USART2_RELEASE_RESET(); __HAL_RCC_USART2_CONFIG(RCC_USART2CLKSOURCE_SYSCLK); }
    else if (mUsart == USART3) { __HAL_RCC_USART3_FORCE_RESET(); __HAL_RCC_USART3_RELEASE_RESET(); __HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_SYSCLK); }

    // Enable DMA
    if (mUsart == USART1) __HAL_RCC_DMA2_CLK_ENABLE();
    else if (mUsart == USART2) __HAL_RCC_DMA1_CLK_ENABLE();
    else if (mUsart == USART3) __HAL_RCC_DMA1_CLK_ENABLE();

    vStopDMA();

    //SDA and SCL must be pulled high
    if (lbMaster)
    {
      if (!bCheckPins()) return;
    }

    //Single wire, only TX line
    if (mUsart == USART1) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART1);}
    else if (mUsart == USART2) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART2);}
    else if (mUsart == USART3) { mPinTx.vSetAF(GPIO_MODE_AF_OD, GPIO_AF7_USART3);}

    // Usart Periph clock enable
    if (mUsart == USART1) __HAL_RCC_USART1_CLK_ENABLE();
    else if (mUsart == USART2) __HAL_RCC_USART2_CLK_ENABLE();
    else if (mUsart == USART3) __HAL_RCC_USART3_CLK_ENABLE();

    // Usart Peripheral Disable
    LL_USART_Disable(mUsart);

    LL_USART_InitTypeDef lstInit;
    LL_USART_StructInit(&lstInit);

    lstInit.BaudRate            = lu32Baudrate;
    lstInit.DataWidth           = LL_USART_DATAWIDTH_9B;
    lstInit.StopBits            = LL_USART_STOPBITS_1;
    lstInit.Parity              = LL_USART_PARITY_NONE;
    lstInit.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    lstInit.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    lstInit.OverSampling        = LL_USART_OVERSAMPLING_8;

    LL_USART_Init(mUsart, &lstInit);

    LL_USART_EnableHalfDuplex(mUsart);

    if (!lbMaster)
    {
      LL_USART_ConfigNodeAddress(mUsart, LL_USART_ADDRESS_DETECT_4B, lui8Adr);
      LL_USART_EnableMuteMode(mUsart);
      LL_USART_SetWakeUpMethod(mUsart, LL_USART_WAKEUP_ADDRESSMARK);
    }

    // Usart Peripheral Enable
    LL_USART_Enable(mUsart);
    mUsart->ICR = 0xFFFFFFFF;

    //if (!mbMaster)
    //{
    //  LL_USART_RequestEnterMuteMode(mUsart);
    //}

    mUsart->CR1 |= LL_USART_CR1_TCIE;
    mUsart->CR3 |= LL_USART_CR3_EIE;

    // ----------- DMA ---------
    // DMA Rx
    DMA_HandleTypeDef lhDMA = {};

    lhDMA.Instance = mChnRx;

    lhDMA.Init.Request             = DMA_REQUEST_2;
    lhDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&lhDMA);

    // Configure DMA Channel data length
    lhDMA.Instance->CNDTR = 0;
    // Configure DMA Channel destination address
    lhDMA.Instance->CPAR = (uint32_t)&(mUsart->RDR);
    // Configure DMA Channel source address
    lhDMA.Instance->CMAR = 0;


    // DMA Tx
    lhDMA.Instance = mChnTx;

    lhDMA.Init.Request             = DMA_REQUEST_2;
    lhDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // DMA_PDATAALIGN_BYTE; 16 bit wegen 9 Bit transfer
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&lhDMA);

    // Configure DMA Channel data length
    lhDMA.Instance->CNDTR = 0;
    // Configure DMA Channel destination address
    lhDMA.Instance->CPAR = (uint32)&(mUsart->TDR);
    // Configure DMA Channel source address
    lhDMA.Instance->CMAR = 0;

    // DMA interrupt -- TX DMA interrupt wird nicht benötigt

    HAL_NVIC_SetPriority(mIrqChannelUsart, 6, 8); // Höhere Prio,wegen Asynchron
    HAL_NVIC_SetPriority(mIrqChannelDMA, 7, 8);  // Höhere Prio,wegen Asynchron
    vEnableIrq();
  }

  void vEnableIrq()
  {
    HAL_NVIC_EnableIRQ(mIrqChannelUsart);
    HAL_NVIC_EnableIRQ(mIrqChannelDMA);
  }

  void vDisableIrq()
  {
    HAL_NVIC_DisableIRQ(mIrqChannelUsart);
    HAL_NVIC_DisableIRQ(mIrqChannelDMA);
  }

  bool  bCheckBusy()
  {
    if (mUsart->ISR & USART_ISR_BUSY)
    {
      return False;
    }
    return True;
  }

  bool  bCheckPins()
  {
    //mPinTx.vSetMode(GPIO_MODE_INPUT);
    //mPinSDA->vSetMode(GPIO_MODE_INPUT);

    //Tx must be pulled high
    if (mPinTx.ui8Get() == 0)
    {
      return False;
    }
    return True;
  }
};


template <typename tcLink>
class cUartMpHdSlave2 : public cUartMpHd2, public cComNodeSlave2<cUartMpHdSlave2<tcLink>>
{
public:
  tcLink* mcLink = nullptr;
  
  using BaseNode = cComNodeSlave2<cUartMpHdSlave2<tcLink>>;

  cUsartMpHd_Slave_Timer mTimer;


  cUartMpHdSlave2(u16 luInitDelay_ms)
    : cUartMpHd2(USART1),
      BaseNode(luInitDelay_ms)
  {
    this->mSm = cComNode2::tenState::enStIdle;
    this->mu32Baudrate = 200000;
  }

  void vAddNode(tcLink& c) requires cptLink<tcLink>
  {
    mcLink = &c;
    this->muAdr = mcLink->mAdr;
    mcLink->mStatus.IsEnabled = True;
    BaseNode::vInit();
    mcLink->vInit(True);
  }

  void vTick1msAbc() requires cptLink<tcLink>
  {
    BaseNode::vTick1ms();
    
    if (mcLink)
    {
      mcLink->vTick(1);
    }
  }

  cComNode2::tenError enInitHw()
  {
    cUartMpHd2::vInitHw(this->muAdr);
    return cComNode2::tenError::enNoError;
  }

  void vHwStop()
  {
    vDisableIrq();
    mUsart->ICR = 0xFFFF;
    LL_USART_Disable(mUsart);
  }
  
  void vHwContinue()
  {
    LL_USART_Enable(mUsart);
    mUsart->ICR = 0xFFFF;
    vEnableIrq();
  }

  void vResetCom()
  {
    if (this->mSm != cComNode2::tenState::enStIdle)
    {
      // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
      LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
      LL_USART_RequestRxDataFlush(mUsart);
      vStopDMA();

      mTimer.vStop();
      mUsart->ICR = 0xFFFF;

      this->mSm    = cComNode2::tenState::enStIdle;
      this->mError = cComNode2::enNoError;
    }
  }

  void vComError()
  {
    cComNode2::tenState lSm = this->mSm;
    cComNode2::tenError lError = this->mError;

    vResetCom();

    if (mcLink)
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
    if (mcLink)
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
    uint32 lui32ISR = mUsart->ISR;
    if (lui32ISR & 0xF)
    {
      mUsart->ICR = 0xF;
      this->mError = cComNode2::enErOverrun;
      vComError();
    }
    else
    {
      switch (lenEvent)
      {
        case cComNode2::tenEvent::enEvDmaRxTc:
          mChnRx->CCR &= ~DMA_CCR_EN;
          mDma->IFCR = mChnRxFlagTc;
          vSm(lenEvent);
          break;
        case cComNode2::tenEvent::enEvUsartTc:
          // DMA startet nur neu, wenn er vorher ausschalten wurde.
          mChnTx->CCR &= ~DMA_CCR_EN;
          mUsart->ICR = LL_USART_ISR_TC;
          vSm(lenEvent);
          break;
        default:
          break;
      }
    }
  }

  inline void vMute()
  {
    LL_USART_RequestEnterMuteMode(mUsart);
  }

  inline void vDone()
  {
    mTimer.vStop();
    mu8SelfTimer = 0;
    // receiver abschalten um mögliche Overrun-Errors zuvermeiden.
    LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
    this->mSm = cComNode2::tenState::enStIdle;
    mcLink->vComDone();
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
        if (!(LL_USART_IsActiveFlag_RWU(mUsart)))    // Nicht im Mute modus
        {
          switch (lenEvent)
          {
            case cComNode2::tenEvent::enEvStart:
            case cComNode2::tenEvent::enEvStartSkipAdr:
              // Rx
              if (this->mpcActiveMsg->isRx())
              {
		            LL_USART_RequestRxDataFlush(mUsart);
		            LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten

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
                  LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX); // Rx ausschalten
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
                LL_USART_RequestRxDataFlush(mUsart);
                LL_USART_SetTransferDirection(mUsart, LL_USART_DIRECTION_TX_RX); // Alles einschalten

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

  bool bAddedToBn(u16 lu16Adr)
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
    vSm(cComNode2::tenEvent::enEvDummy);
  }

  void vComError(cComNode2::tenError lenError, cComNode2::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);
    this->vResetStatusComFlags();
    menSm = tenStates::enStEndError;
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

  void vSm(cComNode2::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
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
            case cComNode2::tenEvent::enEvDone:
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
            case cComNode2::tenEvent::enEvDone:
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
          if (lenEvent == cComNode2::tenEvent::enEvDone)
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
          if (lenEvent == cComNode2::tenEvent::enEvDone)
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
};




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



// ---------------------------- U1 ---------------------------

void DMA2_Channel7_IRQHandler(void)
{
  // USART1 RX
  gcUplink.mcDriver.IrqHandler(cComNode2::tenEvent::enEvDmaRxTc);
}

void USART1_IRQHandler(void)
{
  gcUplink.mcDriver.IrqHandler(cComNode2::tenEvent::enEvUsartTc);
}

void TIM1_UP_TIM16_IRQHandler(void)
{
  if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    gcUplink.mcDriver.TIM_EV_IRQHandler();
  }
}




void MAIN_vTick1msLp(void)
{
  //mcBn.vProcess(1000);
  gcUplink.mcDriver.vTick1msAbc();
}

void MAIN_vTick100msLp(void)
{
  mcLed.Toggle();
  cBnSpop_vResetWdog();
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  //u8 lu8t;

  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
  cBnSpop_vResetWdog();
  cBnMsgPool::vInit();

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  //mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);
  gcUplink.bAddedToBn(0x1100);
  //mu32SpopCounter = 1000 * 60 * 2;

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp               /*1ms_LP*/,
                NULL   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL /*1s_LP*/);
  cBnSpop_vResetWdog();
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  };
}

bool SystemClock_Config_HSE(void)
{
  u16 lu16Retries = 100;
  bool lbError = False;
  while (lu16Retries > 0)
  {
    lbError = False;
    cBnSpop_vResetWdog();

    // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
    // kein Pll

    RCC_OscInitTypeDef RCC_OscInitStruct   = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      //cErr::munErr->stErr.isInitOscCfg = 1;
      lbError = True;
    }

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
      //cErr::munErr->stErr.isInitClkCfg = 1;
      lbError = True;
    }

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
      //cErr::munErr->stErr.isInitVltScl = 1;
      lbError = True;
    }

    if (!lbError) break;

    lu16Retries--;
  }

  if (lu16Retries == 0) return False;

  return True;
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  cBnSpop_vResetWdog();
  SystemInit();
  HAL_Init();
  SystemClock_Config_HSE(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

