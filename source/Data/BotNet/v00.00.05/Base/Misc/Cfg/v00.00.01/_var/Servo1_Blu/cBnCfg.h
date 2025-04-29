#ifndef __BOTNET_CFG_H__
#define __BOTNET_CFG_H__

#include "Typedef.h"

#define cBotNet_MsgSize       64
#define cBotNet_MsgHeaderSize 7

#define cBotNet_UpLinkComBufSize    (cBotNet_MsgSize * 4)
#define cBotNet_DownLinkComBufSize  (cBotNet_MsgSize * 2)
#define cBotNet_DownLinkCnt          0

#define cBotNet_DownLinkSyncAll      1000 // [ms] Es werden alles gesyncht, sonst nur die Online

#define cBotNet_StreamSysPortsCnt           2
#define cBotNet_StreamSysPortsCnxCnt        4
#define cBotNet_CmdPortIdx                  0

#define cBotNet_StreamSysComBufSize       (cBotNet_MsgSize * 2)
#define cBotNet_StreamSysPortRingBufSize  (cBotNet_MsgSize)



class cBotNetCfg
{
  public:
  const char8* mszName;
  u16          mu16BnDeviceID;
  u16          mu16BnAdr;

  cBotNetCfg()
  {}

  cBotNetCfg(const char8* lszName, u16 lu16BnDeviceID, u16 lu16BnAdr)
  {
    mszName        = lszName;
    mu16BnDeviceID = lu16BnDeviceID;
    mu16BnAdr      = lu16BnAdr;
  }
};


class cBotNetCfgDefault : public cBotNetCfg
{
  cBotNetCfgDefault()
  {
    cBotNetCfg::mszName = (const char8*)"Botnet Undef";
    cBotNetCfg::mu16BnDeviceID = 0x0010;
    cBotNetCfg::mu16BnAdr = 0x1000;
  }
};

/* Forward Deklaration */
class cBotNet;

#endif // __BOTNET_CFG_H__
