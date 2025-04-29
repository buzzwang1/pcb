
#ifndef __SYS_PKG_PERFORMACE_MONITOR_H__
#define __SYS_PKG_PERFORMACE_MONITOR_H__

#include "TypeDef.h"
#include "tGPPin.h"
#ifdef PCB_PROJECTCFG_Test
  #include "DiffTimer.h"
#endif
#include "cStrT.h"


#ifdef PCB_PROJECTCFG_Test
  class cTestClass
  {
    public:
    u32  mu32TimeStamp;
    void vSetMaxTimer(u32 lu32TimeStamp)
    {
      if (mu32TimeStamp < lu32TimeStamp) mu32TimeStamp = lu32TimeStamp;
    }

    void vSetMaxTimer(u32 lu32TimeStampOld, u32 lu32TimeStampNew)
    {
      u32 mu32TimeStampDiff;

      if (lu32TimeStampNew > lu32TimeStampOld)
      {
        mu32TimeStampDiff = lu32TimeStampNew - lu32TimeStampOld;
      }
      else
      {
        mu32TimeStampDiff = (U32MAX - lu32TimeStampOld) + lu32TimeStampNew;
      }

      if (mu32TimeStamp < mu32TimeStampDiff) mu32TimeStamp = mu32TimeStampDiff;
    }


  };

  class cTestClassLvl: public cTestClass
  {
    public:
    u8   mu8MaxIntLvl;
    void vSetMaxIntLvl(u8 lu8IntLvl)
    {
      if (mu8MaxIntLvl < lu8IntLvl) mu8MaxIntLvl = lu8IntLvl;
    }
  };
#endif

//    PMON Cycle Info
//     0: I2C1_EV_IRQHandler(void)
//     1: I2C1_ER_IRQHandler(void)
//     2: I2C2_EV_IRQHandler(void)
//     3: I2C2_ER_IRQHandler(void)
//     4: EXTI15_10_IRQHandler(void)
//     5: DMA1_Channel2_IRQHandler(void)
//     6: DMA1_Channel3_IRQHandler(void)
//     7: TIM7_IRQHandler(void)
//     8: DMA2_Channel6_IRQHandler(void)
//     9: DMA2_Channel7_IRQHandler
//    10: USART1_IRQHandler
//    11: USART2_IRQHandler(void)
//    12: TIM1_UP_TIM16_IRQHandler(void)
//    13: MAIN_vTick1msHp
//    14: MAIN_vTick10msLp
//    15: MAIN_vTick100msLp
//    16: MAIN_vTick1000msLp


//    PMON Timing Info
//     0: Nach SystemInit
//     1: Start von Main
//     2: Vor I2C Bausteine Initialisieren
//     3: Nach I2C Bausteine Initialisieren
//     4: frei
//     5: frei
//     6: frei
//     7: frei
//     8: Uhrzeit gesetzt
//     9: frei
//    10: frei
//    11: frei
//    12: Start von bExitRun
//    13: Start von PreparePowerdown
//    14: Start von GotoPowerDown
//    15: vor Sleep

#ifdef PCB_PROJECTCFG_Test

class cSysPkgPMon
{
  public:

  u8 mu8IntLvl;
  cTestClassLvl mcTestMaxCyc[17];
  cTestClass    mcTestTim[16];

  cSysPkgPMon();

  void vInit1(void);
  void vInit2(void);
  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
  static void vInitTimer(u32 lu32InitTime_u32);

  static void vPB00_Init();
  static void vPB00_Set1();
  static void vPB00_Set0();

  static void vPB01_Init();
  static void vPB01_Set1();
  static void vPB01_Set0();

  static void vPA05_Init();
  static void vPA05_Set1();
  static void vPA05_Set0();

  static void vPA08_Init();
  static void vPA08_Set1();
  static void vPA08_Set0();
};

#endif 

#endif // __SYS_PKG_PERFORMACE_MONITOR_H__
