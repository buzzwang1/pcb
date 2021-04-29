#ifndef __BOTNET_CFG_H__
#define __BOTNET_CFG_H__

#include "Typedef.h"

#define cBotNet_MsgSize 64

#define cBotNet_MasterSlaveComBufSize  (cBotNet_MsgSize * 4)

#define cBotNet_SlaveSlaveComBufSize   (cBotNet_MsgSize * 16)

#define cBotNet_MasterSlaveCnt          14

#define cBotNet_StreamSysPortsCnt          16
#define cBotNet_StreamSysPortsCnxCnt        4

#define cBotNet_StreamSysComBufSize       (cBotNet_MsgSize * 2)
#define cBotNet_StreamSysPortRingBufSize  (cBotNet_MsgSize)

/* Forward Deklaration */
class cBotNet;

#endif // __BOTNET_CFG_H__
