#ifndef __BOTNET_H__
#define __BOTNET_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

#include "cBnMsgSys.h"
#include "cBnMsgSysSpop.h"
#include "cBnLinkBase.h"


class cBotNet
{
  public:

  typedef enum
  {
    enCnstSlaveCnt = cBotNet_DownLinkCnt,
  }tenBotnetConsts;

  cBotNetAdress          mcAdr;

  // --- UpLink
  cBotNet_LinkBase*      mcUpLink;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsgPortSpop     mcSpop;

  cBotNet(cBotNetCfg* lcCfg);
  ~cBotNet();

  void vSetName(const char8* lszName);

  bool bAddLink(cBotNet_LinkBase* lcLink);
  void vLinkSyncAll1ms();
  void vLinkMisc10ms();
  void vTickHp1ms();   // High Prio 1ms
  void vTickLp10ms();  // Low  Prio 10ms

  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecodeAndDispatch(cBotNetMsg_Base* lcMsg);
  bool bSendMsg(cBotNetMsg_Base* lcMsg);
  bool bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg);
  void vProcess10ms();
};


#endif // __BOTNET_H__
