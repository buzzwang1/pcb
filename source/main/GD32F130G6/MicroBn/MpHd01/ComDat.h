#ifndef __COMDAT_H__
#define __COMDAT_H__

#include "Typedef.h"
#include "cbArrayExtT.h"

class __attribute__((packed)) cComNode
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
  }tstStatus;

  tstStatus mStatus;
  u16       mMyAdr;

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
    mStatus.IsStartRequested = 0;
  }


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
    enTx  = 0,
    enRx
  }tenDirection;

  tenDirection                enDirection;
  cbArrayExtStaticT<u16>      cData;


  cComDatMsg(uint8* maui8bArray, u16 lu16Size, tenDirection lenDirection)
    : cData(maui8bArray, lu16Size)
  {
    enDirection = lenDirection;
  }
  void vSet(cbArrayExtT<uint16> &lcData)      {cData.Set(lcData);}
};


#endif // __COMDAT_H__
