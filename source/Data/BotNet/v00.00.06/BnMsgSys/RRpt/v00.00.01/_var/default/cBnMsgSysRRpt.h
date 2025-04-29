#ifndef __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__
#define __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"

#define cBotNetMsgPortRRptSlots 8

class cBotNetMsgPortRRpt : public cBotNet_MsgSysProcess
{
public:

  class cBotNetMsgPortRRptSlot
  {
    public:
    u8 mu8Enabled;
    u8 mu8Cnt;

    u8    mu8Msg_Buf[16];
    cbArrayExtT<uint16> mcMsg_BAry;
    cBotNetMsg_MsgProt mcMsg;

    cBotNetMsgPortRRptSlot()
      : mcMsg_BAry(mu8Msg_Buf, 16),
        mcMsg(&mcMsg_BAry)
    {
      mu8Enabled = 0;
      mu8Cnt = 0;
    }

    bool isSame(cBotNetMsg_MsgProt& lcMsg)
    {
      if (mu8Enabled)
      {
        if ((mcMsg.mcPayload[1] == lcMsg.mcPayload[1]) && // MI = Message Index
            (mcMsg.mcPayload[2] == lcMsg.mcPayload[2]) && // SI = Request Index
            (mcMsg.mcPayload[3] == lcMsg.mcPayload[3]) && // S1 = Sub Index
            (mcMsg.mcPayload[4] == lcMsg.mcPayload[4]) && // S2 = Sub Index
            (mcMsg.mcPayload[5] == lcMsg.mcPayload[5]) && // DH.DL = Destination Botnet Adr
            (mcMsg.mcPayload[6] == lcMsg.mcPayload[6]))   // 
        {
          return True;
        }

      }      
      return False;
    }


    bool bProcess10ms()
    {
      if (mu8Enabled)
      {
        if (mu8Cnt > 1)
        {
          mu8Cnt--;
        }
        else
        {
          // Reload
          mu8Cnt = mcMsg.mcPayload[7]; // CT = Cycle Time
          return True;
        }
      }
      return False;
    }
  };


  cBotNetMsgPortRRptSlot mcSlot[cBotNetMsgPortRRptSlots];
  u8                     mu8SlotCnt;

  u8    mu8MsgTx_Buf[cBotNet_MsgSize];
  cbArrayExtT<uint16> mcMsgTx_BAry;
  cBotNetMsg_MsgProt mcMsgTx;

  cBotNetMsgPortRRpt(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet),
      mcMsgTx_BAry(mu8MsgTx_Buf, cBotNet_MsgSize),
      mcMsgTx(&mcMsgTx_BAry)
  {

    mu8SlotCnt = 0;
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
  void vProcess10ms();
};


#endif // __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__
