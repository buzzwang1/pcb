
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB

// --- Core ----------- 

// Stromverbrauch:
//                       Normal   Max.
//CPU + PIR + Buttons +
//2 INA219 + DCF77          3       6

// PIR braucht ~3.9V um zu funktionieren
// 3.5V für den internen 3.3V Stabi + 0.4V für die
// Verpolungsschutzdiode.


// --- Funk ----------- 

// nrf905
// CE = 0                  0

// CE = 1 TXEN=0 PWR = 1  12
// CE = 1 TXEN=1 PSW = 1   9
// CE = 1 TXEN=0 PSW = 0   0

// --- Anzeige ----------- 
// LT1930 5V->21V
// LT1930 Shutdown          3
// LT1930 25%              10   20   50 (PWM 0, PWM mittel, PMW 1)
// LT1930 50%              10   30   50
// LT1930 75%              10   40   50
// LT1930 100%             50   50   50

// Display                10         14


// Erwartung Base + Funk = 15mA

__IO uint32_t TimingDelay = 0;

cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

//Gnd freischalten
cClockInfo mcClockInfo; //On top to init this class at first
//LED<GPIOC_BASE, 13> mcLedRed;

cHx8347 mcDisplay(False);

#define MAIN_nDISPLAY_X HX8347A_WIDTH
#define MAIN_nDISPLAY_Y HX8347A_HEIGHT

uint8                     mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];   // 40.960 Bytes
cBitmap_Bpp1_1G           mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G           mcScreen1(&mcBm);
cSprite_Res8b_Bpp1_1G     mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);

#define MAIN_nGRAPH_POINTS  24 * 60 / 6

stPxli16                  mcGraphChnlUSupplyPts[MAIN_nGRAPH_POINTS];    // 24h in min
stPxli16                  mcGraphChnlISupplyPts[MAIN_nGRAPH_POINTS];  // 24h in min
stPxli16                  mcGraphChnlUBatPts[MAIN_nGRAPH_POINTS]; // 24h in min
stPxli16                  mcGraphChnlIBatPts[MAIN_nGRAPH_POINTS]; // 24h in min
ciGraphXY_Channel         mcGraphChnl_USupply(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcGraphChnlUSupplyPts, MAIN_nGRAPH_POINTS, null);
ciGraphXY_Channel         mcGraphChnl_ISupply(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcGraphChnlISupplyPts, MAIN_nGRAPH_POINTS, null);
ciGraphXY_Channel         mcGraphChnl_UBat(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcGraphChnlUBatPts, MAIN_nGRAPH_POINTS, null);
ciGraphXY_Channel         mcGraphChnl_IBat(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcGraphChnlIBatPts, MAIN_nGRAPH_POINTS, null);
cScreen_Bpp1_1G           mcScreenGraph(2, 40, MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y - 40, &mcScreen1);
ciGraphXY                 mcGraph(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mcScreenGraph.iGfxWidth(), mcScreenGraph.iGfxHeight(), &mcScreenGraph);


// PON=GND -> Empfangsteil ist eingeschaltet.
tcGpPin<GPIOB_BASE, 8>   mPinDcf77Pon(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin                   mPinDcf77Data(GPIOB_BASE, 9, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cDcf77                   mcDcf77(&mPinDcf77Data);

cGpPin lcSCL(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSDA(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cINA219      mcINA219_Bat(&mcI2C1, INA219_I2C_ADDRESS_CONF_0, 100);
cINA219      mcINA219_Supply(&mcI2C1, INA219_I2C_ADDRESS_CONF_1, 100);

tcGpPin<GPIOC_BASE, 14>   mPinTest1(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);
tcGpPin<GPIOB_BASE,  4>   mPinTest2(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cWorkMode     mcOpMode;
cPowerMonitor mcPowerMonitor(&mcOpMode);

// Display
cPowerMonitor::tenStates menSmMain_Display = cPowerMonitor::MAIN_nSmDisplayIdle;

void MAIN_Switch_vInit(void);
void MAIN_Switch_DoAction(void);

void BUTTON_vInit(uint8 lui8Button);
uint8 BUTTON_GetState(uint8 lui8Button);
void MAIN_Button_vReleased(cPowerMonitor::tenButtons len8Button);
void MAIN_Button_vPressed(cPowerMonitor::tenButtons len8Button);

void MAIN_DCF_vPressed(void);
void MAIN_DCF_vReleased(void);

void MAIN_DISP_vInit(void);
void MAIN_DISP_vDeInit(void);


void MAIN_vSetDimming(void);

void Main_vSmDisplay(void);

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

class cBotNetStreamPort_BotNetPowMon_SetMode: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_SetMode():cCliCmd((const char*)"setmode", (const char*)"a=auto m=manual")
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


class cBotNetStreamPort_BotNetPowMon_SetTim: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_SetTim():cCliCmd((const char*)"settim", (const char*)"x=value in s")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lcCli);
      UNUSED(lCallerAdr);

      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Manual mode required");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        i16 li16Value = (lcParam.Atoi());
        mcPowerMonitor.stGlobals.ui8PowerTimer = ((u8)li16Value);
      }

      return True;
    }
};

class cBotNetStreamPort_BotNetPowMon_SetDim: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_SetDim():cCliCmd((const char*)"setdim", (const char*)"x=value in %")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lcCli);
      UNUSED(lCallerAdr);

      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Manual mode required");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        i16 li16Value = (lcParam.Atoi());
        mcPowerMonitor.stGlobals.ui8DimValue = ((u8)li16Value);
      }

      return True;
    }
};

class cBotNetStreamPort_BotNetPowMon_SetDisp: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_SetDisp():cCliCmd((const char*)"SetDisp", (const char*)"x y=display page")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (mcOpMode.IsAuto())
      {
        lcCli->bPrintLn((const char8*)"Manual mode required");
      }
      else
      {
        uint16 li16SpacePos;

        mcOpMode.vSet(cWorkMode::nStManuel);

        li16SpacePos = lcParam.IndexOf(0, ' ');

        if (li16SpacePos == (uint16)-1)
        {
          li16SpacePos = 0;
        }

        if (li16SpacePos > 0)
        {
          lszStr = lcParam;
          lszStr.Cut(li16SpacePos, 1000);
          lszStr.Trim();

          i16 li16Value1 = (lszStr.Atoi() - 1);
          mcPowerMonitor.stGuiTabs.enTopTab = (cPowerMonitor::tenGuiTopMenu)((u8)(li16Value1));

          lszStr = lcParam;
          lszStr.Cut(0, li16SpacePos);
          lszStr.Trim();
          li16Value1 = (lszStr.Atoi() - 1);
          mcPowerMonitor.stGuiTabs.enTopSubTab_Status = (cPowerMonitor::tenGuiTopMenuSub_Status)((u8)(li16Value1));
        }
      }

      return True;
    }
};


