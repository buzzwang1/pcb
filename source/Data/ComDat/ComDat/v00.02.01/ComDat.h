#ifndef __COMDAT_H__
#define __COMDAT_H__

#include "Typedef.h"
#include "cbArrayT.h"


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
  };

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
    mStatus.IsAckTx = 1;
    mStatus.IsAckRx = 0;

    mControl.StartRequest = 0;
  }

  // lbStart wird für RadioLink gebraucht.
  // z.B. das Link erst gestarted wird, sobald eine Session besteht
  virtual void vInit(bool lbStart) = 0;

  //Called, before a transmission is started to prepare data
  virtual void vComStart(cComNode::tenEvent lenEvent) = 0;

  //Called, after a transmission was done
  virtual void vComDone() = 0;

  virtual void vTick(u16 lu16Time_ms)
  {
    UNUSED(lu16Time_ms);
  };

  //Called, on Error
  virtual void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);
    mStatus.IsError = 1;
  };

  bool IsEnabled()       {return mStatus.IsEnabled;}
  bool IsInit()          {return mStatus.IsInit;}
  bool IsOnline()        {return mStatus.IsOnline;}
  bool IsError()         {return mStatus.IsError;}
  bool IsBusy()          {return mStatus.IsBusy;}
  bool IsAckRx()         {return mStatus.IsAckRx;}
  bool IsAckTx()         {return mStatus.IsAckTx;}
  bool IsInitAndOnline() {return (IsInit() && IsOnline());}

  void vSetOnline()           {mStatus.IsOnline = 1;}
  void vSetAckRx(bool lbSate) {mStatus.IsAckRx  = lbSate;}
  void vSetAckTx(bool lbSate) {mStatus.IsAckTx  = lbSate;}
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

// Speziel für I2C, weil Register lesen eine 'TX->repeated start->RX' aktion ist
// Also braucht man 2 Nachrichten
//  - eine sende Nachricht
//  - und eine empfangsnachricht
//
// Die I2C msg ist 8Byte lang
//   - 4Byte: Pointer to Schreib-Nachricht
//   - 4Byte: Pointer to Lese-Nachricht

template <typename tyTyp>
class cComMsgI2c : public cComMsg<tyTyp>
{
  public:
  union
  {
    u8 mData[8];
    struct
    {
      cComMsg<u16>* mpcRx;
      cComMsg<u16>* mpcTx;
    }menMsgs;
  }munMsgPointer;

  cComMsgI2c(cComMsg<u16>* lpcTx, cComMsg<u16>* lpcRx) : cComMsg<tyTyp>(cComNode::tenDirection::enIsTxRx, munMsgPointer.mData, 0, 8)
  {
    vPrepareRx(lpcTx, lpcRx);
  };

  // Nur für I2C
  void vPrepareRx(cComMsg<u16>* lpcTx, cComMsg<u16>* lpcRx)
  {
    munMsgPointer.menMsgs.mpcRx = lpcRx;
    munMsgPointer.menMsgs.mpcTx = lpcTx;
  }

  // Nur für I2C
  void vPrepareRx(u8 lu8RegAdress, tyTyp luLenRx)
  {
    munMsgPointer.menMsgs.mpcTx->Set(lu8RegAdress);
    munMsgPointer.menMsgs.mpcRx->muLen = luLenRx;
  }

  // Nur für I2C
  void vPrepareRx(cComMsg<u16>* lpcTx, cComMsg<u16>* lpcRx, u8 lu8RegAdress, tyTyp luLenRx)
  {
    vPrepareRx(lpcTx, lpcRx);
    vPrepareRx(lu8RegAdress, luLenRx);
  }

  tyTyp GetLenTx()
  {
    return munMsgPointer.menMsgs.mpcTx->muLen;
  }

  tyTyp GetLenRx()
  {
    return munMsgPointer.menMsgs.mpcRx->muLen;
  }

  u8* GetDataTx()
  {
    return munMsgPointer.menMsgs.mpcTx->Data();
  }

  u8* GetDataRx()
  {
    return munMsgPointer.menMsgs.mpcRx->Data();
  }
};



class cComNodeList
{
  public:

  cComNode*       mpacNodes[20];
  SBArry<u8, 20>  mcNodeList;
  u8              mu8Pos;

  cComNodeList()
    : mcNodeList()
  {
    mu8Pos = 0;
  }

  void vStartAll()
  {
    u8 lu8NodeIdx;
    for (lu8NodeIdx = 0; lu8NodeIdx < mcNodeList.mLen; lu8NodeIdx++)
    {
      mpacNodes[lu8NodeIdx]->mControl.StartRequest = 1;
    }
  }

  void vInitAll()
  {
    u8 lu8NodeIdx;
    for (lu8NodeIdx = 0; lu8NodeIdx < mcNodeList.mLen; lu8NodeIdx++)
    {
      mpacNodes[lu8NodeIdx]->vInit(True);
    }
  }

