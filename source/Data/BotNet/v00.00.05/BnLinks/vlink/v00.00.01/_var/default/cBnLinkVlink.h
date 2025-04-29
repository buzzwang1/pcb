#ifndef __BOTNET_LINK_VLINK_H__
#define __BOTNET_LINK_VLINK_H__

#include "Typedef.h"
#include "cUsart.h"

#include "cBnLinkBase.h"


class cBotNet_LinkVLink: public cBotNet_LinkBase
{
  public:

  cBotNet_LinkVLink(cBotNet_LinkBase::tenType lenLink)
    : cBotNet_LinkBase(cBotNet_DownLinkComBufSize, cBotNet_DownLinkComBufSize, lenLink)
  {
    mStatus.IsInit   = 1;
  }

  virtual void vReInit()
  {
    cBotNet_LinkBase::vReInit(False);
    mStatus.IsOnline = 1;
    mStatus.IsInit = 1;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    return cBotNet_LinkBase::bAddedToBn(lu16Adr);
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    (void)lenError; /* avoid warning */
    (void)lenState; /* avoid warning */
  }


  void vComStart(cComNode::tenEvent lenEvent)
  {
    (void)lenEvent;
  }

  void  vComDone()
  {
  }

  void vTick1ms() override
  {
  }
};


class cBotNet_LinkVLink_UpLink : public cBotNet_LinkVLink
{
public:

  cBotNet_LinkBase* mcConnection;

  void vConnect(cBotNet_LinkBase* lcConnection)
  {
    mcConnection = lcConnection;
    mStatus.IsOnline = 1;
  }

  cBotNet_LinkVLink_UpLink()
    : cBotNet_LinkVLink(cBotNet_LinkBase::enUpLink)
  {
  }

  virtual void vReInit()
  {
    cBotNet_LinkVLink::vReInit();
  }

  bool bPut(cbArrayExtT<uint16>* lcEntry) override
  {
    if (IsOnline())
    {
      if (mcConnection->mcRxComBuf.isFitting(lcEntry))
      {
        mcConnection->mcRxComBuf.put(lcEntry);
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

  u16 u16Get(cbArrayExtT<uint16>* lcEntry) override
  {
    mcConnection->mcTxComBuf.get(lcEntry);
    return lcEntry->Len();
  }
};


class cBotNet_LinkVLink_DownLink : public cBotNet_LinkVLink
{
public:

  cBotNet_LinkVLink_DownLink()
    : cBotNet_LinkVLink(cBotNet_LinkBase::enDownLink)
  {
    mStatus.IsOnline = 1;
  }

  virtual void vReInit()
  {
    cBotNet_LinkVLink::vReInit();
  }
};



#endif // __BOTNET_LINK_VLINK_H__
