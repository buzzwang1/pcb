
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB



//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
//
//  PB09  -> Status Led
//
//  PA00   -> Wakeup (100k Pull Down)
//  PA08   -> reserve
//  PB01   -> reserve
//  PB12   -> reserve
//
//  USART1 -> BotCom ComPort (19200)
//  ---
//  ---
//
//  Power Control
//    Battery Control:
//      Balancer:
//      PB00  -> U_Bat4
//      PA06  -> U_Bat3
//      PB02  -> U_Bat2
//      PA15  -> U_Bat1
//
//    Regulator In:
//      PA04  -> Dac
//      PA07  -> En
//
//    Regulator Out:
//      PA05  -> Dac
//      PB08  -> En
//      PA06  -> U_Sys: ADC_IN11
//
//   Switch:
//      PC13: Battery MainSwitch
//      PA10: LowPower(3V3, ADC, I2C)
//
//
//  I2C1  -> BotNet 1.0.0.0
//    PB06  -> I2C1 SCL
//    PB07  -> I2C1 SDA
//
//  U1    -> BotNet 1.E.0.0
//    PA09  -> U1 Tx/Rx
//
//  U2    -> Debug
//    PA02  -> U2 Tx
//    PA03  -> U2 Rx
//
//  I2C2  -> INA3221 2x, Display (Optional) , Apds9960 (Optional)
//    PB10  -> I2C2 SCL
//    PB11  -> I2C2 SDA
//
//
//  nRF905:
//  PB03: SPI1 Clock
//  PB04: SPI1 MISO
//  PB05: SPI1 MOSI
//  PA12: SPI1 Chip Select
//  PB14: TX or RX mode
//  PA11: Standby
//  PB15: Power up
//  PB13: Data Ready
//
//  Timer Usage:
//    TIM2  -> DiffTimer (optional)
//    TIM6  -> CyclicCaller
//    TIM7  -> BotCom nRf905
//    TIM16 -> BotCom MpHd
//
//  DMA Usage:
//    DMA1:
//      0:
//      1:
//      2: SPI1 Rx: NRF905 CS:1
//      3: SPI1 Tx: NRF905 CS:1
//      4: I2C2 Tx: Mainboard  CS:3
//      5: I2C2 Rx: Mainboard  CS:3
//      6: I2C1 Tx: BotNet CS:3 / Usart2 Rx BotNet CS:2
//      7: I2C1 Rx: BotNet CS:3 / Usart2 Tx BotNet CS:2
//
//    DMA2:
//      0:
//      1:
//      2:
//      3:
//      4:
//      5:
//      6: I2C1 Rx: BotNet CS:5 / Usart1 Tx BotNet CS:2
//      7: I2C1 Tx: BotNet CS:5 / Usart1 Rx BotNet CS:2
//
//  Interrupt Usage:
//    TIM6_DAC_IRQHandler:      CyclicCaller:  Prio: 8.8
//    I2C1_EV_IRQHandler:       BotCom:        Prio: 8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio: 8.8
//    I2C2_EV_IRQHandler:       Board:         Prio: 8.8
//    I2C2_ER_IRQHandler:       Board:         Prio: 8.8
//    EXTI15_10_IRQHandler:     BotNet nRF905: Prio: 9.8 => NRF905
//    DMA1_Channel2_IRQHandler: BotNet nRF905: Prio: 9.8 => NRF905
//    DMA1_Channel3_IRQHandler: BotNet nRF905: Prio: 9.8 => NRF905
//    TIM7_IRQHandler:          BotNet nRF905: Prio: 9.8 => NRF905
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio: 6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio: 6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio: 6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio: 6.8 => U1
//    USART2_IRQHandler:        BotCom U2:     Prio: 7.8 => U2
//
//    DiffTimer
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