  u8 uStartRequested()
  {
    u8 lu8NodeIdx;
    for (lu8NodeIdx = 0; lu8NodeIdx < mcNodeList.mLen; lu8NodeIdx++)
    {
      mu8Pos++;
      if (mu8Pos >= mcNodeList.mLen) mu8Pos = 0;
      if (mpacNodes[mu8Pos]->mControl.StartRequest) return mu8Pos;
    }
    return (u8)-1;
  }


  bool isSlotFree()
  {
    return (mcNodeList.mLen < mcNodeList.Size());
  }

  u8 uGetSlot(cComNode* lpcNode)
  {
    u8 lu8NodeIdx;
    for (lu8NodeIdx = 0; lu8NodeIdx < mcNodeList.mLen; lu8NodeIdx++)
    {
      if (mpacNodes[lu8NodeIdx] == lpcNode)
      {
        return lu8NodeIdx;
      }
    }
    return (u8)-1;
  }


  void vAdd(cComNode* lpcNode)
  {
    if (isSlotFree())
    {
      u8 luLen = mcNodeList.mLen;
      mcNodeList += 1;
      mpacNodes[luLen] = lpcNode;
    }
  }

  void vRemove(cComNode* lpcNode)
  {
    u8 lu8NodeIdx;

    lu8NodeIdx = uGetSlot(lpcNode);
    if (lu8NodeIdx != (u8)-1)
    {
      lu8NodeIdx++;
      for (; lu8NodeIdx < mcNodeList.mLen; lu8NodeIdx++)
      {
        mpacNodes[lu8NodeIdx - 1] = mpacNodes[lu8NodeIdx + 1];
      }
      mcNodeList.mLen--;
    }
  }

  cComNode* cGetNext()
  {
    cComNode* lpcNode = NULL;
    u8 lu8NodeIdx = uStartRequested();

    if (lu8NodeIdx != (u8)-1)
    {
      return mpacNodes[lu8NodeIdx];
    }
    return lpcNode;
  }
};



class cComNodeBase
{
  public:
  cComNode* mpcActiveSlave;
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
    mError  = cComNode::enErNoInit;
    mSm     = cComNode::enStError;

    mu16ReInitTicksReload_ms     = 500;
    mu16ReInitTicks_ms           = luInitDelay_ms;

    mu16ComTimeoutTicksReload_ms = 500;
    mu16ComTimeoutTicks_ms       = 0;
  }

  virtual bool               bCheckBusy() = 0;
  virtual cComNode::tenError enInitHw() = 0;
  virtual void               vResetCom() = 0;
  virtual void               vStartTimer(u16 luTime_us) { UNUSED(luTime_us); };
  virtual void               vAddNode(cComNode* lpcNode) = 0;

  void vInit()
  {
    mError = enInitHw();

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
       vResetCom();
       mu16ComTimeoutTicks_ms = 0;
    }
  }

  void vErrorHdl()
  {
    mSm = cComNode::enStError;

    //cComNode::tenState lSm    = mSm;
    //cComNode::tenError lError = mError;


    // vResetCom set active slave to null
    // Therefore make a copy
    //cComNode* lpcActiveSlave_Copy = mpcActiveSlave;
    //vResetCom();

    vComError();
    //if (lpcActiveSlave_Copy != NULL)
    //{
      //lpcActiveSlave_Copy->vComError(lError, lSm);
    //}

    // Wenn nach SW-Fehlerbehandlung HW immer noch Busy, dann einen HW Reset anstoßen
    if (!bCheckBusy())
    {
      mError  = cComNode::enErNoInit;
      mSm     = cComNode::enStError;
    }
  }

  void vTick1ms()
  {
    mu16TickCounter_ms++;
    //vSm(cComNode::tenEvent::enEvTick);

    // Neu Initialisierung, wenn mu16ReInitTicks_ms abgelaufen ist
    if (mSm == cComNode::enStError)
    {
      // Schauen, ob sich der Fehler selbst repariert
      // Busy kommt z.b., wenn ein der Leitungen nicht Pull Up ist
      if ((mError == cComNode::enErStartWhileBusy) &&
          (bCheckBusy()))
      {
        mSm    = cComNode::enStIdle;
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

  virtual void vComError() = 0;
  virtual void vSm(cComNode::tenEvent lenEvent) = 0;
  virtual void vStartMsg(cComMsg<u16>*lpcActiveMsg, bool boSkipAdr = False) = 0;
};


class cComNodeMaster : public cComNodeBase
{
  public:
  cComNodeMaster(u16 luInitDelay_ms)
    : cComNodeBase(luInitDelay_ms)
  {
  }

  void vLock()
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      mSm = cComNode::tenState::enStLock;
    }
  }

  void vSetReInitTicks(u16 lu16ReInitTicks_ms)
  {
    if (!mu16ReInitTicks_ms)
    {
      mu16ReInitTicks_ms = lu16ReInitTicks_ms;
    }
  }

  void vTick1ms()
  {
    cComNodeBase::vTick1ms();
  }


  void vStartMsg(cComMsg<u16> *lpcActiveMsg, bool boSkipAdr=False) override
  {
    if (bCheckBusy())
    {
      vResetComTimeout();
      mpcActiveMsg = lpcActiveMsg;
      if (boSkipAdr)
      {
        vSm(cComNode::tenEvent::enEvStartSkipAdr);
      }
      else
      {
        vSm(cComNode::tenEvent::enEvStart);
      }
    }
    else
    {
      mError = cComNode::enErStartWhileBusy;
      vErrorHdl();
    }
  }

  virtual void vPreStart() = 0;
  virtual void vPostStart() = 0;
};



