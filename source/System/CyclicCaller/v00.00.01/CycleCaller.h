#ifndef CYCLIC_CALLER_H
#define CYCLIC_CALLER_H

#include "Typedef.h"
#include "CycleCallerHw.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define CYCCALL_INLINE
#define CYCCALL_CNT 1


typedef void (*CycCall_Callback)(void);

typedef uint32 uintx;

extern void CycCall_Waitms(uintx luixTime);


/* Init Funktion, muss am Start einmal aufgerufen werden
 * Initialsiert den Hardware - Timer
 * HP(High Prio) Task werden durch den Timer Interrupt aufgerufen - Kommen immer
 * LP(Low Prio) werden von CycCall_Idle aufgerufen 
 * Als Übergabe immer Adresse der aufzurufenden Funktionen angeben
 * Bei Übergabe von NULL gibt es keinen Aufruf
 */
extern void CycCall_Start(CycCall_Callback lptrCycCall_Callback_All_1ms_HP,
                          CycCall_Callback lptrCycCall_Callback_All_10ms_HP,
                          CycCall_Callback lptrCycCall_Callback_All_100ms_HP,
                          CycCall_Callback lptrCycCall_Callback_All_1s_HP,

                          CycCall_Callback lptrCycCall_Callback_All_1ms_LP,
                          CycCall_Callback lptrCycCall_Callback_All_10ms_LP,
                          CycCall_Callback lptrCycCall_Callback_All_100ms_LP,
                          CycCall_Callback lptrCycCall_Callback_All_1s_LP);

/* Idle Funktion muss immer aufgerufen werden wenn nicht zu tun ist
 * z.B. in der main() Endlosschleife
 * Hier werden alle LP Task bearbeitet, bzw. aufgerufen 
 */
extern CYCCALL_INLINE void CycCall_vIdle(void);
extern CYCCALL_INLINE void CycCall_vMain_1ms(void);


/* Gibt Ticks seit Systemstart zurück in ms-Raster zurück
 * uintx entsprechend konfigurieren
 */
extern uintx CycCall_uixTimeGetTime(void);

#ifdef __cplusplus
}
#endif

#endif // CyclicCaller
