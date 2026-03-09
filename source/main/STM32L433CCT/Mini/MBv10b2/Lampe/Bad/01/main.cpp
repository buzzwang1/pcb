
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz



//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
//
//  PB09  -> Status Led
//
//  PA00   -> Wakeup (100k Pull Down)
//
//  PA08   -> reserve
//  PA05   -> reserve
//  PB00   -> reserve
//
//  Power Control
//    Battery Control:
//      Balancer:
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
//      PA06: Led
//
//
// //      PB00  -> U_Bat4
//
//
//
//  I2C2  -> BotNet 1.0.0.0
//    PB06  -> I2C2 SCL
//    PB07  -> I2C2 SDA
//
//  U1    -> BotNet 1.E.0.0
//    PA09  -> U1 Tx/Rx
//
//  U2    -> Debug
//    PA02  -> U2 Tx
//    PA03  -> U2 Rx
//
//  I2C2  -> INA3221 2x, Display (Optional) , Apds9960 (Optional)
//    PB10  -> I2C2 SCL AF4
//    PB14  -> I2C2 SDA AF4
//
//
//  nRF905:
//  PB03: SPI1 Clock
//  PB04: SPI1 MISO
//  PB05: SPI1 MOSI
//  PA12: SPI1 Chip Select
//  PB12: TX or RX mode
//  PA11: Standby
//  PB15: Power up
//  PB13: Data Ready
//  PB00 (option)
//  PB01 (option)
//  PA15 (option)
//
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
//    TIM6_DAC_IRQHandler:      CyclicCaller:  Prio: 15.8 => 1ms SysTick
//    I2C1_EV_IRQHandler:       BotCom:        Prio:  8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio:  8.8
//    I2C2_EV_IRQHandler:       Board:         Prio:  8.8
//    I2C2_ER_IRQHandler:       Board:         Prio:  8.8
//    EXTI15_10_IRQHandler:     BotNet nRF905: Prio:  9.8 => NRF905
//    DMA1_Channel2_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    DMA1_Channel3_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    TIM7_IRQHandler:          BotNet nRF905: Prio:  9.8 => NRF905
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio:  6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio:  6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio:  6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio:  6.8 => U1
//    USART2_IRQHandler:        BotCom U2:     Prio:  7.8 => U2
//
// Backup-Register Usage:
//    00: Clock Valid by Clock
//    01: Wakeup Counter
//
//
//    DiffTimer Max Cycle
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


//    DiffTimer Timing
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

cClockInfo mcClockInfo;


class  __attribute__((__packed__)) cData
{
public:

  typedef struct
  {
    u32  u32ID;
    u32  u32Cnt;
    u8   Res1[32 - 4 - 4];
    // 32
    u16  u16StartTimerReload;
    u8   u8BrigthnessDay;
    u8   u8LedAnimationIdxDay;
    u8   u8BrigthnessNigth;
    u8   u8LedAnimationIdxNigth;

    u8   u8DayHourStart;
    u8   u8DayHourStop;


  }tstData;

  tstData mData;

  cData()
  {
    vSetDefault();
  }

  void vSetDefault()
  {
    mData.u32Cnt                  = 0;
    mData.u16StartTimerReload     = 1000 * 5; // 5s
    mData.u8BrigthnessDay         = 200;  // [digit]
    mData.u8LedAnimationIdxDay    = 16;
    mData.u8BrigthnessNigth       = 70;  // [digit]
    mData.u8LedAnimationIdxNigth  = 16;

    mData.u8DayHourStart =  8;
    mData.u8DayHourStop  = 21;
  }

  void vCheckAndSetDefault()
  {
    if (mData.u32ID != 0x12345678)
    {
      // ROM Defaults
      mData.u32ID               = 0x12345678;
      vSetDefault();
      vStore();
    }
  }

  virtual void vStore() = 0;
};

class cEepData: public cData
{
  public:
  typedef enum
  {
     nSize = (1 * 1024)
  }tenCfg;


  u16      mu16StoreTimer;
  u16      mu16LoadTimer;
  cI2cEep* mcEep;
  bool     mbLoad;
  bool     mbError;

  cEepData(cI2cEep* lcEep)
  {
    mcEep   = lcEep;
    mbLoad  = True;
    mbError = False;
    mu16StoreTimer = 0;
    mu16LoadTimer  = 0;
  }



  void vLoad()
  {
    cMemTools::vMemCpy((u8*)&mData, (u8*)((u32)mcEep->mu8RamMirrow), sizeof(tstData));
  }

  void vStore()
  {
    mData.u32Cnt++;
    cMemTools::vMemCpy((u8*)((u32)mcEep->mu8RamMirrow), (u8*)&mData, sizeof(tstData));
    mcEep->vCmdWriteAll();
  }

  void vUpdate()
  {
    mu16StoreTimer = mData.u16StartTimerReload;
  }