/*Test*/
cGpPin mcWakeup(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

__IO uint32_t TimingDelay = 0;

cBotNetCfg mcMyBotNetCfg1((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBotNetCfg mcMyBotNetCfg2((rsz)"Virtual Node", 0, cBotNetAdress::GetSlaveAdr(RomConst_stDevice_Info->u16BnNodeAdr, 14));

cStatusLed<GPIOB_BASE,  9> mcStatusLed;
cStatusLed<GPIOA_BASE,  8> mcPA08;
cStatusLed<GPIOB_BASE,  1> mcPB01;
cStatusLed<GPIOB_BASE, 12> mcPB12;

// I2C1:
cGpPin mcI2c1_SCL_Bn(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c1_SDA_Bn(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

// I2C2:
cGpPin mcI2c2_SCL_Board(GPIOB_BASE, 10, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c2_SDA_Board(GPIOB_BASE, 11, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


tcUart<USART2_BASE, GPIOA_BASE, 2, GPIOA_BASE, 3> mcComPort2(38400, GPIO_AF7_USART2, 16, 16);


cDigiPower           mcDigiPower(1); // DigiPower (3V3) vor I2C init, sonst error in I2C_Init.CheckPins

cI2cMaster   mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 8);

cWorkMode            mcOpMode;
cLipoMon             mcLipoMon(&mcI2C2_Board);
cMP8859              mcMP8859(&mcI2C2_Board, MP8859_I2C_ADDRESS_CONF_0);

cClock               mClock;   // Clock muss vor Powermanager sein, weil der Powermanager die
                               // WUF- zurücksetzt.
u32                  mu32ClockResync;
u32                  mu32ClockResyncTimeout;

u32                  mu32NoSleepCounter;
u32                  mu32NoSleepCounterReload;

#ifdef PCB_PROJECTCFG_Test
  class cTestClass
  {
  public:
    u8   mu8MaxIntLvl;
    u32  mu32MaxTimer;

    void vSetMaxIntLvl(u8 lu8IntLvl)
    {
      if (mu8MaxIntLvl < lu8IntLvl) mu8MaxIntLvl = lu8IntLvl;
    }

    void vSetMaxTimer(u32 lu32Timer)
    {
      if (mu32MaxTimer < lu32Timer) mu32MaxTimer = lu32Timer;
    }
  };

  u8         mu8IntLvl = 0;
  cTestClass mcTestClass[17];
#endif

class cBn_MsgProcess_0x1000 : public cBotNet_MsgSysProcess
{
public:

  bool bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn)
  {
    bool lbConsumed = False;
    (void)lpcBn;
    (void)lcMsg;

    switch (lcMsg.mu16Idx)
    {
      case 0x11:
        // Überprüfen, ob es eine Clock Nachricht ist
        //                         0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
        //                              0  1    2    3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
        //  Clock:                11 | DH DL | 10 | 00.XX.XX.XX XX.XX.XX.XX YH.YL.MM.DD HH.MM.SS.VV || 00 00 SA SA 80
        // Clock Nachricht ?
        if ((lcMsg.cGetSAdr() == 0x1000) &&
            (lcMsg.u8Len() == 25))
        {
          u16 lu16SourceAdr = (lcMsg.mcPayload[0] << 8) + lcMsg.mcPayload[1];
          if (lu16SourceAdr == 0x08) // Clock Device Adress
          {
            mClock.vDeserialize(&lcMsg.mcPayload[11]);
            mu32ClockResyncTimeout = 0;
            lbConsumed = True;
          }
        }
        break;

      case 0x21: // Set Idx
        switch (lcMsg.mcPayload[0])
        {
          case 10: // VoltageConverterOut
            u16 lu16Value = (lcMsg.mcPayload[3] << 8) + lcMsg.mcPayload[2];
            if (lu16Value > 0)
            {
              mcLipoMon.mcPowerOut.vSetVoltage(lu16Value);
              mcLipoMon.mcPowerOut.vEnable();
            }
            else
            {
              mcLipoMon.mcPowerOut.vDisable();
            }

            lcMsg.vFlipAdress();
            lcMsg.vEncode();
            lpcBn->bSendMsg(&lcMsg);

            lbConsumed = True;
            break;
        }
        break;
    }

    if (lcMsg.u8Len() == 25)
    {
    }
    /*
    // Überprüfen, ob es eine LightBox Nachricht ist
    if ((lcMsg.cGetSAdr() == 0x1200) &&
        (lcMsg.mu16Idx  == 0x50))
    {
      // Weitersenden an Motbox
      cBotNetMsg_Static_MsgProt_Create_Copy(lcMsg, lcMsgLightStatus, 32);

      lcMsgLightStatus.mcFrame.mcSAdr.Set(0x1000);
      lcMsgLightStatus.mcFrame.mcDAdr.Set(0x1100);
      lcMsgLightStatus.vEncode();

      lpcBn->bSendMsg(&lcMsgLightStatus);
    }*/
    return lbConsumed;
  }
};


// BotNet
cBn_MsgProcess_0x1000 mcBn_MsgProcess_0x1000;
cBotNet               mcBn_0x1000(&mcMyBotNetCfg1, &mcBn_MsgProcess_0x1000);
//  0x1000 Masternode for 011[x] all nodes, e.g. downstream to 0111
// 0 CmdPort
// 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)


// --- 0xE000 SideLink => PC
cNRF905                   mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905      mcSideLinkRf(&mcNRF905);
cBotNet_UpLinknRf905Net   mcSideLink(&mcSideLinkRf, &mcBn_0x1000, 0);
cBotNet_LinkBotCom        mcSideLinkBotCom(&mcComPort2);

// --- 0x1000 DownLinks
cI2cMaster  mcI2C1_BnMaster(I2C1, &mcI2c1_SCL_Bn, &mcI2c1_SDA_Bn, 16);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1100(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1200(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1300(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1400(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1500(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1600(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1700(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1800(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1900(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1A00(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1B00(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1C00(&mcI2C1_BnMaster);
cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1D00(&mcI2C1_BnMaster);
cBotNet_LinkVLink_DownLink mcDownLinks_0x1000_to_0x1E00;



// --- 0x1E00 -----
cBotNet               mcBn_0x1E00(&mcMyBotNetCfg2, null);

// --- 0x1E00 Uplink
cBotNet_LinkVLink_UpLink   mcUpLinks_0x1E00_to_0x1000;

// --- 0x1E00 DownLinks
cGpPin                     mcU1TxRx(GPIOA_BASE, 9, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cUartMpHdMaster            mcMasterUartMpHdU1(USART1, cBotNet::enCnstSlaveCnt, 200000);

cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E10(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E20(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E30(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E40(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E50(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E60(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E70(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E80(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E90(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EA0(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EB0(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EC0(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1ED0(&mcMasterUartMpHdU1);
cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EE0(&mcMasterUartMpHdU1);


inline void vEnableIrqRtcWut()
{
  __HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();
}

inline void vDisableIrqRtcWut()
{
  __HAL_RTC_WAKEUPTIMER_EXTI_DISABLE_IT();
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_20); // 20 = RTC wakeup timer
}

void RTC_WKUP_IRQHandler(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_20);
  vDisableIrqRtcWut();
}


inline void vEnableIrqPa0()
{
  EXTI->IMR1 |= (1 << 0);
}

inline void vDisableIrqPa0()
{
  EXTI->IMR1 &= ~(1 << 0);
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_0);
}

void EXTI0_IRQHandler(void)
{
  __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0);
  vDisableIrqPa0();
}


class cMySystemPowerDown : public cSystemPowerDown
{
  void vPreparePA0_Exti_For_Stop()
  {
    // Configurate Wakeup Pin
    {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL,   GPIO_SPEED_FREQ_LOW, 0); }
    //{cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0); } // Only in case of no external Pull Down Resistor

    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0);

    // Enable clock for SYSCFG
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    // Tell system that you will use PA0 for EXTI_Line0
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);

    //EXTI init
    LL_EXTI_InitTypeDef EXTI_InitStruct;
    // PA0 is connected to EXTI_Line0
    EXTI_InitStruct.Line_0_31   = LL_EXTI_LINE_0;
    EXTI_InitStruct.Line_32_63  = LL_EXTI_LINE_NONE;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    //NVIC init
    // Add IRQ vector to NVIC
    HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  }

  void vPreparePA0_Exti_For_StandBy()
  {
    // Configurate Wakeup Pin
    {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); }

    /* Disable all used wakeup sources: WKUP pin */
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

    /* Clear wake up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    PWR->CR3 |= PWR_CR3_EWUP1; // Enable Wakeup pin WKUP1 (PA0)
  }

  void vSetRtcPC13()
  {
    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
    LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
    LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high
  }

  void vPrepareRtc_Exti(u16 lu16WakeUpTime)
  {
    if (lu16WakeUpTime)
    {
      // EXTI init for WakeupTimer WUT -> EXTI line 20
      LL_EXTI_InitTypeDef EXTI_InitStruct;
      // PA0 is connected to EXTI_Line0
      EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_20;
      EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
      EXTI_InitStruct.LineCommand = ENABLE;
      EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
      EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
      LL_EXTI_Init(&EXTI_InitStruct);

      //EXTI->SWIER1 |= 1 << 20; // Software interrupt on EXTI20

      // NVIC init
      // Add IRQ vector to NVIC
      HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
      HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

      LL_RTC_DisableWriteProtection(RTC);

      // Disable wake up timer to modify it
      LL_RTC_WAKEUP_Disable(RTC);

      while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1) {}

      LL_RTC_WAKEUP_SetAutoReload(RTC, lu16WakeUpTime);
      LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);

      LL_RTC_WAKEUP_Enable(RTC);
      LL_RTC_EnableIT_WUT(RTC);

      // Enable RTC registers write protection
      LL_RTC_EnableWriteProtection(RTC);

      LL_RTC_ClearFlag_WUT(RTC);

      //__HAL_RCC_RTCAPB_CLK_SLEEP_ENABLE();   // RTC APB clock enabled by the clock gating(1) during Sleep and Stop modes
      //__HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();
    }
  }

  // 1. stage PowerDown. Should be used to inform connected
  //    devices to go to powerdown
  bool bExitRun() override
  {
    bool lbRet = True;


    // Warten bis Uhrzeit empfangen wurde
    if (mu32ClockResyncTimeout > 0)
    {
      lbRet = False;
    }

    //Warten bis WakeUp Pin Pa01 low ist
    if (mcWakeup.ui8Get() == 1)
    {
      lbRet = False;
      mu32NoSleepCounter = mu32NoSleepCounterReload;
    }

    //Warten bis Counter abgelaufen ist
    if (mu32NoSleepCounter)
    {
      lbRet = False;
    }

    // Warten solange active Funk-Verbindung besteht
    if (mcBn_0x1000.mcSideLink->mStatus.IsOnline)
    {
      lbRet = False;
    }

    if (lbRet)
    {
      lbRet &= mcLipoMon.bPowerDown();
    }

    return lbRet;
  }

  // 2. stage. Stop communication
  bool bPreparePowerdown() override
  {
    return True;
  }

  // 3. goto PowerDown. Turn everything off, that is not needed.
  //    Bring everything to a defined/safe state
  bool bGotoPowerDown() override
  {
    HAL_SuspendTick();
    mcStatusLed.Off();
    mcDigiPower.vPowerOff();

    return True;
  }

  // 4. Set WakeUp Sources
  void vEnterPowerDown(u16 lu16WakeUpTime)
  {
    vSetRtcPC13();
    vPreparePA0_Exti_For_StandBy();
    vPrepareRtc_Exti(lu16WakeUpTime);

    // LSI wird im Shutdown deaktiviert, LSE geht noch im Shutdown
    //          | LSI | LSE | IWDG
    // ---------+-----+-----+-----
    // Standby  |  x  |  X  |  X
    // Shutdown |  -  |  X  |  -
    //
    // IWDG kann in den Option Bytes im Standby deaktiviert werden
    //
    //
    //
    __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
    cBnSpop_vResetWdog();
    //
    //if (mClock.mu8ClockSource == 1) // LSE ?
    //{
      // Shutdown
      //HAL_PWREx_EnterSHUTDOWNMode();
    //}
    //else
    //{
      // Standby
      HAL_PWR_EnterSTANDBYMode();
    //}

  }

  // Continue after PowerDown
  bool bContinue() override
  {
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    SystemInit();
    HAL_Init();
    SystemClock_Config(); // Decomment for 16Mhz HSI
    SystemCoreClockUpdate();

    MAIN_vInitSystem(True);
    return True;
  }
};

cMySystemPowerDown mcMySystemPowerDown;
cPowerManager mcPowerManager(11 /* *[10ms] RunTimer */, 0 /* *[10ms] ExitRunTimer*/, 0 /* *[10ms] PreparePowerDownTimer*/, 15 /* [s] WakeUpTime*/, &mcMySystemPowerDown);
//cPowerManager mcPowerManager(400 /* *[10ms] RunTimer */, 0 /* *[10ms] ExitRunTimer*/, 0 /* *[10ms] PreparePowerDownTimer*/, 2 /* [s] WakeUpTime*/, &mcMySystemPowerDown);


void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


void EnableRTC(void)
{
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
}


class cCliCmd_SysInfo2 : public cCliCmd
{
public:
  cCliCmd_SysInfo2() :cCliCmd((const char*)"SysInfo2", (const char*)"System Info")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lcCli->bPrintLn((const char8*)"Clock:");
    lszStr.Setf((const char8*)"Core: %dMHz", cClockInfo::u32GetClockCore() / 1000000); lcCli->bPrintLn(lszStr);
    lszStr.Setf((const char8*)"Sys:  %dMHz", cClockInfo::u32GetClockSys() / 1000000); lcCli->bPrintLn(lszStr);
    lszStr.Setf((const char8*)"Port: %dMHz", cClockInfo::u32GetClockPort() / 1000000); lcCli->bPrintLn(lszStr);

    if (FLASH->ACR & FLASH_ACR_ICEN) { lszStr.Setf((const char8*)"Flash Data cache: enable");  lcCli->bPrintLn(lszStr); }
    else { lszStr.Setf((const char8*)"Flash Data cache: disable"); lcCli->bPrintLn(lszStr); }

    if (FLASH->ACR & FLASH_ACR_DCEN) { lszStr.Setf((const char8*)"Flash Instruction cache: enable");  lcCli->bPrintLn(lszStr); }
    else { lszStr.Setf((const char8*)"Flash Instruction cache: disable"); lcCli->bPrintLn(lszStr); }

    if (FLASH->ACR & FLASH_ACR_PRFTEN) { lszStr.Setf((const char8*)"Flash Prefetch: enable");  lcCli->bPrintLn(lszStr); }
    else { lszStr.Setf((const char8*)"Flash Prefetch: disable"); lcCli->bPrintLn(lszStr); }

    u8 lu8Lat = (u8)((u32)(FLASH->ACR & FLASH_ACR_LATENCY_Msk) >> FLASH_ACR_LATENCY_Pos);
    lszStr.Setf((const char8*)"Flash Latency: %d", lu8Lat); lcCli->bPrintLn(lszStr);

    return True;
  }
};

class cCliCmd_SysClock : public cCliCmd
{
public:
  cCliCmd_SysClock() :cCliCmd((const char*)"Clock", (const char*)"System Time")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lcCli->bPrintLn((const char8*)"Time:");
    //lszStr.Setf((const char8*)"%02d:%02d:%02d", __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC))); lcCli->bPrintLn(lszStr);
    lcCli->bPrintLn(mClock.toString(lszStr));
    if (mClock.IsValid()) lcCli->bPrintLn((const char8*)"Is Valid");
                     else lcCli->bPrintLn((const char8*)"Is Not Valid");

    return True;
  }
};


class cCliCmd_SetVolt : public cCliCmd
{
public:
  cCliCmd_SetVolt() :cCliCmd((const char*)"SetVolt", (const char*)"idx,1=on/0=off,[mV]")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[16];
    cStr   lszStr(lszStrBuf, 16);


    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    if (mcOpMode.IsAuto())
    {
      lcCli->bPrintLn((const char8*)"Manual mode required");
    }
    else
    {
      mcOpMode.vSet(cWorkMode::nStManuel);

      if (lcParam.Len() != 0)
      {
        u16 lu16PosKomma1 = lcParam.Instr(0, ',');
        u16 lu16PosKomma2 = lcParam.Instr(lu16PosKomma1 + 1, ',');

        u16 lu16Idx = 0xFF;
        u16 lu16Mode = 0;
        u16 lu16Volt = 0;;

        if ((lu16PosKomma1 != (u16)-1) && (lu16PosKomma2 != (u16)-1))
        {
          lszStr = lcParam;
          lszStr.Mid(0, lu16PosKomma1 - 0);
          lu16Idx = lszStr.Atoi();

          lszStr = lcParam;
          lszStr.Mid(lu16PosKomma1 + 1, lu16PosKomma2 - lu16PosKomma1 - 1);
          lu16Mode = lszStr.Atoi();

          lszStr = lcParam;
          lszStr.Mid(lu16PosKomma2 + 1, 0xFFFF);
          lu16Volt = lszStr.Atoi();
        }

        cVoltageConverter* lcVc = null;
        if (lu16Idx == 0) lcVc = &mcLipoMon.mcPowerIn;
        if (lu16Idx == 1) lcVc = &mcLipoMon.mcPowerOut;

        if (lcVc != null)
        {
          if (lu16Mode == 0) lcVc->vDisable();
          if (lu16Mode == 1) lcVc->vEnable();
          lcVc->vSetVoltage(lu16Volt);
          lcCli->bPrintLn((rsz)"ok");
        }
      }
    }
    return True;
  }
};

class cCliCmd_HwStatus : public cCliCmd
{
public:
  cCliCmd_HwStatus() :cCliCmd((const char*)"hwstatus", (const char*)"Status")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    if (lbFirstCall)
    {
      u16 lu16Mode;
      lu16Mode = 0;
      if (mcLipoMon.mcPowerIn.isEnable()) lu16Mode = 1;
      lszStr.Setf((rsz)"Power In: %d, %d mV", lu16Mode, mcLipoMon.mcPowerIn.u16GetVoltage()); lcCli->bPrintLn(lszStr);
      lu16Mode = 0;
      if (mcLipoMon.mcPowerOut.isEnable()) lu16Mode = 1;
      lszStr.Setf((rsz)"Power Out: %d, %d mV", lu16Mode, mcLipoMon.mcPowerOut.u16GetVoltage()); lcCli->bPrintLn(lszStr);

      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Switches:");
      lszStr.Setf((rsz)"Bat:  %d", (u16)mcLipoMon.lenSmS[cLipoMon::enS1_Lipo]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bal1: %d", (u16)mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo1]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bal2: %d", (u16)mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo2]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bal3: %d", (u16)mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo3]); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bal4: %d", (u16)mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo4]); lcCli->bPrintLn(lszStr);
      return False;
    }
    else
    {
      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Adc:");
      lszStr.Setf((rsz)"Bat1: %d mV -> %d mV", (i16)mcLipoMon.i16GetBatVoltageAbs(1), (i16)mcLipoMon.i16GetBatVoltageRel(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bat2: %d mV -> %d mV", (i16)mcLipoMon.i16GetBatVoltageAbs(2), (i16)mcLipoMon.i16GetBatVoltageRel(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bat3: %d mV -> %d mV", (i16)mcLipoMon.i16GetBatVoltageAbs(3), (i16)mcLipoMon.i16GetBatVoltageRel(3)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bat4: %d mV -> %d mV", (i16)mcLipoMon.i16GetBatVoltageAbs(4), (i16)mcLipoMon.i16GetBatVoltageRel(4)); lcCli->bPrintLn(lszStr);

      lcCli->bPrintLn((rsz)"");
      lcCli->bPrintLn((rsz)"Ina:");
      lszStr.Setf((rsz)"Board U: %d", (i16)mcLipoMon.i16GetSysVoltageAbs(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Board I: %d", (i16)mcLipoMon.i16GetSysCurrentAbs(1)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bat   U: %d", (i16)mcLipoMon.i16GetSysVoltageAbs(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Bat   I: %d", (i16)mcLipoMon.i16GetSysCurrentAbs(2)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Sys:  U: %d", (i16)mcLipoMon.i16GetSysVoltageAbs(3)); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"Sys:  I: %d", (i16)mcLipoMon.i16GetSysCurrentAbs(3)); lcCli->bPrintLn(lszStr);

      lcCli->bPrintLn((rsz)"");
      lszStr.Setf((const char8*)"Mode %s (%d)", mcOpMode.ToString(), mcOpMode.mCounter);
      lcCli->bPrintLn(lszStr);
    }
    return True;
  }
};

class cCliCmd_SleepStatus : public cCliCmd
{
public:
  cCliCmd_SleepStatus() :cCliCmd((const char*)"sleepstatus", (const char*)"Sleep Status")
  {}

  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    if (lbFirstCall)
    {
      lszStr.Setf((rsz)"ClockResyncTimeout: %d", (u16)mu32ClockResyncTimeout);                   lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"NoSleepCounter:     %d", (u16)mu32NoSleepCounter);                       lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"WakeUp Pin:         %d", (u16)mcWakeup.ui8Get());                        lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"SideLink Status:    %d", (u16)mcBn_0x1000.mcSideLink->mStatus.IsOnline); lcCli->bPrintLn(lszStr);
      lszStr.Setf((rsz)"LipoMon:            %d", (u16)mcLipoMon.bPowerDown());                   lcCli->bPrintLn(lszStr);
    }
    return True;
  }
};

class cCliCmd_CcStatus: public cCliCmd
{
  public:
    cCliCmd_CcStatus():cCliCmd((const char*)"cc_status", (const char*)"Light Control Status")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lszStr.Setf((const char8*)"Mode %s (%d)", mcOpMode.ToString(), mcOpMode.mCounter);
      lcCli->bPrintLn(lszStr);

      /*lszStr.Setf((const char8*)"Dreh-Mot: %d-%d", mcMot_TurmDrehung.mui16ActualPos, mcMot_TurmDrehung.mui16TargetPos);
        mcPrinter->vPrintLn(lszStr.ToString());
        lszStr.Setf((const char8*)"Neig-Mot: %d-%d", mcMot_TurmNeigung.mui16ActualPos, mcMot_TurmNeigung.mui16TargetPos);
        mcPrinter->vPrintLn(lszStr.ToString());*/
      return True;
    }
};


class cCliCmd_CcSetMode: public cCliCmd
{
  public:
    cCliCmd_CcSetMode():cCliCmd((const char*)"cc_setmode", (const char*)"a=auto m=manual")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (lcParam.Instr(0, (const char8*)"a") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        mcOpMode.vSet(cWorkMode::nStAuto);
      }
      else if (lcParam.Instr(0, (const char8*)"m") == 0)
      {
        lcCli->bPrintLn((const char8*)"ok");
        mcOpMode.vSet(cWorkMode::nStManuel);
      }
      return True;
    }
};


class cCliCmd_CcSetAdc: public cCliCmd
{
  public:
    cCliCmd_CcSetAdc():cCliCmd((const char*)"cc_setdac", (const char*)"x=value in mV")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lcCli);
      UNUSED(lCallerAdr);

      /*if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Manual mode required");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        i16 li16Value = (lcParam.Atoi() - 1);
        mu8DispPageX = ((u8)li16Value);
      }*/

      return True;
    }
};

class cCliCmd_CcSetSwitch: public cCliCmd
{
  public:
    cCliCmd_CcSetSwitch():cCliCmd((const char*)"cc_sets", (const char*)"x=swtich to set")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lcCli);
      UNUSED(lCallerAdr);

      /*if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Manual mode required");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        i16 li16Value = (lcParam.Atoi() - 1);
        mu8DispPageX = ((u8)li16Value);
      }*/

      return True;
    }
};




class cMiniCli : public cBotNetStreamPort
{
  public:
  cCliCmd_SysInfo2    mcBnCliCmd_SysInfo2;
  cCliCmd_SysClock    mcBnCliCmd_Clock;
  cCliCmd_HwStatus    mcBnCliCmd_HwStatus;
  cCliCmd_SleepStatus mcBnCliCmd_SleepStatus;
  cCliCmd_SetVolt     mcBnCliCmd_SetVolt;
  cCliCmd_CcStatus    mcBnCliCmd_CcStatus;
  cCliCmd_CcSetMode   mcBnCliCmd_CcSetMode;
  cCliCmd_CcSetAdc    mcBnCliCmd_CcSetAdc;
  cCliCmd_CcSetSwitch mcBnCliCmd_CcSetSwitch;

  cMiniCli()
  {
    mszName     = (const char*)"BnBoardCntl v01.00.00";
  }

  inline uint32 SizeOf()
  {
    return sizeof(cCliCmd);
  }

  void vAddedToBn(cBotNet* lcBotNet) override
  {
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SysInfo2);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Clock);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_HwStatus);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SleepStatus);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetVolt);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcStatus);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetMode);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetAdc);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetSwitch);
  }


  void vProcess(void) override
  {
  }
};

