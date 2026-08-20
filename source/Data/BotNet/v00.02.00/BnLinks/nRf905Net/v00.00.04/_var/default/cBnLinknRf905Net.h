#ifndef __BOTNET_LINK_NRF905_NET_H__
#define __BOTNET_LINK_NRF905_NET_H__

#include "Typedef.h"
// Driver
#include "cnRF905Bn.h"

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

  void vClear()
  {
    mu16Counter = 0;
  }

  void vSet(u16 lu16Counter)
  {
    mu16Counter = lu16Counter;
  }

  bool isDone()
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


class cBotNet_ComLinknRf905Net
{
  public:

  typedef enum
  {
    StIdle = 0,
    StStart,

    StUpCfgStart,

    StUpStatusSendStart,
    StUpStatusWait,

    StUpPingSendStart,

    StUpSessionWaitCfgStart,
    StUpSessionWaitStart,
    StUpSessionWait,
    StUpSessionUpCfg,
    StUpSessionUp,

    StUpKeepReceiverOnWhileWaitingCfg,
    StUpKeepReceiverOnWhileWaiting,

    StUpEndError,
    StUpEndSession,
    StUpEnd,


    StDownWaitPingCfgStart,
    StDownWaitPingStart,
    StDownWaitPingDone,

    StDownSessionStartWait,
    StDownSessionCfgStart,
    StDownSessionStart,

    StDownSessionUpWait,
    StDownSessionUp,

    StDownEndError,
    StDownEndSession,
    StDownEnd
  }tenStates;


  typedef enum
  {
    EvDummy = 0,
    EvPingTimeOut,
    EvPingIntervall,
    EvLoop
  }tenEvents;


  typedef enum
  {
    CntTicksToms    = 10,
    Cnt60s          = 60000,
    Cnt1s           =  1000,
    Cnt50ms         =    50,

    MdNonSessionAdr = 1

  }tenConsts;

  cBotNet* mcBotnet;

  tenStates mSmRadio;

  cBotNet_ComLinknRf905Net(cBotNet* lcBotNet)
  {
    mcBotnet = lcBotNet;
  }
};

class cBotNet_UpLinknRf905Net:public cBotNet_ComLinknRf905Net, public cBotNet_UpLinkNrf905, public cEventHandler
{
  public:
  cNRF905BnSlave* mcNRF905;
  cDownCounter mcCntSendPingIntervall;
  cDownCounter mcCntPingTimeout;

  u16          mu16PingIntervall_10ms;
  u16          mu16PingTimeout_10ms;

  u8           mu8SendStatus;
  u8           mu8SendStatusIdx;
  u8           mu8SendStatusData[16];
  u8           mu8SendStatusPing[14];

  cComMsgS<u16, 30> mpcMsgSession;
  cComMsgS<u16, 1>  mpcMsgCfg;

  u8           mu8Counter1ms;

  u8           mu8KeepReceiverOnWhileWaiting;

  bool         mbFirstPingDone;
  bool         mbStartPing;

