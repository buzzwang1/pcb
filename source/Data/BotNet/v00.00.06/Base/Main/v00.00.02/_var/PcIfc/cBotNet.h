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


  u8                     mu8MsgRxBuf[cBotNet_MsgSize];
  cBotNetMsg_Base        mcMsgRx;

  u8                     mu8MsgTxBuf[cBotNet_MsgSize];
  cBotNetMsg_Base        mcMsgTx;

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

  uint32 SizeOf();

  void vSetName(const char8* lszName);

  bool bAddLink(cBotNet_LinkBase* lcLink);
  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);

  // High Prio 1ms
  void vLinkSyncAllHp1ms(); 
  void vTickHp1ms();

  // Low  Prio 10ms
  void vLinkMisc10ms();
  void vProcess(u32 lu32DiffTime_us);

  bool bCallMsgHandler(cBotNetMsg_MsgProt& lcMsgProt);
  bool bDecodeAndDispatch();
  bool bSendMsg(cBotNetMsg_Base* lcMsg);
  bool bForwardTxMsg();
  void vProcessData();
};


#endif // __BOTNET_H__
