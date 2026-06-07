#ifndef __BOTNET_LINK_I2C_H__
#define __BOTNET_LINK_I2C_H__

#include "Typedef.h"

#include "cBnMsg.h"
#include "cBnLinkBase.h"


class cBotNet_DownLinkI2c : public cBotNet_DownLink //<1, 0, False>
{
public:
  cBotNet_DownLinkI2c(cComNodeMaster* lcDownLink)
    : cBotNet_DownLink(lcDownLink, 0, 0, False)
  {}
};

// -------------------------------------------------------------------------------------------

class cBotNet_UpLinkI2c : public cBotNet_UpLink<0, 0, 0, False>
{
public:
  cBotNet_UpLinkI2c(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};

class cBotNet_UpLinkI2cNoCheck : public cBotNet_UpLink<0, 0, 0, True>
{
public:
  cBotNet_UpLinkI2cNoCheck(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};


#endif // __BOTNET_LINK_USART_MP_HD_H__
