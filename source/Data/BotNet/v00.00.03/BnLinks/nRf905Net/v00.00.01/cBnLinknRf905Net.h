#ifndef __BOTNET_LINK_NRF905_NET_H__
#define __BOTNET_LINK_NRF905_NET_H__

#include "Typedef.h"
// Driver
#include "cnRF905.h"

#ifdef cNRF905_SYNC_DEBUG_NET
  #include "stdlib.h"
  #include "cUsart.h"
#endif

// Decomment for Debug
//#define cNRF905_SYNC_DEBUG_NET

#include "cBnLinkBase.h"
#include "cBnLinknRf905.h"


class cEventHandler
{
  public:
  virtual void vDoEvent(u32 lu32Event) = 0;
};

class cDownCounter
{
  u16 mu16Event;
  u16 mu16Counter;

  public:

  cDownCounter(u16 lu16Event)
  {
    mu16Event = lu16Event;
  }

  inline void vSet(u16 lu16Counter)
  {
    mu16Counter = lu16Counter;
  }

  inline bool isDone()
  {
    return (mu16Counter==0);
  }

  void vTick(cEventHandler *lcCb)
  {
    if (!isDone())
    {
      mu16Counter--;
      if (isDone())
      {
        lcCb->vDoEvent(mu16Event);
      }
    }
  }
};


class cBotNet_ComLinknRf905Net: public cBotNet_SyncedLinkBase
{
  public:

  typedef enum
  {
    StIdle = 0,

    // Alive States
    StAlive_UpSendPing,
    StAlive_UpSendPingCfgStart,
    StAlive_UpSendPingCfgStart_Done,
    StAlive_UpSendStatusTxStart,
    StAlive_UpSendStatusTxStart_Done,
    StAlive_UpSendPingTxStart,
    StAlive_UpSendPingTxStart_Done,
    StAlive_UpWaitPing,
    StAlive_UpWaitSessionStart,
    StAlive_UpWaitSessionSync_DoneAck,
    StAlive_UpWaitSessionSync_DoneNack,
    StAlive_UpWaitSessionStatusCfgStart,
    StAlive_UpWaitSessionStatusCfgDone,
    StAlive_UpWaitStatusStart,
    StAlive_UpWaitStatusDone,

    StAlive_UpWait,
    StRemoteUp,

    StAlive_DownWaitPing,
    StAlive_DownWaitPingCfgStart,
    StAlive_DownWaitPingCfgStart_Done,
    StAlive_DownWaitPingRx,
    StAlive_DownSendSessionStart_Init,
    StAlive_DownSendSessionStart_Init2,
    StAlive_DownSendSessionStart_Start,
    StAlive_DownSendSessionStart_Wait,
    StAlive_DownSendSessionStart_Done,
    StRemoteDown,

  }tenStates;


  typedef enum
  {
    EvDummy = 0,
    Ev1s,
    Ev60s,
    EvLoop
  }tenEvents;


  typedef enum
  {
    CntTicksToms    = 10,
    Cnt60s          = 60000 / CntTicksToms,
    Cnt1s           =  1000 / CntTicksToms,

    MdNonSessionAdr = 1

  }tenConsts;

  cNRF905* mcNRF905;
  cBotNet_ComLinknRf905* mcLinkNRF905;

  tenStates mSmMain;
  tenStates mSmRadio;

  u16 mu16RxIdx;
  u16 mu16TxIdx;

  #ifdef cNRF905_SYNC_DEBUG_NET
    tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcUart;
  #endif

  cComDatMsg mpcMsgData;

  u16                    mu16SessionDAdr;
  u16                    mu16SessionBnAdr;
  u16                    mu16SessionKanal;
  bool                   mbSessionStart;
  bool                   mbSessionStop;

  cBotNet_ComLinknRf905Net(cBotNet_ComLinknRf905* lcLinkNRF905)
    : cBotNet_SyncedLinkBase(0, 0/*lu32RxComBufSize*/, 0/*lu32TxComBufSize*/)
  {
    mu16SessionDAdr  = 0;
    mu16SessionBnAdr = 0;
    mu16SessionKanal = 0;
    mbSessionStart   = False;
    mbSessionStop    = False;

    mcLinkNRF905 = lcLinkNRF905;
    mcNRF905     = mcLinkNRF905->mcNRF905;

    mcAdr  = lcLinkNRF905->mcAdr;
    mcDAdr = lcLinkNRF905->mcDAdr;
    mcSAdr = lcLinkNRF905->mcSAdr;
  }

  void vResetCom(bool bAlways) override
  {
    mcLinkNRF905->vResetCom(bAlways);
    mStatus.IsInit = mcLinkNRF905->mStatus.IsInit;
  }

  bool bPut(cbArrayExtT<uint16>* lcEntry) override
  {
    return mcLinkNRF905->bPut(lcEntry);
  }

  u16 u16Get(cbArrayExtT<uint16>* lcEntry) override
  {
    return mcLinkNRF905->u16Get(lcEntry);
  }

