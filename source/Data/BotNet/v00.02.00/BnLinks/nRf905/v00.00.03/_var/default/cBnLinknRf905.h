#ifndef __BOTNET_LINK_NRF905_H__
#define __BOTNET_LINK_NRF905_H__

#include "Typedef.h"
// Driver
#include "cnRF905.h"
#include "cBnMsg.h"
#include "cBnLinkBase.h"

class cBotNet_DownLinkNrf905 : public cBotNet_DownLink //<1, 0, False>
{
public:
  cBotNet_DownLinkNrf905(cComNodeMaster* lcDownLink)
    : cBotNet_DownLink(lcDownLink, cNRF905::NRF905_WAIT_TX_DELAY, cNRF905::NRF905_WAIT_TX_DELAY, False)
  {}
};

// -------------------------------------------------------------------------------------------

class cBotNet_UpLinkNrf905 : public cBotNet_UpLink<0, cNRF905::NRF905_WAIT_TX_DELAY, cNRF905::NRF905_WAIT_TX_DELAY, False>
{
public:
  cBotNet_UpLinkNrf905(cComNodeSlave* lcUpLink)
    : cBotNet_UpLink(lcUpLink)
  {}
};


#endif // __BOTNET_LINK_NRF905_H__
