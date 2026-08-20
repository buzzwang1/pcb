#ifndef __BOTNET_H__
#define __BOTNET_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

#include "cBnMsgSys.h"
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

  // --- UpLink
  cBotNet_LinkBase*      mcUpLink;

  // --- DownLinks
  cBotNet_LinkBase*      mcDownLink;

  // Dynamische Lastanpassung
  // Downcounter in ms, bei 0 wird ein Sync ausgelöst
  u8                     mcDownLinkDynSyncCnt;
  // Reload value für mcDownLinkDynSyncCnt. Wenn es für 10ms kein Traffic für den Slave gab, 
  // dann wird dieser Wert verdoppelt (1->2->4->8->16->32->64). 64ms ist max. Falls Traffic ansteht
  // dann wird der Wert zurückgesetzt auf 1ms.
  u8                     mcDownLinkDynSyncReload;
  // für 10ms offset counter. Nachdem Traffic war, wird 10ms..20ms Delay eingefügt, bevor der
  // Reload verdoppelt wird.
  u8                     mcDownLinkDynSyncReloadCnt;

  u8 mu8PoolIdx;

  cBotNet_MsgSysProcess *mcpMsgProcess;


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


  void vSetName(const char8* lszName);

  bool bAddLink(cBotNet_LinkBase* lcLink);
  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vLinkDynSyncReset(u16 lu16Idx);


  // Low  Prio 10ms
  void vLinkSyncAllTick1ms();
  void vLinkMisc10ms();
  void vMessageAgeing10ms();
  void vProcess(u32 lu32DiffTime_us);

  void vDecodeMsgType(u8 &lu8PoolIdx);
  void vDropMsg(u8 &lu8PoolIdx);
  void vCheckMsg(u8 &lu8PoolIdx);
  void vSendMsg(u8 &lu8PoolIdx);
  void vSendMsg(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u16 liIdx, u8* lpuData, u8 luSize);
  void vForwardMsgToLink(cBotNet_LinkBase* lpcLink, u8 &lu8PoolIdx);

  void vProcessData();
};


#endif // __BOTNET_H__
