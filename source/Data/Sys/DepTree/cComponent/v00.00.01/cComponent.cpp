#include "cComponent.h"

cComponent* cComponentList::macList[cDepTreeCfg::cComp::nLast] = {0};

tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> cComponentList::mReqRun;
tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> cComponentList::mReqState;


SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList1ms;    // Liste mit Componenten die alle 1ms aufgerufen werden soll
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList16ms;   // Liste mit Componenten die alle 16ms aufgerufen werden soll
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList128ms;  // Liste mit Componenten die alle 128ms aufgerufen werden soll
SBArry<u8, cDepTreeCfg::cComp::nLast>  cComponentList::mcList1024ms; // Liste mit Componenten die alle 1024ms aufgerufen werden soll

void cComponentList::vAdd(cComponent* lpcComp)
{
  cComponentList::macList[lpcComp->mu8Idx] = lpcComp;
}
