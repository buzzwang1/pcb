#pragma once

#include "cSysDPool.h"
#include "cBnMsgSysBtr.h"
#include "cBnMsgSysSpop.h"
#include "cBnMsgSysRRpt.h"
#include "cBnMsgSysMView.h"
#include "cBnMsgSysVDisk.h"
#include "cBnMsgSysFatFs.h"

class cCompCom : public cComponent
{
  public:

  cBotNetCfg mcMyBotNetCfg;
  cBotNet    mcBn;

  cBotNetMsgPortBtr   mcBtr;
  cBotNetMsgPortSpop  mcSpop;
  cBotNetMsgPortRRpt  mcRRpt;
  cBotNetMsgPortMView mcMView;
  cBotNetMsgPortVDisk mcVDisk;
  cBotNetMsgPortFatFs mcFatFs;

  cCompCom()
    : cComponent(cDepTreeCfg::cComp::nCom, { cDepTreeCfg::cComp::nBoardI2C2 }),
    mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcBn(&mcMyBotNetCfg),
    mcBtr(&mcBn),
    mcSpop(&mcBn, &mcBtr),
    mcRRpt(&mcBn),
    mcMView(&mcBn),
    mcVDisk(&mcBn),
    mcFatFs(&mcBn, &cSysDPool::mFs.macFsJob[0], &cSysDPool::mFs.mstFs)
  {
    cSysDPool::mCom.mpcBn = &mcBn;
  }

  bool bInit() override
  {
    cBnMsgPool::vInit();

    // Add MsgSys
    mcBtr.vAddMsgSys();
    mcSpop.vAddMsgSys();
    mcRRpt.vAddMsgSys();
    mcMView.vAddMsgSys();
    mcVDisk.vAddMsgSys();
    mcFatFs.vAddMsgSys();

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
    if (mcSpop.isBusy())
    {
      lszStrBuf.Setf((rsz)"Spop");
      if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
      lcStatus += lszStrBuf;
    }
  
    return lbRet;
  }
};