class cBotNetStreamPort_BotNetPowMon_PowMon1: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_PowMon1():cCliCmd((const char*)"PowMon1", (const char*)"Show Power Monitor 1")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr   lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      mcPowerMonitor.stGlobals.mClock.toString(mcPowerMonitor.stGlobals.mcStr);
      lszStr.Setf((const char8*)"Clock: %s", (char8*)mcPowerMonitor.stGlobals.mcStr.ToString());
      lcCli->bPrintLn(lszStr);

      lszStr.Setf((const char8*)"U_Sup: %4d", mcPowerMonitor.stGlobals.i32U_Supply); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I_Sup: %4d", mcPowerMonitor.stGlobals.i32I_Supply); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"U_Bat: %4d", mcPowerMonitor.stGlobals.i32U_Bat); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I_Bat: %4d", mcPowerMonitor.stGlobals.i32I_Bat); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"\nI_SysMin: %4d", mcPowerMonitor.stGlobals.i32I_Sys_NoRadio_NoDisplay); lcCli->bPrintLn(lszStr);
      
      return True;
    }
};


class cBotNetStreamPort_BotNetPowMon_PowMon2: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_PowMon2():cCliCmd((const char*)"PowMon2", (const char*)"Show Power Monitor 2")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr   lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (lcParam.Len() > 0)
      {
        mcPowerMonitor.stGlobals.u32PowerCounterIn  = 0;
        mcPowerMonitor.stGlobals.u32PowerCounterOut = 0;
      }

      lszStr.Setf((const char8*)"I_Bat_in:   %10d", mcPowerMonitor.stGlobals.u32PowerCounterIn); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I_Bat_out:  %10d", mcPowerMonitor.stGlobals.u32PowerCounterOut); lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I_Bat_diff: %10d", (i32)mcPowerMonitor.stGlobals.u32PowerCounterIn - (i32)mcPowerMonitor.stGlobals.u32PowerCounterOut); lcCli->bPrintLn(lszStr);
      
      return True;
    }
};


class cBotNetStreamPort_BotNetPowMon_Charge: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetPowMon_Charge():cCliCmd((const char*)"Charge", (const char*)"Charge on/off")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr   lszStr(lszStrBuf, 32);
      bool   bChargeOn = False;

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      if (lcParam.Len() > 0)
      {
        if (lcParam.Instr(0, (const char8*)"on") == 0)
        {
          bChargeOn = True;
        }
      }

      if (bChargeOn)
      {
        mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1] = 1;
        mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS3] = 1;
        lcCli->bPrintLn((const char8*)"Set charge on");
      }
      else
      {
        mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1] = 0;
        mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS3] = 0;
        lcCli->bPrintLn((const char8*)"Set charge off");
      }
      
      return True;
    }
};



class cBotNetStreamPort_BotNetPowMon: public cBotNetStreamPort
{
  public:
  cBotNetStreamPort_BotNetPowMon_SetMode  mcBnCliCmd_SetMode;
  cBotNetStreamPort_BotNetPowMon_SetTim   mcBnCliCmd_SetTim;
  cBotNetStreamPort_BotNetPowMon_SetDim   mcBnCliCmd_SetDim;
  cBotNetStreamPort_BotNetPowMon_SetDisp  mcBnCliCmd_SetDisp;
  cBotNetStreamPort_BotNetPowMon_PowMon1  mcBnCliCmd_PowMon1;
  cBotNetStreamPort_BotNetPowMon_PowMon2  mcBnCliCmd_PowMon2;
  cBotNetStreamPort_BotNetPowMon_Charge   mcBnCliCmd_Charge;

  cBotNetStreamPort_BotNetPowMon()
  {
    mszName     = (const char*)"BnPowMon v01.00.00";
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort_BotNetPowMon);
  }

  void vAddedToBn(cBotNet* lcBotNet) override
  {
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetMode);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetTim);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDim);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDisp);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_PowMon1);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_PowMon2);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Charge);
  }


  void vProcess(void) override
  {
  }
};

cBotNetStreamPort_BotNetPowMon mcBnPortPowMon;


class cBn_MsgProcess_0x1000 : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    (void)lpcBn;
    switch (lcMsg.mu16Idx)
    {
      case 80:
        lcMsg.mu16Idx++;
      break;
      default:
      break;
    }
  }
};

cBn_MsgProcess_0x1000 mcBn_MsgProcess_0x1000;

cBotNetStreamPort_BotNetMemPort  mcMemPort(RomConst_Partition_Count, RomConst_stDevice_Info->stPartitions);
cBotNetStreamPort_TextPort       mcTextPort(cBotNetStreamPort_TextPort::nFitSmartRev, 256+128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);

// Device ID: 8 = Clock Master
cBotNet mcBnNode(&mcMyBotNetCfg, &mcBn_MsgProcess_0x1000);


// --- 0xE000 SideLink => PC
cNRF905                  mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905     mcSideLnk_0x1000Rf(&mcNRF905);
cBotNet_UpLinknRf905Net  mcSideLnk_0x1000(&mcSideLnk_0x1000Rf, &mcBnNode);


void I2C1_EV_IRQHandler(void)
{
  mcI2C1.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1.I2C_ER_IRQHandler();
}


void TIM1_BRK_TIM15_IRQHandler(void)
{
  // BotNet nRF905 Timeout
  if (TIM15->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM15->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    //mcI2C2_BnMaster.TIM_EV_IRQHandler();
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  // SPI TX Display
  DMA1_Channel5->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_GL5 | DMA_FLAG_HT5 | DMA_FLAG_TC5;
  mcDisplay.vEvDmaTc();
}



void EXTI15_10_IRQHandler(void)
{
  // Adress Match nRF905
  if (__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_15) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_15);
    mcSideLnk_0x1000.bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}


void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX nRF905
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC2;
  mcSideLnk_0x1000.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX nRF905
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC3;
  mcSideLnk_0x1000.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
}

void TIM7_IRQHandler(void)
{
  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSideLnk_0x1000.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}

