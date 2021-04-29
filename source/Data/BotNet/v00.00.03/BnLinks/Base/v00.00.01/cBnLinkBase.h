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

class cBotNet;

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
  cBotNetAdress mcSAdr;  // Source and Destination Adresses for communication
  cBotNetAdress mcDAdr;  // e.g. in Master-Slave communication
                         //      Master is always Source and Slave is Dest
                         //      SAdr and DAdr will be set depending if this Node is Master or Slave
                         //      DownLink you are Master, Uplink you are Slave

  cBotNet *mcBotnet;

  u8 mu8MsgCntTx;
  u8 mu8MsgCntRx;
  u8 mu8MsgCntRx_Last;

  cBotNet_LinkBase(uint16 lui16Adr, u32 lu32RxComBufSize, u32 lu32TxComBufSize);

  virtual void vAddedToBn(cBotNet* lcBotNet, cBotNet_LinkBase::tenType lenType);

  virtual ~cBotNet_LinkBase()
  {
  }

  virtual void vSync()
  {
  }

  virtual void vResetCom(bool bAlways)
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
  //  M: 4Bit: MessageCounter
  inline void vCreateSync(u8* lpu8Dest, u8 lu8Bussy, u8 lu8DataCntTx)
  {
    mu8MsgCntTx &= 0x0F;
    uint8 lu8Checksum = (lu8DataCntTx + lu8Bussy + mu8MsgCntTx + 1) & 0x7;

    lpu8Dest[0] = lu8DataCntTx;
    lpu8Dest[1] = (uint8)((lu8Bussy << 7) + (lu8Checksum << 4) + mu8MsgCntTx);
  }

  inline void vUpdateSync(u8* lpu8Dest, u8 lu8Bussy)
  {
    lpu8Dest[1] &= 127;
    lpu8Dest[1] |= (uint8)(lu8Bussy << 7);
  }

  bool IsSyncCheckOk(u8* lpu8Source)
  {
    uint8 lu8Bussy      =  lpu8Source[1] >> 7;
    uint8 lu8ChecksumRx = (lpu8Source[1] >> 4) & 0x7;
    uint8 lu8MsgCntRx   =  lpu8Source[1] & 0x7;
    uint8 lu8Checksum   = (lpu8Source[0] + lu8Bussy + lu8MsgCntRx + 1) & 0x7;

    mu8MsgCntRx = lpu8Source[1] & 0xF;

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


  inline cBotNetAdress cGetDAdr() {return mcDAdr;};
  inline cBotNetAdress cGetSAdr() {return mcSAdr;};

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

  u16  mu16SyncCounter;

  cBotNet_SyncedLinkBase(uint16 lui16Adr, u32 lu32RxComBufSize, u32 lu32TxComBufSize)
    : cBotNet_LinkBase(lui16Adr, lu32RxComBufSize, lu32TxComBufSize)
  {
    vResetCounter();
  }

  void vResetCounter()
  {
    mu16SyncCounter = 0;
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

  virtual void vTick10ms() override
  {
    if (IsOnline())
    {
      if (mu16SyncCounter < 10) // 100ms
      {
        mu16SyncCounter++;
      }
      else
      {
        vOnEnterOffline();
      }
    }
    else
    {
      mu16SyncCounter = 0;
    }
  }
};


#endif // __BOTNET_LINK_BASE_H__
