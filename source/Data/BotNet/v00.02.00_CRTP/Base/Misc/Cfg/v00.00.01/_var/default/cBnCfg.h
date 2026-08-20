#ifndef __BOTNET_CFG_H__
#define __BOTNET_CFG_H__

#include "Typedef.h"

#define cBotNet_MsgSize       64
#define cBotNet_MsgHeaderSize 10

#define cBotNet_MsgPool16B     0
#define cBotNet_MsgPool32B    16
#define cBotNet_MsgPool64B    24

#define cBotNet_DownLinkCnt          14

#define cBotNet_DownLinkSyncAll      1000 // [ms] Es werden alles gesyncht, sonst nur die Online

#define cBotNet_StreamSysPortsCnt          16
#define cBotNet_StreamSysPortsCnxCnt        4
#define cBotNet_CmdPortIdx                  0

#define cBotNet_StreamSysRingBufSize       (4)
#define cBotNet_StreamSysPortRingBufSize  (cBotNet_MsgSize)

#define cBotNet_UpLinkComBufSize    (cBotNet_MsgSize * 16) // 16 * 64 = 
#define cBnMsgSysBtrCfgRamSize            (4*1024)

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

class cBotNet_ComLinkUsartMpHdCfg
{
public:
  typedef enum
  {
    enCnstNoDelay   =   0,
    enCnstWait1us   =   1,
    enCnstWait5us   =   5,
    enCnstWait10us  =  10,
    enCnstWait20us  =  20,
    enCnstWait25us  =  25,
    enCnstWait30us  =  30,
    enCnstWait50us  =  50,
    enCnstWait75us  =  75,
    enCnstWait100us = 100,
    enCnstWaitTSyncUp   = enCnstWait10us,
    enCnstWaitTDataUp   = enCnstWait10us,
    enCnstWaitTSyncDwn  = enCnstWait10us,
    enCnstWaitTDataDwn  = enCnstWait10us,
    enCnstWaitEndDwn    = enCnstWait50us,
  }tenConsts;
};

/* Forward Deklaration */
class cBotNet;

#endif // __BOTNET_CFG_H__
