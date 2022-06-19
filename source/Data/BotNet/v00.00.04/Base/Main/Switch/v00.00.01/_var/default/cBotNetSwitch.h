#ifndef __BOTNET_SWITCH_H__
#define __BOTNET_SWITCH_H__

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
#include "cBnLinkBase.h"


class cBotNetSwitch
{
  public:

  typedef enum
  {
    enCnstSlaveCnt = cBotNet_DownLinkCnt,
  }tenBotnetConsts;

  cBotNetCfg*            mcCfg;
 
  cBotNetAdress          mcAdr;
  cBotNetAdress          mcDeviceID;

  // --- UpLink
  cBotNet_LinkBase*      mcUpLink;

  // --- DownLinks
  cBotNet_LinkBase*      mcDownLink;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsg_BaseDyn     mcMsgTxDyn;
  cBotNetMsg_Base        mcMsgTx;

  cBotNet_MsgSysProcess *mcpMsgProcess;

  cBotNetSwitch(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess* lcpMsgProcess);

  ~cBotNetSwitch();

  uint32 SizeOf();

  bool bSetUpLink(cBotNet_LinkBase* lcLink);
  bool bSetDownLink(cBotNet_LinkBase* lcLink);


  void put(cbArrayExtT<uint16> *lcEntry);
  void get(cbArrayExtT<uint16> *lcEntry);
  void vLinkSyncAll1ms();
  void vLinkMisc10ms();
  void vTickHp1ms();   // High Prio 1ms
  void vTickLp10ms();  // Low  Prio 10ms


  bool bDecodeAndDispatch();
  void vProcess10ms();
};


#endif // __BOTNET_SWITCH_H__
