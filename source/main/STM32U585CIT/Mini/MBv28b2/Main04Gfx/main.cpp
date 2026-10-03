#include <stdlib.h>
#include "main.h"

// STM32U5XX
// ARM®-based Cortex®-M33 32b MCU
// Rom 1024KB/2048KB
// Ram 768KB
// 160Mhz
//
// Phase1: Img Bereich im iFlash
//  000000 - 001FFF:   8kb BL
//  002000 - 007FFF:  24kb BLU Img
//
//  008000 - 097FFF: 576kb BLU Img
//  098000 - 0FDFFF: 408kb BLU Img
//
//  0FE000 - 0FFFFF:   8kb Romconst


//// v00.00.06/Base/Misc/Spop/_var/STM32L433U5xx
//// UartMpHd


//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//
//  PB09  -> Status Led
//
//  PA00  -> Wakeup In (100k Pull Down)
//  PC09  -> Wakeup Out
//
//  PC08  -> reserve (auf Stecker)
//  PC09  -> reserve (auf Stecker)
//
//  Board
//    Power Control
//      PE09 <- TPS62125 PG Power Good
//      PE07 -> TPS62125 S1   S2   S1     R2    VOut
//      PE08 -> TPS62125 S2    0    0    620K   2V09
//                             0    1    409k   2V75
//                             1    0    447k   2V59
//                             1    1    325k   3V26
//
//      PC05 -> MX22917_S1: Versorgung I2C ExCom (Switch)
//      PC04 -> MX22917_S2: Versorgung INAs und I2C Board
//
//      PB12 -> Pomo2 TPS55288 enable
//
//    I2C Board
//      PB14 -> SDA2 Board
//      PB13 -> SCL2 Board
//
//    Flash
//      PE10 -> OSPI_P1_CLK
//      PE11 -> OSPI_P1_NCS
//      PE12 -> OSPI_P1_IO0
//      PE13 -> OSPI_P1_IO1
//      PE14 -> OSPI_P1_IO2
//      PE15 -> OSPI_P1_IO3
//
//  Kommunikation
//    ExIn
//      PD13 -> SDA4  (Input)
//      PD12 -> SCL4  (Input)
//      PD08 -> U3 TX (Input)
//
//    ExOut
//      PB00 -> CH1 Enable
//      PB01 -> CH2 Enable
//      PB02 -> CH3 Enable
//
//      PC01 -> SDA3  (Output)
//      PC00 -> SCL3  (Output)
//      PA02 -> U2 TX (Output)
//
//
//            nRF905:            Display:
//      PB03: SPI1 Clock         SPI1 Clock
//      PB04: SPI1 MISO          SPI1 MISO
//      PB05: SPI1 MOSI          SPI1 MOSI
//      PE06: SPI1 Chip Select   SPI1 Display Chip Select
//      PB08: TX or RX mode      SPI1 Touch Chip Select
//      PE00: Standby/CSN        Display Reset
//      PE01: Power up           Display DC
//      PE02: CLK                ---
//      PE03: Carrier Detected   ---
//      PE04: Adress Match       ---
//      PE05: Data Ready         Touch IRQ
//
//      INA1 Adr. 0x40
//        CH1: ExCom CH3
//        CH2: ExCom CH2
//        CH3: ExCom CH1
//
//      INA2 Adr. 0x41
//        CH1: SysIn
//        CH2: PomoIn
//        CH3: PomoOut
//
//      Pomo:
//        TPS55288: I2C; TPS55288 Adr. 0x74 + Tmp102 Adr. 0x90
//
//
//
//  Timer Usage:
//    TIM2  -> SysTick/DiffTimer
//    TIM6  -> Stage 5: 1ms HP Timer
//    TIM7  -> BotCom nRf905
//    TIM16 -> BotCom MpHd
//
//  DMA Usage:
//    GPDMA1:
//      0:
//      1: I2C2 Board
//      2: I2C4 In
//      3: Uart3 In
//      4: I2C3 Out
//      5: Uart2 Out
//      6: Uart1 Rx Debug
//      7: Uart1 Tx Debug
//      8: SPI1 Rx nRf905
//      9: SPI1 Tx nRf905
//     10:
//     11:
//
//
//  Interrupt Usage:
//    #TIM6_IRQHandler:          CyclicCaller:  Prio: 15.8 => 1ms SysTick
//    #I2C1_EV_IRQHandler:       BotCom:        Prio:  8.8
//    #I2C1_ER_IRQHandler:       BotCom:        Prio:  8.8
//    #I2C2_EV_IRQHandler:       Board:         Prio:  8.8
//    #I2C2_ER_IRQHandler:       Board:         Prio:  8.8
//    #EXTI15_10_IRQHandler:     BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA1_Channel2_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA1_Channel3_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    #TIM7_IRQHandler:          BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio:  6.8 => U1
//    #DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio:  6.8 => U1
//    #USART1_IRQHandler:        BotNet U1:     Prio:  6.8 => U1
//    #TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio:  6.8 => U1
//    #USART2_IRQHandler:        BotCom U2:     Prio:  7.8 => U2
//
//
// I2C-Adressen
// 
// Hex(Dez)                           Hex(Dez)                           Hex(Dez)                           Hex(Dez)
// 0x00 (0)  = ____________________ | 0x20 (32) = BAL: PCF8574(def)    | 0x40 (64) = MAIN: INA3221_COM    | 0x60 (96)  = ____________________
// 0x01 (1)  = ____________________ | 0x21 (33) = BAL: PCF8574(opt)    | 0x41 (65) = MAIN: INA3221_BOard  | 0x61 (97)  = ____________________
// 0x02 (2)  = ____________________ | 0x22 (34) = BAL: PCF8574(opt)    | 0x42 (66) = BAL:  INA3221        | 0x62 (98)  = ____________________
// 0x03 (3)  = ____________________ | 0x23 (35) = BAL: PCF8574(opt)    | 0x43 (67) = ____________________ | 0x63 (99)  = ____________________
// 0x04 (4)  = ____________________ | 0x24 (36) = BAL: PCF8574(opt)    | 0x44 (68) = ____________________ | 0x64 (100) = ____________________
// 0x05 (5)  = ____________________ | 0x25 (37) = BAL: PCF8574(opt)    | 0x45 (69) = ____________________ | 0x65 (101) = ____________________
// 0x06 (6)  = ____________________ | 0x26 (38) = BAL: PCF8574(opt)    | 0x46 (70) = ____________________ | 0x66 (102) = ____________________
// 0x07 (7)  = ____________________ | 0x27 (39) = ADDON: PCF8574(def)  | 0x47 (71) = ____________________ | 0x67 (103) = ____________________
// 0x08 (8)  = ____________________ | 0x28 (40) = ____________________ | 0x48 (72) = POMO: TMP102 (def)   | 0x68 (104) = ____________________
// 0x09 (9)  = ____________________ | 0x29 (41) = ____________________ | 0x49 (73) = POMO: TMP102 (opt)   | 0x69 (105) = ____________________
// 0x0A (10) = ____________________ | 0x2B (42) = ____________________ | 0x4A (74) = ____________________ | 0x6A (106) = ____________________
// 0x0B (11) = ____________________ | 0x2B (43) = ____________________ | 0x4B (75) = ____________________ | 0x6B (107) = ADDON: BQ25798(def)
// 0x0C (12) = ____________________ | 0x2C (44) = ____________________ | 0x4C (76) = ____________________ | 0x6C (108) = ____________________
// 0x0D (13) = ____________________ | 0x2D (45) = ____________________ | 0x4D (77) = ____________________ | 0x6D (109) = ____________________
// 0x0E (14) = ____________________ | 0x2E (46) = ____________________ | 0x4E (78) = ____________________ | 0x6E (110) = ____________________
// 0x0F (15) = ____________________ | 0x2F (47) = ____________________ | 0x4F (79) = ____________________ | 0x6F (111) = ____________________
// 0x10 (16) = ____________________ | 0x30 (48) = ____________________ | 0x50 (80) = ____________________ | 0x70 (112) = ____________________
// 0x11 (17) = ____________________ | 0x31 (49) = ____________________ | 0x51 (81) = ____________________ | 0x71 (113) = ____________________
// 0x12 (18) = ____________________ | 0x32 (50) = ____________________ | 0x52 (82) = ____________________ | 0x72 (114) = ____________________
// 0x13 (19) = ____________________ | 0x33 (51) = ____________________ | 0x53 (83) = ____________________ | 0x73 (115) = ____________________
// 0x14 (20) = ____________________ | 0x34 (52) = ____________________ | 0x54 (84) = ____________________ | 0x74 (116) = POMO: TPS55288 (def)
// 0x15 (21) = ____________________ | 0x35 (53) = ____________________ | 0x55 (85) = ____________________ | 0x75 (117) = POMO: TPS55288 (opt)
// 0x16 (22) = ____________________ | 0x36 (54) = ____________________ | 0x56 (86) = ____________________ | 0x76 (118) = ____________________
// 0x17 (23) = ____________________ | 0x37 (55) = ____________________ | 0x57 (87) = ____________________ | 0x77 (119) = ____________________
// 0x18 (24) = ____________________ | 0x38 (56) = ____________________ | 0x58 (88) = ____________________ | 0x78 (120) = ____________________
// 0x19 (25) = ____________________ | 0x39 (57) = ____________________ | 0x59 (89) = ____________________ | 0x79 (121) = ____________________
// 0x1A (26) = ____________________ | 0x3A (58) = ____________________ | 0x5A (90) = ____________________ | 0x7A (122) = ____________________
// 0x1B (27) = ____________________ | 0x3B (59) = ____________________ | 0x5B (91) = ____________________ | 0x7B (123) = ____________________
// 0x1C (28) = ____________________ | 0x3C (60) = ____________________ | 0x5C (92) = ____________________ | 0x7C (124) = ____________________
// 0x1D (29) = ____________________ | 0x3D (61) = ____________________ | 0x5D (93) = ____________________ | 0x7D (125) = ____________________
// 0x1E (30) = ____________________ | 0x3E (62) = ____________________ | 0x5E (94) = ____________________ | 0x7E (126) = ____________________
// 0x1F (31) = ____________________ | 0x3F (63) = ____________________ | 0x5F (95) = ____________________ | 0x7F (127) = ____________________

