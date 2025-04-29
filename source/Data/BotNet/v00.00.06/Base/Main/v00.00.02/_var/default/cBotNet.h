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
#include "cBnMsgSysBtr.h"
#include "cBnMsgSysSpop.h"
#include "cBnMsgSysRRpt.h"

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
  // Nach dieser Zeit wirk ein Sync aller Downlinks, unabhänging vom der dynamischen Lastanspassung
  // Der Wert ist normalerweise 1000ms
  uint16                 mu16DownLinkSyncCnt_ms;
  uint16                 mu16DownLinkSyncCntReload_ms;
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


  cBotNetMsgPortBtr      mcBtr;
  cBotNetMsgPortSpop     mcSpop;
  cBotNetMsgPortRRpt     mcRRpt;

  // Für Nachrichtenbehandlung
  bool mbMsgTxOk;
  bool mbMsgRxOk;

  // Startup Delay für die Nachrichtenbehandlung.
  // Damit Zeit ist einen Sync aufzubauen.
  // Damit initiale Nachrichten nicht durch den
  // initialen Offline Status gelöscht werden.
  u16                    mStartupDelayCnt;

  char8                  mszBufName[16];
  cStr                   mszName;

  // Für Timing
  u32 mu32Time_us;
  u8  mu32Time_ms;

  typedef struct
  {
    u32 u32MsgCnt;
    u32 u32MsgDropCnt;
  }tstStatus;

  tstStatus             mstStatus;

  cBotNet(cBotNetCfg* lcCfg);

  ~cBotNet();

  uint32 SizeOf();

  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);
  void vSetName(const char8* lszName);

  void vSetHandleHardware(u8 lu8IdxDownLink, bool lbState);
  bool bAddLink(cBotNet_LinkBase* lcLink);
  bool bAddLink(cBotNet_LinkBase* lcLink, u16 lu16Adr);
  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vLinkDynSyncReset(u16 lu16Idx);

  // High Prio 1ms
  void vLinkSyncAllHp1ms(); 
  void vTickHp1ms();

  // Low  Prio 10ms
  void vLinkMisc10ms();
  void vMessageAgeing10ms();
  void vProcess(u32 lu32DiffTime_us);

  bool bCallMsgHandler(cBotNetMsg_MsgProt& lcMsgProt);
  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecodeAndDispatch();
  bool bSendMsg(cBotNetMsg_Base* lcMsg);
  bool bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg);
  bool bForwardTxMsg();
  void vProcessData();
};


#endif // __BOTNET_H__
