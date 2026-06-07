#ifndef __BOTNET_LINK_USART_MP_HD_H__
#define __BOTNET_LINK_USART_MP_HD_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"

class cBotNet_DownLinkUsartMpHd : public cBotNet_DownLink //<1, 0, False>
{
  public:
    cBotNet_DownLinkUsartMpHd(cComNodeMaster* lcDownLink)
    : cBotNet_DownLink(lcDownLink, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncDwn, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataDwn, False)
  {}
};

class cBotNet_DownLinkUsartMpHdNoCheck : public cBotNet_DownLink //<1, 0, False>
{
  public:
    cBotNet_DownLinkUsartMpHdNoCheck(cComNodeMaster* lcDownLink)
    : cBotNet_DownLink(lcDownLink, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncDwn, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataDwn, True)
  {}
};


// -------------------------------------------------------------------------------------------

class cBotNet_UpLinkUsartMpHd : public cBotNet_UpLink<1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, False>
{
public:
  cBotNet_UpLinkUsartMpHd(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};

class cBotNet_UpLinkUsartMpHdNoCheck : public cBotNet_UpLink<1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, True>
{
public:
  cBotNet_UpLinkUsartMpHdNoCheck(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};


#endif // __BOTNET_LINK_USART_MP_HD_H__