void MAIN_vUpdateGraph(void)
{
  static i32 li32USupplyAvg = -9999;
  static i32 li32ISupplyAvg = -9999;
  static i32 li32UBatAvg = -9999;
  static i32 li32IBatAvg = -9999;
  static u16 li16Idx = 0;

  if (li32USupplyAvg != -9999)
  {
    li32USupplyAvg = (li32USupplyAvg + mcPowerMonitor.stGlobals.i32U_Supply) / 2;
  }
  else
  {
    li32USupplyAvg = mcPowerMonitor.stGlobals.i32U_Supply;
  }

  if (li32ISupplyAvg != -9999)
  {
    li32ISupplyAvg = (li32ISupplyAvg + mcPowerMonitor.stGlobals.i32I_Supply) / 2;
  }
  else
  {
    li32ISupplyAvg = mcPowerMonitor.stGlobals.i32I_Supply;
  }

  if (li32UBatAvg != -9999)
  {
    li32UBatAvg = (li32UBatAvg + mcPowerMonitor.stGlobals.i32U_Bat) / 2;
  }
  else
  {
    li32UBatAvg = mcPowerMonitor.stGlobals.i32U_Bat;
  }

  if (li32IBatAvg != -9999)
  {
    li32IBatAvg = (li32IBatAvg + mcPowerMonitor.stGlobals.i32I_Bat) / 2;
  }
  else
  {
    li32IBatAvg = mcPowerMonitor.stGlobals.i32I_Bat;
  }

  li16Idx++;
  if (li16Idx > MAIN_nGRAPH_POINTS)
  {
    li16Idx = MAIN_nGRAPH_POINTS;
    mcGraphChnl_USupply.vSubX(1);
    mcGraphChnl_ISupply.vSubX(1);
    mcGraphChnl_UBat.vSubX(1);
    mcGraphChnl_IBat.vSubX(1);
  }

  mcGraphChnl_USupply.vAddXY(li16Idx, li32USupplyAvg);
  mcGraphChnl_ISupply.vAddXY(li16Idx, li32ISupplyAvg);
  mcGraphChnl_UBat.vAddXY(li16Idx, li32UBatAvg);
  mcGraphChnl_IBat.vAddXY(li16Idx, li32IBatAvg);

  while (li32ISupplyAvg > mcGraphChnl_ISupply.mi16SkalYMax)
  {
    switch (mcGraphChnl_ISupply.mi16SkalYMax)
    {
    case  10: mcGraphChnl_ISupply.vSkalY_Value(0,20);   break;
    case  20: mcGraphChnl_ISupply.vSkalY_Value(0,50);   break;
    case  50: mcGraphChnl_ISupply.vSkalY_Value(0,100);  break;
    case 100: mcGraphChnl_ISupply.vSkalY_Value(0,200);  break;
    case 200: mcGraphChnl_ISupply.vSkalY_Value(0,500);  break;
    case 500: mcGraphChnl_ISupply.vSkalY_Value(0,1000); break;
    default:  mcGraphChnl_ISupply.vSkalY_Value(0,1000);
    }
  }

  while (li32IBatAvg > mcGraphChnl_IBat.mi16SkalYMax)
  {
    switch (mcGraphChnl_IBat.mi16SkalYMax)
    {
    case  10: mcGraphChnl_IBat.vSkalY_Value(0,20);   break;
    case  20: mcGraphChnl_IBat.vSkalY_Value(0,50);   break;
    case  50: mcGraphChnl_IBat.vSkalY_Value(0,100);  break;
    case 100: mcGraphChnl_IBat.vSkalY_Value(0,200);  break;
    case 200: mcGraphChnl_IBat.vSkalY_Value(0,500);  break;
    case 500: mcGraphChnl_IBat.vSkalY_Value(0,1000); break;
    default:  mcGraphChnl_IBat.vSkalY_Value(0,1000);
    }
  }
}


void MAIN_vTick1d(void)
{
}


void MAIN_vTick1h(void)
{
}


void MAIN_vTick1000msLp(void)
{
  static u16 lu16Counter12Min = 0;

  //static uint8 lui8DayChanged = 0;

  if (mcPowerMonitor.stGlobals.ui8PowerTimer)
  {
    if (mcOpMode.IsAuto())
    {
      mcPowerMonitor.stGlobals.ui8PowerTimer--;
    }
  }

  mcPowerMonitor.stGlobals.mClock.vAdd1s();

  //if (lui8DayChanged != mcPowerMonitor.stGlobals.stClock.stDate.ui8Day)
  //{
    //Datumwechsel
    // 24Stunden Anzeigen rücksetzten

    //lui8DayChanged = mcPowerMonitor.stGlobals.stClock.stDate.ui8Day;
  //}

  if (lu16Counter12Min == 0)
  {
    MAIN_vUpdateGraph();
    lu16Counter12Min = 60 * 12;
  }
  else
  {
    lu16Counter12Min--;
  }

  // kein 220V Relais ?
  // Dann Powercounter erhöhen
  if (mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1] == 0)
  {
    if (mcPowerMonitor.stGlobals.i32I_Bat > 0)
    {
      mcPowerMonitor.stGlobals.u32PowerCounterIn  += mcPowerMonitor.stGlobals.i32I_Bat;
    }
    else
    {
      mcPowerMonitor.stGlobals.u32PowerCounterOut += (-mcPowerMonitor.stGlobals.i32I_Bat);
    }
  }

  {
    u8 lszData[14];
    
    cMemTools::vMemSet(lszData, 0, 14);
    lszData[0] = ((u16)mcPowerMonitor.stGlobals.i32U_Supply) >> 8;
    lszData[1] = ((u8)mcPowerMonitor.stGlobals.i32U_Supply);
    lszData[2] = ((u16)mcPowerMonitor.stGlobals.i32I_Supply) >> 8;
    lszData[3] = ((u16)mcPowerMonitor.stGlobals.i32I_Supply);
    lszData[4] = ((u16)mcPowerMonitor.stGlobals.fU_BatAvg) >> 8;
    lszData[5] = ((u16)mcPowerMonitor.stGlobals.fU_BatAvg);
    lszData[6] = ((u16)((i16)mcPowerMonitor.stGlobals.fI_BatAvg)) >> 8;
    lszData[7] = ((u16)((i16)mcPowerMonitor.stGlobals.fI_BatAvg));

    lszData[8] = (u8)((mcPowerMonitor.stGlobals.aui8Switches[0] << 2) + 
                      (mcPowerMonitor.stGlobals.aui8Switches[1] << 1) + 
                      (mcPowerMonitor.stGlobals.aui8Switches[2] << 0));


    mcSideLnk_0x1000.vSetPingData(lszData);
  }

  {
    u8 lszData[16];

    cMemTools::vMemSet((u8*)lszData, 0, 16);
    lszData[8] = mcPowerMonitor.stGlobals.mClock.mDate.stDate.ui16Year >> 8;
    lszData[9] = mcPowerMonitor.stGlobals.mClock.mDate.stDate.ui16Year &  0xFF;
    lszData[10] = mcPowerMonitor.stGlobals.mClock.mDate.stDate.ui8Month;
    lszData[11] = mcPowerMonitor.stGlobals.mClock.mDate.stDate.ui8Day;
    lszData[12] = mcPowerMonitor.stGlobals.mClock.mTime.stTime.ui8Hour;
    lszData[13] = mcPowerMonitor.stGlobals.mClock.mTime.stTime.ui8Minute;
    lszData[14] = mcPowerMonitor.stGlobals.mClock.mTime.stTime.ui8Second;
    lszData[15] = mcPowerMonitor.stGlobals.mClock.mValid;

    mcSideLnk_0x1000.vSetStatus(0x10, (u8*)lszData);
  }

  mcOpMode.vTick1000ms();
}


