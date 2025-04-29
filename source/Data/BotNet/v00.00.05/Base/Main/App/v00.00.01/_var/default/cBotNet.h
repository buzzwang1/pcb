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
#include "cBnStreamSys.h"
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

  // --- DownLinks
  uint8                  mu8DownLinkCnt;
  uint16                 mu16DownLinkSyncCnt;
  cBotNet_LinkBase*      mcDownLinks[enCnstSlaveCnt];
  // Alle DownLinks, die die gleiche HW-Schnittstelle nutzen,
  // braucht die HW-ReInit oder 1ms Tick nur einmal aufgerufen werden
  bool                   mbHandleHardware[enCnstSlaveCnt];
  cBotNet_LinkBase*      mcDownLinkActive;

  // Dynamische Lastanpassung
  // Downcounter in ms, bei 0 wird ein Sync ausgelöst
  u8                     mcDownLinkDynSyncCnt[enCnstSlaveCnt];
  // Reload value für mcDownLinkDynSyncCnt. Wenn es für 10ms kein Traffic für den Slave gab, 
  // dann wird dieser Wert verdoppelt (1->2->4->8->16->32->64). 64ms ist max. Falls Traffic ansteht
  // dann wird der Wert zurückgesetzt auf 1ms.
  u8                     mcDownLinkDynSyncReload[enCnstSlaveCnt];
  // für 10ms offset counter. Nachdem Traffic war, wird 10ms..20ms Delay eingefügt, bevor der
  // Reload verdoppelt wird.
  u8                     mcDownLinkDynSyncReloadCnt[enCnstSlaveCnt];

  cBotNetStreamSystem    mcStreamSys;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsg_BaseDyn     mcMsgTxDyn;
  cBotNetMsg_Base        mcMsgTx;

  cBotNet_MsgSysProcess *mcpMsgProcess;

  // Startup Delay für die Nachrichtenbehandlung.
  // Damit Zeit ist einen Sync aufzubauen.
  // Damit initiale Nachrichten nicht durch den
  // initialen Offline Status gelöscht werden.
  u16                    mStartupDelayCnt;

  char8                  mszBufName[16];
  cStr                   mszName;

  typedef struct
  {
    u32 u32MsgCnt;
    u32 u32MsgDropCnt;
  }tstStatus;

  tstStatus             mstStatus;

  //cBotNet(uint16 lu16DevId, uint16 lu16Adr, cBotNet_MsgSysProcess *lcpMsgProcess, cBotNet_LinkBase* lpcSideLink, cBotNet_LinkBase* lpcUpLink, cBotNet_LinkBase* lpcDownLink[enCnstSlaveCnt], uint8 lcSlaveCnt);
  cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess* lcpMsgProcess);

  ~cBotNet();

  void vReInit();

  uint32 SizeOf();

  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);
  void vSetName(const char8* lszName);

  void vSetHandleHardware(u8 lu8IdxDownLink, bool lbState);
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
  bool bForwardTxMsg();
  void vProcess10ms();
};


#endif // __BOTNET_H__
