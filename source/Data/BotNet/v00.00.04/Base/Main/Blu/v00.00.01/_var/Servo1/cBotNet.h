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
#include "cBnStreamSys.h"
#include "cBnLinkUsartMpHd.h"

class cBotNet
{
  public:

  typedef enum
  {
    enCnstSlaveCnt = cBotNet_DownLinkCnt,
  }tenBotnetConsts;

  cBotNetCfg*            mcCfg;
 
  cBotNetAdress          mcAdr;


  // --- UpLink
  cUartMpHdSlave           mcUartMpHdU0;
  cBotNet_UpLinkUsartMpHd  mcUpLink;


  cBotNetStreamSystem    mcStreamSys;

  cBotNetMsg_BaseDyn     mcMsgRxDyn;
  cBotNetMsg_Base        mcMsgRx;

  cBotNetMsg_BaseDyn     mcMsgTxDyn;
  cBotNetMsg_Base        mcMsgTx;


  cBotNet(cBotNetCfg* lcCfg);

  ~cBotNet();

  bool bAddLink(cBotNet_UpLinkUsartMpHd* lcLink);

  u16 u16StreamPortAdd(cBotNetStreamPort* lcPort);
  void vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx);
  void vSetName(const char8* lszName);

  void vLinkSyncAll1ms();
  void vLinkMisc10ms();
  void vTickHp1ms();   // High Prio 1ms
  void vTickLp10ms();  // Low  Prio 10ms

  bool bDecodeMsgType(cBotNetMsg_Base* lcMsg);
  bool bDecodeAndDispatch(cBotNetMsg_Base* lcMsg);
  void vProcess10ms();
  void vResetCom(bool lbAlways);
};


#endif // __BOTNET_H__