cDepTree mcSystem;


void NMI_Handler(void)
{
  while (1) {} //Wait for watchdog reset
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  cSysDPool::mSys.mcErr.munErr->stErr.isHardFault = 1;
  while (1) {} //Wait for watchdog reset
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  cSysDPool::mSys.mcErr.munErr->stErr.isMemManage = 1;
  while (1) {} //Wait for watchdog reset
}


void BusFault_Handler(void)
{
  cSysDPool::mSys.mcErr.munErr->stErr.isBusFault = 1;
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1) {} //Wait for watchdog reset
}


void UsageFault_Handler(void)
{
  cSysDPool::mSys.mcErr.munErr->stErr.isUsageFault = 1;
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1) {} //Wait for watchdog reset
}


void DebugMon_Handler(void)
{
  cSysDPool::mSys.mcErr.munErr->stErr.isDebugMon = 1;
  while (1) {} //Wait for watchdog reset
}




#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(u8 *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

#include "cCompBase.h"
#include "cComp3V3.h"
#include "cCompLed.h"
#include "cCompBoardI2C2.h"
#include "cCompBoardMonitor.h"

#include "cCompQSpi1.h"
#include "cCompQSpi1Fs.h"

#include "cCompComDownCntrl.h"

#include "cCompPomoOut1Temp.h"
#include "cCompPomoOut1.h"

