#ifndef __BOTNET_H__
#define __BOTNET_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"

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

  cBotNetCfg*            mcCfg;
 
  cBotNetAdress          mcAdr;
  cBotNetAdress          mcDeviceID;

  // --- SideLink
  cBotNet_LinkBase*      mcSideLink;

  // --- UpLink
  cBotNet_LinkBase*      mcUpLink;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNet_MsgSysProcess *mcpMsgProcess;
  cBotNetMsgPortSpop     mcSpop;


  char8                  mszBufName[16];
  cStr                   mszName;
  cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess* lcpMsgProcess);
  ~cBotNet();

  void vSetName(const char8* lszName);

  bool bAddLink(cBotNet_LinkBase* lcLink);
  bool bAddLink(cBotNet_LinkBase* lcLink, u16 lu16Adr);
  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vLinkDynSyncReset(u16 lu16Idx);
  void vLinkSyncAll1ms();
  void vLinkMisc10ms();
  void vTickHp1ms();   // High Prio 1ms
  void vTickLp10ms();  // Low  Prio 10ms

  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecodeAndDispatch();
  bool bSendMsg(cBotNetMsg_Base* lcMsg);
  bool bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg);
  void vProcess10ms();
};


#endif // __BOTNET_H__
