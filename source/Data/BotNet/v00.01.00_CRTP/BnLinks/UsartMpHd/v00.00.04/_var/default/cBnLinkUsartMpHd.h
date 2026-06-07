#ifndef __BOTNET_LINK_USART_MP_HD_H__
#define __BOTNET_LINK_USART_MP_HD_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"




// -------------------------------------------------------------------------------------------

//class cBotNet_UpLinkUsartMpHd : public cBotNet_UpLink<1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, False>
//{
//public:
//  cBotNet_UpLinkUsartMpHd(cComNodeSlave* lcUpLink)
//    : cBotNet_UpLink(lcUpLink)
//  {}
//};

class cBotNet_UpLinkUsartMpHdNoCheck : public cBotNet_UpLink<1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, True>
{
public:
  cBotNet_UpLinkUsartMpHdNoCheck(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};


#endif // __BOTNET_LINK_USART_MP_HD_H__
