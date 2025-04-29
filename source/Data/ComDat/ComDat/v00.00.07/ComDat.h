#ifndef __COMDAT_H__
#define __COMDAT_H__

#include "Typedef.h"
#include "cbArrayExtT.h"

typedef enum
{
  enDeviceNotInit = 0,
  enDeviceInitError = 0,
  enDeviceError,
  enDeviceReady
}tenComDeviceStatus;

typedef enum
{
  enBusNoError = 0,
  enBusNotReady,
  enDeviceInitErrorPins,
  enDeviceInitErrorDevice,
}tenComBusStatus;

typedef enum
{
  enStatusIdle = 0,
  enStatusActive,
  enStatusStuck
}tenComStatus;

#ifdef __GNUC__
class __attribute__((packed)) cComNode
#else
class cComNode
#endif
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
    enErStartWhileBussy,
    enErOverrun,
    enErTimeout,
    enErReInit
  }tenError;

  typedef enum
  {
    enStIdle,
    enStStart,
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
    enEvStart  = 0,
    enEvDone,
    enEvTick,
    enEvError,
    enEvIrq,
    enEvDummy,

    // Irqs
    enEvAdress,
    enEvDma,
    enEvDmaRxTc, // Transmission Complete
    enEvDmaRxEr, // Error
    enEvDmaTxTc, // Transmission Complete
    enEvDmaTxEr, // Error
    enEvI2cTc,   // Transmission Complete
    enEvUsartTc, // Transmission Complete
    enEvUsartAm, // Adress Match
    enEvUsartErOre, // Error Overrun
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
    enEvTyIrq,
    enEvTyError,
    enEvTySw,
  }tenEventType;

  typedef enum
  {
    enNop  = 0,
    enResetHwFull,
    enResetHwDma
  }tenConsts;

  typedef struct
  {
    u8 IsEnabled        : 1; // Manually enabled by User
    u8 IsInit           : 1; // 0 = Error in LinkInit, Like HW Error
    u8 IsOnline         : 1; // 1 = There was a communication in the last 200ms. Link online

    u8 IsStartRequested : 1; // 1 = Request to action
    u8 IsError          : 1; // 1 = Runtime-Error, like Runtime NACK for I2C
    u8 IsBusy           : 1; // 1 = Rx-Buffer is full
    u8 IsAckTx          : 1; // Last transmission was confirmed
    u8 IsAckRx          : 1; // Confirmation for last receiption

    // Legacy will be reomove in future
    u8 IsFirstTx :  1;
    u8 IsThisTxOk : 1;
    u8 IsLastTxOk : 1;
  }tstStatus;

  tstStatus mStatus;
  u16       mAdr;

  cComNode()
  {
    mStatus.IsInit    = 0;
    mStatus.IsEnabled = 1;
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
    mStatus.IsStartRequested = 0;
    mStatus.IsAckTx = 1;
    mStatus.IsAckRx = 0;

	// Legacy will be reomove in future
	mStatus.IsFirstTx  = 1;
    mStatus.IsThisTxOk = 0;
    mStatus.IsLastTxOk = 0;
  }

  virtual ~cComNode() {};

  //bool isInit()   {return mbIsInit;};
  //bool isOnline() {return mbIsOnline;};

  uint32 virtual SizeOf() {return 0;};

  //Called, before a transmission is started to prepare data
  void virtual vComStart(tenEvent lenEvent) = 0;

  //Called, after a transmission was done
  void virtual vComDone() = 0;

  //Called, on Error
  void virtual vComError(tenError lenError, tenState lenState)
  {
     UNUSED(lenError);
     UNUSED(lenState);
     mStatus.IsError = 1;
     mStatus.IsStartRequested = 0;
  };

  inline virtual bool IsInit()          {return mStatus.IsInit;}
  inline virtual bool IsOnline()        {return mStatus.IsOnline;}
  inline virtual bool IsError()         {return mStatus.IsError;}
  inline virtual bool IsBusy()          {return mStatus.IsBusy;}
  inline virtual bool IsAckRx()         {return mStatus.IsAckRx;}
  inline virtual bool IsAckTx()         {return mStatus.IsAckTx;}
  inline virtual bool IsInitAndOnline() {return (IsInit() && IsOnline());}

  inline virtual void vSetOnline()           {mStatus.IsOnline = 1;}
  inline virtual void vSetAckRx(bool lbSate) {mStatus.IsAckRx  = lbSate;}
  inline virtual void vSetAckTx(bool lbSate) {mStatus.IsAckTx  = lbSate;}
};


