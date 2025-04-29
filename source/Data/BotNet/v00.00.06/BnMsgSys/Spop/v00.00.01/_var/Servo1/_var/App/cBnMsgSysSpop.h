#ifndef __BOTNET_MESSAGE_SYS_SPOP_H__
#define __BOTNET_MESSAGE_SYS_SPOP_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"
#include "cBnSpop.h"


class cBotNetMsgPortSpop
{
  public:
    u8    mu8MsgTx_Buf[cBotNet_MsgSize];
    cbArrayExtT<uint16> mcTxMsgTx_BAry;
    cBotNetMsg_MsgProt mcTxMsgTx;
    cBotNet* mcBn;

    cBotNetMsgPortSpop()
      : mcTxMsgTx_BAry(mu8MsgTx_Buf, cBotNet_MsgSize),
        mcTxMsgTx(&mcTxMsgTx_BAry)
    {
    }

    void vEnter(uint16 lu16SAdr, uint16 lu16DAdr);
    bool bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn);
    void vProcess10ms() {};
};


#endif // __BOTNET_MESSAGE_SYS_SPOP_H__