void MAIN_vTick100msLp(void)
{
  InAppProg_Platform_vResetWdog();
}

void MAIN_vTick50msLp(void)
{
  u8 lu8BtnIdx;

  //CLOCK_vTimeDate_from_32Bit(CLOCK_ui32RTC_Get(), &mcPowerMonitor.stGlobals.stClock);

  for (lu8BtnIdx = (u8)cPowerMonitor::MAIN_nButton_1; lu8BtnIdx < (u8)cPowerMonitor::MAIN_nButton_Max; lu8BtnIdx++)
  {
    if (mcPowerMonitor.stGlobals.aui8Buttons[lu8BtnIdx] != BUTTON_GetState(lu8BtnIdx))
    {
      if (BUTTON_GetState(lu8BtnIdx))
      {
        MAIN_Button_vPressed((cPowerMonitor::tenButtons)lu8BtnIdx);
      }
      else
      {
        MAIN_Button_vReleased((cPowerMonitor::tenButtons)lu8BtnIdx);
      }

      mcPowerMonitor.stGlobals.aui8Buttons[lu8BtnIdx] = BUTTON_GetState(lu8BtnIdx);
    }
  }

  MAIN_Switch_DoAction();



  MAIN_vSetDimming();

  mcINA219_Bat.i8ReadVShunt_digit();
  mcINA219_Bat.i8ReadVBus_digit();

  mcINA219_Supply.i8ReadVShunt_digit();
  mcINA219_Supply.i8ReadVBus_digit();

  mcPowerMonitor.stGlobals.i32U_Supply = mcINA219_Supply.i32CalcVBus_uV() / 1000;
  mcPowerMonitor.stGlobals.i32I_Supply = mcINA219_Supply.i32CalcIShunt_uA() / 1000;
  mcPowerMonitor.stGlobals.i32U_Bat    = mcINA219_Bat.i32CalcVBus_uV() / 1000;
  mcPowerMonitor.stGlobals.i32I_Bat    = mcINA219_Bat.i32CalcIShunt_uA() / 1000;

  mcPowerMonitor.stGlobals.fU_BatAvg += (float)((float)mcPowerMonitor.stGlobals.i32U_Bat - mcPowerMonitor.stGlobals.fU_BatAvg) / (20.0f * 60.0f); /* 1min Dämpfung */


  if (mcPowerMonitor.stGlobals.i32I_Supply > 15)
  {
    //kleinere Dämpfung bei Aufladung
    mcPowerMonitor.stGlobals.fI_BatAvg += (float)((float)mcPowerMonitor.stGlobals.i32I_Bat - mcPowerMonitor.stGlobals.fI_BatAvg) / (20.0f); /* 1s Dämpfung */
  }
  else
  {
    mcPowerMonitor.stGlobals.fI_BatAvg += (float)((float)mcPowerMonitor.stGlobals.i32I_Bat - mcPowerMonitor.stGlobals.fI_BatAvg) / (20.0f * 60.0f); /* 1min Dämpfung */
  }

  if ((menSmMain_Display == cPowerMonitor::MAIN_nSmDisplayIdle) ||
      (menSmMain_Display == cPowerMonitor::MAIN_nSmDisplayPowerSave))
  { 
    if (mcNRF905.menMode == cNRF905::NRF905_RADIO_MODE_PWR_DOWN)
    {
      mcPowerMonitor.stGlobals.i32I_Sys_NoRadio_NoDisplay = mcPowerMonitor.stGlobals.i32I_Bat;
    }
  }

  Main_vSmDisplay();
}

void MAIN_vTick10msLp(void)
{
  static uint8 lui8Tick10msCounter = 0;

  //vMain_SmAdc(MAIN_nSmAdcRun);

  //50ms ?
  if (lui8Tick10msCounter >= 50 / 10)
  {
    MAIN_vTick50msLp();
    lui8Tick10msCounter = 0;
  }
  else
  {
    lui8Tick10msCounter++;
  }

  mcBnNode.vTickLp10ms();
}


void MAIN_vTick1msHp(void)
{
  mcBnNode.vTickHp1ms();
}

void MAIN_vTick10msHp(void)
{
  mcDcf77.vSm(cDcf77::nEvTick);
  if (mcDcf77.IsDataReady())
  {
    mcPowerMonitor.stGlobals.mClock = mcDcf77.mcClock;
    mcPowerMonitor.stGlobals.mClock.mDate.stDate.ui16Year += 2000;
  }

  if (mPinDcf77Data.ui8Get())
  {
    MAIN_DCF_vReleased();
  }
  else
  {
    MAIN_DCF_vPressed();
  }

  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }
}

void MAIN_vSetDimming(void)
{
  if (mcPowerMonitor.stGlobals.ui8PowerTimer > 0)
  {
    TIM1->CCR2 = mcPowerMonitor.stGlobals.ui8DimValue;
  }
  else
  {
    TIM1->CCR2 = 0;
  }
}


