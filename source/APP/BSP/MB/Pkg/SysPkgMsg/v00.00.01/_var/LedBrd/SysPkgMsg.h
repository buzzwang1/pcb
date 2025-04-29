
#ifndef __SYS_PKG_MSG_H__
#define __SYS_PKG_MSG_H__

#include "TypeDef.h"
#include "cBnStreamSys.h"
#include "cBnMsgSys.h"


class cCliCmd_SysInfo2 : public cCliCmd
{
public:
  cCliCmd_SysInfo2();
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override;
};

class cCliCmd_SysClock : public cCliCmd
{
public:
  cCliCmd_SysClock();
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override;
};

class cCliCmd_ErrorCounter : public cCliCmd
{
public:
  cCliCmd_ErrorCounter();
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override;
};


class cCliCmd_SleepStatus : public cCliCmd
{
public:
  cCliCmd_SleepStatus();
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override;

};






class cMiniCli : public cBotNetStreamPort
{
public:
  cCliCmd_SysInfo2     mcBnCliCmd_SysInfo2;
  cCliCmd_SysClock     mcBnCliCmd_Clock;
  cCliCmd_SleepStatus  mcBnCliCmd_SleepStatus;
  cCliCmd_ErrorCounter mcBnCliCmd_ErrorCounter;

  cMiniCli();
  inline uint32 SizeOf(){return 0;}
  void vAddedToBn(cBotNet* lcBotNet) override;
  void vProcess(void) override;
};


class cBnMsgHandler : public cBotNet_MsgSysProcess
{
  public:
  cBnMsgHandler();
  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
};


class cSysPkgMsg
{
  public:
  cBnMsgHandler mcBnMsgHandler;
  cMiniCli      mcMiniCli;

  cSysPkgMsg();

  void vInit1();
  void vInit2();

  void vTick1msHp();
  void vTick1msLp();
  void vTick10msLp();
  void vTick100msLp();
  void vTick1000msLp();
};



#endif // __SYS_PKG_MSG_H__