#include "cCompAddOn.h"
#include "cCompAddOnBatOut.h"
#include "cCompAddOnUsb.h"
#include "cCompAddOn5V0.h"
#include "cCompAddOn5V0CanFd1.h"
#include "cCompAddOn5V0Out.h"
#include "cCompAddOnCharger.h"

#include "cCompCom.h"
#include "cCompComSideCanFd1Offline.h"
#include "cCompComSideCanFd1Online.h"
#include "cComUpUsbOffline.h"
#include "cComUpUsbOnline.h"


#include "cCompGfxSpi2.h"
#include "cCompGfx.h"


cCompBase mcCompBase;
cComp3V3  mcComp3V3;
cCompLed  mcCompLed;
cCompBoardI2C2     mcCompBoardI2C2;
cCompBoardMonitor  mcCompBoardMonitor;

cCompQSpi1   mcCompQSpi1;
cCompQSpi1Fs mcCompQSpi1Fs;

//cCompPomoOut1Tmp   mcCompPomoOut1Tmp;
//cCompPomoOut1      mcCompPomoOut1;

cCompAddOn          mcCompAddOn;
cCompAddOnUsb       mcCompAddOnUsb;
cCompAddOnBatOut    mcCompAddOnBatOut;
cCompAddOn5V0       mcCompAddOn5V0;
cCompAddOn5V0Out    mcCompAddOn5V0Out;
cCompAddOn5V0CanFd1 mcCompAddOn5V0CanFd1;
cCompAddOnCharger   mcCompAddOnCharger;