void MAIN_Button_vPressed(cPowerMonitor::tenButtons len8Button)
{
  mcPowerMonitor.stGlobals.aui8Buttons[len8Button] = 1;

  if (mcPowerMonitor.stGlobals.ui8PowerTimer > 0)
  {
    switch (len8Button)
    {
      // Button1 (links oben) für den Hauptreiter
      // ...->Status->Konsole->Info->Einstellungen->...
      case cPowerMonitor::MAIN_nButton_1:
        mcPowerMonitor.stGuiTabs.enTopTab = (cPowerMonitor::tenGuiTopMenu)((u8)(mcPowerMonitor.stGuiTabs.enTopTab) + 1);
        if ((u8)mcPowerMonitor.stGuiTabs.enTopTab >= (u8)cPowerMonitor::MAIN_nGuiTopMenu_LastEntry)
        {
          mcPowerMonitor.stGuiTabs.enTopTab = cPowerMonitor::MAIN_nGuiTopMenu_FirstEntry;
        }
        break;

      // Button2 (rechts oben) für den Unterreiter      
      case cPowerMonitor::MAIN_nButton_2:
        switch (mcPowerMonitor.stGuiTabs.enTopTab)
        {
          // Haupt: Status
          // ...->Power->ADC1->ADC2->ADC3->ADC4->DCF->Funk->...
          case cPowerMonitor::MAIN_nGuiTopMenu_Status:
            mcPowerMonitor.stGuiTabs.enTopSubTab_Status = (cPowerMonitor::tenGuiTopMenuSub_Status)((u8)(mcPowerMonitor.stGuiTabs.enTopSubTab_Status) + 1);
            if ((u8)mcPowerMonitor.stGuiTabs.enTopSubTab_Status >= (u8)cPowerMonitor::MAIN_nGuiTopMenuSub_Status_LastEntry)
            {
              mcPowerMonitor.stGuiTabs.enTopSubTab_Status = cPowerMonitor::MAIN_nGuiTopMenuSub_Status_FirstEntry;
            }
            break;
          // Haupt: Einstellungen
          // ...->Dimmung->...
          // Auskommentiert, solange es nur einen Reiter gibt
          /*case cPowerMonitor::MAIN_nGuiTopMenu_Einstellungen:
            mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen = (cPowerMonitor::tenGuiTopMenuSub_Einstellungen)((u8)(mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen) + 1);
            if ((u8)mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen >= (u8)cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_LastEntry)
            {
              mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen = cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_FirstEntry;
            }
            break;*/
          default:
            break;
        }
        break;

      // Button3 (links unten) für den Hauptreiter
      case cPowerMonitor::MAIN_nButton_3:
        switch (mcPowerMonitor.stGuiTabs.enTopTab)
        {
          case cPowerMonitor::MAIN_nGuiTopMenu_Status:
            switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Status)
            {
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Power: // Status->Power
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC1:  // Status->ADC1
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC2:  // Status->ADC2
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC3:  // Status->ADC3
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC4:  // Status->ADC4
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_DCF:   // Status->DCF
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Funk:  // Status->Funk
                break;
              default:
                break;
            }

          break;

          case cPowerMonitor::MAIN_nGuiTopMenu_Konsole:
            break;
          case cPowerMonitor::MAIN_nGuiTopMenu_Info:
            break;
          case cPowerMonitor::MAIN_nGuiTopMenu_Einstellungen:
            switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen)
            {
              // Einstellungen->Dimmung
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_Dimmung:
                // Dimmung verringern
                if (mcPowerMonitor.stGlobals.ui8DimValue > 5)
                {
                  mcPowerMonitor.stGlobals.ui8DimValue -= 5;
                }
                else
                {
                  mcPowerMonitor.stGlobals.ui8DimValue = 0;
                }
                break;
            default:
              break;
          }
          break;
          default:
            break;
        }
        break;

      case cPowerMonitor::MAIN_nButton_4:
        switch (mcPowerMonitor.stGuiTabs.enTopTab)
        {
          case cPowerMonitor::MAIN_nGuiTopMenu_Status:
            switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Status)
            {
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Power: // Status->Power
                // PowerCounter auf 0 setzen
                mcPowerMonitor.stGlobals.u32PowerCounterIn  = 0;
                mcPowerMonitor.stGlobals.u32PowerCounterOut = 0;
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC1:  // Status->ADC1
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC2:  // Status->ADC2
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC3:  // Status->ADC3
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC4:  // Status->ADC4
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_DCF:   // Status->DCF
                break;
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Funk:  // Status->Funk
                break;
              default:
                break;
            }

            break;
          case cPowerMonitor::MAIN_nGuiTopMenu_Konsole:
            break;
          case cPowerMonitor::MAIN_nGuiTopMenu_Info:
            break;
          case cPowerMonitor::MAIN_nGuiTopMenu_Einstellungen:
            switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen)
            {
              // Einstellungen->Dimmung
              case cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_Dimmung:
                // Dimmung erhöhen
                mcPowerMonitor.stGlobals.ui8DimValue += 5;
                if (mcPowerMonitor.stGlobals.ui8DimValue > 100) mcPowerMonitor.stGlobals.ui8DimValue = 100;
                break;
              default:
                break;
            }
          break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }

  mcPowerMonitor.vInitPowerTimer();
}

void MAIN_Button_vReleased(cPowerMonitor::tenButtons len8Button)
{
  mcPowerMonitor.stGlobals.aui8Buttons[len8Button] = 0;
}

void MAIN_DCF_vPressed(void)
{
  mcPowerMonitor.stGlobals.ui8DCF = 1;
}

void MAIN_DCF_vReleased(void)
{
  mcPowerMonitor.stGlobals.ui8DCF = 0;
}

void MAIN_PIR_vPressed(void)
{
  mcPowerMonitor.vInitPowerTimer();
}

void MAIN_PIR_vReleased(void)
{
  mcPowerMonitor.vInitPowerTimer();
}


