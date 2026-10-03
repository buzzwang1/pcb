#include "cComponent.h"

cComponent* cComponentList::macList[cDepTreeCfg::cComp::nLast] = {0};

tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> cComponentList::mReqRun;
tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> cComponentList::mReqState;


SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList1ms;
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList16ms;
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList128ms;
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList1024ms;

void cComponentList::vAdd(cComponent* lpcComp)
{
  cComponentList::macList[lpcComp->mu8Idx] = lpcComp;
}