cCompCom              mcCompCom;
cCompComUpUsbOffline  mcCompComUpOffline;
cCompComUpUsbOnline   mcCompComUpOnline;

cCompComSideCanFd1Offline  mcCompComSideOffline;
cCompComSideCanFd1Online   mcCompComSideOnline;

cCompGfxSpi2      mcCompGfxSpi2;
cCompGfx          mcCompGfx;

extern void  TaskcDepTreeBase(void* argument);

void TaskMcp(void* argument)
{
  UNUSED(argument);

  cComponentList::macList[cDepTreeCfg::cComp::nBoardMonitor]->vRequestState(cDepTreeRequester::nMcp);


  // 50ms nach Start überprüfen, ob die Versorgungspannung passt
  // Wenn Ja, System weiter hoch fahren, ansonsten System runterfahren
  vTaskDelay(pdMS_TO_TICKS(50));

  if (cSysDPool::mBoard.mcMonitor.mu8SysVoltOk)
  {
    cComponentList::macList[cDepTreeCfg::cComp::nLed]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::nQSpi1Fs]->vRequestState(cDepTreeRequester::nMcp);

    //cComponentList::macList[cDepTreeCfg::cComp::nPomoOut1]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::n5V0Out]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::nBatOut]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::nCharger]->vRequestState(cDepTreeRequester::nMcp);

    cComponentList::macList[cDepTreeCfg::cComp::nComUpOnline]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::nComSideOnline]->vRequestState(cDepTreeRequester::nMcp);
    cComponentList::macList[cDepTreeCfg::cComp::nGfx]->vRequestState(cDepTreeRequester::nMcp);
  }
  else
  {
    cComponentList::macList[cDepTreeCfg::cComp::nBoardMonitor]->vReleaseState(cDepTreeRequester::nMcp);

    // Mcp anhalten, System soll nun in den Sleep gehen.
    vTaskSuspend(NULL);
  }


  while (1)
  {
    if (!cSysDPool::mBoard.mcMonitor.mu8SysVoltOk)
    {
      cComponentList::macList[cDepTreeCfg::cComp::nGfx]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::nComUpOnline]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::nComSideOnline]->vReleaseState(cDepTreeRequester::nMcp);

      cComponentList::macList[cDepTreeCfg::cComp::nCharger]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::nBatOut]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::n5V0Out]->vReleaseState(cDepTreeRequester::nMcp);
      //cComponentList::macList[cDepTreeCfg::cComp::nPomoOut1]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::nQSpi1Fs]->vReleaseState(cDepTreeRequester::nMcp);
      cComponentList::macList[cDepTreeCfg::cComp::nLed]->vReleaseState(cDepTreeRequester::nMcp);

      cComponentList::macList[cDepTreeCfg::cComp::nBoardMonitor]->vReleaseState(cDepTreeRequester::nMcp);

      // Mcp anhalten, System soll nun in den Sleep gehen.
      vTaskSuspend(NULL);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                                   StackType_t** ppxIdleTaskStackBuffer,
                                   u32* pulIdleTaskStackSize)
{
  cSysDPool::mSys.mcTasks.Idle.vInit();
  *ppxIdleTaskTCBBuffer   = &cSysDPool::mSys.mcTasks.Idle.Tcb;
  *ppxIdleTaskStackBuffer = cSysDPool::mSys.mcTasks.Idle.Stack;
  *pulIdleTaskStackSize   = cSysDPool::mSys.mcTasks.Idle.StackSize();
}

void MAIN_vInitComponentList()
{
  mcCompBase.vAdd();
  mcComp3V3.vAdd();
  mcCompLed.vAdd();
  mcCompQSpi1.vAdd();
  mcCompQSpi1Fs.vAdd();
  mcCompBoardI2C2.vAdd();
  mcCompBoardMonitor.vAdd();

  mcCompAddOn.vAdd();
  mcCompAddOnBatOut.vAdd();
  mcCompAddOnUsb.vAdd();
  mcCompAddOn5V0.vAdd();
  mcCompAddOn5V0Out.vAdd();
  mcCompAddOn5V0CanFd1.vAdd();
  mcCompAddOnCharger.vAdd();


  // Com nach Addon, weil USB und CANFD Abhängigkeit
  mcCompCom.vAdd();
  mcCompComUpOffline.vAdd();
  mcCompComUpOnline.vAdd();
  mcCompComSideOffline.vAdd();
  mcCompComSideOnline.vAdd();

  //mcCompPomoOut1Tmp.vAdd();
  //mcCompPomoOut1.vAdd();

  mcCompGfxSpi2.vAdd();
  mcCompGfx.vAdd();
}


