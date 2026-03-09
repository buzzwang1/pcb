#ifndef _BURAM_DEF_H
#define _BURAM_DEF_H

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  // 0..15
  u32 u32WuReason;   // Wakeup Reason
  u32 u32WakeupCnt;  // Wird bei jedem Aufwachen um eins erhöht
  u32 u32Err;        // System Fehler, fürs debuggen
  u32 u32Bl;         // Bootloader schreibt hier Wert. Wenn kein BL, dann 0

  // 16..31
  u32 u32SpopMp;     // Spop Magic Pattern "SPOP"
                     // Spop special Reset to enter Bootloader-Updater
  u32 u32SpopBnAdr;  // High 16Bit-BN-DST Adr: z.B. EE12 | Low 16Bit-BN-SRC Adr: z.B. 1EEE

  u32 u32RtcSyncCnt;        // 0: RTC wurde noch nie gesynct, sonst Sync count 
  u32 u32RtcSyncDate;       // 0: RTC wurde noch nie gesynct, sonst letzte SyncZeit

  // 32..47
  u32 u32RtcSyncTime;       // 0: RTC wurde noch nie gesynct, sonst letzte SyncZeit
  u32 u32RtcSyncAttmptDate; // RTC des letzten Sync Versuchs
  u32 u32RtcSyncAttmptTime; // RTC des letzten Sync Versuchs
  u32 u32Reserve1;

  // 48..63
  u32 u32BluReason;         // 0: kein Blu, 1: u8PartitionCnt < 3, 2: isWuIWdg, 3: isWuIWdg during sleep, , 4: SpopMp
  u32 u32BluCnt;            // Wird um 1 erhöht, Blu gestartet wird und wenn u32BluReason < 4
  u32 u32Reserve2;
  u32 u32SysTime_ms;

  // 64..79
  u8 au8LogPos[16]; // Log für 15 Einträge für Programm Position, 16. Eintrag ist Index und zeigt auf den nächsten zu schreibenden

  // 80..128
  u8 au8LogSys[16]; // Log für 15 Einträge für Programm SystemState, 16. Eintrag ist Index und zeigt auf den nächsten zu schreibenden


}tstBuRamDef;


#ifdef __cplusplus
}
#endif

#endif /* _BURAM_DEF_H */
