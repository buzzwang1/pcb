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
    // 01 MI RI S1 S2 DH DL CT  // Payload Len: 8;      01 = Kommando
                                //                  [0] MI = Message Index
                                //                  [1] RI = Request Index
                                //                  [2] S1 = Sub Index
                                //                  [3] S2 = Sub Index
                                //                  [4] DH.DL = Destination Botnet Adr
                                //                  [6] CT = Cycle Time [10ms] 0 = single request or cancle cycle
                                //                                             1 = 10ms cycle
    public:

      enum class cByte : u8
      {
        nMI = 0, // Message Index
        nRI,     // Request Index
        nS1,     // Sub Index
        nS2,     // Sub Index
        nDH,     // Destination Botnet Adr
        nDL,     // Destination Botnet Adr
        nCT,     // Cycle Time [10ms]
        nLast    //
      };


    u8 mu8Payload[7];
    u8 mu8Enabled;
    u8 mu8Cnt;
    
    cBotNetMsgPortRRptSlot()
    {
      mu8Enabled = 0;
      mu8Cnt     = 0;
    }

    bool isSame(cBotNetMsg_MsgProt& lcMsg)
    {
      if (mu8Enabled)
      {
        if ((mu8Payload[0] == lcMsg.mcPayload[1]) && // MI = Message Index
            (mu8Payload[1] == lcMsg.mcPayload[2]) && // SI = Request Index
            (mu8Payload[2] == lcMsg.mcPayload[3]) && // S1 = Sub Index
            (mu8Payload[3] == lcMsg.mcPayload[4]) && // S2 = Sub Index
            (mu8Payload[4] == lcMsg.mcPayload[5]) && // DH.DL = Destination Botnet Adr
            (mu8Payload[5] == lcMsg.mcPayload[6]))   // 
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
          mu8Cnt = mu8Payload[(u8)cByte::nCT]; // CT = Cycle Time
          return True;
        }
      }
      return False;
    }
  };


  cBotNetMsgPortRRptSlot mcSlot[cBotNetMsgPortRRptSlots];
  u8                     mu8SlotCnt;

  cBotNetMsgPortRRpt(cBotNet* lcBotNet)
    : cBotNet_MsgSysProcess(lcBotNet)  
  {
    mu8SlotCnt = 0;
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
  void vProcess10ms();
};


#endif // __BOTNET_MESSAGE_SYS_REQUEST_REPEATER_H__