  void vTick10ms()
  {
    if (mu16StoreTimer > 0)
    {
      if (mu16StoreTimer > 10)
      {
        mu16StoreTimer -= 10;
      }
      else
      {
        mu16StoreTimer = 0;
        vStore();
      }
    }

    // Warten bis Daten gelesen wurden
    if (mbLoad)
    {
      // Timeout zum warten auf EEPROM
      if (mu16LoadTimer < 1000)
      {
        mu16LoadTimer += 10;
      }
      else // EEPROM wurde nicht geladen
      {
        mbError = True;
        mbLoad = False;
        vCheckAndSetDefault();
      }

      if (mcEep->mStatus.IsInit)
      {
        mbLoad = False;
        vLoad();
        vCheckAndSetDefault();
      }
    }
  }
};

cEepData mcData(&mcSys.mcBoard.mcEep);

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  cErr::munErr->stErr.isHardFault = 1;
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  cErr::munErr->stErr.isMemManage = 1;
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  cErr::munErr->stErr.isBusFault = 1;
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  cErr::munErr->stErr.isUsageFault = 1;
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  cErr::munErr->stErr.isSVC = 1;
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  cErr::munErr->stErr.isDebugMon = 1;
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  cErr::munErr->stErr.isPendSV = 1;
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

typedef struct
{
  u8 u8Enable;
  u8 u8RotSwitch;
  u8 u8AnimIdx;
  u8 u8Updated;
}tstSLedState;

#define MAIN_ROTSWITCHCNT 2
tstSLedState mstLedStateSoll[MAIN_ROTSWITCHCNT];
tstSLedState mstLedStateIst[MAIN_ROTSWITCHCNT];

tcGpPin<GPIOA_BASE, 1> mcPinPIR(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);

void vPwrReqMsgSetChn(u16 lu16BnDstAdr, u8 lu8Relais, u8 lu8Chl1, u8 lu8Chl2, u8 lu8Chl3)
{
  u8 lu8Data[7];

  // TX 02 | 00 | 00 | RM.1M.2M.3M
  lu8Data[0] = 2;
  lu8Data[1] = 0;
  lu8Data[2] = 0;

  lu8Data[3] = lu8Relais; // relais an
  lu8Data[4] = lu8Chl1;   // Ch1: SLED
  lu8Data[5] = lu8Chl2;   // Ch2: Normale Led
  lu8Data[6] = lu8Chl3;   // Ch3: Relais2 100=100% PWM => an

  mcSys.mcCom.mcBn.vSendMsg(0x1000, lu16BnDstAdr, 34, lu8Data, sizeof(lu8Data));
}


void vPwrReqMsgSetLed(u16 lu16BnDstAdr, u8 lu8Enable, u8 lu8Brigthness, u8 lu8AnimIdx)
{
  u8 lu8Data[11];

  // Set                  RX 00 | 00 | 00 | ES.P1.AI.RR.GG.BB
  lu8Data[0] = 0;
  lu8Data[1] = 0;
  lu8Data[2] = 0;

  lu8Data[3] = lu8Enable;       // Enable
  lu8Data[4] = lu8Brigthness;   // 0..255 Helligkeit
  lu8Data[5] = lu8AnimIdx;      // Pink - nur Rot und Blau, kein Grün
  lu8Data[6] = 255; // R
  lu8Data[7] = 255; // G
  lu8Data[8] = 255; // B
  lu8Data[9] =   0;
  lu8Data[10] = 100; // Temp

  mcSys.mcCom.mcBn.vSendMsg(0x1000, lu16BnDstAdr, 42, lu8Data, sizeof(lu8Data));
}

void vPSwitchReqMsgSetChn(u16 lu16BnDstAdr, u8 lu8Chl1, u8 lu8Chl2, u8 lu8Chl3)
{
  u8 lu8Data[6];

  // TX 02 | 00 | 00 | 1M.2M.3M
  lu8Data[0] = 2;
  lu8Data[1] = 0;
  lu8Data[2] = 0;

  lu8Data[3] = lu8Chl1; // Ch1
  lu8Data[4] = lu8Chl2; // Ch2
  lu8Data[5] = lu8Chl3; // Ch3

  mcSys.mcCom.mcBn.vSendMsg(0x1000, lu16BnDstAdr, 34, lu8Data, sizeof(lu8Data));
}

void vPSwitchReqMsgSetLed(u16 lu16BnDstAdr, u8 lu8Enable, u8 lu8RotSwtchPos, u8 lu8AnimIdx)
{
  u8 lu8Data[11];

  // TX 00 | 00 | 00 | ES.P1.AI.RR.GG.BB.TH.TL
  lu8Data[0] = 0;
  lu8Data[1] = 0;
  lu8Data[2] = 0;

  lu8Data[3] = lu8Enable;
  lu8Data[4] = lu8RotSwtchPos; // 1 ... 12
  lu8Data[5] = lu8AnimIdx;   // Animation Index
  lu8Data[6] = 0xFF;
  lu8Data[7] = 0xFF;
  lu8Data[8] = 0xFF;
  lu8Data[9] = 0;
  lu8Data[10] = 100;

  mcSys.mcCom.mcBn.vSendMsg(0x1000, lu16BnDstAdr, 42, lu8Data, sizeof(lu8Data));
}


class cReqSm
{
  public:

  enum tenState
  {
    stIdle,
    stReq,
    stWait,
    stOn,
    stError
  };

  enum tenEvent
  {
    evDummy,
    evStart,
    evError
  };

