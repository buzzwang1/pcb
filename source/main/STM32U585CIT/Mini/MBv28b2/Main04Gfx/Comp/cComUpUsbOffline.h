#pragma once

#include "cSysDPool.h"

class cCompComUpUsbOffline : public cComponent
{
  public:


  cCompComUpUsbOffline()
    : cComponent(cDepTreeCfg::cComp::nComUpOffline, { cDepTreeCfg::cComp::nCom,  cDepTreeCfg::cComp::nUSB })
  {
  }

  bool bInit() override
  {
    //cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    //cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };

};

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