void MAIN_vInitSystem(void)
{
  mcSystem.vInit();

  MAIN_vInitComponentList();

  cSysDPool::mSys.mcTasks.Mcp.vInit();

  const osThreadAttr_t TaskMcp_attributes = {
    .name       = "Mcp",                                   ///< name of the thread
    .attr_bits  = 0,                                       ///< attribute bits
    .cb_mem     = &cSysDPool::mSys.mcTasks.Mcp.Tcb,        ///< memory for control block
    .cb_size    = sizeof(cSysDPool::mSys.mcTasks.Mcp.Tcb), ///< size of provided memory for control block
    .stack_mem  = cSysDPool::mSys.mcTasks.Mcp.Stack,       ///< memory for stack
    .stack_size = cSysDPool::mSys.mcTasks.Mcp.StackSize(), ///< size of stack
    .priority   = (osPriority_t)osPriorityNormal,          ///< initial thread priority (default: osPriorityNormal)
    .tz_module  = 0,                                       ///< TrustZone module identifier
    .reserved   = 0                                        ///< reserved (must be 0)
  };

  // Thread mit den Attributen erstellen
  cSysDPool::mSys.mcTasks.Mcp.Handle = osThreadNew(TaskMcp, NULL, &TaskMcp_attributes);

  mcSystem.vStart();
  /* We should never get here as control is now taken by the scheduler */
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  // Uncomment to avoid losing connection with debugger after wakeup from Standby (Consumption will be increased)
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);

  MAIN_vInitSystem();

  while (1)
  {
  }
}


void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  }
}


//@brief  System Clock Configuration
//         The system Clock is configured as follows :
//            System Clock source            = PLL (MSI)
//            SYSCLK(Hz)                     = 160000000
//            HCLK(Hz)                       = 160000000
//            AHB Prescaler                  = 1
//            APB1 Prescaler                 = 1
//            APB2 Prescaler                 = 1
//            MSI Frequency(Hz)              = 4000000
//            PLL_MBOOST                     = 1
//            PLL_M                          = 1
//            PLL_N                          = 80
//            PLL_Q                          = 2
//            PLL_R                          = 2
//            PLL_P                          = 2
//            Flash Latency(WS)              = 4

static void SystemClock_Config_160Mhz(void)
{
  // Enable PWR clock
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

  // Set the regulator supply output voltage
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }

  // Switch to SMPS regulator instead of LDO
  //LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  //while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  //{
  //}

  // Enable MSI oscillator
  LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4); // => 4Mhz
  LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
  LL_RCC_MSIS_Enable();
  while (LL_RCC_MSIS_IsReady() != 1)
  {
  }

  // Set FLASH latency
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  // Configure PLL clock source
  LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

  // Enable the EPOD to reach max frequency
  LL_PWR_EnableEPODBooster();
  while (LL_PWR_IsActiveFlag_BOOST() == 0)
  {
  }

  // Main PLL configuration and activation
  // 4Mhz (MSI)  * 80 / 2 => 160Mhz
  LL_RCC_PLL1_EnableDomain_SYS();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
  LL_RCC_PLL1_SetDivider(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);

  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1)
  {
  }

  // Set AHB, APB1, APB2 and APB3 prescalers
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  // Set PLL1 as System Clock Source
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  // Set systick to 1ms with frequency set to 160MHz
  LL_Init1msTick(160000000);

  // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
  LL_SetSystemCoreClock(160000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}


/*
static void SystemClock_Config_32MHz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;


  // EPOD booster enable wird erst ab 55Mhz benötigt

  // Range is wichtig für SRAM zugriff
  // Range1 für <160 Mhz, bei Range 1 kann man aber flash waitstates auf 0 setzen
  // Range3 für  <55 Mhz, bei Range 3 muss man aber flash waitstates auf 1 setzen
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3);

  // Initializes the CPU, AHB and APB buses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                              | RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  // Set systick to 1ms with frequency set to 160MHz
  LL_Init1msTick(32000000);

  // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
  LL_SetSystemCoreClock(32000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();

  //DCACHE_HandleTypeDef hdcache1;
  //hdcache1.Instance = DCACHE1;
  //hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
  //HAL_DCACHE_Init(&hdcache1);
}
*/


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  SystemClock_Config_160Mhz(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}