class cComDatMsg
{
  public:

  typedef enum
  {
    enFree  = 0,
    enUnset,
    enSet,
    enStart,
    enBusy,
    enDone,
    enError,
  }cComDatMsg_Status;

  typedef enum
  {
    enDataUndef  = 0,
    enDataBusy,
    enDataSet,
  }cComDatMsg_DataStatus;

  cComDatMsg_DataStatus enDataStatus;
  cComDatMsg_Status     enStatus;

  cbArrayExtDynT<uint16>   cTxData;
  cbArrayExtDynT<uint16>   cRxData;

  void vSetTx(cbArrayExtT<uint16> &lcTxData)      {cTxData.Set(lcTxData);}
  void vSetTx(uint8* lpaui8Data, uint16 lui16Len) {cTxData.mpu8Data = lpaui8Data; cTxData.muiLen = cTxData.muiSize = lui16Len;}
  void vSetRx(cbArrayExtT<uint16> &lcRxData)      {cRxData.Set(lcRxData);}
  void vSetRx(uint8* lpaui8Data, uint16 lui16Len) {cRxData.mpu8Data = lpaui8Data; cRxData.muiLen = cRxData.muiSize = lui16Len;}
  void vSetAll(uint8* lpaui8TxData, uint16 lui16TxLen, uint8* lpaui8RxData, uint16 lui16RxLen)
  {
    vSetTx(lpaui8TxData, lui16TxLen);
    vSetRx(lpaui8RxData, lui16RxLen);
    vSet();
  }

  cComDatMsg()
  {
    enStatus = enFree;
    enDataStatus = enDataUndef;
  }

  inline uint32 SizeOf()
  {
    return cRxData.SizeOf() + cTxData.SizeOf() - 2 * sizeof(cbArrayExtDynT<uint16>) + sizeof(cComDatMsg);
  }

  uint8 u8Checksum(cbArrayExtDynT<uint16> *lcBuf)
  {
    uint8 lu8ChkSum;
    u8*   lu8Buf = lcBuf->mpu8Data;
    u16   lu16Cnt;

    lu8ChkSum = 1;
    lu16Cnt = lcBuf->Len();

    while (lu16Cnt > 4)
    {
      lu8ChkSum += *lu8Buf++;
      lu8ChkSum += *lu8Buf++;
      lu8ChkSum += *lu8Buf++;
      lu8ChkSum += *lu8Buf++;
      lu16Cnt -= 4;
    }

    while (lu16Cnt)
    {
      lu8ChkSum += *lu8Buf++;
      lu16Cnt --;
    }

    return lu8ChkSum;
  }

  uint8 u8RxChecksum()
  {
    return u8Checksum(&cRxData);
  }

  uint8 u8TxChecksum()
  {
    return u8Checksum(&cTxData);
  }

  void vMemAlloc(uint16 lui16TxLen, uint32 lui16RxLen)
  {
    cTxData.Create(lui16TxLen);
    cRxData.Create(lui16RxLen);
    cTxData.muiLen = cTxData.muiSize;
    cRxData.muiLen = cRxData.muiSize;
    vUnset();
  }

  void vMemFree()
  {
    cTxData.~cbArrayExtDynT();
    cTxData.~cbArrayExtDynT();
    vFree();
  }

