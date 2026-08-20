#ifndef __COMDAT_H__
#define __COMDAT_H__

#include "Typedef.h"
#include "cbArrayT.h"

#include <concepts>

class cComNode
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

  cComNode()
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


template <typename tyTyp>
class cComMsg : public cBarryPtrT<tyTyp>
{
  public:
  union
  {
    u8 mu8Byte;
    struct
    {
      cComNode::tenDirection mDir : 2;
      u8 mData                    : 6;
    }stBits;
  }munCfg;

  cComMsg(cComNode::tenDirection lDir) : cBarryPtrT<tyTyp>() { munCfg.stBits.mDir = lDir; };
  cComMsg(cComNode::tenDirection lDir, u8* lpaArray, tyTyp luiLen, tyTyp luiSize) : cBarryPtrT<tyTyp>(lpaArray, luiLen, luiSize) { munCfg.stBits.mDir = lDir; };
  cComMsg(cComNode::tenDirection lDir, cBarryPtrT<tyTyp> &lcBAryP) : cBarryPtrT<tyTyp>(lcBAryP) { munCfg.stBits.mDir = lDir; };

  void UseMem(u8* lpaArray, tyTyp luSize)
  {
    cBarryPtrT<tyTyp>::From(lpaArray, luSize, luSize);
  };

  void SetDir(cComNode::tenDirection lDir)
  {
    munCfg.stBits.mDir = lDir;
  }

  cComNode::tenDirection enDir()
  {
    return munCfg.stBits.mDir;
  }

  bool isTx()   { return (munCfg.stBits.mDir == cComNode::tenDirection::enIsTx); }
  bool isRx()   { return (munCfg.stBits.mDir == cComNode::tenDirection::enIsRx); }
  bool isTxRx() { return (munCfg.stBits.mDir == cComNode::tenDirection::enIsTxRx); }

  // Nur für Radio, e.g. nrf905
  bool isCfgWrite() { return (munCfg.stBits.mDir == cComNode::tenDirection::enCfgWrite); }

  u8 u8Checksum()
  {
    return cComMsg<tyTyp>::u8Sum()++;
  }
};


template <typename tyTyp, tyTyp tySize>
class cComMsgS : public cComMsg<tyTyp>
{
  public:
  u8 mData[tySize];

  cComMsgS(cComNode::tenDirection lDir) : cComMsg<tyTyp>(lDir, mData, 0, tySize) {};

  // Nur für I2C
  void vPrepareTx(u8 lu8RegAdress, tyTyp luSizeTx, u8* lu8TxData)
  {
    cComMsg<tyTyp>::SetDir(cComNode::tenDirection::enIsTx);
    cComMsg<tyTyp>::Set(lu8RegAdress);
    cComMsg<tyTyp>::Add(lu8TxData, luSizeTx);
  }
};

template <typename T>
concept cptComDriver = requires(T v, cComNode::tenEvent lenEvent)
{
  { v.enInitHw() }    -> std::same_as<cComNode::tenError>;
  { v.vResetCom() }   -> std::same_as<void>;
  { v.vComError() }   -> std::same_as<void>;
  { v.bCheckBusy() }  -> std::same_as<bool>;
  { v.vSm(lenEvent) } -> std::same_as<void>;
};

//template <cptComDriver Derived>
template <typename Derived>
class cComNodeBase
{
public:
  cComMsg<u16>* mpcActiveMsg;

  u32             mu32Baudrate;

  u16             mu16TickCounter_ms;    // Algemeiner Counter
  u16             mu16ReInitTicks_ms;    // HW-ReInit nach dieser Zeit
  u16             mu16ComTimeoutTicks_ms;  // Stillstandserkennung
  u16             mu16ComTimeoutTicksReload_ms;
  u16             mu16ReInitTicksReload_ms;

  cComNode::tenState  mSm;
  cComNode::tenError  mError;

