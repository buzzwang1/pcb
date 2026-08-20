#pragma once

#include "cSysDPool.h"


class cCompCom : public cComponent
{
  public:

  cBotNetCfg mcMyBotNetCfg;
  cBotNet    mcBn;

  cGpPin mSCK;
  cGpPin mMISO;
  cGpPin mMOSI;

  cCompCom()
    : cComponent(cDepTreeCfg::cComp::nCom, cDepTreeCfg::cComp::nBoardMonitor), 
      mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
      mcBn(&mcMyBotNetCfg),
      mSCK(GPIOB_BASE, 3),
      mMISO(GPIOB_BASE, 4),
      mMOSI(GPIOB_BASE, 5)
  {
    cSysDPool::mCom.mpcBn = &mcBn;
  }

  bool bInit() override
  {
    cBnMsgPool::vInit();

    mSCK.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMISO.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMOSI.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);   // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MOSI  
 
    mcBn.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

    cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    mcBn.vProcess(1000);

    // return True to signal finished
    return cComponent::bRun();
  }

  bool isReadyForSleep(cStr& lcStatus)
  {
    bool lbRet = True;
    cStr_Create(lszStrBuf, 32);
  
    // Warten bis SPOP fertig ist
    if (mcBn.mcSpop.isBusy())
    {
      lszStrBuf.Setf((rsz)"Spop");
      if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
      lcStatus += lszStrBuf;
    }
  
    return lbRet;
  }

};

