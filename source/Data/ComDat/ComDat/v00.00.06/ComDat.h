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
    enErChecksum,
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
    u8 IsInit           : 1;
    u8 IsOnline         : 1;
    u8 IsStartRequested : 1;
    u8 IsError          : 1;
    u8 IsBusy           : 1;

    u8 IsFirstTx :  1;
    u8 IsThisTxOk : 1;
    u8 IsLastTxOk : 1;
  }tstStatus;

  tstStatus mStatus;
  u16       mAdr;

  cComNode()
  {
    mStatus.IsInit = 0;
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
  inline virtual bool IsInitAndOnline() {return (IsInit() && IsOnline());}

  inline virtual void vSetOnline()      { mStatus.IsOnline = 1; }
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
    : cTxData(), cRxData()
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

    while (lu16Cnt > 3)
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

  #ifdef _WIN32
    void vMemFree()
    {
      cTxData.~cbArrayExtDynT();
      cTxData.~cbArrayExtDynT();
      vFree();
    }
  #endif // _WIN32

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

  #ifdef _WIN32
    ~cComDatMsg() { vMemFree(); }
  #endif // _WIN32
};


class cComNodeList
{
  public:

  uint32         ui32Size;
  uint32         ui32Used;
  uint32         ui32Pos;
  cComNode     **mpacNodes;

  cComNodeList(uint32 lui32NodesCnt);

  #ifdef _WIN32
    ~cComNodeList();
  #endif // _WIN32

  uint32 SizeOf();

  void   vResetPos();

  bool   isEmpty();
  bool   isSlotFree();

  void   vStartAll();
  void   vSetErrorAll(cComNode::tenError lenError, cComNode::tenState lenState);

  uint32 ui32GetFreeSlot();
  uint32 ui32GetSlot(cComNode *lpcNode);
  void vAdd(cComNode *lpcNode);
  void vRemove(cComNode *lpcNode);
  cComNode* cGetNext();
};



#endif // __COMDAT_H__
