#ifndef __cUSART_MPHD_H__
#define __cUSART_MPHD_H__

#include "Typedef.h"

#include "cUartMpHdTimer.h"

#include "ComDat.h"



class cUartMpHd
{
  public:

  cComNode::tenState   mSm;

  u16                  mu16DmaCounter2ms_Old;

  u32                  mu32Baudrate;
  //Single Wire, only Tx Pin
  //cGpPin               mPinTx;

  uint8 mui8Adr;

  cUartMpHd()
  {
  }

  void  vStopDMA()
  {
  }

  void vInitHwUart()
  {
  }

  void  vInitHw()
  {
    mSm  = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption, bool lbMaster = False)
  {
    UNUSED(lenOption);
    UNUSED(lbMaster);
    // Immer cComNode::enResetHwFull, Spart ein paar Byte
    vInitHw();
  }

  void vTick10ms()
  {
  }
};


// Dummy Class
class cUartMpHdMaster : public cUartMpHd
{
  public:
    cUsartMpHd_Slave_Timer mTimer;


  cUartMpHdMaster()
    : cUartMpHd()
  {
  }

  ~cUartMpHdMaster()
  {
  }

  void vReInit() {}


  inline void SetLock(u8 lu8Val = 1)   {UNUSED(lu8Val);}
  inline void ResetLock() {}

  void vResetCom()
  {
  }

  void vSupressErrorHandling()
  {
  }

  void vError(cComNode::tenError lenError)
  {
    UNUSED(lenError);
  }

  void vHandleTimeout()
  {
  }

  void vPrepareRx()
  {
  }

  void TIM_EV_IRQHandler()
  {
  }

  void ComIrqHandler(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEventType);
    UNUSED(lenEvent);
  }

  void vSetReInitTicks(u16 lu16ReInitTicks)
  {
    UNUSED(lu16ReInitTicks);
  }

  void vTick1ms()
  {
  }


  void vAddSlave(cComNode *lpcSlave)
  {
    UNUSED(lpcSlave);
  }

  void vRemoveSlave(cComNode *lpcSlave)
  {
    UNUSED(lpcSlave);
  }

  void vReInitAllSlave()
  {
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    UNUSED(lenEventType);
    UNUSED(lenEvent);
  }

  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    UNUSED(lpcActiveMsg);
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {

    return True;
  }
};



class cUartMpHdSlave : public cUartMpHd
{
  public:
  cComDatMsg          *mpcMsg;

  cComNode            *mpcComNode;

  cUsartMpHd_Slave_Timer mTimer;

  u8                   mu8SelfTimer;
  bool                 isNoErrorHandling;

  cUartMpHdSlave()
  : cUartMpHd()
  {
    mSm         = cComNode::tenState::enStIdle;
    mpcComNode  = NULL;
    mpcMsg      = NULL;
    mu8SelfTimer = 0;
    isNoErrorHandling = False;
  }

  ~cUartMpHdSlave()
  {
  }

  void vTick1ms()
  {
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    if (mpcComNode != null)
    {
      mui8Adr = (u8)mpcComNode->mAdr;
      vInitHw();
    }
  }

  void vSupressErrorHandling()
  {
    isNoErrorHandling = True;
  }

  void vResetCom()
  {
    mSm = cComNode::tenState::enStIdle;
  }

  void vError(cComNode::tenError lenError)
  {
    UNUSED(lenError);
  }

  void TIM_EV_IRQHandler()
  {
  }


  void vSetMsg(cComDatMsg* lpcMsg)
  {
    mpcMsg = lpcMsg;
  };

  inline void vMute()
  {
  }

  inline void vDone()
  {
    mSm = cComNode::tenState::enStIdle;
    mpcMsg->vDone();
    mpcComNode->vComDone();
  }


  void vWaitMsg(cComDatMsg* lpcMsg)
  {
    UNUSED(lpcMsg);
    mSm = cComNode::tenState::enStWaitAdress;
  }

  void vStartMsg(cComDatMsg* lpcMsg)
  {
    UNUSED(lpcMsg);
  }

};


#endif // __cUSART_MPHD_H__