  u16      mTimeoutCnt_ms;
  u16      mTimeout_ms;
  tenState mState;

  cReqSm(u16 lu16TimeoutCnt_ms = 0)
  {
    vReset();
    mTimeoutCnt_ms = lu16TimeoutCnt_ms;
  }

  void vReset()
  {
    mState = stIdle;
    mTimeout_ms = 0;
  }

  virtual void vActReq()
  {
  }

  virtual bool isEvExitWait()
  {
    return True;
  }

  virtual void vActWait()
  {
  }

  virtual void vActEntryOn()
  {
  }

  void vSm(tenEvent lenEvent = evDummy)
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;

      switch (mState)
      {
        case stIdle:
          if (lenEvent == evStart)
          {
            vActReq();
            mState = stReq;
            lbLoop = True;
          }
          break;
        case stReq:
          mState = stWait;
          mTimeoutCnt_ms = mTimeoutCnt_ms;
          lbLoop = True;
          break;
        case stWait:
          if (isEvExitWait())
          {
            vActEntryOn();
            mState = stOn;
            lbLoop = True;
          }
          else
          {
            vActWait();
          }
          break;
        case stOn:
          break;
        default:
          break;
      }
    }
  }


  void vTick100ms()
  {
    if (mTimeout_ms)
    {
      if (mTimeout_ms > 100)
      {
        mTimeout_ms -= 100;
      }
      else
      {
        mTimeout_ms = 0;
        mState = stError;
      }
    }
    vSm();
  }
};

class cSm220V : public cReqSm
{
  public:
  u8 mu8ReqCnt;

  cSm220V()
    : cReqSm(300)
  {
    mu8ReqCnt = 0;
  }


  void vActReq() override
  {
    // LedBoard anschalten
    // Schaltet auch PSwitch an
    mcSys.mcBoard.mcLipoMon.mcPowerOut->vSetHwOutputVoltage(5000);
    mcSys.mcBoard.mcLipoMon.mcPowerOut->vEnable();
  }

  bool isEvExitWait() override
  {
    // Warten, bis LedBoard Knoten online ist
    return (mcSys.mcCom.mcBn.mcDownLinks[1]->mStatus.IsOnline == 1);
  }

  void vActWait() override
  {
  }

  void vActEntryOn() override
  {
    vPwrReqMsgSetChn(0x1200, 1, 0, 0, 100);
  }



  bool isReady()
  {
    return (mState == stOn);
  }

  void vRequest()
  {
    mu8ReqCnt++;
    if (mState == stIdle)
    {
      cReqSm::vSm(cReqSm::tenEvent::evStart);
    }
  }

  void vRelease()
  {
    if (mu8ReqCnt)
    {
      mu8ReqCnt--;
      if (mu8ReqCnt == 0)
      {
        cReqSm::vReset();
        // LedBoard ausschalten
        // Schaltet auch PSwitch aus
        mcSys.mcBoard.mcLipoMon.mcPowerOut->vDisable();
      }
    }
  }

  void vTick100ms()
  {
    cReqSm::vTick100ms();
  }
};


u8 u8BrigthnessDigitToRotSwitch(u8 lu8Brigness_digit)
{
  if (lu8Brigness_digit <  20) return  0;
  if (lu8Brigness_digit <  40) return  1;
  if (lu8Brigness_digit <  60) return  2;
  if (lu8Brigness_digit <  80) return  3;
  if (lu8Brigness_digit < 100) return  4;
  if (lu8Brigness_digit < 120) return  5;
  if (lu8Brigness_digit < 140) return  6;
  if (lu8Brigness_digit < 160) return  7;
  if (lu8Brigness_digit < 180) return  8;
  if (lu8Brigness_digit < 200) return  9;
  if (lu8Brigness_digit < 220) return 10;
  if (lu8Brigness_digit < 240) return 11;
  return 12;
}

u8 u8RotSwitchToBrigthnessDigit(u8 lu8RotSwitch)
{
  if (lu8RotSwitch ==  0) return   0;
  if (lu8RotSwitch ==  1) return  30;
  if (lu8RotSwitch ==  2) return  50;
  if (lu8RotSwitch ==  3) return  70;
  if (lu8RotSwitch ==  4) return  90;
  if (lu8RotSwitch ==  5) return 110;
  if (lu8RotSwitch ==  6) return 130;
  if (lu8RotSwitch ==  7) return 150;
  if (lu8RotSwitch ==  8) return 170;
  if (lu8RotSwitch ==  9) return 190;
  if (lu8RotSwitch == 10) return 210;
  if (lu8RotSwitch == 11) return 230;
  return 255;
}


cSm220V mcSm220V;

class cSmCharger
{
  public:
  enum tenState
  {
    stOff,
    stWait,
    stOn
  };

  tenState mState;

  cSm220V *mcSmPower;

  cSmCharger(cSm220V* lcSmPower)
  {
    mcSmPower = lcSmPower;
    mState = stOff;
  }

  void vSm()
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;

