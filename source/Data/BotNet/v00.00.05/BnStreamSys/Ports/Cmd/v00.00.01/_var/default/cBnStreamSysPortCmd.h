#ifndef __BOTNET_STREAM_SYS_PORT_CMD_H__
#define __BOTNET_STREAM_SYS_PORT_CMD_H__

#include "Typedef.h"
#include "cCli.h"
#include "cBnStreamSysPortBase.h"


class cBotNet_LinkBase;


class cCliCmd_Ports: public cCliCmd
{
  public:
    cCliCmd_Ports():cCliCmd((const char*)"Ports", (const char*)"Show Ports") {}
    void vPrintPort(u16 lu16Idx, cBotNetStreamPort* lcPort, cStr& lszStr, cCli *lcCli);
    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};

class cCliCmd_Links: public cCliCmd
{
  public:
    cCliCmd_Links():cCliCmd((const char*)"Links", (const char*)"Show Links") {}
    void vPrintLink(u16 lu16Idx, cBotNet_LinkBase* lcLink, cStr& lszStr, cCli *lcCli);
    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};

class cCliCmd_Status: public cCliCmd
{
  public:
    cCliCmd_Status():cCliCmd((const char*)"Status", (const char*)"Show Status") {}
    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override;
};


class cBotNetStreamPort_BotNetCmdPort: public cBotNetStreamPort, public cCli
{
  public:
  
  uint8                     mpui8TextBufOut[512];
  cRingBufT<uint8, uint16>  mcTextOut;

  cCliCmd_Version     mcCliCmd_Ver;
  cCliCmd_Help        mcCliCmd_Help;
  cCliCmd_Ports       mcCliCmd_Ports;
  cCliCmd_Links       mcCliCmd_Links;
  cCliCmd_Status      mcCliCmd_Status;

  cCliCmdList_Dyn     mcMyCmdList;

  cBotNetStreamPort_BotNetCmdPort(cBotNet* lcBotNet);

  virtual void vReInit()
  {
    cBotNetStreamPort::vReInit();
    mcTextOut.reset();
  }

  uint32 SizeOf() override
  {
    return sizeof(cBotNetStreamPort_BotNetCmdPort);
  }

  void vProcess()
  {
    u16 lui16t;
    cCli::bDoProcess();

    lui16t = mcTextOut.cnt();
    if (lui16t)
    {
      if (lui16t > mcTxRingBuf.space_left()) lui16t = mcTxRingBuf.space_left();

      mcTextOut.move(&mcTxRingBuf, lui16t);
    }
  }
};

#endif // __BOTNET_STREAM_SYS_PORT_BASE_H__
