#ifndef __BOTNET_LINK_BASE_H__
#define __BOTNET_LINK_BASE_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

#ifdef __GNUC__
class cBotNet_LinkBase: public cComNode
#else
class cBotNet_LinkBase: public cComNode
#endif
{
public:

  typedef enum
  {
    enCnstMaxData = cBotNet_MsgSize + 1, // 64 (Max Message Lenght) + 1 (Protokoll Byte)
    enCnstMaxDataPlusCheckSum = enCnstMaxData
  }tenConsts;

  typedef enum
  {
    enUpLink = 0,
    enDownLink,
    enSideLink,
    enOpenLink
  }tenType;

  cComBuf  mcRxComBuf;
  cComBuf  mcTxComBuf;

  cBotNetAdress mcAdr;   // My Adress
                         // Bei Uplink, d.h. es ist ein Slave, steht hier die meine Adresse drin
                         // Bei Downlink steht die Dest-Adresse drin, also die Slave-Adresse

  u8 mu8MsgCntTx;
  u8 mu8MsgCntRx;
  u8 mu8MsgCntRx_Last;

  cBotNet_LinkBase::tenType menType;

  cBotNet_LinkBase(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
    : mcRxComBuf(lu32RxComBufSize), mcTxComBuf(lu32TxComBufSize)
  {
    mu8MsgCntTx      = 0;
    mu8MsgCntRx      = 0;
    mu8MsgCntRx_Last = 0xFF;
    menType          = lenType;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    mcAdr.Set(lu16Adr);
    mAdr = mcAdr.GetIdx();
    return True;
  }

  virtual ~cBotNet_LinkBase() {}

  bool isUpLink()   {return (menType == enUpLink);}
  bool isDownLink() {return (menType == enDownLink);}
  bool isSideLink() {return (menType == enSideLink);}

  virtual void vSync()                           {}
  virtual void vTick1ms()                        {}
  virtual void vReInit(bool lbHandleHardware)    {UNUSED(lbHandleHardware); vResetCom(True);}

  // Reset Buffer of Offline Links
  void vResetCom(bool bAlways)
  {
    if ((!IsOnline()) || (bAlways))
    {
      vResetStatusFlags();

      mu8MsgCntTx      = 0;
      mu8MsgCntRx      = 0;
      mu8MsgCntRx_Last = 0xFF;

      mcRxComBuf.reset();
      mcTxComBuf.reset();
    }
  }

  virtual uint32 SizeOf()
  {
    return 0;
  }

  // Sync Bytes setzen
  // DDDD DDDD BCCC MMMM
  //  D: 8Bit: Anzahl Daten zu senden 0..64
  //  B: 1Bit: Bussy Flag:
  //              0: kann Daten emmpfangen
  //              1: kann keine Daten empfangen
  //  C: 3Bit: Checksumme: D + M + B + 1
  //  R: 1Bit: Reduced Message Request
  //  M: 3Bit: MessageCounter
  inline void vCreateSync(u8* lpu8Dest, u8 lu8DataCntTx, u8 lu8ReducedMessage, u8 lu8ChecksumOffset = 0)
  {
    mu8MsgCntTx &= 0x07;
    u8 lu8Busy = mStatus.IsBusy; 
    uint8 lu8Checksum = (lu8DataCntTx + lu8Busy + mu8MsgCntTx + lu8ReducedMessage + lu8ChecksumOffset + 1) & 0x7;
    lpu8Dest[0] = lu8DataCntTx;
    lpu8Dest[1] = (uint8)((lu8Busy << 7) + (lu8Checksum << 4) + (lu8ReducedMessage << 3) + mu8MsgCntTx);
  }


  inline void vUpdateSync(u8* lpu8Dest, u8 lu8Bussy)
  {
    lpu8Dest[1] &= 127;
    lpu8Dest[1] |= (uint8)(lu8Bussy << 7);
  }

  bool IsSyncCheckOk(u8* lpu8Source, u8 lu8ChecksumOffset = 0)
  {
    u8 lu8Busy       =  lpu8Source[1] >> 7;
    u8 lu8ChecksumRx = (lpu8Source[1] >> 4) & 0x7;
    u8 lu8MsgReduced = (lpu8Source[1] >> 3) & 1;
    u8 lu8MsgCntRx   =  lpu8Source[1] & 0x7;
    u8 lu8Checksum   = (lpu8Source[0] + lu8Busy + lu8MsgCntRx + lu8MsgReduced + lu8ChecksumOffset + 1) & 0x7;

    mu8MsgCntRx = lpu8Source[1] & 0x7;

    return (lu8Checksum == lu8ChecksumRx);
  }

  bool IsSyncNewData()
  {
    if (mu8MsgCntRx_Last != mu8MsgCntRx)
    {
      mu8MsgCntRx_Last = mu8MsgCntRx;
      return True;
    }
    return False;
  }

  bool IsSyncReducedMsgRequest(u8* lpu8Source)
  {
    return (lpu8Source[1] & 0x08);
  }

  virtual bool bPut(cbArrayExtT<uint16>* lcEntry)
  {
    if (IsOnline())
    {
      if (mcTxComBuf.isFitting(lcEntry))
      {
        mcTxComBuf.put(lcEntry);
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

  virtual u16 u16Get(cbArrayExtT<uint16>* lcEntry)
  {
    mcRxComBuf.get(lcEntry);
    return lcEntry->Len();
  }

  virtual void vTick10ms()
  {}
};


class cBotNet_SyncedLinkBase : public cBotNet_LinkBase
{
public:

  // Online Counter/Überwachung
  // 200ms kein vOnSync aufgerufen, dann wird vOnEnterOffline
  u16  mu16NoSyncCounter;
  u16  mu16NoSyncTimeout;

  // Offline Counter
  // 10s nach vOnEnterOffline wird vOnOffline10s aufgerufen
  u16  mu16OfflineCounter;

  // ResetCom Counter
  // 300ms nach vOnEnterOffline werden die ComBuffer reseted und vOnResetCom aufgerufen
  u16  mu16ResetComCounter;

  typedef enum
  {
    enStIdle = 0,
    enStSync,
    enStSyncWait,
    enStSyncWaitForSlave,
    enStSyncWaitForTimer,
    enStSyncDone,
    enStData,
    enStDataWait,
    enStDataWaitRx,
    enStDataWaitTx,
    enStDataWaitForSlave,
    enStDataWaitForTimer,
    enStDataDone,
    enStDataDoneTx,
    enStDataDoneRx,
    enStAck,
    enStAckWait,
    enStAckWaitForSlave,
    enStAckWaitForTimer,
    enStAckDone,
    enStAckDoneTx,
    enStAckDoneRx,
    enStWaitForSlave,
    enStEnd,
    enStEndWait,
    enStEndWaitForTimer
  }tenStates;

  cBotNet_SyncedLinkBase(u32 lu32RxComBufSize, u32 lu32TxComBufSize, cBotNet_LinkBase::tenType lenType)
    : cBotNet_LinkBase(lu32RxComBufSize, lu32TxComBufSize, lenType)
  {
    vInit();
  }

  void vInit()
  {
    mu16NoSyncCounter = 0;
    mu16OfflineCounter = 0;
    mu16ResetComCounter = 30;

    vSetOnlineTimeout(200);  // 200 ms;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    return cBotNet_LinkBase::bAddedToBn(lu16Adr);
  }

  virtual void vReInit(bool lbHandleHardware)
  {
    cBotNet_LinkBase::vReInit(lbHandleHardware);
    vInit();
  }

  void vSetOnlineTimeout(u16 lu16Timeout_ms)
  {
    mu16NoSyncTimeout = lu16Timeout_ms / 10;
  }

  virtual void vOnSync()
  {
    mStatus.IsOnline = True;
    mu16NoSyncCounter = 0;
    mu16ResetComCounter = 0;
  }

  virtual void vOnEnterOffline()
  {
    vResetStatusFlags();
  }

  virtual void vOnResetCom()
  {
  }


  virtual void vOnOffline10s()
  {
  }

  virtual void vTick10ms()
  {
    if (IsOnline())
    {
      mu16OfflineCounter = 0;
      mu16ResetComCounter = 0;

      if (mu16NoSyncCounter < 60000) // 60000 x 10msTicks = 600.000ms = 600s
      {
        mu16NoSyncCounter++;
      }

      // Eine Art von Watchdog
      // Wurde seit 200ms kein vOnSync aufgerufen, wird der Link deaktiviert
      if (mu16NoSyncCounter == mu16NoSyncTimeout) // 200ms
      {
        vOnEnterOffline();
      }
    }
    else
    {
      mu16NoSyncCounter = 0;

      if (mu16ResetComCounter < 60000) 
      {
        mu16ResetComCounter++;
      }
      
      if (mu16ResetComCounter == 30) // 300ms
      {
        //GPIOA->BSRR = 1;
        vResetCom(False);
        vOnResetCom();
        //GPIOA->BRR = 1;
      }

      if (mu16OfflineCounter < 60000) // 60000 x 10msTicks = 600.000ms = 600s
      {
        mu16OfflineCounter++;
      }

      if (mu16OfflineCounter == 1000)
      {
        vOnOffline10s();
      }
    }
  }

  void vSyncStart()
  {
    // Falls ein Sync gestarted wurde, damit nicht ein
    // Reset dem Sync in die Quere kommt, den Reset counter zurücksetzten
    if (mu16ResetComCounter < 30)
    {
      mu16ResetComCounter = 0;
    }
  }
};


#endif // __BOTNET_LINK_BASE_H__
