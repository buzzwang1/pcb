#ifndef __BOTNET_BLU_H__
#define __BOTNET_BLU_H__

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
#include "cBnStreamSysBlu.h"
#include "cBnLinkBase.h"


class cBotNetBlu
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
  //cBotNet_LinkBase*      mcUpLink;

  // --- DownLinks
  //uint8                  mu8DownLinkCnt;
  uint16                 mu16DownLinkSyncCnt;
  //cBotNet_LinkBase*      mcDownLinks[enCnstSlaveCnt];
  //cBotNet_LinkBase*      mcDownLinkActive;

  cBotNetStreamSystemBlu mcStreamSys;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsg_BaseDyn     mcMsgTxDyn;
  cBotNetMsg_Base        mcMsgTx;

  //cBotNet_MsgSysProcess *mcpMsgProcess;

  // Startup Delay für die Nachrichtenbehandlung.
  // Damit Zeit ist einen Sync aufzubauen.
  // Damit initiale Nachrichten nicht durch den
  // initialen Offline Status gelöscht werden.
  u16                   mStartupDelayCnt;

  typedef struct
  {
    u32 u32MsgCnt;
    u32 u32MsgDropCnt;
  }tstStatus;

  tstStatus             mstStatus;

  //cBotNet(uint16 lu16DevId, uint16 lu16Adr, cBotNet_MsgSysProcess *lcpMsgProcess, cBotNet_LinkBase* lpcSideLink, cBotNet_LinkBase* lpcUpLink, cBotNet_LinkBase* lpcDownLink[enCnstSlaveCnt], uint8 lcSlaveCnt);
  cBotNetBlu(cBotNetCfg* lcCfg);

  ~cBotNetBlu();

  uint32 SizeOf();

  bool bAddLink(cBotNet_LinkBase* lcLink);

  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);

  void vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vLinkSyncAll1ms();
  void vLinkMisc10ms();
  void vTickHp1ms();   // High Prio 1ms
  void vTickLp10ms();  // Low  Prio 10ms

  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecodeAndDispatch();
  bool bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg);
  bool bForwardTxMsg();
  void vProcess10ms();
  void vResetCom(bool lbAlways);
};


#endif // __BOTNET_BLU_H__