      switch (mState)
      {
        case stOff:
          if (mcSys.mcBoard.mcLipoMon.isChargeNeeded())
          {
            mcSmPower->vRequest();
            mState = stWait;
            lbLoop = True;
          }
          break;
        case stWait:
          if (mcSmPower->isReady())
          {
            mState = stOn;
            lbLoop = True;
          }
          break;
        case stOn:
          if (!mcSys.mcBoard.mcLipoMon.isCharging())
          {
            mcSmPower->vRelease();
            mState = stOff;
            lbLoop = True;
          }
          break;
        default:
          break;
      }
    }
  }

  void vTick100ms()
  {
    vSm();
  }
};


class cSmLightBrightness
{
  public:
  enum tenState
  {
    stOff,
    stOn
  };

  tenState mState;
  tenState mStateOld;
  bool     mbNewState;

  typedef enum
  {
    enModeDay = 0,
    enModeNigth,
  }tenSmLedMode;

  tenSmLedMode menMode = enModeDay;
  i16 mu16SollBrigthness = 0;
  i16 mu16SollBrigthnessSave = 0;
  i16 mu16IstBrigthness = 0;
  u16 mu16OnSettleTime_ms;
  u32 mu32ChangeTimeout_ms;
  bool mbUserChange;

  cSmLightBrightness()
  {
    mState = stOff;
    mbNewState = False;
    mbUserChange = False;
    mu16OnSettleTime_ms = 0;
  }

  void vSm()
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;

