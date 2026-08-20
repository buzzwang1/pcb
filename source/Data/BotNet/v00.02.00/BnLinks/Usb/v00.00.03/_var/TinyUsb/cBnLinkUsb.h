#ifndef __BOTNET_LINK_BOTCOM_H__
#define __BOTNET_LINK_BOTCOM_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"

#include "tusb.h"

// -------------------------------------------------------------------------------------------

class cBotNet_LinkUsb : public cBotNet_LinkBase
{
  public:

  cBotNetMsg_Base mpcMsgDataR;
  cBotNetMsg_Base mpcMsgDataT;

  u8 mu8PoolIdxTx;
  u8 mu8PoolIdxRx;

  bool mbWriteBusy;

  cBotNet_LinkUsb()
    : cBotNet_LinkBase(cBotNet_LinkBase::enSideLink)
  {
    mStatus.IsInit    = 1;
    mStatus.IsOnline  = 0;
    mStatus.IsEnabled = 0;

    mu8PoolIdxTx = 0;
    mu8PoolIdxRx = 0;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_LinkBase::bAddedToBn(lu16Adr);

    mStatus.IsOnline  = 1;
    mStatus.IsEnabled = 1;

    mbWriteBusy = False;

    return lbRet;
  }

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
  }


  void vSync() override
  {
    if (!mbWriteBusy)
    {
      if (mu8PoolIdxTx == 0)
      {
        mu8PoolIdxTx = mcTxComBuf.get();

        if (mu8PoolIdxTx)
        {
          cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, mu8PoolIdxTx);
          mpcMsgDataT.From(lcMsg);
          tud_vendor_write(mpcMsgDataT.Data(), mpcMsgDataT.Len());
          mbWriteBusy = True;
          cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);
          mu8PoolIdxTx = 0;
        }
      }
    }
  }

  void vDataWriteDone()
  {
    mbWriteBusy = False;
  }

  void vDataReceived(const u8* lpu8Buf, u16 lu16BufSize)
  {
    if (mu8PoolIdxRx == 0)
    {
      cBotNetMsg_Base lcMsg; cBnMsgPool::vReqMsg(lcMsg, mu8PoolIdxRx, lu16BufSize);
      if (mu8PoolIdxRx != 0)
      {
        mpcMsgDataR.From(lcMsg);
        mpcMsgDataR.Set(lpu8Buf, lu16BufSize);

        mcRxComBuf.put(mu8PoolIdxRx);
        cBnMsgPool::vPutMsg(mu8PoolIdxRx);
        cBnMsgPool::vSetLen(mu8PoolIdxRx, mpcMsgDataR.Len());

        cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
        mu8PoolIdxRx = 0;
      }
    }
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError); /* avoid warning */
    UNUSED(lenState); /* avoid warning */
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
  }

  void vComDone() override
  {
  }

  void vAddChecksum(u8 lu8PoolIdx) override
  {
    UNUSED(lu8PoolIdx);
  }
};


#endif // __BOTNET_LINK_BOTCOM_H__