cMiniCli mcMiniCli;


void I2C1_EV_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  mcI2C1_BnMaster.I2C_EV_IRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[0].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[0].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void I2C1_ER_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  mcI2C1_BnMaster.I2C_ER_IRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[1].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[1].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void I2C2_EV_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  mcI2C2_Board.I2C_EV_IRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[2].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[2].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void I2C2_ER_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  mcI2C2_Board.I2C_ER_IRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[3].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[3].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}



void EXTI15_10_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_13) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_13);
    mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[4].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[4].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void DMA1_Channel2_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC2;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[5].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[5].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void DMA1_Channel3_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC3;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[6].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[6].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void TIM7_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[7].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[7].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void DMA2_Channel6_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  // USART1 TX
  DMA2_Channel6->CCR &= ~DMA_CCR_EN;
  DMA2->IFCR = DMA_FLAG_TC6;
  mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[8].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[8].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void DMA2_Channel7_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  // USART1 RX
  DMA2_Channel7->CCR &= ~DMA_CCR_EN;
  DMA2->IFCR = DMA_FLAG_TC7;
  mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[9].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[9].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void USART1_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  if (USART1->ISR & LL_USART_ISR_TC)
  {
    USART1->ICR = LL_USART_ISR_TC;
    mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
  }
  if (USART1->ISR & 0xF)
  {
    USART1->ICR = LL_USART_ISR_ORE;
    mcMasterUartMpHdU1.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[10].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[10].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void USART2_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  mcComPort2.vIRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[11].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[11].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void TIM1_UP_TIM16_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcMasterUartMpHdU1.TIM_EV_IRQHandler();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[12].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[12].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void MAIN_vTick1msHp(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;

    mcPA08.vSet1();
  #endif

  if (mcPowerManager.enSysState == cPowerManager::tenSysStates::nStRun)
  {
    mcBn_0x1000.vTickHp1ms();
    mcBn_0x1E00.vTickHp1ms();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA08.vSet0();

    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[13].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[13].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}

void MAIN_vTick10msLp(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;
  #endif

  mcPowerManager.vTick10ms();
  mcLipoMon.vUpdateAll(mcOpMode.IsAuto());

  if (mcPowerManager.enSysState == cPowerManager::tenSysStates::nStRun)
  {
    mcBn_0x1000.vTickLp10ms();
    mcBn_0x1E00.vTickLp10ms();
    mcI2C2_Board.bStartNext();
    mcI2C2_Board.vTick10ms();
  }

  #ifdef PCB_PROJECTCFG_Test
    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[14].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[14].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}




void MAIN_vTick100msLp(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;
  #endif

  mcStatusLed.vTick100ms();


  cBnSpop_vResetWdog();

  #ifdef PCB_PROJECTCFG_Test
    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[15].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[15].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void MAIN_vTick1000msLp(void)
{
  #ifdef PCB_PROJECTCFG_Test
    u32 lu32TimStart = u32GetTimer();
    u32 lu32TimEnd; mu8IntLvl++;
  #endif

  static u8 lu2s    = 0;

  mcMP8859.i8SetVoltage_mV(4200);

  if (lu2s < 2)
  {
    lu2s++;

    if (mcOpMode.IsAuto())
    {
      mcStatusLed.vSetMode(cStatusLed<GPIOB_BASE, 9>::nStToggle500ms);
    }
    else
    {
      mcStatusLed.vSetMode(cStatusLed<GPIOB_BASE, 9>::nStToggle200ms);
    }
  }
  else
  {
    lu2s = 0;
  }

  mcOpMode.vTick1000ms();

  mClock.vAdd1s();

  if (mu32ClockResync)
  {
    mu32ClockResync--;
  }
  else
  {
    mu32ClockResyncTimeout = 3 * 60;
    mu32ClockResync        = 24 * 60 * 60;
    mcSideLink.vKeepReceiverOnWhileWaiting(True);
  }

  if (mu32ClockResyncTimeout)
  {
    mu32ClockResyncTimeout--;
  }
  else
  {
    mcSideLink.vKeepReceiverOnWhileWaiting(False);
  }

  if (mu32NoSleepCounter) mu32NoSleepCounter--;


  //vEnterShutDown(5);

  ////u8 MyMsg[64] = {0x20,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  //00,01,02,03,04,05,06,07,
  ////                  00,01,02,0x1E,0x10,0xE0,0x00,0x80};
  ////
  ////cbArrayExtT<uint16> lcMyMsg(MyMsg, 64-6*8);
  ////
  ////lcMyMsg.muiLen = 64-6*8;
  ////
  //////Empfangnachricht faken.
  ////mcSideLinkRf.mcRxComBuf.put(&lcMyMsg);


  #ifdef PCB_PROJECTCFG_Test
    lu32TimEnd = u32GetTimer();
    if (lu32TimEnd > lu32TimStart)
    {
      mcTestClass[16].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    }
    mcTestClass[16].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  #endif
}


void MAIN_vInitSystem(bool bFirstCall)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  cBnSpop_vResetWdog();
  HAL_Init();


  // Return PC13 control to gpio
  EnableRTC();
  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_DISABLE);

  #ifdef PCB_PROJECTCFG_Test
    InitTimer();
  #endif

  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 7, 8);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  mcDigiPower.vPowerOn();

  mu32ClockResync = 24 * 60 * 60;
  if (!mClock.IsValid())
  {
    mu32ClockResyncTimeout = 3 * 60;
  }
  else
  {
    mu32ClockResyncTimeout = 0;
  }

  mu32NoSleepCounterReload = 60*10; // 60s * 10
  if (mcWakeup.ui8Get() == 1)
  {
    mu32NoSleepCounter = mu32NoSleepCounterReload;
  }
  else
  {
    mu32NoSleepCounter = 0;
  }

  if (bFirstCall)
  {
    // Virtuelle Ports verbinden
    mcUpLinks_0x1E00_to_0x1000.vConnect((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1E00);

    #ifdef PCB_PROJECTCFG_Test
      mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcSideLinkBotCom, 0xE000);
	  #else
      mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
	  #endif

    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1100);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1200);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1300);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1400);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1500);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1600);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1700);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1800);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1900);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1A00);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1B00);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1C00);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1D00);
    mcBn_0x1000.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1000_to_0x1E00);
    mcBn_0x1000.vSetHandleHardware( 1, True);
    mcBn_0x1000.vSetHandleHardware(14, True);


    // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
    mcBn_0x1000.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);


    // Add LightControl Port (not connected)
    mcBn_0x1000.u16StreamPortAdd(&mcMiniCli);


    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcUpLinks_0x1E00_to_0x1000);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E10);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E20);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E30);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E40);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E50);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E60);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E70);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E80);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1E90);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EA0);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EB0);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EC0);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1ED0);
    mcBn_0x1E00.bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1E00_to_0x1EE0);
    mcBn_0x1E00.vSetHandleHardware(1, True);

    // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
    mcBn_0x1E00.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);
  }

  mcSideLink.vSetTiming(15*1000, 50);
  mcSideLink.vKeepReceiverOnWhileWaiting(True);

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  cBnSpop_vResetWdog();

  if (mcI2C2_Board.bStartNext())
  {
    cClockInfo::Delay_ms(5);
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  if ((mcPowerManager.munWuSources.stWakeupSources.isWuPinRst) ||
      (mcPowerManager.munWuSources.stWakeupSources.isWuSftRst))
  {
    mClock.vClear();
    MAIN_vInitSystem(True);
    mcPowerManager.vStart();
  }
  else
  if ((mcPowerManager.munWuSources.stWakeupSources.isWuTimerRtc) &&
      (mcPowerManager.munWuSources.stWakeupSources.isWuStandBy))
  {
    mcPowerManager.vContinueAfterWakeup();
  }
  else
  {
    MAIN_vInitSystem(True);
    mcPowerManager.vStart();
  }

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  // SystemClock = HSI (== 16Mhz)
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = //RCC_PERIPHCLK_USART3| // Wird nicht benötigt
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART1|
                                       //RCC_PERIPHCLK_I2C3|   // Wird nicht benötigt
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  //PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI; // Wird nicht benötigt
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  //PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI; // Wird nicht benötigt
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  if (!RomConst_IsValid())
  {
    // RomConst Error
    while (1)
    {
      __asm("nop");
    }
  }

  // 0x20007800
  //   Heap 32k
  // 0x2000F800
  //   Stack 2k
  // 0x20010000
  cMemTools::vMemSet((u8*)0x2000F800, 0x55, 0x800 - 128);
  cMemTools::vMemSet((u8*)0x20007800, 0x55, 0x8000);

  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

