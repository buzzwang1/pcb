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

class cBotNet;

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
    EvPingPowerOnTimeOut,
    EvPingIntervall,
    EvLoop
  }tenEvents;


  typedef enum
  {
    CntTicksToms    = 10,
    Cnt60s          = 60000,
    Cnt1s           =  1000,

    MdNonSessionAdr = 1

  }tenConsts;

  cNRF905* mcNRF905;
  cBotNet_ComLinknRf905* mcLinkNRF905;
  cBotNet* mcBotnet;

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

  cBotNet_ComLinknRf905Net(cBotNet_ComLinknRf905* lcLinkNRF905, cBotNet_LinkBase::tenType lenType, cBotNet* lcBotNet);

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_SyncedLinkBase::vReInit(lbHandleHardware);
    mcLinkNRF905->vReInit(lbHandleHardware);
    mStatus.IsInit = mcLinkNRF905->mStatus.IsInit;
  }

  void vOnResetCom() override
  {
    mcLinkNRF905->vResetCom(False);
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

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    cBotNet_SyncedLinkBase::bAddedToBn(lu16Adr);
    bool lbRet = mcLinkNRF905->bAddedToBn(lu16Adr);
    mStatus.IsInit   = mcLinkNRF905->mStatus.IsInit;
    return lbRet;
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
  cDownCounter mcCntSendPingIntervall;
  cDownCounter mcCntPingTimeout;
  cBotNet_UpLinknRf905* mcUpLinknRf905;

  u8           mu8SendStatus;
  u8           mu8SendStatusIdx;
  u8           mu8SendStatusData[16];
  u8           mu8SendStatusPing[14];

  u8           mu8KeepReceiverOnWhileWaiting;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_MsgProt     mcMsgRx;

  u16          mu16PingIntervall_10ms;
  u16          mu16PingTimeout_10ms;

  u16          mu16StartUpDelay_10ms;

  cBotNet_UpLinknRf905Net(cBotNet_UpLinknRf905* lcUpLinknRf905, cBotNet* lcBotNet, u16 lu16StartUpDelay_10ms = cBotNet_ComLinknRf905Net::Cnt1s)
    #ifdef cNRF905_SYNC_DEBUG_NET
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905, cBotNet_LinkBase::enUpLink, lcBotNet),
       mcCntSendPingIntervall((u16)cBotNet_ComLinknRf905Net::EvPingIntervall),
       mcCntPingTimeout((u16)cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut),
       mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn)
    #else
     : cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905, cBotNet_LinkBase::enUpLink, lcBotNet),
       mcCntSendPingIntervall((u16)cBotNet_ComLinknRf905Net::EvPingIntervall),
       mcCntPingTimeout((u16)cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut),
       mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn)
    #endif
  {
    mpcMsgData.vMemAlloc(enCnstMaxData, enCnstMaxData);
    mcUpLinknRf905 = lcUpLinknRf905;
    mu16StartUpDelay_10ms = lu16StartUpDelay_10ms;
    vInit();
  }

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_ComLinknRf905Net::vReInit(lbHandleHardware);
    mcUpLinknRf905->vReInit(lbHandleHardware);
    vInit();
  }

  void vInit()
  {
    mSmMain = StIdle;
    mSmRadio = StIdle;

    mu8SendStatus = 0;
    mu8SendStatusIdx = 0;
    mu8KeepReceiverOnWhileWaiting = 0;

    cMemTools::vMemSet(mu8SendStatusData, 0, 16);
    cMemTools::vMemSet(mu8SendStatusPing, 0, 14);


    vSetTiming(cBotNet_ComLinknRf905Net::Cnt60s, cBotNet_ComLinknRf905Net::Cnt1s);

    if (mu16StartUpDelay_10ms)
    {
      mcCntPingTimeout.vSet(mu16PingTimeout_10ms);
    }
    else
    {
      //Startet das Senden des 1. Ping
      vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
    }
  }

  void vSetStartUpDelay(u32 lu32StartUpDelay_ms)
  {
    if (lu32StartUpDelay_ms > 650000) lu32StartUpDelay_ms = 650000;

    mu16StartUpDelay_10ms = lu32StartUpDelay_ms / CntTicksToms;
  }

  void vSetTiming(u32 lu32PingIntervall_ms, u32 lu32PingTimeout_ms)
  {
    if (lu32PingIntervall_ms > 650000) lu32PingIntervall_ms = 650000;
    if (lu32PingTimeout_ms   > 650000) lu32PingTimeout_ms   = 650000;
    if (lu32PingIntervall_ms < lu32PingTimeout_ms)  lu32PingTimeout_ms = lu32PingIntervall_ms / 2;

    mu16PingIntervall_10ms  = lu32PingIntervall_ms / CntTicksToms;
    mu16PingTimeout_10ms    = lu32PingTimeout_ms   / CntTicksToms;
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

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vComStart(cComNode::tenEvent lenEvent)
  {
    (void)lenEvent;
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
          mcCntPingTimeout.vSet(mu16PingTimeout_10ms);
          mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms);
          vSendPing();
          mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWaitPing;
          lenEvent = cBotNet_ComLinknRf905Net::EvLoop;
          break;


        case cBotNet_ComLinknRf905Net::StAlive_UpWaitPing:
          if (lenEvent == cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut)
          {
            mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWait;
            vPowerDown();
          }
          break;

        case cBotNet_ComLinknRf905Net::StAlive_UpWait:
          if (lenEvent == cBotNet_ComLinknRf905Net::EvPingIntervall)
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
    cBotNet_SyncedLinkBase::vResetCom(True);
    mcUpLinknRf905->vResetCom(True);
    mSmMain = cBotNet_ComLinknRf905Net::StIdle;
    vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
  }

  void vTick10ms() override
  {
    cBotNet_SyncedLinkBase::vTick10ms();
    mcUpLinknRf905->vTick10ms();
    mcCntPingTimeout.vTick(this);
    mcCntSendPingIntervall.vTick(this);
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
  cDownCounter mcCntSendPingIntervall;
  cDownCounter mcCntPingTimeout;
  cBotNet_DownLinknRf905* mcDownLinknRf905;

  cBotNet_DownLinknRf905Net(cBotNet_DownLinknRf905* lcUpLinknRf905, cBotNet* lcBotNet)
    #ifdef cNRF905_SYNC_DEBUG_NET
     : mcUart(9600, USART1_IRQn, GPIO_AF_7, 128, 128),
       cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*) lcUpLinknRf905, cBotNet_LinkBase::enDownLink, lcBotNet),
       mcCntSendPingIntervall((u16)cBotNet_ComLinknRf905Net::EvPingIntervall),
       mcCntPingTimeout((u16)cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut)
    #else
     : cBotNet_ComLinknRf905Net((cBotNet_ComLinknRf905*)lcUpLinknRf905, cBotNet_LinkBase::enDownLink, lcBotNet),
       mcCntSendPingIntervall((u16)cBotNet_ComLinknRf905Net::EvPingIntervall),
       mcCntPingTimeout((u16)cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut)
    #endif
  {
    mcDownLinknRf905 = lcUpLinknRf905;
    mpcMsgData.vMemAlloc(enCnstMaxData,  enCnstMaxData);

    vInit();
  }

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_ComLinknRf905Net::vReInit(lbHandleHardware);
    mcDownLinknRf905->vReInit(lbHandleHardware);
    vInit();
  }

  void vInit()
  {
    mSmMain = StIdle;
    mSmRadio = StIdle;

    mcCntPingTimeout.vSet((u16)(Cnt1s / CntTicksToms));
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

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vComStart(cComNode::tenEvent lenEvent)
  {
    (void)lenEvent;
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

  void vTick10ms() override
  {
    //cBotNet_SyncedLinkBase::vTick10ms(); // Not needed. Timeout handling from
                                           // mcDownLinknRf905 is used

    mcDownLinknRf905->vTick10ms();

    mcCntPingTimeout.vTick(this);
    mcCntSendPingIntervall.vTick(this);

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