      switch (mState)
      {
        case stOff:
          if (mbNewState)
          {
            mbNewState = False;
            // Entry
            for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
            {
              //                   BnDstAdr,                lu8Enable, lu8RotSwtchPos, lu8AnimIdx
              vPSwitchReqMsgSetLed(0x1110 + 0x10 * lu8Node, 0,         0,              mstLedStateSoll[lu8Node].u8AnimIdx);
            }
            //               lu16BnDstAdr, lu8Relais, lu8Chl1, lu8Chl2, lu8Chl3
            vPwrReqMsgSetChn(0x1200,       1,         0,       0,       100); // Spiegellampe
            vPwrReqMsgSetChn(0x1150,       1,         0,       0,       100); // Deckenlampe

            //               lu16BnDstAdr, lu8Enable, lu8Brigthness, lu8AnimIdx)
            vPwrReqMsgSetLed(0x1200,       0,         0,             mstLedStateSoll[0].u8AnimIdx); // Spiegellampe
            vPwrReqMsgSetLed(0x1150,       0,         0,             mstLedStateSoll[0].u8AnimIdx); // Deckenlampe
          }
          break;
        case stOn:
          {
            bool lbDoUpdate = False;
            u8   lu8DiffNode = 0;

            if (mbNewState) // Entry
            {
              mbNewState = False;
              mu32ChangeTimeout_ms = mu16OnSettleTime_ms = 2000;
            }

            if (mu16OnSettleTime_ms)
            {
              // Eine Einschalt-Verzögerung bis sich alles stabilisiert hat.
              // Dieser Teil wird nur die ersten 2s (mu16OnSettleTime_ms)  nachdem Einschalten gemacht
              
              //                  lu16BnDstAdr, lu8Chl1, lu8Chl2, lu8Chl3)
              vPSwitchReqMsgSetChn(0x1100,      1,       1,       1); // PSwitch
             
              for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
              {
                //                   BnDstAdr,                lu8Enable,                         lu8RotSwtchPos,                       lu8AnimIdx
                vPSwitchReqMsgSetLed(0x1110 + 0x10 * lu8Node, mstLedStateSoll[lu8Node].u8Enable, mstLedStateSoll[lu8Node].u8RotSwitch, mstLedStateSoll[lu8Node].u8AnimIdx);
              }

              u8 lu8SLedEnable = 0;
              if (mu16IstBrigthness > 0)
              {
                lu8SLedEnable = 100;
              }

              vPwrReqMsgSetChn(0x1200, 1, lu8SLedEnable, (mu16IstBrigthness * 100) / 255, 100);
              vPwrReqMsgSetChn(0x1150, 1, lu8SLedEnable, (mu16IstBrigthness * 100) / 255, 100);
              vPwrReqMsgSetLed(0x1200, ((u8)mu16IstBrigthness > 0), mu16IstBrigthness, mstLedStateSoll[0].u8AnimIdx);
              vPwrReqMsgSetLed(0x1150, ((u8)mu16IstBrigthness > 0), mu16IstBrigthness, mstLedStateSoll[0].u8AnimIdx);

              if (mu16OnSettleTime_ms > 100) mu16OnSettleTime_ms -= 100;
              else mu16OnSettleTime_ms = 0;
            }

            if (mu32ChangeTimeout_ms == 0)
            {
              // Schauen, ob sich Istzustand verändert hat.
              for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
              {
                if (mstLedStateIst[lu8Node].u8Updated)
                {
                  mstLedStateIst[lu8Node].u8Updated = 0;
                  lbDoUpdate = True;
                  lu8DiffNode = lu8Node;
                  mu16SollBrigthness  = u8RotSwitchToBrigthnessDigit(mstLedStateIst[lu8Node].u8RotSwitch);
                  mu16SollBrigthness *= mstLedStateIst[lu8Node].u8Enable;                  
                  mbUserChange = True;
                  mu32ChangeTimeout_ms = 200;
                  break;
                }
              }
            }
            else
            {
              if (mu32ChangeTimeout_ms > 100) mu32ChangeTimeout_ms -= 100;
              else mu32ChangeTimeout_ms = 0;
            }

            // geänderten Wert nur an die anderen schicken
            if (lbDoUpdate)
            {
              for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
              {
                mstLedStateSoll[lu8Node].u8Updated   = mstLedStateIst[lu8DiffNode].u8Updated;
                mstLedStateSoll[lu8Node].u8Enable    = mstLedStateIst[lu8DiffNode].u8Enable;
                mstLedStateSoll[lu8Node].u8RotSwitch = mstLedStateIst[lu8DiffNode].u8RotSwitch;
                mstLedStateSoll[lu8Node].u8AnimIdx   = mstLedStateIst[lu8DiffNode].u8AnimIdx;
                if (lu8DiffNode != lu8Node)
                {
                  //                   BnDstAdr,                lu8Enable,                         lu8RotSwtchPos,                       lu8AnimIdx
                  vPSwitchReqMsgSetLed(0x1110 + 0x10 * lu8Node, mstLedStateSoll[lu8Node].u8Enable, mstLedStateSoll[lu8Node].u8RotSwitch, mstLedStateSoll[lu8Node].u8AnimIdx);
                }
              }
            }

            if (mu16IstBrigthness != mu16SollBrigthness)
            {
              lbDoUpdate = True;
              if (mu16IstBrigthness + 5 < mu16SollBrigthness)
              {
                mu16IstBrigthness += 5;
              }
              else
              {
                if (mu16IstBrigthness - 5 > mu16SollBrigthness)
                {
                  mu16IstBrigthness -= 5;
                }
                else
                {
                  mu16IstBrigthness = mu16SollBrigthness;
                }
              }
            }

            if (lbDoUpdate)
            {
              u8 lu8SLedEnable = 0;
              if (mu16IstBrigthness > 0)
              {
                lu8SLedEnable = 100;
              }
              vPSwitchReqMsgSetChn(0x1100, 1, 1, 1);
              vPwrReqMsgSetChn(0x1200, 1, lu8SLedEnable, (mu16IstBrigthness * 100) / 255, 100);
              vPwrReqMsgSetChn(0x1150, 1, lu8SLedEnable, (mu16IstBrigthness * 100) / 255, 100);
              vPwrReqMsgSetLed(0x1200, ((u8)mu16IstBrigthness > 0), mu16IstBrigthness, mstLedStateSoll[0].u8AnimIdx);
              vPwrReqMsgSetLed(0x1150, ((u8)mu16IstBrigthness > 0), mu16IstBrigthness, mstLedStateSoll[0].u8AnimIdx);
            }
          }
          break;
        default:
          break;
      }
      if (mStateOld != mState)
      {
        mStateOld  = mState;
        mbNewState = True;
      }
    }
  }

  void vSetDayNigthMode()
  {
    menMode = enModeDay;
    if (mcSys.mcClock.mClock.IsValid())
    {
      // Hour Start = 8 Stop = 21
      if ((mcData.mData.u8DayHourStart <= 24) && (mcData.mData.u8DayHourStop <= 24))
      {
        if (mcData.mData.u8DayHourStart < mcData.mData.u8DayHourStop)
        {
          if ((mcSys.mcClock.mClock.mTime.stTime.ui8Hour < mcData.mData.u8DayHourStart) ||
              (mcSys.mcClock.mClock.mTime.stTime.ui8Hour > mcData.mData.u8DayHourStop))
          { // 22 bis 7 uhr
            menMode = enModeNigth;
          }
        }
        else
        if (mcData.mData.u8DayHourStart > mcData.mData.u8DayHourStop)
        {
          if ((mcSys.mcClock.mClock.mTime.stTime.ui8Hour > mcData.mData.u8DayHourStart) ||
              (mcSys.mcClock.mClock.mTime.stTime.ui8Hour < mcData.mData.u8DayHourStop))
          {
            menMode = enModeNigth;
          }
        }
      }
    }

    if (menMode == enModeNigth)
    {
      for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
      {
        mstLedStateIst[lu8Node].u8Updated   = mstLedStateSoll[lu8Node].u8Updated   = 0;
        mstLedStateIst[lu8Node].u8Enable    = mstLedStateSoll[lu8Node].u8Enable    = 1;
        mstLedStateIst[lu8Node].u8RotSwitch = mstLedStateSoll[lu8Node].u8RotSwitch = u8BrigthnessDigitToRotSwitch(mcData.mData.u8BrigthnessNigth);
        mstLedStateIst[lu8Node].u8AnimIdx   = mstLedStateSoll[lu8Node].u8AnimIdx   = mcData.mData.u8LedAnimationIdxNigth;
      }
      mu16SollBrigthness = mcData.mData.u8BrigthnessNigth;
    }
    else
    {
      for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
      {
        mstLedStateIst[lu8Node].u8Updated   = mstLedStateSoll[lu8Node].u8Updated   = 0;
        mstLedStateIst[lu8Node].u8Enable    = mstLedStateSoll[lu8Node].u8Enable    = 1;
        mstLedStateIst[lu8Node].u8RotSwitch = mstLedStateSoll[lu8Node].u8RotSwitch = u8BrigthnessDigitToRotSwitch(mcData.mData.u8BrigthnessDay);
        mstLedStateIst[lu8Node].u8AnimIdx   = mstLedStateSoll[lu8Node].u8AnimIdx   = mcData.mData.u8LedAnimationIdxDay;
      }
      mu16SollBrigthness = mcData.mData.u8BrigthnessDay;
    }
  }

  bool isLigthOff()
  {
    return ((mu16IstBrigthness == mu16SollBrigthness) && (mu16SollBrigthness == 0));
  }

  bool isUserChange()
  {
    bool lbTemp  = mbUserChange;
    mbUserChange = False;
    return lbTemp;
  }

  void vEnable()
  {    
    
    mbNewState = True;
    mu32ChangeTimeout_ms = 200;
    if (mState != stOn)
    {
      vSetDayNigthMode();
    }
    mState = stOn;
    mbUserChange = False;
  }

  void vDisable()
  {
    mState = stOff;
    mbNewState = True;
  }

  void vTick100ms()
  {
    vSm();
  }
};


