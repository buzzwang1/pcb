#ifndef __BOTNET_STREAM_SYS_PORT_BASE_H__
#define __BOTNET_STREAM_SYS_PORT_BASE_H__

#include "Typedef.h"

#include "cRingBufT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

class cBotnet;

class cBotNetStreamPort
{
  public:

  cBotNetAdress mcBnDestAdr;     // == 0 keine externe Connection, != 0 externe Connecion. In dem Fall wird noch der Port Idx benötigt
  uint8         mcBnDestPortIdx;

  char8         mszBufName[24];
  cStr          mszName;

  uint8         mcRxBuf[cBotNet_StreamSysPortRingBufSize];
  uint8         mcTxBuf[cBotNet_StreamSysPortRingBufSize];
  cRingBufT<uint8, uint16>  mcRxRingBuf;
  cRingBufT<uint8, uint16>  mcTxRingBuf;

  cBotNetStreamPort* mcConnection[cBotNet_StreamSysPortsCnxCnt];

  cBotNetStreamPort()
  : mszName(mszBufName, 0, 24), mcRxRingBuf(mcRxBuf, sizeof(mcRxBuf)), mcTxRingBuf(mcTxBuf, sizeof(mcTxBuf))
  {
    mszName = "";

    mcBnDestAdr.Set(0);
    mcBnDestPortIdx = 0;

    for (int i = 0; i < cBotNet_StreamSysPortsCnxCnt; i++)
    {
      mcConnection[i] = null;
    }
  }


  // interne connection
  void Connect(cBotNetStreamPort* lcPort)
  {
    // schon connected ?
    for (int i = 0; i < cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == lcPort)
      {
        return;
      }
    }

    for (int i = 0; i < cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == null)
      {
        mcConnection[i] = lcPort;
        return;
      }
    }
  }

  virtual void vAddedToBn(cBotNet* lcBotNet)
  {
    UNUSED(lcBotNet);
  }

  // externe connection
  void Connect(u16 lu8DestPortAdr,  u8 lu8DestPortIdx)
  {
     mcBnDestAdr.Set(lu8DestPortAdr);
     mcBnDestPortIdx = lu8DestPortIdx;
  }

  void Put(cBarryPtrT<u16> &lcBAry)
  {
    mcRxRingBuf.put(lcBAry.mpu8Data, lcBAry.Len());
  }

  void Put(cBarryPtrT<u16> &lcBAry, uint16 lui16MaxDataCnt)
  {
    uint16 lui16DataCnt;

    lui16DataCnt = lcBAry.Len();
    if (lui16DataCnt > lui16MaxDataCnt) lui16DataCnt = lui16MaxDataCnt;
    mcRxRingBuf.put(lcBAry.mpu8Data, lui16DataCnt);
  }

  void Get(cBarryPtrT<u16>& lcBAry)
  {
    lcBAry.muLen = mcTxRingBuf.cnt();
    if (lcBAry.muLen > lcBAry.muSize) lcBAry.muLen = lcBAry.muSize;
    mcTxRingBuf.get(lcBAry.mpu8Data, lcBAry.muLen);
  }

  void Get(cBarryPtrT<u16>& lcBAry, uint16 lui16MaxDataCnt)
  {
    lcBAry.muLen = mcTxRingBuf.cnt();
    if (lcBAry.muLen > lcBAry.muSize)  lcBAry.muLen = lcBAry.muSize;
    if (lcBAry.muLen > lui16MaxDataCnt) lcBAry.muLen = lui16MaxDataCnt;
    mcTxRingBuf.get(lcBAry.mpu8Data, lcBAry.muLen);
  }

  void Disconnect(cBotNetStreamPort* lcPort)
  {
    for (int i = 0; i < cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == lcPort)
      {
        mcConnection[i] = null;
        return;
      }
    }
  }

  virtual void vTick10ms() {};
  virtual void vProcess()    = 0;
};


#endif // __BOTNET_STREAM_SYS_PORT_BASE_H__