void MAIN_Paint_vTopTab(void)
{
  switch (mcPowerMonitor.stGuiTabs.enTopTab)
  {
  case cPowerMonitor::MAIN_nGuiTopMenu_Status:
    mcSpriteEng.i8Set(0, 0, (uint8*)SPRTMST_DATA_MainTop1_Status);

    switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Status)
    {
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Power:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_1_Power);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC1:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_2_ADC1);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC2:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_3_ADC2);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC3:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_4_ADC3);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC4:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_5_ADC4);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_DCF:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_6_DCF);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Funk:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop1_Status, (uint8*)SPRTMST_DATA_MainTop1_7_Funk);
      break;
    default:
      break;
    }

    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Konsole:
    mcSpriteEng.i8Set(0, 0, (uint8*)SPRTMST_DATA_MainTop2_Konsole);
    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Info:
    mcSpriteEng.i8Set(0, 0, (uint8*)SPRTMST_DATA_MainTop3_Info);
    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Einstellungen:
    mcSpriteEng.i8Set(0, 0, (uint8*)SPRTMST_DATA_MainTop4_Einstellungen);

    switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen)
    {
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_Dimmung:
      mcSpriteEng.i8Set(0, 0 + SPRTMST_HEIGHT_nMainTop4_Einstellungen, (uint8*)SPRTMST_DATA_MainTop4_1_Dim);
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void MAIN_Paint_vMid_Status(void)
{
  if (mcPowerMonitor.stGuiTabs.enTopSubTab_Status == cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC1)
  {
    mcPowerMonitor.stGlobals.ui8AdcIdx = 0;
  }

  if (mcPowerMonitor.stGuiTabs.enTopSubTab_Status == cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC2)
  {
    mcPowerMonitor.stGlobals.ui8AdcIdx = 1;
  }

  if (mcPowerMonitor.stGuiTabs.enTopSubTab_Status == cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC3)
  {
    mcPowerMonitor.stGlobals.ui8AdcIdx = 2;
  }

  if (mcPowerMonitor.stGuiTabs.enTopSubTab_Status == cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC4)
  {
    mcPowerMonitor.stGlobals.ui8AdcIdx = 3;
  }

  switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Status)
  {
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Power:
    {
      uint8 lui8BatLvl;
      uint8 ui8Col;
      uint8 ui8t;
      static uint8 ui8Blink500ms = 0;

      mcSpriteEng.i8Set(7, 60, SPRTMST_DATA_MainMidPower);

      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"In:  %d", mcPowerMonitor.stGlobals.u32PowerCounterIn/1000);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(5, 37, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Out: %d", mcPowerMonitor.stGlobals.u32PowerCounterOut/1000);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(5, 45, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);


      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%d", mcPowerMonitor.stGlobals.i32U_Bat);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(25, 106, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%d", mcPowerMonitor.stGlobals.i32I_Bat);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(25, 114, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);


      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%d", mcPowerMonitor.stGlobals.i32U_Supply);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 76, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%d", mcPowerMonitor.stGlobals.i32I_Supply);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 84, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);


      if (mcPowerMonitor.stGlobals.aui8Switches[0]) mcScreen1.vLine(20,67, 30,67,   1);
                                               else mcScreen1.vLine(20,67, 30,67-5, 1);

      if (mcPowerMonitor.stGlobals.aui8Switches[1]) mcScreen1.vLine(59,90, 69,90,   1);
                                               else mcScreen1.vLine(59,90, 69,90-5, 1);
      if (mcPowerMonitor.stGlobals.aui8Switches[2]) mcScreen1.vLine(88,90, 98,90,   1);
                                               else mcScreen1.vLine(88,90, 98,90-5, 1);

      /*
        BatLvl: (4200-2400 / 9) = 200mV

        Lvl 0:        < 2400mV: 0 Bars
        Lvl 1: 2400mV - 2600mV: 1 Bars (blinking)
        Lvl 2: 2600mV - 2800mV: 1 Bars
        Lvl 3: 2800mV - 3000mV: 2 Bars (blinking)
        Lvl 4: 3000mV - 3200mV: 2 Bars
        Lvl 5: 3200mV - 3400mV: 3 Bars (blinking)
        Lvl 6: 3400mV - 3600mV: 3 Bars
        Lvl 7: 3600mV - 3800mV: 4 Bars (blinking)
        Lvl 8: 3800mV - 4000mV: 4 Bars
        Lvl 9: >4000mV        : 4 Bars
      */

      ui8Col = 1;

      if (mcPowerMonitor.stGlobals.i32U_Bat < 2400)
      {
        lui8BatLvl = 0;
      }
      else if (mcPowerMonitor.stGlobals.i32U_Bat > 4000)
      {
        lui8BatLvl = 9;
      }
      else
      {
        lui8BatLvl = (mcPowerMonitor.stGlobals.i32U_Bat - 2400) / 200;
        if (ui8Blink500ms < (500 / 25))
        {
          ui8Col = 0;
        }
      }

      for (ui8t = 0; ui8t < lui8BatLvl / 2; ui8t++)
      {
        cPaint::vRectFull(46, 60 + 54 - 4 * ui8t, 6, 3, 1, &mcScreen1);
      }

      // 1, 3, 5, 7
      if ((lui8BatLvl < 9) && (lui8BatLvl & 1) && (ui8Col == 1))
      {
        cPaint::vRectFull(46, 60 + 54 - 4 * ((lui8BatLvl - 1) / 2), 6, 3, 1, &mcScreen1);
      }


      ui8Blink500ms++;
      if (ui8Blink500ms > (1000 / 25))
      {
        ui8Blink500ms = 0;
      }
    }
    break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC1:
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(8, 38, (char8*)"Spannnung Supply", &mcScreen1);
      mcGraph.vPaint(&mcGraphChnl_USupply);
      mcSpriteEng.vSetParam(Sprite_nModeOr, &mcScreen1);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC2:
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(8, 38, (char8*)"Strom Supply", &mcScreen1);
      mcGraph.vPaint(&mcGraphChnl_ISupply);
      mcSpriteEng.vSetParam(Sprite_nModeOr, &mcScreen1);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC3:
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(8, 38, (char8*)"Spannnung Bat", &mcScreen1);
      mcGraph.vPaint(&mcGraphChnl_UBat);
      mcSpriteEng.vSetParam(Sprite_nModeOr, &mcScreen1);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_ADC4:
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(8, 38, (char8*)"Strom Bat", &mcScreen1);
      mcGraph.vPaint(&mcGraphChnl_IBat);
      mcSpriteEng.vSetParam(Sprite_nModeOr, &mcScreen1);
      break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_DCF:
    {
      GfxInt liY = 40;

      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"SmDCF77: %d ", mcDcf77.mstRtData.ui8SmDCF77);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Data: %d ", mcDcf77.mstRtData.ui8DCF77_Data);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Parität: %d ", mcDcf77.mstRtData.ui8DCF77_DataPar);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Valid: %d ", mcDcf77.mstRtData.ui8DCF77_DataValid);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Count: %d ", mcDcf77.mstRtData.ui8DCF77_DataCnt);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Pin: %d ", mcDcf77.mstRtData.ui8DCF77_Pin);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Filter: %08b ", mcDcf77.mstRtData.ui8DCF77_Filter);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"OnTicks: %d ", mcDcf77.mstRtData.ui8DCF77_Pin_OnTicks);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

      liY += 10;
      mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"OffTicks: %d ", mcDcf77.mstRtData.ui8DCF77_Pin_OffTicks);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(5, liY, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);
    }
    break;
    case cPowerMonitor::MAIN_nGuiTopMenuSub_Status_Funk:
      break;
    default:
      break;
  }
}

void MAIN_Paint_vMid_Konsole(void)
{
  cScreen_Bpp1_1G lcMid(2, 17, MAIN_nDISPLAY_X - 3, MAIN_nDISPLAY_Y - 38, &mcScreen1);
  mcTextPort.vPaint(&lcMid);
}


void MAIN_Paint_vMid_Info(void)
{
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(3, 25, (const char8*)"SW-LBL:  Solarbox", &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(3, 35, (const char8*)"SW-VER:  01.01.pre", &mcScreen1);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(3, 45, (const char8*)"SW-DATE: 2020-08-05", &mcScreen1);
}

void MAIN_Paint_vMid_Einstellungen_Dim(void)
{
  if (mcPowerMonitor.stGlobals.ui8DimValue > 0)
  {
    cPaint::vRectFull(10, 85, mcPowerMonitor.stGlobals.ui8DimValue, 10, 1, &mcScreen1);
  }

  mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"Dimmung: %3d ", mcPowerMonitor.stGlobals.ui8DimValue);
  cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(20, 80, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);
}