  void vFree()   {enStatus = enFree;  enDataStatus = enDataUndef;}
  void vUnset()  {enStatus = enUnset; enDataStatus = enDataUndef;}
  void vSet()    {enStatus = enSet;   enDataStatus = enDataUndef;}
  void vStart()  {enStatus = enStart;}
  void vBusy()   {enStatus = enBusy;  enDataStatus = enDataBusy;}
  void vDone()   {enStatus = enDone;  enDataStatus = enDataSet;}
  void vError()  {enStatus = enError; enDataStatus = enDataUndef;}

  bool isDataUndef() {return (enDataStatus == enDataUndef);}
  bool isDataSet()   {return (enDataStatus == enDataSet);}
  bool isDataBusy()  {return (enDataStatus == enDataBusy);}

  bool isFree()  {return (enStatus == enFree);}
  bool isUnSet() {return (enStatus == enUnset);}
  bool isSet()   {return (enStatus == enSet);}
  bool isStart() {return (enStatus == enStart);}
  bool isBusy()  {return (enStatus == enBusy);}
  bool isDone()  {return (enStatus == enDone);}
  bool isError() {return (enStatus == enError);}

  ~cComDatMsg() {vMemFree();}
};


class cComNodeList
{
  public:

  u8             mu8Size;
  u8             mu8Used;
  u8             mu8Pos;
  cComNode     **mpacNodes;

  cComNodeList(uint32 lui32NodesCnt);
  ~cComNodeList();

  uint32 SizeOf();

  bool   isEmpty();
  bool   isSlotFree();

  void   vStartAll();
  void   vSetErrorAll(cComNode::tenError lenError, cComNode::tenState lenState);

  u8   ui32GetFreeSlot();
  u8   ui32GetSlot(cComNode *lpcNode);
  void vAdd(cComNode *lpcNode);
  void vRemove(cComNode *lpcNode);
  cComNode* cGetNext();
};



class cComNodeDriver
{
  public:
  u16             mu16TickCounter;
  u16             mu16ReInitTicks;
  u16             mu16ComStuckTicks;
  u16             mu16ComStuckTicksReload;

  cComNode                   *mpcActiveSlave;
  cComDatMsg                  *mpcActiveMsg;

  cComNode::tenState   mSm;
  tenComDeviceStatus   munDeviceStatus;
  tenComBusStatus      munBusStatus;
  tenComStatus         munComStatus;
  cComNode::tenState   menComState;


  cComNodeDriver()
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm         = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;

    mpcActiveSlave = NULL;
    mpcActiveMsg   = NULL;

    mu16ReInitTicks         = 100;
    mu16ComStuckTicksReload = 2000;
    mu16ComStuckTicks       = mu16ComStuckTicksReload;
  }

  void virtual vReInit() = 0;

  void virtual vTick1ms()
  {
    mu16TickCounter++;
    vSmFrame(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);

    if (mSm == cComNode::enStError)
    {
      if (mu16ReInitTicks)
      {
        mu16ReInitTicks--;

        if (!mu16ReInitTicks)
        {
          mpcActiveMsg   = NULL;
        }
      }
    }
    else
    {
      mu16ReInitTicks   = 100;
    }

    if (munComStatus != tenComStatus::enStatusStuck)
    {
      if (mu16ComStuckTicks)
      {
        if (mSm != cComNode::tenState::enStIdle)
        {
          mu16ComStuckTicks--;
        }
      }
      else
      {
        mSm = cComNode::enStError;
        munDeviceStatus   = tenComDeviceStatus::enDeviceReady;
        munBusStatus      = tenComBusStatus::enBusNoError;
        munComStatus      = tenComStatus::enStatusStuck;
        mu16ComStuckTicks = mu16ComStuckTicksReload;
        mu16ReInitTicks   = 100;
      }
    }
  }

  void virtual vReInitHw(cComNode::tenConsts lenOption, bool lbMaster)
  {
    UNUSED(lenOption);
    UNUSED(lbMaster);
  };

  bool virtual bCheckBus() = 0;

  void virtual vSmFrame(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    cComNode::tenState mSm_Old;

    mSm_Old = mSm;

    vSm(lenEventType, lenEvent);

    if (mSm_Old != mSm)
    {
      mu16ComStuckTicks = mu16ComStuckTicksReload;
    }
  }

  void virtual vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) = 0;
  void virtual vStartMsg(cComDatMsg *lpcActiveMsg) = 0;
  bool virtual bStartNext() = 0;
};


