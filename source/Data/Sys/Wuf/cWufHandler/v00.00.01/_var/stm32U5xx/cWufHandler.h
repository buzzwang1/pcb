#ifndef __SYS_WUF_HANDLER_H__
#define __SYS_WUF_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "WufDef.h"
#include "cBuRam.h"

class cWufHandler
{
public:

  static tunWakeupSources munWakeupSources;

  // Für Bootloader: Nothing todo
  // Fpr App: Auf true setzen
  cWufHandler()
  {
    vInit();
  }

  void vInit()
  {
    cBuRam::vEnable();

    munWakeupSources.u32WakeupSources = cBuRam::mBuRam->u32WuReason;

    // Die ResetReasons werden normalerweise vom Bootloader gesetzt
    // Für den Fall, dass keine BL vorhanden ist, manuel setzen
    if ((munWakeupSources.u32WakeupSources == 0) ||
      (cBuRam::mBuRam->u32Bl == 0))
    {
      WufDef_vCheckWuReason(&munWakeupSources);
      cBuRam::mBuRam->u32WuReason = munWakeupSources.u32WakeupSources;
    }
  }

};

#ifdef __cplusplus
}
#endif

#endif /* __SYS_WUF_HANDLER_H__ */
