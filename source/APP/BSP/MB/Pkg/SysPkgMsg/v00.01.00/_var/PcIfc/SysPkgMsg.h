
#ifndef __SYS_PKG_MSG_H__
#define __SYS_PKG_MSG_H__

#include "TypeDef.h"
#include "cBnMsgSys.h"





class cBnMsgHandler : public cBotNet_MsgSysProcess
{
  public:
  cBnMsgHandler();
  bool bMsg(cBotNetMsg_MsgProt& lcMsg);
};

class cSysPkgMsgWakeup
{
  public:
    u32  mu32Data;
    u8   mu8WakeupIdx;
    bool mbRequest;

    cSysPkgMsgWakeup()
    {
      vReset();
    }

    void vReset()
    {
      mu8WakeupIdx = 0xFF;
      mbRequest = False;
    }

    bool isRequest(u8 lu8Idx = 0)
    {
      return (mbRequest) && (lu8Idx == mu8WakeupIdx);
    }

    bool isRequestAndClear(u8 lu8Idx = 0)
    {
      if (isRequest(lu8Idx))
      {
        vReset();
        return True;
      }
      return False;
    }

    void vSet(u8 lu8WakeupIdx, u32 lu32Data)
    {
      mu32Data = lu32Data;
      mu8WakeupIdx = lu8WakeupIdx;
      mbRequest = True;
    }


};

class cSysPkgMsg
{
  public:
  cBnMsgHandler    mcBnMsgHandler;
  cSysPkgMsgWakeup mcWakeupSim;

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