class cSmLight
{
  public:
  enum tenState
  {
    stOff,
    stWaitPower,
    stWaitPSwitch,
    stOn,
    stOnHold,
    stDimOff,
  };

  u32 mu32PirCnt_ms;
  u32 mu32Delay_ms;

  tenState mState;
  tenState mStateOld;
  bool     mbNewState;

  cSm220V *mcSmPower;
  cSmLightBrightness mcLightCntrl;

  cSmLight(cSm220V* lcSmPower)
    : mcLightCntrl()
  {
    mcSmPower = lcSmPower;
    mState = stOff;
    mbNewState = False;
  }

  void vSm()
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;

      switch (mState)
      {
        case stOff:
          mbNewState = False;
          if (mu32PirCnt_ms)
          {
            mcSmPower->vRequest();
            mState = stWaitPower;
            lbLoop = True;
          }
          break;
        case stWaitPower:
          mbNewState = False;
          if (mcSmPower->isReady())
          {
            mState = stWaitPSwitch;
            lbLoop = True;
          }
          break;
        case stWaitPSwitch:
          if (mbNewState)  // Entry
          {
            mbNewState = False;
            mu32Delay_ms = 200;
          }

          if (mu32Delay_ms == 0)
          {
            mState = stOn;
            lbLoop = True;
          }
          else
          {
            mu32Delay_ms -= 100;
            // Sende Nachricht an PowerSwitch (0x1100)  Ch1, Ch2 und Ch3 zu aktivieren
            vPSwitchReqMsgSetChn(0x1100, 1, 1, 1);
          }
          break;
        case stOn:
          if (mbNewState)  // Entry
          {
            mbNewState = False;
            mcLightCntrl.vEnable();
          }

          if (mu32PirCnt_ms == 0)
          {
            mcLightCntrl.mu16SollBrigthnessSave = mcLightCntrl.mu16SollBrigthness;
            mcLightCntrl.mu16SollBrigthness = mcLightCntrl.mu16SollBrigthness / 2;
            mState = stOnHold;
            lbLoop = True;
          }

          if ((mcPinPIR.ui8Get()) ||
              (mcSys.mcSMsg.mcWakeupSim.isRequestAndClear(1)) ||
              (mcLightCntrl.isUserChange()))
          {
            mu32PirCnt_ms = 1000 * 60 * 10;
          }
          break;
        case stOnHold:
          if (mbNewState)  // Entry
          {
            mbNewState = False;
            mu32Delay_ms = 1000 * 60 * 1; // 1min
          }

          if (mu32Delay_ms == 0)
          {
            mState = stDimOff;
            lbLoop = True;
          }
          else
          {
            mu32Delay_ms -= 100;
          }

          if ((mcPinPIR.ui8Get()) ||
              (mcSys.mcSMsg.mcWakeupSim.isRequestAndClear(1)) ||
              (mcLightCntrl.isUserChange()))
          {
            mcLightCntrl.mu16SollBrigthness = mcLightCntrl.mu16SollBrigthnessSave;
            mu32PirCnt_ms = 1000 * 60 * 10;
            mState = stOn;
            lbLoop = True;
          }
          break;
        case stDimOff:
          if (mbNewState)  // Entry
          {
            mbNewState = False;
            mcLightCntrl.mu16SollBrigthness = 0;
          }

          if (mcLightCntrl.isLigthOff())
          {
            mcLightCntrl.vDisable();
            mcSmPower->vRelease();
            mState = stOff;
            lbLoop = True;
          }

          if ((mcPinPIR.ui8Get()) ||
              (mcSys.mcSMsg.mcWakeupSim.isRequestAndClear(1)) ||
              (mcLightCntrl.isUserChange()))
          {
            mu32PirCnt_ms = 1000 * 60 * 10;
            mState = stOn;
            lbLoop = True;
          }
          break;
        default:
          break;
      }
      if (mStateOld != mState)
      {
        mStateOld  = mState;
        mbNewState = True;
      }
    }
  }

  void vTick100ms()
  {
    bool lbPIR = (mcPinPIR.ui8Get() || // High aktive
                 (mcSys.mcSMsg.mcWakeupSim.isRequestAndClear(1))); 

    if (lbPIR)
    {
      mu32PirCnt_ms = 1000 * 60 * 10;
    }

    vSm();
    mcLightCntrl.vTick100ms();

    if (mu32PirCnt_ms >= 100)
    {
      mu32PirCnt_ms -= 100;
      if (mu32PirCnt_ms < 100)
      {
        mu32PirCnt_ms = 0;
      }
    }
  }
};

