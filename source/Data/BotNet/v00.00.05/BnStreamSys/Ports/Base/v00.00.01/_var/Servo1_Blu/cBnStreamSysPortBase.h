#ifndef __BOTNET_STREAM_SYS_PORT_BASE_H__
#define __BOTNET_STREAM_SYS_PORT_BASE_H__

#include "Typedef.h"

#include "cRingBufT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"


#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

class cBotnet;

class cBotNetStreamPort
{
  public:

  cBotNetAdress mcBnDestAdr;     // == 0 keine externe Connection, != 0 externe Connecion. In dem Fall wird noch der Port Idx benötigt
  uint8         mcBnDestPortIdx;


  uint8         mcRxBuf[cBotNet_StreamSysPortRingBufSize];
  uint8         mcTxBuf[cBotNet_StreamSysPortRingBufSize];
  cRingBufT<uint8, uint16>  mcRxRingBuf;
  cRingBufT<uint8, uint16>  mcTxRingBuf;

  cBotNetStreamPort()
  : mcRxRingBuf(mcRxBuf, sizeof(mcRxBuf)), mcTxRingBuf(mcTxBuf, sizeof(mcTxBuf))
  {
    mcBnDestAdr.Set(0);
    mcBnDestPortIdx = 0;
  }

  virtual void vReInit()
  {
    mcTxRingBuf.reset();
    mcRxRingBuf.reset();
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

  void Put(cbArrayExtT<uint16>* lpcBAryExt)
  {
    mcRxRingBuf.put(lpcBAryExt->mpu8Data, lpcBAryExt->Len());
  }

  void Put(cbArrayExtT<uint16>* lpcBAryExt, uint16 lui16MaxDataCnt)
  {
    uint16 lui16DataCnt;

    lui16DataCnt = lpcBAryExt->Len();
    if (lui16DataCnt > lui16MaxDataCnt) lui16DataCnt = lui16MaxDataCnt;
    mcRxRingBuf.put(lpcBAryExt->mpu8Data, lui16DataCnt);
  }

  void Get(cbArrayExtT<uint16>* lpcBAryExt)
  {
    lpcBAryExt->muiLen = mcTxRingBuf.cnt();
    if (lpcBAryExt->muiLen > lpcBAryExt->muiSize) lpcBAryExt->muiLen = lpcBAryExt->muiSize;
    mcTxRingBuf.get(lpcBAryExt->mpu8Data, lpcBAryExt->muiLen);
  }

  void Get(cbArrayExtT<uint16>* lpcBAryExt, uint16 lui16MaxDataCnt)
  {
    lpcBAryExt->muiLen = mcTxRingBuf.cnt();
    if (lpcBAryExt->muiLen > lpcBAryExt->muiSize) lpcBAryExt->muiLen = lpcBAryExt->muiSize;
    if (lpcBAryExt->muiLen > lui16MaxDataCnt)     lpcBAryExt->muiLen = lui16MaxDataCnt;
    mcTxRingBuf.get(lpcBAryExt->mpu8Data, lpcBAryExt->muiLen);
  }


  virtual void vTick10ms() {};
  virtual void vProcess()    = 0;
};


#endif // __BOTNET_STREAM_SYS_PORT_BASE_H__