  cBotNet_UpLinknRf905Net(cNRF905BnSlave* lcNRF905, cBotNet* lcBotNet, bool lbStartPing = True)
     : cBotNet_ComLinknRf905Net(lcBotNet),
       cBotNet_UpLinkNrf905((cComNodeSlave*)lcNRF905),
       mcCntSendPingIntervall((u16)cBotNet_ComLinknRf905Net::EvPingIntervall),
       mcCntPingTimeout((u16)cBotNet_ComLinknRf905Net::EvPingTimeOut),
       mpcMsgSession(cComNode::tenDirection::enIsTx),
       mpcMsgCfg(cComNode::tenDirection::enCfgWrite)
  {
    mcNRF905 = lcNRF905;
    mu8KeepReceiverOnWhileWaiting = 0;
    vSetTiming(cBotNet_ComLinknRf905Net::Cnt60s, cBotNet_ComLinknRf905Net::Cnt50ms);
    mu8SendStatus = 0;
    mu8SendStatusIdx = 0;

    mbStartPing = lbStartPing;

    cMemTools::vMemSet(mu8SendStatusData, 0, 16);
    cMemTools::vMemSet(mu8SendStatusPing, 0, 14);
  }
  //
  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);

    cBotNet_UpLinkNrf905::vInit(False);

    mSmRadio = StIdle;

    mbFirstPingDone = False;

    mcNRF905->vSetRxAdressWidth(4);
    mcNRF905->vSetTxAdressWidth(4);

    if (mbStartPing)
    {
      //Startet das Senden des 1. Ping
      vStartPing();
    }
  }

  void vSetTiming(u32 lu32PingIntervall_ms, u32 lu32PingTimeout_ms)
  {
    if (lu32PingIntervall_ms > 650000) lu32PingIntervall_ms = 650000;
    if (lu32PingTimeout_ms   > 650000) lu32PingTimeout_ms   = 650000;
    if (lu32PingIntervall_ms < lu32PingTimeout_ms)  lu32PingTimeout_ms = lu32PingIntervall_ms / 2;

    mu16PingIntervall_10ms  = lu32PingIntervall_ms / cBotNet_ComLinknRf905Net::CntTicksToms;
    mu16PingTimeout_10ms    = lu32PingTimeout_ms   / cBotNet_ComLinknRf905Net::CntTicksToms;

    mcCntPingTimeout.vSet(mu16PingTimeout_10ms);
    mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms);
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

  bool bPut(u8 lu8PoolIdx) override
  {
    if (isSessionUp())
    {
      return cBotNet_UpLinkNrf905::bPut(lu8PoolIdx);
    }
    else
    {
      return False;
    }
  }

  u8 u8Get() override
  {
    if (isSessionUp())
    {
      return cBotNet_UpLinkNrf905::u8Get();
    }
    else
    {
      return 0;
    }
  }

  void vDoEvent(u32 lu32Event) override
  {
    cBotNet_ComLinknRf905Net::tenEvents lenEvent = (cBotNet_ComLinknRf905Net::tenEvents)lu32Event;

    switch (lenEvent)
    {
      case cBotNet_ComLinknRf905Net::EvPingTimeOut: // z.B. 50ms
        vSm(cComNode::tenEvent::enEvTimer);
        break;
      case cBotNet_ComLinknRf905Net::EvPingIntervall: // z.B. 15s
        if ((mSmRadio == cBotNet_ComLinknRf905Net::StIdle) &&
            (mcNRF905->cComNodeSlave::mSm == cComNode::tenState::enStIdle))
        {
          vStartPing();
        }
        else
        {
          vSm(cComNode::tenEvent::enEvTimer);
          mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms);
        }
        break;
      default:
        break;
    }
  }

  bool isReadyForSleep()
  {
    return (mbFirstPingDone);
  }


  void vTick(u16 lu16Time_ms) override
  {
    if (isSessionUp())
    {
      cBotNet_UpLinkNrf905::vTick(lu16Time_ms);

      // Verbindung verloren
      if (!mStatus.IsOnline)
      {
        // dann zurück in  Non-Session Mode
        mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
        mcNRF905->vStopCom();
        vStartPing();
      }
    }
    else
    {
      mu8Counter1ms += lu16Time_ms;

      if (mu8Counter1ms >= 10)
      {
        mcCntPingTimeout.vTick(this);
        mcCntSendPingIntervall.vTick(this);
        mu8Counter1ms -= 10;
      }
    }
  }

  bool isSessionUp()
  {
    return (mSmRadio == cBotNet_ComLinknRf905Net::StUpSessionUp);
  }



  void vStartPing()
  {
    if (mSmRadio == cBotNet_ComLinknRf905Net::StIdle)
    {
      vSm(cComNode::tenEvent::enEvStart);
    }
  }

  //void vPowerDown()
  //{
  //  mSmRadio = StAlive_UpWait;
  //  if (mu8KeepReceiverOnWhileWaiting)
  //  {
  //    mSmRadio = StAlive_UpWaitSessionStatusCfgStart;
  //  }
  //  bEventHandler(cNRF905Bn::NRF905_EvDummy);
  //}

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    cComNode::vComError(lenError, lenState);
    if (isSessionUp())
    {
      cBotNet_UpLinkNrf905::vComError(lenError, lenState);
    }

    if ((lenError == cComNode::enErTimeout) &&
        (lenState == cComNode::tenState::enStEndRx))
    {
      // Bei RxTimeout nichts machen weil Sync Timeout wird es machen
    }
    else
    {
      vResetStatusComFlags();
      mSmRadio = cBotNet_ComLinknRf905Net::StDownEndError;
      vSm(cComNode::tenEvent::enEvError);
    }
  }

  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    if (isSessionUp())
    {
      cBotNet_UpLinkNrf905::vSm(lenEvent);
    }
    else
    {
      vSm(lenEvent);
    }
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    if (isSessionUp())
    {
      cBotNet_UpLinkNrf905::vSm(cComNode::tenEvent::enEvDone);
    }
    else
    {
      vSm(cComNode::tenEvent::enEvDone);
    }
  }

  void vSm(cComNode::tenEvent lenEvent)
  {
    bool lbLoop = True;
    while (lbLoop)
    {
      lbLoop = False;

      switch(mSmRadio)
      {
        case cBotNet_ComLinknRf905Net::StIdle:
          if (lenEvent == cComNode::tenEvent::enEvStart)
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StStart;
            lbLoop = True;
          }
          break;
        case cBotNet_ComLinknRf905Net::StStart:
          mSmRadio = cBotNet_ComLinknRf905Net::StUpCfgStart;
          mcCntSendPingIntervall.vSet(mu16PingIntervall_10ms); // 15s
          mpcMsgSession.SetDir(cComNode::tenDirection::enIsTx);
          lbLoop = True;
          break;

        case cBotNet_ComLinknRf905Net::StUpCfgStart:
          if (mu8SendStatus == 1)
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StUpStatusSendStart;
          }
          else
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StUpPingSendStart;
          }

          vResetStatusFlags();

          mcNRF905->vSetRxAdress(0x00000000);
          mcNRF905->vSetTxAdress((u32)(MdNonSessionAdr << 16));

          mcNRF905->vSetTxPayloadWidth(25);
          mcNRF905->vSetRxPayloadWidth(25);

          mcNRF905->SetChannel_433(108, False);
          mcNRF905->SetCrc(16);
          mcNRF905->vStartMsg(&mpcMsgCfg);
          break;

        case cBotNet_ComLinknRf905Net::StUpStatusSendStart:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            u16 lu16BnAdr = mcBotnet->mcAdr.Get();
            u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

            mSmRadio = cBotNet_ComLinknRf905Net::StUpStatusWait;

            u8* lu8Tx = mpcMsgSession.Data();
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

            mpcMsgSession.Len(25);
            mcNRF905->vStartMsg(&mpcMsgSession);
          }
          break;


          case cBotNet_ComLinknRf905Net::StUpStatusWait:
            mSmRadio = cBotNet_ComLinknRf905Net::StUpPingSendStart;
            mcNRF905->vStartTimer(cNRF905Bn::NRF905_WAIT_TIMEOUT_1ms);
            break;


          case cBotNet_ComLinknRf905Net::StUpPingSendStart:
            {
              if (((lenEvent == cComNode::tenEvent::enEvTimer) && (mu8SendStatus == 1)) || // Timer, wenn vom Status gekommen
                  ((lenEvent == cComNode::tenEvent::enEvDone)  && (mu8SendStatus == 0)))    // Done, wenn ohne Status
              {
                mSmRadio = cBotNet_ComLinknRf905Net::StUpSessionWaitCfgStart;

                u16 lu16BnAdr = mcBotnet->mcAdr.Get();
                u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

                u8* lu8Tx = mpcMsgSession.Data();
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

                mpcMsgSession.Len(25);
                mcNRF905->vStartMsg(&mpcMsgSession);
              }
            }
            break;

          case cBotNet_ComLinknRf905Net::StUpSessionWaitCfgStart:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              mSmRadio = cBotNet_ComLinknRf905Net::StUpSessionWaitStart;

              u16 lu16BnAdr = mcBotnet->mcAdr.Get();
              u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

              mcNRF905->vSetRxAdress((((u32)lu16BnDId << 16) + (u32)lu16BnAdr));

              mcNRF905->SetChannel_433(123, False);
              mcNRF905->SetCrc(0);

              mcNRF905->vStartMsg(&mpcMsgCfg);
            }
            break;

          case cBotNet_ComLinknRf905Net::StUpSessionWaitStart:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              mSmRadio = cBotNet_ComLinknRf905Net::StUpSessionWait;

              // 50ms Timeout
              mcCntPingTimeout.vSet(mu16PingTimeout_10ms);

              mpcMsgSession.SetDir(cComNode::tenDirection::enIsRx);

              mcNRF905->vWaitMsg(&mpcMsgSession);
            }
            break;

          // Warten auf Antwort vom Master oder 50ms Timeout
          case cBotNet_ComLinknRf905Net::StUpSessionWait:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              u8* lu8Rx = mpcMsgSession.Data();

              if ((lu8Rx[0] == 0x11) &&
                  (lu8Rx[3] == 0x01))
              {
                mSmRadio = cBotNet_ComLinknRf905Net::StUpSessionUpCfg;

                vOnSync();
                mcCntPingTimeout.vClear();
                mcCntSendPingIntervall.vClear();
                mbFirstPingDone = True;

                u16 lu16BnAdr = (lu8Rx[22] << 8) + lu8Rx[23];
                u16 lu16BnDId = (lu8Rx[ 1] << 8) + lu8Rx[2];

                mcNRF905->vSetTxAdress((((u32)lu16BnDId << 16) + (u32)lu16BnAdr));

                mcNRF905->vStartMsg(&mpcMsgCfg);
              }
            }
            if (lenEvent == cComNode::tenEvent::enEvTimer) // 50ms
            {
              mcNRF905->vStopCom();
              if (mu8KeepReceiverOnWhileWaiting)
              {
                mSmRadio = cBotNet_ComLinknRf905Net::StUpKeepReceiverOnWhileWaitingCfg;
                mcNRF905->vSetRxAdress((u32)(MdNonSessionAdr << 16));

                mcNRF905->SetChannel_433(108, False);
                mcNRF905->SetCrc(16);
                mcNRF905->vStartMsg(&mpcMsgCfg);
              }
              else
              {
                mSmRadio = cBotNet_ComLinknRf905Net::StUpEnd;
                lbLoop = True;
              }
              mbFirstPingDone = True;
            }
            break;

          case cBotNet_ComLinknRf905Net::StUpSessionUpCfg:
            {
              mSmRadio = cBotNet_ComLinknRf905Net::StUpSessionUp;
              cBotNet_UpLinkNrf905::vInit(True);
            }


          case cBotNet_ComLinknRf905Net::StUpSessionUp:
            break;

          case cBotNet_ComLinknRf905Net::StUpKeepReceiverOnWhileWaitingCfg:
            mSmRadio = cBotNet_ComLinknRf905Net::StUpKeepReceiverOnWhileWaiting;
            mpcMsgSession.SetDir(cComNode::tenDirection::enIsRx);
            mcNRF905->vWaitMsg(&mpcMsgSession);
            break;

          case cBotNet_ComLinknRf905Net::StUpKeepReceiverOnWhileWaiting:
            if (lenEvent == cComNode::tenEvent::enEvDone) // Irgendwas empfangen
            {
              u8 mu8PoolIdxRx = 0;
              cBotNetMsg_Base lcMsg; cBnMsgPool::vReqMsg(lcMsg, mu8PoolIdxRx, 25);

              if (mu8PoolIdxRx)
              {
                lcMsg.Len(25);
                cBnMsgPool::vSetLen(mu8PoolIdxRx, lcMsg.muLen);

                lcMsg.Set(mpcMsgSession);
                mcBotnet->vDecodeMsgType(mu8PoolIdxRx);
                cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
              }
              mcNRF905->vWaitMsg(&mpcMsgSession);
            }
            if (lenEvent == cComNode::tenEvent::enEvTimer) // 15s Timeout
            {
              mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
              mcNRF905->vResetCom();
              vStartPing();
            }
            break;

          case cBotNet_ComLinknRf905Net::StUpEndSession:
          case cBotNet_ComLinknRf905Net::StUpEndError:
            mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
            cBotNet_UpLinkNrf905::vResetCom(True);
            cBotNet_UpLinkNrf905::vInit(False);
            vStartPing();
            break;

          case cBotNet_ComLinknRf905Net::StUpEnd:  // 15s Timeout
            // Wird vom 15s Timer neu gestartet.
            mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
            cBotNet_UpLinkNrf905::vResetCom(True);
            cBotNet_UpLinkNrf905::vInit(False);
            mcNRF905->vResetCom();
            mcNRF905->vPowerDown(); // Power Down
            break;

          default:
            break;
      }
    }
  }
};