  void vAddedToBn(cBotNet* lcBotNet, cBotNet_LinkBase::tenType lenType) override
  {
    cBotNet_LinkBase::vAddedToBn(lcBotNet, lenType);
    mcLinkNRF905->vAddedToBn(lcBotNet, lenType);

    mStatus.IsInit   = mcLinkNRF905->mStatus.IsInit;
  }

  void vOnSync() override
  {
    cBotNet_SyncedLinkBase::vOnSync();
    mcLinkNRF905->vOnSync();
  }
};

class cBotNet_UpLinknRf905Net:public cBotNet_ComLinknRf905Net, public cEventHandler
{
  public:
  cDownCounter mcCnt60s;
  cDownCounter mcCnt1s;
  cBotNet_UpLinknRf905* mcUpLinknRf905;

  u8           mu8SendStatus;
  u8           mu8SendStatusIdx;
  u8           mu8SendStatusData[16];
  u8           mu8SendStatusPing[14];

  u8           mu8KeepReceiverOnWhileWaiting;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_MsgProt     mcMsgRx;


  cBotNet_UpLinknRf905Net(cBotNet_UpLinknRf905* lcUpLinknRf905)
    #ifdef cNRF905_SYNC_DEBUG_NET
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905),
       mcCnt60s((u16)cBotNet_ComLinknRf905Net::Ev60s),
       mcCnt1s((u16)cBotNet_ComLinknRf905Net::Ev1s),
       mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn)
    #else
     : cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905),
       mcCnt60s((u16)cBotNet_ComLinknRf905Net::Ev60s),
       mcCnt1s((u16)cBotNet_ComLinknRf905Net::Ev1s),
       mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn)
    #endif
  {
    mSmMain  = StIdle;
    mSmRadio = StIdle;

    mcUpLinknRf905 = lcUpLinknRf905;
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    mcCnt1s.vSet((u16)cBotNet_ComLinknRf905Net::Cnt1s);

    mu8SendStatus = 0;
    mu8SendStatusIdx = 0;
    mu8KeepReceiverOnWhileWaiting = 0;

    cMemTools::vMemSet(mu8SendStatusData, 0, 16);
    cMemTools::vMemSet(mu8SendStatusPing, 0, 14);
  }

  virtual uint32 SizeOf()
  {
    /*uint32 lui32Size;

    lui32Size  = sizeof(cBotNet_DownLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();

    lui32Size -= 3 * sizeof(cComDatMsg);*/

    return 0; //lui32Size;
  }

  void vComError(cComNode::tenComNodeError lenError, tenComNodeState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vComStart(tenComNodeStartState lenState)
  {
    (void)lenState;
  }

  void  vComDone()
  {
  }

  void vSetStatus(u8 lu8StatusID, u8* lpu8StatusData)
  {
    mu8SendStatus = 1;
    mu8SendStatusIdx  = lu8StatusID;

    cMemTools::vMemCpy(mu8SendStatusData, lpu8StatusData, 16);
  }

  void vSetPingData(u8* lpu8PingData)
  {
    cMemTools::vMemCpy(mu8SendStatusPing, lpu8PingData, 14);
  }

  void vKeepReceiverOnWhileWaiting(bool lbOn)
  {
    if (lbOn) mu8KeepReceiverOnWhileWaiting = 1;
         else mu8KeepReceiverOnWhileWaiting = 0;
  }

  void vDoEvent(u32 lu32Event) override
  {
    volatile tenEvents lenEvent = (tenEvents)lu32Event;

    do
    {
      if (lenEvent == cBotNet_ComLinknRf905Net::EvLoop)
      {
        lenEvent = cBotNet_ComLinknRf905Net::EvDummy;
      }

      switch(mSmMain)
      {
        case cBotNet_ComLinknRf905Net::StIdle:
          mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpSendPing;
          lenEvent = cBotNet_ComLinknRf905Net::EvLoop;
          break;

        case cBotNet_ComLinknRf905Net::StAlive_UpSendPing:
          mcCnt1s.vSet((u16)cBotNet_ComLinknRf905Net::Cnt1s);
          mcCnt60s.vSet((u16)cBotNet_ComLinknRf905Net::Cnt60s);
          vSendPing();
          mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWaitPing;
          lenEvent = cBotNet_ComLinknRf905Net::EvLoop;
          break;


        case cBotNet_ComLinknRf905Net::StAlive_UpWaitPing:
          if (lenEvent == cBotNet_ComLinknRf905Net::Ev1s)
          {
            mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWait;
            vPowerDown();
          }
          break;

        case cBotNet_ComLinknRf905Net::StAlive_UpWait:
          if (lenEvent == cBotNet_ComLinknRf905Net::Ev60s)
          {
            mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpSendPing;
            lenEvent = cBotNet_ComLinknRf905Net::EvLoop;
          }
          break;
        case cBotNet_ComLinknRf905Net::StRemoteUp:
            cBotNet_SyncedLinkBase::vOnSync();
          break;

        default:
          break;
      }
    } while (lenEvent == cBotNet_ComLinknRf905Net::EvLoop);
  }

  virtual void vOnEnterOffline() override
  {
    cBotNet_SyncedLinkBase::vResetStatusFlags();
    mSmMain = cBotNet_ComLinknRf905Net::StIdle;
    vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
  }

  void vTick10ms()
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    mcUpLinknRf905->vTick10ms();
    mcCnt1s.vTick(this);
    mcCnt60s.vTick(this);
  }

  void vSendPing()
  {
    mSmRadio = StAlive_UpSendPingCfgStart;
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  void vPowerDown()
  {
    mSmRadio = StAlive_UpWait;
    if (mu8KeepReceiverOnWhileWaiting)
    {
      mSmRadio = StAlive_UpWaitSessionStatusCfgStart;
    }
    bEventHandler(cNRF905::NRF905_EvDummy);
  }

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent);
};


