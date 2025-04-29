#ifndef CYCLIC_CALLER_H
#define CYCLIC_CALLER_H

#include "Typedef.h"
#include "CycleCallerHw.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef void (*CycCall_Callback)(void);

typedef uint32 uintx;

extern void CycCall_Waitms(uintx luixTime);

extern void SysTime_vInit(void);
extern void SysTime_vUpdateTime(void);
extern u32  SysTime_u32TimeGetTime_us(void);
extern u32  SysTime_u32TimeGetTime_ms(void);
extern u32  SysTime_u32GetDiffTime(u32& lu32TimeLast, u32 lu32TimeNew);
extern void SysTime_Delay_us(uint32 lui32Delay);
extern void SysTime_Delay_ms(uint32 lui32Delay);


/* Init Funktion, muss am Start einmal aufgerufen werden
 * Initialsiert den Hardware - Timer
 * HP(High Prio) Task werden durch den Timer Interrupt aufgerufen - Kommen immer
 * LP(Low Prio) werden von CycCall_Idle aufgerufen 
 * Als Übergabe immer Adresse der aufzurufenden Funktionen angeben
 * Bei Übergabe von NULL gibt es keinen Aufruf
 */
extern void CycCall_Start(CycCall_Callback lptrCycCall_Callback_All_1ms_HP,
                          CycCall_Callback lptrCycCall_Callback_All_1ms_LP);

/* Idle Funktion muss immer aufgerufen werden wenn nicht zu tun ist
 * z.B. in der main() Endlosschleife
 * Hier werden alle LP Task bearbeitet, bzw. aufgerufen 
 */
extern void CycCall_vMain_Hp_1ms(void);
extern void CycCall_vIdle(void);



/* Gibt Ticks seit Systemstart zurück in ms-Raster zurück
 * uintx entsprechend konfigurieren
 */

#ifdef __cplusplus
}
#endif

#endif // CyclicCaller
