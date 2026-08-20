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


  // --- UpLink
  cBotNet_LinkBase*      mcUpLink;

  cBotNetStreamSystem    mcStreamSys;


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


  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);
  void vSetName(const char8* lszName);

  bool bAddLink(cBotNet_LinkBase* lcLink);
  bool bAddLink(cBotNet_LinkBase* lcLink, u16 lu16Adr);
  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vLinkDynSyncReset(u16 lu16Idx);

  // Low  Prio 10ms
  void vLinkMisc10ms();
  void vProcess(u32 lu32DiffTime_us);

  void vDecodeMsgType(u8 &lu8PoolIdx);
  void vDropMsg(u8 &lu8PoolIdx);
  void vCheckMsg(u8 &lu8PoolIdx);
  void vSendMsg(u8 &lu8PoolIdx);
  void vForwardMsgToLink(cBotNet_LinkBase* lpcLink, u8 &lu8PoolIdx);

  void vProcessData();
};


#endif // __BOTNET_H__