void MAIN_Paint_vMid_Einstellungen(void)
{
  switch (mcPowerMonitor.stGuiTabs.enTopSubTab_Einstellungen)
  {
  case cPowerMonitor::MAIN_nGuiTopMenuSub_Einstellungen_Dimmung:
    MAIN_Paint_vMid_Einstellungen_Dim();
    break;
  default:
    break;
  }
}


void MAIN_Paint_vMid(void)
{
  mcScreen1.vLineV(0, 0, 175, 1);
  mcScreen1.vLineV(119, 0, 175, 1);

  switch (mcPowerMonitor.stGuiTabs.enTopTab)
  {
  case cPowerMonitor::MAIN_nGuiTopMenu_Status:
    MAIN_Paint_vMid_Status();
    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Konsole:
    MAIN_Paint_vMid_Konsole();
    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Info:
    MAIN_Paint_vMid_Info();
    break;
  case cPowerMonitor::MAIN_nGuiTopMenu_Einstellungen:
    MAIN_Paint_vMid_Einstellungen();
    break;
  default:
    break;
  }
}


void MAIN_Paint_vBottom(void)
{
  mcSpriteEng.vSetParam(Sprite_nModeOr, &mcScreen1);
  mcSpriteEng.i8Set(0, 176 - SPRTMST_HEIGHT_nMainBottom, SPRTMST_DATA_MainBottom);

  mcPowerMonitor.stGlobals.mClock.toString(mcPowerMonitor.stGlobals.mcStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(25, 169, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

  mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%3d", mcPowerMonitor.stGlobals.ui8DimValue);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(46, 147, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

  mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%3d", mcPowerMonitor.stGlobals.ui8PowerTimer);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(48, 157, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

  mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%4d", (i32)mcPowerMonitor.stGlobals.fU_BatAvg);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(86, 147, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

  mcPowerMonitor.stGlobals.mcStr.Setf((const char8*)"%4d", (i32)mcPowerMonitor.stGlobals.fI_BatAvg);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(86, 157, (char8*)mcPowerMonitor.stGlobals.mcStr.ToString(), &mcScreen1);

  if (mcPowerMonitor.stGlobals.aui8Buttons[cPowerMonitor::MAIN_nButton_1]) cPaint::vRectFull( 4, 144, 4, 4, 1, &mcScreen1);
  if (mcPowerMonitor.stGlobals.aui8Buttons[cPowerMonitor::MAIN_nButton_2]) cPaint::vRectFull(12, 144, 4, 4, 1, &mcScreen1);
  if (mcPowerMonitor.stGlobals.aui8Buttons[cPowerMonitor::MAIN_nButton_3]) cPaint::vRectFull( 4, 153, 4, 4, 1, &mcScreen1);
  if (mcPowerMonitor.stGlobals.aui8Buttons[cPowerMonitor::MAIN_nButton_4]) cPaint::vRectFull(12, 153, 4, 4, 1, &mcScreen1);
  if (mcPowerMonitor.stGlobals.ui8DCF)                                     cPaint::vRectFull(3, 166, 3, 3, 1, &mcScreen1);
}


void MAIN_vPaint_vAll(void)
{
  if (mcDisplay.isReady())
  {
    mcScreen1.vFill(0);

    MAIN_Paint_vTopTab();
    MAIN_Paint_vMid();
    MAIN_Paint_vBottom();

    mcDisplay.vShowScreenDma(mcScreen1.mpcBm->mpui8Data);
  }
}


void Main_vSmDisplay(void)
{
  switch (menSmMain_Display)
  {
  case cPowerMonitor::MAIN_nSmDisplayIdle:
  case cPowerMonitor::MAIN_nSmDisplayPowerSave:
    if (mcPowerMonitor.stGlobals.ui8PowerTimer > 0)
    {
      menSmMain_Display = cPowerMonitor::MAIN_nSmDisplayOn;

      // PWM Display Dimming
      MAIN_DISP_vInit();
      mcDisplay.vInit();
    }
    break;

  case cPowerMonitor::MAIN_nSmDisplayOn:
    if (mcPowerMonitor.stGlobals.ui8PowerTimer > 0)
    {
      MAIN_vPaint_vAll();
    }
    else
    {
      menSmMain_Display = cPowerMonitor::MAIN_nSmDisplayPowerSave;

      MAIN_DISP_vDeInit();
      mcDisplay.vDeInit();
    }
    break;
  default:
    break;
  }
}



// -------------------- PIR -----------------------------------------

tcGpPin<GPIOA_BASE, 0>  mS1(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // 220V Relais
tcGpPin<GPIOA_BASE, 1>  mS2(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1); // Lipo System Versorgung
tcGpPin<GPIOB_BASE,10>  mS3(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); // 5V Boost Schalter

void MAIN_Switch_vInit(void)
{
}

void MAIN_Switch_DoAction(void)
{
  // Schalter automatic

  if (mcPowerMonitor.stGlobals.i32U_Bat < 2500)
  {
    mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS2] = 0;
  }

  if (mcPowerMonitor.stGlobals.i32U_Bat < 3100)
  {
    mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1] = 1;
    mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS3] = 1;
  }
  else
  if ((mcPowerMonitor.stGlobals.i32U_Bat > 4150) || 
      ((mcPowerMonitor.stGlobals.i32U_Bat > 4000) && (mcPowerMonitor.stGlobals.i32I_Bat <   25)))
  {
    mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1] = 0;
    mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS3] = 0;
  }

  // Zwischen Tri-State und Output Gnd schalten
  // Relais ist low aktiv
  if (mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS1])
  {
    mS1.vSetMode(GPIO_MODE_OUTPUT_PP);
    mS1.vSet(0);
  }
  else
  {
    mS1.vSetMode(GPIO_MODE_ANALOG);
  }  

  mS2.vSet(mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS2]);
  mS3.vSet(mcPowerMonitor.stGlobals.aui8Switches[cPowerMonitor::MAIN_nS3]);
  
}


