#ifndef __BOTNET_LINK_USART_MP_HD_H__
#define __BOTNET_LINK_USART_MP_HD_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"
#include "cUsart_MpHd.h"


class cBotNet_UpLinkUsartMpHdNoCheck : public cBnUpLink<cUartMpHdSlave, 1, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTSyncUp, cBotNet_ComLinkUsartMpHdCfg::enCnstWaitTDataUp, True>

{
public:
  cBotNet_UpLinkUsartMpHdNoCheck()
    : cBnUpLink()
  {}
};


#endif // __BOTNET_LINK_USART_MP_HD_H__