class cComNodeMasterMulti : public cComNodeMaster
{
public:
  cComNodeList  mcSlaves;

  cComNodeMasterMulti(u16 luInitDelay_ms)
    : cComNodeMaster(luInitDelay_ms), mcSlaves()
  {
  }

  void vTick1ms()
  {
    cComNodeMaster::vTick1ms();
    vStart();

    for (u8 lu8NodeIdx = 0; lu8NodeIdx < mcSlaves.mcNodeList.mLen; lu8NodeIdx++)
    {
      if ((lu8NodeIdx & 3) == (mu16TickCounter_ms & 3))
      {
        mcSlaves.mpacNodes[lu8NodeIdx]->vTick(4);
      }
    }
  }


  void vAddNode(cComNode* lpcNode) override
  {
    mcSlaves.vAdd(lpcNode);
    lpcNode->mStatus.IsEnabled = True;
    lpcNode->vInit(True);
  }

  void vRemoveNode(cComNode* lpcNode)
  {
    mcSlaves.vRemove(lpcNode);
  }

  void vStart()  // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      u8 lu8SlaveIdx = mcSlaves.uStartRequested();

      if (lu8SlaveIdx != (u8)-1)
      {
        vPreStart();
        mpcActiveSlave = (cComNode*)mcSlaves.mpacNodes[lu8SlaveIdx];
        mpcActiveSlave->mControl.StartRequest = 0;
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
        vPostStart();
      }
    }
  }
};


class cComNodeMasterSingle : public cComNodeMaster
{
public:

  cComNodeMasterSingle(u16 luInitDelay_ms)
    : cComNodeMaster(luInitDelay_ms)
  {
    mpcActiveSlave = null;
  }


  void vTick1ms()
  {
    cComNodeBase::vTick1ms();
    vStart();

    if (mpcActiveSlave)
    {
      mpcActiveSlave->vTick(1);
    }
  }


  void vAddNode(cComNode* lpcNode) override
  {
    mpcActiveSlave = lpcNode;
    //muAdr = lpcNode->mAdr;
    lpcNode->mStatus.IsEnabled = True;
    cComNodeBase::vInit();
    lpcNode->vInit(True);
  }

  void vRemoveNode(cComNode* lpcNode)
  {
    lpcNode->mStatus.IsEnabled = False;
  }


  void vStart()  // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      if (mpcActiveSlave->mControl.StartRequest)
      {
        vPreStart();
        mpcActiveSlave->mControl.StartRequest = 0;
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
        vPostStart();
      }
    }
  }
};



class cComNodeSlave : public cComNodeBase
{
  public:

  u16 muAdr;

  cComNodeSlave(u16 luInitDelay_ms)
    : cComNodeBase(luInitDelay_ms)
  {
    mpcActiveSlave = null;
  }

  void vSetReInitTicks(u16 lu16ReInitTicks_ms)
  {
    if (!mu16ReInitTicks_ms)
    {
      mu16ReInitTicks_ms = lu16ReInitTicks_ms;
    }
  }

  void vTick1ms()
  {
    cComNodeBase::vTick1ms();

    if (mpcActiveSlave)
    {
      mpcActiveSlave->vTick(1);
    }
  }

  void vAddNode(cComNode *lpcNode) override
  {
    mpcActiveSlave = lpcNode;
    muAdr = lpcNode->mAdr;
    lpcNode->mStatus.IsEnabled = True;
    cComNodeBase::vInit();
    lpcNode->vInit(True);
  }

  void vWaitMsg(cComMsg<u16>* lpcActiveMsg)
  {
    if (bCheckBusy())
    {
      vResetComTimeout();
      mpcActiveMsg = lpcActiveMsg;
      vSm(cComNode::tenEvent::enEvStartWait);
    }
    else
    {
      mError = cComNode::enErStartWhileBusy;
      vErrorHdl();
    }
  }

  void vStartMsg(cComMsg<u16> *lpcActiveMsg, bool boSkipAdr = False) override
  {
    // Beim Slave kann kein StartWhileBusy kommen
    // weil er durch interrupt gesteuert wird
    // und nicht alle 1ms wie bei Master
    //if (bCheckBusy())
    {
      vResetComTimeout();
      mpcActiveMsg = lpcActiveMsg;
      if (boSkipAdr)
      {
        vSm(cComNode::tenEvent::enEvStartSkipAdr);
      }
      else
      {
        vSm(cComNode::tenEvent::enEvStart);
      }
    }
    //else
    //{
    //  mError = cComNode::enErStartWhileBusy;
    //  vErrorHdl();
    //}
  }

  virtual void vHwStop() = 0;
  virtual void vHwContinue() = 0;
};


#endif // __COMDAT_H__