// -------------------- BUTTONS -----------------------------------------
tcGpPin<GPIOA_BASE, 10>  mPinBtn1(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
tcGpPin<GPIOB_BASE, 11>  mPinBtn2(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
tcGpPin<GPIOA_BASE, 11>  mPinBtn3(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
tcGpPin<GPIOA_BASE, 12>  mPinBtn4(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

void MAIN_BUTTON_vInit()
{
}

uint8 BUTTON_GetState(uint8 lui8Button)
{
  switch (lui8Button)
  {
    case 0:
      return (!mPinBtn1.ui8Get());
      break;
    case 1:
      return (!mPinBtn2.ui8Get());
      break;
    case 2:
      return (!mPinBtn3.ui8Get());
      break;
    default:
      return (!mPinBtn4.ui8Get());
      break;
  }
}




// -------------------- ILLU -----------------------------------------

void TIM1_vInit(void)
{
  __HAL_RCC_TIM1_CLK_ENABLE();

  TIM_HandleTypeDef TimHandle = {};

  /* Set TIM1 instance */
  TimHandle.Instance = TIM1;

  /* Initialize TIM2 peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/1000000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 99; // 10ms
  TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 10000) -1;
  //TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  //TimHandle.Init.RepetitionCounter = 0;

  HAL_TIM_Base_Init(&TimHandle);

  TIM_OC_InitTypeDef sConfigOC = {};
  HAL_TIM_PWM_Init(&TimHandle);

  sConfigOC.OCMode       = TIM_OCMODE_PWM1;
  sConfigOC.Pulse        = mcPowerMonitor.stGlobals.ui8DimValue;; //25
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_2);

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  HAL_TIM_Base_Start(&TimHandle);
  HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_2);
}

void TIM1_vDeInit(void)
{
  TIM_HandleTypeDef TimHandle = {};
  /* Set TIM1 instance */
  TimHandle.Instance = TIM1;

  HAL_TIM_Base_Stop(&TimHandle);
  HAL_TIM_Base_DeInit(&TimHandle);
  __HAL_RCC_TIM1_CLK_DISABLE();
}

//tcGpPin<GPIOB_BASE,  0>  mPinDispIllu(   GPIO_MODE_AF_PP ,    GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);
tcGpPin<GPIOB_BASE, 0>  mPinDispIllu(    GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);
tcGpPin<GPIOA_BASE, 15>  mPinDispBooster(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);


void MAIN_DISP_vInit(void)
{
  TIM1_vInit();
  mPinDispIllu.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);
  mPinDispBooster.vSet1();
}

void MAIN_DISP_vDeInit(void)
{
  TIM1_vDeInit();
  mPinDispIllu.vSetMode(GPIO_MODE_OUTPUT_PP);
  mPinDispIllu.vSet0();
  mPinDispBooster.vSet0();
}

// -------------------------------------------------------------

void MAIN_vInitSystem(void)
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
  InAppProg_Platform_vResetWdog();
  HAL_Init();
  HAL_SuspendTick();

  mcSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mcScreen1);

  mcGraphChnl_USupply.vSkalY_Value(0, 7000);
  mcGraphChnl_USupply.vSet_Y_Label((const char8*)"V [mV]");
  mcGraphChnl_USupply.mi16SkalYCnt = 5;

  mcGraphChnl_ISupply.vSkalY_Value(0,10);
  mcGraphChnl_ISupply.vSet_Y_Label((const char8*)"I [mA]");
  mcGraphChnl_ISupply.mi16SkalYCnt = 10;

  mcGraphChnl_UBat.vSkalY_Value(2000, 5000);
  mcGraphChnl_UBat.vSet_Y_Label((const char8*)"V [mV]");
  mcGraphChnl_UBat.mi16SkalYCnt = 5;

  mcGraphChnl_IBat.vSkalY_Value(-50,10);
  mcGraphChnl_IBat.vSet_Y_Label((const char8*)"I [mA]");
  mcGraphChnl_IBat.mi16SkalYCnt = 10;

  mcGraph.vSet_X_Label((const char8*)"t");
  mcGraph.mi16SkalXCnt = 4;
  mcGraph.mi16SkalXMax = MAIN_nGRAPH_POINTS;

  MAIN_BUTTON_vInit();
  MAIN_Switch_vInit();


  mcBnNode.bAddLink((cBotNet_LinkBase*)&mcSideLnk_0x1000, 0xE000);

  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBnNode.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);


  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  lu16PortIdx = mcBnNode.u16StreamPortAdd(&mcMemPort);
  mcBnNode.vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Add TextPort and connect the CmdPort's output to the Textport
  lu16PortIdx = mcBnNode.u16StreamPortAdd(&mcTextPort);
  mcBnNode.vStreamPortConnect(cBotNet_CmdPortIdx, 0, lu16PortIdx);

  // Add Power Monitor Commands Port (not connected)
  lu16PortIdx = mcBnNode.u16StreamPortAdd(&mcBnPortPowMon);

  mcBnNode.vSetName(RomConst_stDevice_Info->szDevice_Name);

  mcSideLnk_0x1000.vSetTiming(60 * 1000, 100);

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  mcINA219_Supply.i8Setup(INA219_BusVoltageRange_16V,
                          INA219_GainRange_2_80mV,
                          INA219_ShuntADCResolution_12Bit_1S_532uS,
                          INA219_ShuntADCResolution_12Bit_1S_532uS,
                          INA219_Mode_ShuntAndBusVoltageContinuous);

  mcINA219_Bat.i8Setup(INA219_BusVoltageRange_16V,
                       INA219_GainRange_2_80mV,
                       INA219_ShuntADCResolution_12Bit_1S_532uS,
                       INA219_ShuntADCResolution_12Bit_1S_532uS,
                       INA219_Mode_ShuntAndBusVoltageContinuous);

  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(10);
  }

  mcINA219_Bat.i8ReadVShunt_digit();
  mcINA219_Bat.i8ReadVBus_digit();
  mcINA219_Supply.i8ReadVShunt_digit();
  mcINA219_Supply.i8ReadVBus_digit();

  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(10);
  }

  mcPowerMonitor.stGlobals.i32U_Supply = mcINA219_Supply.i32CalcVBus_uV() / 1000;
  mcPowerMonitor.stGlobals.i32I_Supply = mcINA219_Supply.i32CalcIShunt_uA() / 1000;
  mcPowerMonitor.stGlobals.i32U_Bat    = mcINA219_Bat.i32CalcVBus_uV() / 1000;
  mcPowerMonitor.stGlobals.i32I_Bat    = mcINA219_Bat.i32CalcIShunt_uA() / 1000;

  mcPowerMonitor.stGlobals.fU_BatAvg = mcPowerMonitor.stGlobals.i32U_Bat;
  mcPowerMonitor.stGlobals.fI_BatAvg = mcPowerMonitor.stGlobals.i32I_Bat;

  if (mcPowerMonitor.stGlobals.i32U_Bat > 3100)
  {
    mcPowerMonitor.vInitPowerTimer();
  }

  InAppProg_Platform_vResetWdog();
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  if (!RomConst_IsValid())
  {
    // RomConst Error
    while (1) {}
  }

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
  {};
}

void SystemClock_Config(void)
{
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1|
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

