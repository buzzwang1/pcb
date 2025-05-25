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

#include "cBotnet.h"
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

  cBotNet_ComLinknRf905Net(cBotNet_ComLinknRf905* lcLinkNRF905, cBotNet_LinkBase::tenType lenType, cBotNet* lcBotNet)
    : cBotNet_SyncedLinkBase(0/*lu32RxComBufSize*/, 0/*lu32TxComBufSize*/, lenType)
  {
    mu16SessionDAdr = 0;
    mu16SessionBnAdr = 0;
    mu16SessionKanal = 0;
    mbSessionStart = False;
    mbSessionStop = False;

    mcLinkNRF905 = lcLinkNRF905;
    mcNRF905 = mcLinkNRF905->mcNRF905;
    mcBotnet = lcBotNet;
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

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
    vResetStatusComFlags();
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    (void)lenEvent;
  }

  void  vComDone() override
  {
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

  bool         mbFirstPingDone;

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

  void vInit()
  {
    mSmMain = StIdle;
    mSmRadio = StIdle;

    mu8SendStatus = 0;
    mu8SendStatusIdx = 0;
    mu8KeepReceiverOnWhileWaiting = 0;
    mbFirstPingDone = False;

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

    mcCntPingTimeout.vSet(mu16PingTimeout_10ms);
    mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms);
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
          // Wird andesprungen nach Idle(Startup) oder nachdem
          // SendPingIntervall abgelaufen ist
          mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWaitPing;
          lenEvent = cBotNet_ComLinknRf905Net::EvLoop;

          // Ping senden und Ping-Timeout und Ping-Intervall starten
          mcCntPingTimeout.vSet(mu16PingTimeout_10ms);
          mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms);
          vSendPing();
          break;


        case cBotNet_ComLinknRf905Net::StAlive_UpWaitPing:
          // Ping wurde gesendet
          //  - wenn keine Antwort kommt => Timeout und warten bis Ping-Intervall abläuft
          //  - wenn  eine Antwort kommt => bEventHandler setzt StRemoteUp
          if (lenEvent == cBotNet_ComLinknRf905Net::EvPingPowerOnTimeOut)
          {
            mSmMain = cBotNet_ComLinknRf905Net::StAlive_UpWait;
            mbFirstPingDone = True;
            vPowerDown();
          }
          break;

        case cBotNet_ComLinknRf905Net::StAlive_UpWait:
          // Warten bis Ping - Intervall abgelaufen ist. Dann
          // wieder einen Ping senden
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

  bool isReadyForSleep()
  {
    return (mbFirstPingDone);
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

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent)
  {
    // im Remote Modus weiterleiten zum Uplink
    if (mSmMain == cBotNet_ComLinknRf905Net::StRemoteUp)
    {
      // Setzt dem Timer zurück
      vDoEvent(cBotNet_ComLinknRf905Net::EvDummy);
      return mcUpLinknRf905->bEventHandler(lenEvent);
    }
    else
    {
      do
      {
        if (lenEvent == cNRF905::NRF905_EvLoop)
        {
          lenEvent = cNRF905::NRF905_EvDummy;
        }

        if (!mcNRF905->bEventHandler(lenEvent))
        {
          switch(mSmRadio)
          {
            case StAlive_UpSendPingCfgStart:
              mSmRadio = StAlive_UpSendPingCfgStart_Done;

              vResetStatusFlags();
              mcUpLinknRf905->vResetCom(True);

              mcNRF905->mstNRF905.mstConfig.RxAdress = 0x00000000;
              mcNRF905->mstNRF905.mui32TxAdress      = (u32)(MdNonSessionAdr << 16);

              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

              mcNRF905->SetChannel_433(108, False);
              mcNRF905->SetCrc(16);

              mcNRF905->bStartUpdateCfg();
              break;

            case StAlive_UpSendPingCfgStart_Done:
              {
                mSmRadio = StAlive_UpSendPingTxStart;

                if (mu8SendStatus)
                {
                  mSmRadio = StAlive_UpSendStatusTxStart;
                }

                u32 u32Delay = mcNRF905->u32Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_TX);
                if (u32Delay)
                {
                  mcNRF905->mTimer.vStart(u32Delay);
                }
                else
                {
                  lenEvent = cNRF905::NRF905_EvLoop;
                }
              }
              break;

            case StAlive_UpSendStatusTxStart:
              {
                mSmRadio = StAlive_UpSendStatusTxStart_Done;
                mcNRF905->mTimer.vStop();

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;
                //Bn-Msg-Header
                *lu8Tx++ = 0x11;
                //Bn-Msg
                *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress

                *lu8Tx++ = mu8SendStatusIdx; // SubId
                cMemTools::vMemCpy(lu8Tx, mu8SendStatusData, 16);
                lu8Tx += 16;

                //Bn-Frame
                *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
                *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
                *lu8Tx++ = 0x80;

                mcNRF905->vStartTransmit();
              }
              break;

            case StAlive_UpSendStatusTxStart_Done:
              mSmRadio = StAlive_UpSendPingTxStart;
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_25ms);
              break;


            case StAlive_UpSendPingTxStart:
              {
                mSmRadio = StAlive_UpSendPingTxStart_Done;
                mcNRF905->mTimer.vStop();

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;
                //Bn-Msg-Header
                *lu8Tx++ = 0x11;
                //Bn-Msg
                *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress
                *lu8Tx++ = 0x00; // SubId
                *lu8Tx++ = (123 >> 8); *lu8Tx++ = (123 & 0xFF); //Channel
                cMemTools::vMemCpy(lu8Tx, mu8SendStatusPing, 14);
                lu8Tx += 14;

                //Bn-Frame
                *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
                *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
                *lu8Tx++ = 0x80;

                mcNRF905->vStartTransmit();
              }
              break;

            case StAlive_UpSendPingTxStart_Done:
              {
                mSmRadio = StAlive_UpWaitPing;

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                mcNRF905->mstNRF905.mstConfig.RxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);

                mcNRF905->SetChannel_433(123, False);
                mcNRF905->SetCrc(0);

                mcNRF905->bStartUpdateCfg();
                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
              }
              break;

            case StAlive_UpWaitPing:
              mSmRadio = StAlive_UpWaitSessionStart;
              mcNRF905->mTimer.vStop();
              mcNRF905->vStartReceive();
              break;

            case StAlive_UpWaitSessionStart:
              if ((mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 25) &&
                  (mcNRF905->mstNRF905.mui8RxPayLoad[0] == 0x11) &&
                  (mcNRF905->mstNRF905.mui8RxPayLoad[3] == 0x01))
              {
                vOnSync();
                mbFirstPingDone = True;

                mSmRadio = StAlive_UpWaitSessionSync_DoneAck;

                u16 lu16BnAdr = (mcNRF905->mstNRF905.mui8RxPayLoad[22] << 8) +
                                 mcNRF905->mstNRF905.mui8RxPayLoad[23];
                u16 lu16BnDId = (mcNRF905->mstNRF905.mui8RxPayLoad[1] << 8) +
                                 mcNRF905->mstNRF905.mui8RxPayLoad[2];

                mcNRF905->mstNRF905.mui32TxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);
                mcNRF905->SetCrc(0);
                mcNRF905->bStartUpdateCfg();
              }
              break;

            case StAlive_UpWaitSessionSync_DoneAck:
              {
                mSmRadio = StRemoteUp;
                mSmMain  = StRemoteUp;
                mcUpLinknRf905->vSlaveStartSync();
              }
              break;

            case StAlive_UpWaitSessionSync_DoneNack:
              mSmRadio = StAlive_UpWait;

              if (mu8KeepReceiverOnWhileWaiting)
              {
                mSmRadio = StAlive_UpWaitSessionStatusCfgStart;
              }

              lenEvent = cNRF905::NRF905_EvLoop;
              break;

            case StRemoteUp:
              break;

            case StAlive_UpWaitSessionStatusCfgStart:
              {
                mSmRadio = StAlive_UpWaitSessionStatusCfgDone;

                mcNRF905->mstNRF905.mstConfig.RxAdress = (u32)(MdNonSessionAdr << 16);

                mcNRF905->SetChannel_433(108, False);
                mcNRF905->SetCrc(16);

                mcNRF905->bStartUpdateCfg();
                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
              }
              break;
            case StAlive_UpWaitSessionStatusCfgDone:
              mSmRadio = StAlive_UpWaitStatusStart;
              mcNRF905->mTimer.vStop();
              lenEvent = cNRF905::NRF905_EvLoop;
              break;
            case StAlive_UpWaitStatusStart:
              mSmRadio = StAlive_UpWaitStatusDone;
              mcNRF905->vStartReceive();
              break;
            case StAlive_UpWaitStatusDone:
              cMemTools::vMemCpy(mcMsgRx.mpu8Data, mcNRF905->mstNRF905.mui8RxPayLoad, 25);
              mcMsgRx.muiLen = 25;
              mcMsgRx.vDecode(0, 0);
              mcBotnet->bCallMsgHandler(mcMsgRx);
              mcNRF905->vStartReceive();
              break;

            case StAlive_UpWait:
              mcNRF905->u32Start_Set_Mode(cNRF905::NRF905_RADIO_MODE_PWR_DOWN);
              break;

            default:
              break;
          }
        }
      } while (lenEvent == cNRF905::NRF905_EvLoop);

      return False;
    }
  }
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

  bool bEventHandler(cNRF905::tenCmdEvents lenEvent)
  {
    // im Remote Modus weiterleiten zum Uplink
    if (mSmMain == cBotNet_ComLinknRf905Net::StRemoteDown)
    {
      return mcDownLinknRf905->bEventHandler(lenEvent);
    }
    else
    {
      do
      {
        if (lenEvent == cNRF905::NRF905_EvLoop)
        {
          lenEvent = cNRF905::NRF905_EvDummy;
        }

        if (!mcNRF905->bEventHandler(lenEvent))
        {
          switch(mSmRadio)
          {
            case StAlive_DownWaitPingCfgStart:
              mSmRadio = StAlive_DownWaitPingCfgStart_Done;

              mcDownLinknRf905->vResetCom(True);

              mcNRF905->mstNRF905.mstConfig.RxAdress = (u32)(MdNonSessionAdr << 16);
              mcNRF905->mstNRF905.mui32TxAdress      = 0x00000000;

              mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
              mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

              mcNRF905->SetChannel_433(108, False);
              mcNRF905->SetCrc(16);

              mcNRF905->bStartUpdateCfg();
              break;

            case StAlive_DownWaitPingCfgStart_Done:
              mcNRF905->mTimer.vStop();
              mSmRadio = StAlive_DownWaitPingRx;
              mcNRF905->vStartReceive();
              break;

            case StAlive_DownWaitPingRx:
              mSmRadio = StAlive_DownWaitPingCfgStart_Done;

              if (mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width == 25)
              {
                cMemTools::vMemCpy(mpcMsgData.cRxData.mpu8Data, mcNRF905->mstNRF905.mui8RxPayLoad, 25);
                mpcMsgData.cRxData.muiLen = 25;
                mcDownLinknRf905->mcRxComBuf.put(&mpcMsgData.cRxData);
              }

              if (mbSessionStart)
              {
                // Überprüfen,obes eine Session Start Nachricht ist und zu meiner Session passt
                // DH DL: Destination Device ID
                // KH KL: Kanal
                // Info:  11 | DH DL | 00 | KH.KL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
                // Start: 11 | DH DL | 01 | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
                if (mpcMsgData.cRxData.muiLen == 25)
                {
                  // Info Nachricht ?
                  if ((mpcMsgData.cRxData[24] == 0x80) &&
                      (mpcMsgData.cRxData[0]  == 0x11))
                  {
                    //Start
                    if (mpcMsgData.cRxData[3] == 0x00)
                    {
                      u16 lu16BnAdr     = (u16)(((u16)mpcMsgData.cRxData[22] << 8) + (u16)mpcMsgData.cRxData[23]);
                      u16 lu16DeviceAdr = (u16)(((u16)mpcMsgData.cRxData[1]  << 8) + (u16)mpcMsgData.cRxData[2]);

                      if (lu16DeviceAdr == mu16SessionDAdr)
                      {
                        cBotNetAdress lcReceivedAdress(lu16BnAdr);
                        cBotNetAdress lcTargetAdress(mu16SessionBnAdr);

                        if ((lcReceivedAdress == lcTargetAdress) ||
                            (lcReceivedAdress.isMasterOf(lcTargetAdress)))
                        {
                          mu16SessionBnAdr = lu16BnAdr;
                          mu16SessionKanal = (u16)(((u16)mpcMsgData.cRxData[4] << 8) + (u16)mpcMsgData.cRxData[5]);
                          mbSessionStart   = False;
                          mSmRadio = StAlive_DownSendSessionStart_Init;
                        }
                      }
                    }
                  }
                }
              }

              lenEvent = cNRF905::NRF905_EvLoop;
              break;

            case StAlive_DownSendSessionStart_Init:
              {
                mSmRadio = StAlive_DownSendSessionStart_Init2;

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                mcNRF905->mstNRF905.mstConfig.RxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);
                mcNRF905->mstNRF905.mui32TxAdress      = (mu16SessionDAdr << 16) + mu16SessionBnAdr;

                mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
                mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

                mcNRF905->SetChannel_433(mu16SessionKanal, False);
                mcNRF905->SetCrc(0);

                mcNRF905->bStartUpdateCfg();
                //Timeout setzen
                mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_10ms);
              }
              break;

            case StAlive_DownSendSessionStart_Init2:
              mSmRadio = StAlive_DownSendSessionStart_Start;
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_1ms * 5);
              break;

            case StAlive_DownSendSessionStart_Start:
              {
                mSmRadio = StAlive_DownSendSessionStart_Wait;

                mcNRF905->mTimer.vStop();

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                u8* lu8Tx = mcNRF905->mstNRF905.mui8TxPayLoad;

                //Bn-Msg-Header
                *lu8Tx++ = 0x11;

                //Bn-Msg
                *lu8Tx++ = lu16BnDId >> 8; *lu8Tx++ = lu16BnDId & 0xFF; //Device Adress
                *lu8Tx++ = 0x01; // Start
                cMemTools::vMemSet(lu8Tx, 0, 16);
                lu8Tx += 16;

                //Bn-Frame
                *lu8Tx++ = 0x00;           *lu8Tx++ = 0x00;
                *lu8Tx++ = lu16BnAdr >> 8; *lu8Tx++ = lu16BnAdr & 0xFF;
                *lu8Tx++ = 0x80;

                mcNRF905->vStartTransmit();
              }
              break;

            case StAlive_DownSendSessionStart_Wait:
              mSmRadio = StAlive_DownSendSessionStart_Done;
              //Kurz Warten, damit Slave umschalten kann
              mcNRF905->mTimer.vStart(cNRF905::NRF905_WAIT_TIMEOUT_1ms * 5);
              break;

            case StAlive_DownSendSessionStart_Done:
              vOnSync();

              mSmRadio = StRemoteDown;
              mSmMain  = StRemoteDown;
              lenEvent = cNRF905::NRF905_EvLoop;
              break;

            case StRemoteDown:
              break;

            default:
              break;
          }
        }
      } while (lenEvent == cNRF905::NRF905_EvLoop);

      return False;
    }
  }
};


#endif // __BOTNET_LINK_NRF905_H__
