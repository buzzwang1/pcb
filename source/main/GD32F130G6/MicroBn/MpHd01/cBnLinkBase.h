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


class cBotNet_LinkBase: public cComNode
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

  cBotNetAdress mcMyAdr;
  cBotNetAdress mcDAdr;


  u8 mu8MsgCntTx;
  u8 mu8MsgCntRx;
  u8 mu8MsgCntRx_Last;

  cBotNet_LinkBase(u16 lu16MyAdr, u16 lu16DAdr)
  {
    mcMyAdr.Set(lu16MyAdr);
    mcDAdr.Set(lu16DAdr);

    mu8MsgCntTx      = 0;
    mu8MsgCntRx      = 0;
    mu8MsgCntRx_Last = 0xFF;
  }

  virtual void vSync()      {}
  virtual void vTick1ms()   {}

  // Reset Buffer of Offline Links
  virtual void vResetCom(bool bAlways)
  {
    if ((!IsOnline()) || (bAlways))
    {
      vResetStatusFlags();

      mcRxComBuf.reset();
      mcTxComBuf.reset();
    }
  }



  // Sync Bytes setzen
  // DDDD DDDD BCCC MMMM
  //  D: 8Bit: Anzahl Daten zu senden 0..64
  //  B: 1Bit: Bussy Flag:
  //              0: kann Daten emmpfangen
  //              1: kann keine Daten empfangen
  //  C: 3Bit: Checksumme: D + M + B + 1
  //  R: 1Bit: Reduced Message Request, always 1
  //  M: 3Bit: MessageCounter
  inline void vCreateSync(u8* lpu8Dest, u8 lu8Bussy, u8 lu8DataCntTx, u8 lu8ChecksumOffset = 0)
  {
    uint8 lu8Checksum = (lu8DataCntTx + lu8Bussy + mu8MsgCntTx + lu8ChecksumOffset + 1) & 0x7;
    lpu8Dest[0] = lu8DataCntTx;
    lpu8Dest[1] = (uint8)((lu8Bussy << 7) + (lu8Checksum << 4) + 0x8 + (mu8MsgCntTx & 7));
  }

  /*inline void vCreateSync(u8* lpu8Dest, u8 lu8Bussy, u8 lu8DataCntTx)
  {
    vCreateSync(lpu8Dest, lu8Bussy, lu8DataCntTx, 0);
  }*/

  inline void vUpdateSync(u8* lpu8Dest, u8 lu8Bussy)
  {
    lpu8Dest[1] &= 127;
    lpu8Dest[1] |= (uint8)(lu8Bussy << 7);
  }

  bool IsSyncCheckOk(u8* lpu8Source, u8 lu8ChecksumOffset = 0)
  {
    uint8 lu8Bussy      =  lpu8Source[1] >> 7;
    uint8 lu8ChecksumRx = (lpu8Source[1] >> 4) & 0x7;
    uint8 lu8MsgCntRx   =  lpu8Source[1] & 0x7;
    uint8 lu8Checksum   = (lpu8Source[0] + lu8Bussy + lu8MsgCntRx + lu8ChecksumOffset + 1) & 0x7;

    return (lu8Checksum == lu8ChecksumRx);
  }



  inline cBotNetAdress cGetDAdr()  {return mcDAdr;};
  inline cBotNetAdress cGetMyAdr() {return mcMyAdr;};

  virtual void put(cbArrayExtT<uint16>* lcEntry)
  {
    if (IsOnline())
    {
      if (mcTxComBuf.isFitting())
      {
        mcTxComBuf.put(lcEntry);
      }
    }
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

  u16  mu16OnlineCounter;
  u16  mu16OnlineTimeout;

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

  cBotNet_SyncedLinkBase(u16 lu16MyAdr, u16 lu16DAdr)
    : cBotNet_LinkBase(lu16MyAdr, lu16DAdr)
  {
    vResetCounter();
    mu16OnlineTimeout  = 10; // 100 ms;
  }

  void vResetCounter()
  {
    mu16OnlineCounter = 0;
  }

  void vSetOnlineTimeout(u16 lu16Timeout_ms)
  {
    mu16OnlineTimeout = lu16Timeout_ms / 10;
  }

  virtual void vOnSync()
  {
    mStatus.IsOnline = True;
    vResetCounter();
  }

  virtual void vOnEnterOffline()
  {
    vResetStatusFlags();
  }

  virtual void vTick10ms()
  {
    if (IsOnline())
    {
      if (mu16OnlineCounter < 60000) // 60000 x 10msTicks = 600.000ms = 600s
      {
        mu16OnlineCounter++;
      }

      // Eine Art von Watchdog
      // Wurde seit 200ms kein vOnSync aufgerufen, wird der Link deaktiviert
      if (mu16OnlineCounter == mu16OnlineTimeout) // 200ms
      {
        vOnEnterOffline();
      }
    }
  }
};


#endif // __BOTNET_LINK_BASE_H__