cSmLight   mcSmLight(&mcSm220V);
cSmCharger mcSmCharger(&mcSm220V);

void vDoGuiControl_100ms()
{
  if (mcSys.mcSMan.mcOpMode.IsAuto())
  {
    mcSm220V.vTick100ms();
    mcSmCharger.vTick100ms();
    mcSmLight.vTick100ms();

    if (mcSm220V.mState != cReqSm::tenState::stIdle)
    {
      mcSys.mcSMan.mcPowerManager.vStart(1000 * 1 / 10); //  1000ms * 5 = 5sec; /10m weil Tick in 10ms
    }
  }
}


typedef enum
{
  enStart,
  enLedBoardPowerOn,
  enLedBoardWaitCom,
  enLedBoardIsOnline,
  enLedBoardWaitHighPowerOn,
  enLedBoardHighPowerOn,
  enLedBoardHighPowerOnLedOff,
  enLedBoardWaitFailed,
}tenSmLedBoard;

tenSmLedBoard menSmLedBoard = enStart;
u8            mu8SmLedBoardRetrys;


class cBnMsgHandlerApp : public cBotNet_MsgSysProcess
{
public:
  cBnMsgHandlerApp()
    : cBotNet_MsgSysProcess(&mcSys.mcCom.mcBn)
  {
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    bool lbConsumed = False;
    u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;

    switch (lcMsg.u16GetIdx())
    {
      // --------------------------- SLED Messages -----------------------------
      case 41: // Sled Response
        switch (lpu8PayloadRx[0])
        {
          case 0: // SLED: Status         TX 00 | 00 | 00 | ES.P1.AI.RR.GG.BB.TH.TL
            if ((lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
            {
              lbConsumed = True;
              for (u8 lu8Node = 0; lu8Node < MAIN_ROTSWITCHCNT; lu8Node++)
              {
                if (lcMsg.cGetSAdr() == (0x1110 + 0x10 * lu8Node))
                {
                  mstLedStateIst[lu8Node].u8Enable    = lpu8PayloadRx[3];
                  mstLedStateIst[lu8Node].u8RotSwitch = lpu8PayloadRx[4];
                  mstLedStateIst[lu8Node].u8AnimIdx   = lpu8PayloadRx[5];
                  mstLedStateIst[lu8Node].u8Updated   = 1;
                  break;
                }
              }
            }
            break;
          }
          break;
    }

    return lbConsumed;
  }
};

cBnMsgHandlerApp mcBnMsgHandlerApp;



class cCliCmd_AppStat : public cCliCmd
{
public:
  cCliCmd_AppStat() :cCliCmd((const char*)"AppStat", (const char*)"App Status") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    if (mcSmLight.mcLightCntrl.menMode == cSmLightBrightness::enModeDay)
    {
      lszStr.Setf((rsz)"B Mode: Day");
    }
    else
    {
      lszStr.Setf((rsz)"B Mode: Nigth");
    }
    lcCli->bPrintLn(lszStr);

    switch (mcSmLight.mcLightCntrl.mState)
    {
      case cSmLightBrightness::stOff:
        lszStr.Setf((rsz)"B State: Off");
        break;
      case cSmLightBrightness::stOn:
        lszStr.Setf((rsz)"B State: On");
        break;
    }
    lcCli->bPrintLn(lszStr);
    
    lszStr.Setf((rsz)"220V Req: %d", mcSm220V.mu8ReqCnt); lcCli->bPrintLn(lszStr);

    switch (mcSmLight.mState)
    {
      case cSmLight::stOff:
        lszStr.Setf((rsz)"L State: Off");
        break;
      case cSmLight::stWaitPower:
        lszStr.Setf((rsz)"L State: WaitPower");
        break;
      case cSmLight::stWaitPSwitch:
        lszStr.Setf((rsz)"L State: WaitPSwitch");
        break;
      case cSmLight::stOn:
        lszStr.Setf((rsz)"L State: On");
        break;
      case cSmLight::stOnHold:
        lszStr.Setf((rsz)"L State: OnHold");
        break;
      case cSmLight::stDimOff:
        lszStr.Setf((rsz)"L State: DimOff");;
        break;
    }
    lcCli->bPrintLn(lszStr);


    switch (mcSmCharger.mState)
    {
      case cSmCharger::stOff:
        lszStr.Setf((rsz)"C State: Off");
        break;
      case cSmCharger::stOn:
        lszStr.Setf((rsz)"C State: On");
        break;
      case cSmCharger::stWait:
        lszStr.Setf((rsz)"C State: Wait");
        break;
    }
    lcCli->bPrintLn(lszStr);

    return True;
  }
};

class cCliCmd_WupReq : public cCliCmd
{
public:
  cCliCmd_WupReq() :cCliCmd((const char*)"WupReq", (const char*)"Wakeup Request") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    mcSys.mcSMsg.mcWakeupSim.vSet(1, 0);

