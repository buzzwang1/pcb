#ifndef __BOTNET_MESSAGE_SYS_SPOP_H__
#define __BOTNET_MESSAGE_SYS_SPOP_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"
#include "cBnSpop.h"


class cBotNetMsgPortSpop: public cBotNet_MsgSysProcess
{
  public:
    cBotNetMsgPortSpop(cBotNet* lcBotNet);

    void vEnter(uint16 lu16SAdr, uint16 lu16DAdr);
    bool bMsg(cBotNetMsg_MsgProt& lcMsg);
};


#endif // __BOTNET_MESSAGE_SYS_SPOP_H__