  cComNodeBase(u16 luInitDelay_ms)
  {
    mError = cComNode::enErNoInit;
    mSm    = cComNode::enStError;

    mpcActiveMsg   = NULL;

    mu16ReInitTicksReload_ms = 500;
    mu16ReInitTicks_ms = luInitDelay_ms;

    mu16ComTimeoutTicksReload_ms = 500;
    mu16ComTimeoutTicks_ms = 0;
  }

  //virtual bool               bCheckBusy() = 0;
  //virtual cComNode::tenError enInitHw() = 0;
  //virtual void               vResetCom() = 0;
  //virtual void               vStartTimer(u16 luTime_us) { UNUSED(luTime_us); };
  //virtual void               vAddNode(cComNode* lpcNode) = 0;

  void vInit() requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
    mError = pDerived->enInitHw(); // Garantiert durch Concept!

    if (mError != cComNode::enNoError)
    {
      // Init hat nicht geklappt
      // Etwas später nochmals versuchen
      mu16ReInitTicks_ms = mu16ReInitTicksReload_ms;

      //mError = cComNode::enErNoInit;
      //mSm = cComNode::enStError;
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

    mSm = cComNode::enStError;

    pDerived->vComError();

    // Wenn nach SW-Fehlerbehandlung HW immer noch Busy, dann einen HW Reset anstoßen
    if (!pDerived->bCheckBusy())
    {
      mError = cComNode::enErNoInit;
      mSm    = cComNode::enStError;
    }
  }

  void vTick1ms() requires cptComDriver<Derived>
  {
    Derived* pDerived = static_cast<Derived*>(this);
    mu16TickCounter_ms++;

    pDerived->vSm(cComNode::tenEvent::enEvTick);

    // Neu Initialisierung, wenn mu16ReInitTicks_ms abgelaufen ist
    if (mSm == cComNode::enStError)
    {
      // Schauen, ob sich der Fehler selbst repariert
      // Busy kommt z.b., wenn ein der Leitungen nicht Pull Up ist
      if (mError == cComNode::enErStartWhileBusy)
      ////if ((mError == cComNode::enErStartWhileBusy) && (bCheckBusy()))
      {
        mSm = cComNode::enStIdle;
        mError = cComNode::enNoError;
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
      if ((mSm != cComNode::tenState::enStIdle) &&
          (mSm != cComNode::tenState::enStWaitAdress) &&
          (mSm != cComNode::enStError))
      {
        mu16ComTimeoutTicks_ms--;
        if (mu16ComTimeoutTicks_ms == 0)
        {
          mError = cComNode::enErComNodeSmTimeout;
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
  //virtual void vSm(cComNode::tenEvent lenEvent) = 0;
  //virtual void vStartMsg(cComMsg<u16>* lpcActiveMsg, bool boSkipAdr = False) = 0;
};


//template <cptComDriver Derived>
template <typename Derived>
class cComNodeSlave : public cComNodeBase<Derived>
{
public:

  u16 muAdr;

  cComNodeSlave(u16 luInitDelay_ms)
    : cComNodeBase<Derived>(luInitDelay_ms)
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
  //  cComNodeBase::vTick1ms();
  //
  //  if (this->mpcActiveSlave)
  //  {
  //    this->mpcActiveSlave->vTick(1);
  //  }
  //}

  ////void vAddNode(cComNode* lpcNode)
  ////{
  ////  mpcActiveSlave = lpcNode;
  ////  muAdr = lpcNode->mAdr;
  ////  lpcNode->mStatus.IsEnabled = True;
  ////  cComNodeBase::vInit();
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
      pDerived->vSm(cComNode::tenEvent::enEvStartWait);
    }
    else
    {
      this->mError = cComNode::enErStartWhileBusy;
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
        pDerived->vSm(cComNode::tenEvent::enEvStartSkipAdr);
      }
      else
      {
        pDerived->vSm(cComNode::tenEvent::enEvStart);
      }
    }
    else
    {
      this->mError = cComNode::enErStartWhileBusy;
      this->vErrorHdl();
    }
  }

  //virtual void vHwStop() = 0;
  //virtual void vHwContinue() = 0;
};


#endif // __COMDAT_H__