class cBotNet_DownLinknRf905Net:public cBotNet_ComLinknRf905Net, public cBotNet_DownLinkNrf905, public cEventHandler
{
  public:
  cNRF905BnMaster* mcNRF905;

  bool         mbSessionStart;
  bool         mbSessionStop;

  u16          mu16SessionDAdr;
  u16          mu16SessionBnAdr;
  u16          mu16SessionKanal;

  cComMsgS<u16, 30> mpcMsgSession;
  cComMsgS<u16, 1>  mpcMsgCfg;

  u8           mu8Counter1ms;

  u8           mu8KeepReceiverOnWhileWaiting;

  bool         mbFirstPingDone;

  cBotNet_DownLinknRf905Net(cNRF905BnMaster* lcNRF905, cBotNet* lcBotNet)
     : cBotNet_ComLinknRf905Net(lcBotNet),
       cBotNet_DownLinkNrf905((cNRF905BnMaster*)lcNRF905),
       mpcMsgSession(cComNode::tenDirection::enIsTx),
       mpcMsgCfg(cComNode::tenDirection::enCfgWrite)
  {
    mcNRF905 = lcNRF905;
  }
  //
  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
    cBotNet_DownLinkNrf905::vInit(False);

    mbFirstPingDone = False;

    mu16SessionDAdr  = 0;
    mu16SessionBnAdr = 0;
    mu16SessionKanal = 0;
    mbSessionStart   = False;
    mbSessionStop    = False;

    mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
  }

  bool bPut(u8 lu8PoolIdx) override
  {
    if (isSessionUp())
    {
      return cBotNet_DownLinkNrf905::bPut(lu8PoolIdx);
    }
    else
    {
      return False;
    }
  }

  bool isReadyForSleep()
  {
    return False;
  }

  void vStopSession()
  {
    vResetStatusFlags();
    cBotNet_DownLinkNrf905::vResetCom(True);
    cBotNet_DownLinkNrf905::vInit(False);
    mcNRF905->vResetCom();
    mSmRadio = cBotNet_ComLinknRf905Net::StIdle;
    vSm(cComNode::tenEvent::enEvDummy);
  }

  void vStartWaitPing()
  {
    mSmRadio = cBotNet_ComLinknRf905Net::StDownWaitPingCfgStart;
    vSm(cComNode::tenEvent::enEvDummy);
  }

  void vTick(u16 lu16Time_ms) override
  {
    if (isSessionUp())
    {
      // Gab es einen Session Stop request oder
      // Sync Timeout ?
      if ((mbSessionStop == True) || (!mStatus.IsOnline))
      {
        mbSessionStop = False;
        mSmRadio = cBotNet_ComLinknRf905Net::StDownEndSession;
        vSm(cComNode::tenEvent::enEvDummy);
      }
      else
      {
        cBotNet_DownLinkNrf905::vTick(lu16Time_ms);
      }
    }
    else
    {
      mControl.StartRequest = 1;
    }
  }

  bool isSessionUp()
  {
    return (mSmRadio == cBotNet_ComLinknRf905Net::StDownSessionUp);
  }


  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override // __attribute__((optimize("-O0")))
  {
    cComNode::vComError(lenError, lenState);
    if (isSessionUp())
    {
      cBotNet_DownLinkNrf905::vComError(lenError, lenState);
    }

    if ((lenError == cComNode::enErTimeout) &&
        (lenState == cComNode::tenState::enStEndRx))
    {
      // Bei RxTimeout nichts machen weil Sync Timeout wird es machen
    }
    else
    {
      vResetStatusComFlags();
      mSmRadio = cBotNet_ComLinknRf905Net::StDownEndError;
      vSm(cComNode::tenEvent::enEvError);
    }
  }

  // ComStart wird vor Start der Communication aufgerufen, z.B. nach adress match
  void vComStart(cComNode::tenEvent lenEvent) override // __attribute__((optimize("-O0")))
  {
    if (isSessionUp())
    {
      cBotNet_DownLinkNrf905::vSm(lenEvent);
    }
    else
    {
      vSm(lenEvent);
    }
  }

  void  vComDone() override  // __attribute__((optimize("-O0")))
  {
    if (isSessionUp())
    {
      cBotNet_DownLinkNrf905::vSm(cComNode::tenEvent::enEvDone);
    }
    else
    {
      vSm(cComNode::tenEvent::enEvDone);
    }
  }

  void vSync() override
  {
    mControl.StartRequest = 1;
  }

  void vDoEvent(u32 lu32Event) override
  {
    cBotNet_ComLinknRf905Net::tenEvents lenEvent = (cBotNet_ComLinknRf905Net::tenEvents)lu32Event;

    switch (lenEvent)
    {
      case cBotNet_ComLinknRf905Net::EvPingTimeOut: // z.B. 50ms
        mSmRadio = cBotNet_ComLinknRf905Net::StDownEndError;
        vSm(cComNode::tenEvent::enEvError);
        break;
      default:
        break;
    }
  }

  void vSm(cComNode::tenEvent lenEvent)
  {
    bool lbLoop = True;
    while (lbLoop)
    {
      lbLoop = False;

      switch(mSmRadio)
      {
        case cBotNet_ComLinknRf905Net::StIdle:
          if ((lenEvent == cComNode::tenEvent::enEvStart) ||
              (lenEvent == cComNode::tenEvent::enEvPrepareToSendData))
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StDownWaitPingCfgStart;
            lbLoop = True;
          }
          break;
        case cBotNet_ComLinknRf905Net::StDownWaitPingCfgStart:
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StDownWaitPingStart;

            vResetStatusFlags();

            mcNRF905->mstNRF905.mstConfig.RxAdress = (u32)(MdNonSessionAdr << 16);
            mcNRF905->mstNRF905.mui32TxAdress = 0x00000000;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

            mcNRF905->SetChannel_433(108, False);
            mcNRF905->SetCrc(16);

            mcNRF905->vStartMsg(&mpcMsgCfg);
          }
          break;

        case cBotNet_ComLinknRf905Net::StDownWaitPingStart:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StDownWaitPingDone;
            mpcMsgSession.SetDir(cComNode::tenDirection::enIsRx);
            mpcMsgSession.Len(25);
            mcNRF905->vWaitMsg(&mpcMsgSession);
          }
          break;

        case cBotNet_ComLinknRf905Net::StDownWaitPingDone:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            u8 lu8PoolIdx;
            cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, 30);

            if (lu8PoolIdx)
            {
              lcMsgTx.Set(mcNRF905->mstNRF905.mui8RxPayLoad, 25);
              cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.Len());
              cBnMsgPool::vPutMsg(lu8PoolIdx);
              mcRxComBuf.put(lu8PoolIdx);
              cBnMsgPool::vReleaseMsg(lu8PoolIdx);
            }

            if (mbSessionStart)
            {
              // Überprüfen,obes eine Session Start Nachricht ist und zu meiner Session passt
              // DH DL: Destination Device ID
              // KH KL: Kanal
              // Info:  11 | DH DL | 00 | KH.KL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
              // Start: 11 | DH DL | 01 | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || 80 00 00 SA SA 80
              // Info Nachricht ?
              u8* lu8Rx = mpcMsgSession.Data();
              if ((lu8Rx[24] == 0x80) &&
                  (lu8Rx[0]  == 0x11))
              {
                //Start
                if (lu8Rx[3] == 0x00)
                {
                  u16 lu16BnAdr     = (u16)(((u16)lu8Rx[22] << 8) + (u16)lu8Rx[23]);
                  u16 lu16DeviceAdr = (u16)(((u16)lu8Rx[1]  << 8) + (u16)lu8Rx[2]);

                  if (lu16DeviceAdr == mu16SessionDAdr)
                  {
                    cBotNetAdress lcReceivedAdress(lu16BnAdr);
                    cBotNetAdress lcTargetAdress(mu16SessionBnAdr);

                    if ((lcReceivedAdress == lcTargetAdress) ||
                        (lcReceivedAdress.isMasterOf(lcTargetAdress)))
                    {
                      mu16SessionBnAdr = lu16BnAdr;
                      mu16SessionKanal = (u16)(((u16)lu8Rx[4] << 8) + (u16)lu8Rx[5]);
                      mbSessionStart   = False;
                      mSmRadio = cBotNet_ComLinknRf905Net::StDownSessionStartWait;
                      lbLoop = True;
                      break;
                    }
                  }
                }
              }
            }
            mSmRadio = cBotNet_ComLinknRf905Net::StDownWaitPingStart;
            lbLoop = True;
          }
          break;

        case cBotNet_ComLinknRf905Net::StDownSessionStartWait:
          mSmRadio = cBotNet_ComLinknRf905Net::StDownSessionCfgStart;
          mcNRF905->mTimer.vStart(cNRF905Bn::NRF905_WAIT_TIMEOUT_1ms);
          break;


        case cBotNet_ComLinknRf905Net::StDownSessionCfgStart:
          if (lenEvent == cComNode::tenEvent::enEvTimer)
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StDownSessionStart;

            u16 lu16BnAdr = mcBotnet->mcAdr.Get();
            u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

            mcNRF905->mstNRF905.mstConfig.RxAdress = (((u32)lu16BnDId << 16) + (u32)lu16BnAdr);
            mcNRF905->mstNRF905.mui32TxAdress = (mu16SessionDAdr << 16) + mu16SessionBnAdr;

            mcNRF905->mstNRF905.mstConfig.Tx_Payload_Width = 25;
            mcNRF905->mstNRF905.mstConfig.Rx_Payload_Width = 25;

            mcNRF905->SetChannel_433(mu16SessionKanal, False);
            mcNRF905->SetCrc(0);

            mcNRF905->vStartMsg(&mpcMsgCfg);
          }
          break;

        case cBotNet_ComLinknRf905Net::StDownSessionStart:
          if (lenEvent == cComNode::tenEvent::enEvDone)
          {
            mSmRadio = cBotNet_ComLinknRf905Net::StDownSessionUpWait;

            mcNRF905->mTimer.vStop();

            u16 lu16BnAdr = mcBotnet->mcAdr.Get();
            u16 lu16BnDId = mcBotnet->mcDeviceID.Get();

            u8* lu8Tx = mpcMsgSession.Data();

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

            mpcMsgSession.Len(25);
            mpcMsgSession.SetDir(cComNode::tenDirection::enIsTx);
            mcNRF905->vStartMsg(&mpcMsgSession);
          }
          break;


          case cBotNet_ComLinknRf905Net::StDownSessionUpWait:
            if (lenEvent == cComNode::tenEvent::enEvDone)
            {
              mSmRadio = cBotNet_ComLinknRf905Net::StDownSessionUp;
              cBotNet_DownLinkNrf905::vInit(True);
              mcBotnet->vLinkDynSyncReset(0);
              vOnSync();
              lbLoop = True;
            }
            break;

          case cBotNet_ComLinknRf905Net::StDownSessionUp:
            break;

          case cBotNet_ComLinknRf905Net::StDownEndSession:
          case cBotNet_ComLinknRf905Net::StDownEndError:
          case cBotNet_ComLinknRf905Net::StDownEnd:
            vStopSession();
            break;

          default:
            break;
      }
    }
  }
};




#endif // __BOTNET_LINK_NRF905_H__
