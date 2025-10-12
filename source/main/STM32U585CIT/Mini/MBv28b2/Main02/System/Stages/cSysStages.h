#ifndef __SYSTEM_STAGES_H__
#define __SYSTEM_STAGES_H__

#include "cSysStageS1.h"
#include "cSysStageS2.h"
#include "cSysStageS4.h"
#include "cSysStageS5.h"

#ifdef __cplusplus
  extern "C" {
#endif

class cStages
{
  public:
  cStage1 mcS1;
  cStage2 mcS2;
  cStage4 mcS4;
  cStage5 mcS5;  
};

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGES_H__
