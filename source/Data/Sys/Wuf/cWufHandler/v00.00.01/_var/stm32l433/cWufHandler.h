#ifndef __SYS_WUF_HANDLER_H__
#define __SYS_WUF_HANDLER_H__

#include "typedef.h"
#include "WufDef.h"
#include "cBuRam.h"

class cWufHandler
{
public:

  static tunWakeupSources *munWakeupSources;

  // Für Bootloader: Nothing todo
  // Fpr App: Auf true setzen
  cWufHandler()
  {
    cBuRam::vEnable();

    munWakeupSources = (tunWakeupSources*)(&cBuRam::mBuRam->u32WuReason);

    // Die ResetReasons werden normalerweise vom Bootloader gesetzt
    // Für den Fall, dass keine BL vorhanden ist, manuel setzen
    if ((munWakeupSources->u32WakeupSources == 0) ||
        (cBuRam::mBuRam->u32Bl == 0))
    {
      WufDef_vCheckWuReason(munWakeupSources);
    }
  }
};

#endif /* __SYS_WUF_HANDLER_H__ */