class cBotNet_DownLinknRf905Net:public cBotNet_ComLinknRf905Net, public cEventHandler
{
  public:
  cDownCounter mcCnt60s;
  cDownCounter mcCnt1s;
  cBotNet_DownLinknRf905* mcDownLinknRf905;

  cBotNet_DownLinknRf905Net(cBotNet_DownLinknRf905* lcUpLinknRf905)
    #ifdef cNRF905_SYNC_DEBUG_NET
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*) lcUpLinknRf905),
       mcCnt60s((u16)cBotNet_ComLinknRf905Net::Ev60s),
       mcCnt1s((u16)cBotNet_ComLinknRf905Net::Ev1s)
    #else
     : cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905),
       mcCnt60s((u16)cBotNet_ComLinknRf905Net::Ev60s),
       mcCnt1s((u16)cBotNet_ComLinknRf905Net::Ev1s)
    #endif
  {
    mSmMain  = StIdle;
    mSmRadio = StIdle;

    mcDownLinknRf905 = lcUpLinknRf905;
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    mcCnt1s.vSet((u16)cBotNet_ComLinknRf905Net::Cnt1s);
  }

  virtual uint32 SizeOf()
  {
    /*uint32 lui32Size;

    lui32Size  = sizeof(cBotNet_DownLinkI2c);

    lui32Size += mcRxComBuf.SizeOf();
    lui32Size += mcTxComBuf.SizeOf();

    lui32Size -= 2 * sizeof(cComBuf);

    lui32Size += mpcMsgSync.SizeOf();
    lui32Size += mpcMsgData.SizeOf();
    lui32Size += mpcMsgAck.SizeOf();

    lui32Size -= 3 * sizeof(cComDatMsg);*/

    return 0; //lui32Size;
  }

  void vComError(cComNode::tenComNodeError lenError, tenComNodeState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vComStart(tenComNodeStartState lenState)
  {
    (void)lenState;
  }

  void  vComDone()
  {
  }

  void vDoEvent(u32 lu32Event) override
  {
    volatile tenEvents lenEvent = (tenEvents)lu32Event;

    do
    {
      if (lenEvent == cBotNet_ComLinknRf905Net::EvLoop)
      {
        lenEvent = cBotNet_ComLinknRf905Net::EvDummy;
      }

      switch(mSmMain)
      {
        case cBotNet_ComLinknRf905Net::StIdle:
          mSmMain = cBotNet_ComLinknRf905Net::StAlive_DownWaitPing;
          lenEvent = cBotNet_ComLinknRf905Net::EvLoop;
          break;

        case cBotNet_ComLinknRf905Net::StAlive_DownWaitPing:
          vStartWaitPing();
          break;

        case cBotNet_ComLinknRf905Net::StRemoteDown:
          break;

        default:
          break;
      }
    } while (lenEvent == cBotNet_ComLinknRf905Net::EvLoop);
  }


  void vStopSession()
  {
    vResetStatusFlags();

    mSmMain = StIdle;
    mSmRadio = StIdle;
    mStatus.IsOnline = false;

    vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
  }


  void vOnEnterOffline() override
  {
    // Do nothing here
    // This is done by mcDownLinknRf905
  }

  void vTick10ms()
  {
    //cBotNet_SyncedLinkBase::vTick10ms(); // Not needed. Timeout handling from
                                           // mcDownLinknRf905 is used

    mcDownLinknRf905->vTick10ms();

    mcCnt1s.vTick(this);
    mcCnt60s.vTick(this);

    if ((mbSessionStop == True) ||
       (!mcDownLinknRf905->IsOnline()))
    {
      mbSessionStop = False;
      if (IsOnline())
      {
        vStopSession();
      }
    }

    if (IsOnline())
    {
      mcDownLinknRf905->vMasterStartSync();
    }
  }

  void vStartWaitPing()
  {
    mSmRadio = StAlive_DownWaitPingCfgStart;
    bEventHandler(cNRF905::NRF905_EvDummy);
  }


  bool bEventHandler(cNRF905::tenCmdEvents lenEvent);
};


#endif // __BOTNET_LINK_NRF905_H__