class cComNodeDriverMaster : public cComNodeDriver
{
  public:

  cComNodeList  mcSlaves;

  u8 mu8Lock;

  cComNodeDriverMaster(uint32 lui32Slaves)
    : mcSlaves(lui32Slaves)
  {
    mu8Lock        = 0;
    vReInitHw(cComNode::enResetHwFull, True);
  }


  inline void SetLock()   {mu8Lock = 1;}
  inline void ResetLock() {mu8Lock = 0;}

  void vSetReInitTicks(u16 lu16ReInitTicks)
  {
    if (!mu16ReInitTicks)
    {
      mu16ReInitTicks = lu16ReInitTicks;
    }
  }

  void vTick1ms()
  {
    mu16TickCounter++;
    vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);
  }


  void vAddSlave(cComNode *lpcSlave)
  {
    mcSlaves.vAdd(lpcSlave);
  }

  void vRemoveSlave(cComNode *lpcSlave)
  {
    mcSlaves.vRemove(lpcSlave);
  }

  void vReInitAllSlave()
  {
    mcSlaves.vSetErrorAll(cComNode::tenError::enErReInit, cComNode::cComNode::tenState::enStIdle);
  }

  void virtual vReInit()
  {
    mu8Lock        = 0;
    this->vReInitHw(cComNode::tenConsts::enResetHwFull, True);
    this->vReInitAllSlave();
  }


  void vStartMsg(cComDatMsg *lpcActiveMsg)
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (mpcActiveMsg != NULL)
      {
        if (mpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      lbStart &= bCheckBus();

      if (lbStart)
      {
        mpcActiveMsg = lpcActiveMsg;
        mpcActiveMsg->vBusy();
        vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
      }
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if ((mSm == cComNode::tenState::enStIdle) && (!mu8Lock))
    {
      bool lbStart;

      lbStart = true;

      if (mpcActiveMsg != NULL)
      {
        if (mpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        lbStart &= (!mcSlaves.isEmpty());
      }

      lbStart &= bCheckBus();

      if (lbStart)
      {
        mpcActiveSlave = (cComNode*)mcSlaves.cGetNext();
        mpcActiveSlave->mStatus.IsStartRequested = 0;
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
        return True;
      }
    }

    return False;
  }
};


class cComNodeDriverSlave : public cComNodeDriver
{
 public:
  cComDatMsg          *lpcActiveMsg;

  cComNode            *mpcComNode;


  cComNodeDriverSlave()
  {
  }


  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    if (mpcComNode != null)
    {
       vReInitHw(cComNode::enResetHwFull, False);
    }
  }

  void vSetMsg(cComDatMsg* lpcMsg)
  {
    lpcActiveMsg = lpcMsg;
  };


  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        lpcActiveMsg = lpcActiveMsg;
        lpcActiveMsg->vBusy();
        vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
      }
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (lpcActiveMsg != NULL)
      {
        if (lpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (mpcComNode != NULL)
      {
        if (!mpcComNode->mStatus.IsStartRequested)
        {
          lbStart = false;
        }
      }

      if (lbStart)
      {
        mpcComNode->mStatus.IsStartRequested = 0;
        mpcComNode->vComStart(cComNode::enEvPrepareToReceiveData);
      }
    }
    return True;
  }
};
#endif // __COMDAT_H__