    lszStr.Setf((rsz)"Ok");
    lcCli->bPrintLn(lszStr);

    return True;
  }
};

class cCliCmd_ShtDwmReq : public cCliCmd
{
public:
  cCliCmd_ShtDwmReq() :cCliCmd((const char*)"ShtDwmReq", (const char*)"Shutdown request") {}
  bool bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    UNUSED(lcParam);
    UNUSED(lCallerAdr);
    UNUSED(lbFirstCall);

    cStr_Create(lszStr, 32);

    mcSmLight.mu32PirCnt_ms = 200;

    lszStr.Setf((rsz)"Ok");
    lcCli->bPrintLn(lszStr);

    return True;
  }
};

cCliCmd_WupReq        mcCliCmd_WupReq;
cCliCmd_AppStat       mcCliCmd_AppStat;
cCliCmd_ShtDwmReq     mcCliCmd_ShtDwmReq;

cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_AppStat, &mcCliCmd_WupReq, &mcCliCmd_ShtDwmReq}, 3);


void MAIN_vTick1msHp(void)
{
  mcSys.vTick1msHp();
}


void MAIN_vTick1msLp(void)
{
  static u8 lu8Tick1msCnt = 0;
  mcSys.vTick1msLp();

  lu8Tick1msCnt++;
  if ((lu8Tick1msCnt % 10) == 0)
  {
    mcData.vTick10ms();
  }

  if ((lu8Tick1msCnt % 100) == 0)
  {
    vDoGuiControl_100ms();
  }
}


void MAIN_vInitSystem(void)
{
  cBuRam::vAddLogPos(3);
  mcSys.vInit1();

  cBuRam::vAddLogPos(4);
  mcSys.mcCom.mcBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);
  mcBnMsgHandlerApp.vAddMsgSys();
  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  cBuRam::vAddLogPos(5);
  mcSys.vInit2();

  cBuRam::vAddLogPos(6);
  vDoGuiControl_100ms();

  if (mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter > 10)
  {
    mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload = 10; // 10
    mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter = mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload;
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  #ifdef PCB_PROJECTCFG_Test
    #ifdef TESTSYSRUNTIME
      mcPB00.vSet1();
      mcTestClassTim[1].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
    #endif
  #endif

  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  }
}

bool SystemClock_Config_HSE(void)
{
  u16 lu16Retries = 100;
  bool lbError = False;
  while (lu16Retries > 0)
  {
    lbError = False;
    cBnSpop_vResetWdog();

    // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
    // kein Pll

    RCC_OscInitTypeDef RCC_OscInitStruct   = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      cErr::munErr->stErr.isInitOscCfg = 1;
      lbError = True;
    }

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
      cErr::munErr->stErr.isInitClkCfg = 1;
      lbError = True;
    }

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
      cErr::munErr->stErr.isInitVltScl = 1;
      lbError = True;
    }

    if (!lbError) break;

    lu16Retries--;
  }

  if (lu16Retries == 0) return False;

  return True;
}


// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  #ifdef PCB_PROJECTCFG_Test
    // Messung um die PowerUp Starttime zu messen
    // Wird vor Sleep wieder ausgeschaltet
    // Set PB01
  cSysPkgPMon::vPA08_Init();
  cSysPkgPMon::vPA08_Set1();
  #endif

  //cBuRam::vEnable(); // cBuRam::vEnable() wird auch in cErr::vInit() gemacht
  cErr::vInit();
  cBuRam::mBuRam->u32SysTime_ms = 0;
  cBuRam::vAddLogPos(1);

  SystemInit();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA08_Set0();
  #endif
  HAL_Init();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA08_Set1();
  #endif
  SystemClock_Config_HSE();
  SystemCoreClockUpdate();

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA08_Set0();
  #endif

  #ifdef PCB_PROJECTCFG_Test
    // Timer aufziehen nachdem der uC auf 16Mhz ist
    // Nach dem Reset, braucht der es ~6.3ms bis dieser Punkt
    //   + 5ms bei einer 10k/1uF Reset-Beschaltung
    //   + 1,3ms Laufzeit bis hierhin
    cSysPkgPMon::vInitTimer(6300);
  #endif

  if (!RomConst_IsValid())
  {
    // RomConst Error
    SysError_Handler();
  }

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA08_Set1();
  #endif

  cBnSpop_vStartTim15();

  // 0x20007800
  //   Heap 32k
  // 0x2000F800
  //   Stack 2k
  // 0x20010000
  #ifdef PCB_PROJECTCFG_Test
    //cMemTools::vMemSet((u8*)0x2000F800, 0x55, 0x800 - 128);
    //cMemTools::vMemSet((u8*)0x20007800, 0x55, 0x8000);
  #endif

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA08_Set0();
  #endif

  cBuRam::vAddLogPos(2);
}

