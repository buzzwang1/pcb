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
  uint8                  mu8DownLinkSyncCnt;
  cBotNet_LinkBase*      mcDownLinks[enCnstSlaveCnt];
  cBotNet_LinkBase*      mcDownLinkActive;

  cBotNetStreamSystem    mcStreamSys;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsg_BaseDyn     mcMsgTxDyn;
  cBotNetMsg_Base        mcMsgTx;

  cBotNet_MsgSysProcess *mcpMsgProcess;

  // Startup Delay f�r die Nachrichtenbehandlung.
  // Damit Zeit ist einen Sync aufzubauen.
  // Damit initiale Nachrichten nicht durch den
  // initialen Offline Status gel�scht werden.
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

  uint32 SizeOf();

  bool bAddLink(cBotNet_LinkBase* lcLink);

  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);
  void vSetName(const char8* lszName);

  void vSyncLink(cBotNet_LinkBase* lpcLink, bool lbForceSync);
  void vSync();
  void vTick10ms();

  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecode();
  bool bSendMsg(cBotNetMsg_Base* lcMsg);
  bool bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg);
  bool bForwardTxMsg();
  void vProcess();
  void vResetCom(bool lbAlways);
};


#endif // __BOTNET_H__
