
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB



//  SolarBox 2.0. BotNetId 20 = 0x14
//  Master:   BnAdr: 1.0.0.0
//  MotBx:    BnAdr: 1.1.0.0
//  LightBx:  BnAdr: 1.2.0.0
//
//  PC: BotNetId 7;
//            BnAdr: E.0.0.0
//
//  Master:
//
//  Motor 1.1.0.0:
//    MotorStatus: 100ms: An 1.0.0.0: Idx: 0x40 | u8Flags i16IstPosDreh i16IstNeig
//                                                0000 000x: Referenzlauf aktiv 
//                                                0000 00x0: Referenzlauf fertig
//                                                0xxx 0000: Sensor (100 rechts 010 mitte 001 links)
//    MotorControl1: Event: An 1.1.0.0: Idx: 0x41 | u8Flags i16SollDreh i16SollNeig
//                                                0000 000x: Referenz-Lauf starten
//                                                0000 00x0: Stopp (Motoren oder Referenzlauf)
//    MotorControl2: Event: An 1.1.0.0: Idx: 0x42 | u8Flags i16MaxDreh i16MaxNeig
//                                                0000 000x: Lichtsanpassung aktivieren
//            Positionen: i16: -1800..1800: -180°..180° vom Nullpunkt
//
//
//
//  Light 1.2.0.0:
//    LichtStatus: 100ms: An 1.0.0.0: Idx: 0x50 | u8LR u8LL u8LO u8LU u8LH i16Neig i16Kompass
//    LichtStatus: 100ms: An 1.1.0.0: Idx: 0x50 | u8LR u8LL u8LO u8LU u8LH i16Neig i16Kompass
//            Neigung: i16: -1800..1800: -180°..180° von der Horizontalen; Negativ im Uhrzeigersinn
//            Kompass: i16: -1800..1800: -180°..180° von Norden;           Posetiv im Uhrzeigersinn
//
//
//  PA0   -> Wakeup
//
//  USART1 -> BotCom ComPort (19200)
//  PA09  -> UsartTx
//  PA10  -> UsartRx
//
//
//  PA08  -> Charger: Schalter S14
//  PB15  -> Charger: Schalter S10
//  PB14  -> Charger: Schalter S11
//  PB13  -> Charger: Schalter S13
//  PB12  -> Charger: Schalter S12
//  PA04  -> Charger: DAC1
//
//  PB0   -> Mainboard: Schalter S20
//
//  I2C1  -> INA319 3x, Display (Optional)
//  PB06  -> I2C1 SCL
//  PB07  -> I2C1 SDA
//
//  I2C2  -> BotNet
//  PB10  -> I2C2 SCL
//  PB11  -> I2C2 SDA
//
//  nRF905
//  PA05: SPI Clock
//  PA06: SPI MISO
//  PA07: SPI MOSI
//  PA01: Chip Select
//  PA02: TX or RX mode
//  PA03: Standby
//  PB03: Adress Match
//  PC13: Power up
//  PC14: Carrier Detected
//  PC15: Data Ready
//
//  TIM6  -> CyclicCaller
//  TIM15 -> BotCom
//
//  PB09  -> Status Led
//
//  PA15  -> Masse für Display und LED

/*Test*/
cGpPin mcI2c2_Wakeup(GPIOA_BASE, 0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

__IO uint32_t TimingDelay = 0;

cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cStatusLed<GPIOB_BASE, 9> mcStatusLed;

// I2C1:
cGpPin mcI2c1_SCL_Disp(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c1_SDA_Disp(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

// I2C2:
cGpPin mcI2c2_SCL_Bn(GPIOB_BASE, 10, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c2_SDA_Bn(GPIOB_BASE, 11, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


cI2cMaster   mcI2C1_Disp(I2C1, &mcI2c1_SCL_Disp, &mcI2c1_SDA_Disp, 8);
cSSD1306     mcSSD1306(&mcI2C1_Disp, 0x78, 0x38);

cGpPin *lcUsartRx;
cGpPin *lcUsartTx;


#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G          mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeCopy);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);

u8   mu8DispPage = 0;


cDigiPower       mcDigiPower;
cWorkMode        mcOpMode;
cLipoMon         mcLipoMon(&mcI2C1_Disp);

cClock           mClock;
u32              mu32ClockResync = 0;
u32              mu32ClockResyncTimeout;


 //lcUsartTx = (cGpPin*)  new cGpPin(GPIOA_BASE, 2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz, 0);
 //lcUsartRx = (cGpPin*)  new cGpPin(GPIOA_BASE, 3, GPIO_Mode_IPD,   GPIO_Speed_50MHz, 0);


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




class cBn_MsgProcess_0x1000 : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    (void)lpcBn;

    if (lcMsg.u8Len() == 25)
    {
      // Überprüfen, ob es eine Clock Nachricht ist
      //                         0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
      //                              0  1    2    3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
      //  Clock:                11 | DH DL | 10 | 00.XX.XX.XX XX.XX.XX.XX YH.YL.MM.DD HH.MM.SS.VV || 00 00 SA SA 80
      // Clock Nachricht ?
      if ((lcMsg.cGetSAdr() == 0x1000) &&
          (lcMsg.mu16Idx  == 0x11))
      {
        u16 lu16SourceAdr = (lcMsg.mcPayload[0] << 8) + lcMsg.mcPayload[1];
        if (lu16SourceAdr == 0x08) // Clock Device Adress
        {
          mClock.vDeserialize(&lcMsg.mcPayload[11]);
          mu32ClockResyncTimeout = 0;
        }
      }
    }

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
    }
  }
};


// BotNet
// I2C2
cI2cMaster  mcI2C2_BnMaster(I2C2, &mcI2c2_SCL_Bn, &mcI2c2_SDA_Bn, 16);

cBotNet                        *mcBn_0x1000; //  0x1000 Masternode for 011[x] all nodes, e.g. downstream to 0111
                                             // 0 CmdPort
                                             // 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)
                                             // 2 MemPort
                                             // Connections:
                                             //    - CmdPort->Comport
                                             //    - Comport->Cmdport

cBn_MsgProcess_0x1000 mcBn_MsgProcess_0x1000;

// --- 0xE000 SideLink => PC
cNRF905                   mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905      mcSideLinkRf(0xE000, &mcNRF905);
cBotNet_UpLinknRf905Net   mcSideLink(&mcSideLinkRf);

// --- 0x1000 DownLinks
cBotNet_DownLinkI2c  mcDownLinks_0x1100(0x1100, &mcI2C2_BnMaster);
cBotNet_DownLinkI2c  mcDownLinks_0x1200(0x1200, &mcI2C2_BnMaster);

cBotNetStreamPort_BotNetMemPort mcBnMemPort(RomConst_Partition_Count, RomConst_stDevice_Info->stPartitions);


class cBotNetStreamPort_BotNetChargerControl_Status: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetChargerControl_Status():cCliCmd((const char*)"cc_status", (const char*)"Light Control Status")
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


class cBotNetStreamPort_BotNetChargerControl_SetMode: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetChargerControl_SetMode():cCliCmd((const char*)"cc_setmode", (const char*)"a=auto m=manual")
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


class cBotNetStreamPort_BotNetChargerControl_SetAdc: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetChargerControl_SetAdc():cCliCmd((const char*)"cc_setdac", (const char*)"x=value in mV")
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
        mu8DispPage = ((u8)li16Value);
      }*/

      return True;
    }
};

class cBotNetStreamPort_BotNetChargerControl_SetSwitch: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetChargerControl_SetSwitch():cCliCmd((const char*)"cc_sets", (const char*)"x=swtich to set")
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
        mu8DispPage = ((u8)li16Value);
      }*/

      return True;
    }
};

class cBotNetStreamPort_BotNetChargerControl_SetDisp: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetChargerControl_SetDisp():cCliCmd((const char*)"cc_SetDisp", (const char*)"x=display page")
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
        mcOpMode.vSet(cWorkMode::nStManuel);
        i16 li16Value = (lcParam.Atoi() - 1);
        mu8DispPage = ((u8)li16Value);
      }

      return True;
    }
};


class cBotNetStreamPort_BotNetChargerControl: public cBotNetStreamPort
{
  public:
  cBotNetStreamPort_BotNetChargerControl_Status    mcBnCliCmd_Status;
  cBotNetStreamPort_BotNetChargerControl_SetMode   mcBnCliCmd_SetMode;
  cBotNetStreamPort_BotNetChargerControl_SetAdc    mcBnCliCmd_SetAdc;
  cBotNetStreamPort_BotNetChargerControl_SetSwitch mcBnCliCmd_SetSwitch;
  cBotNetStreamPort_BotNetChargerControl_SetDisp   mcBnCliCmd_SetDisp;

  cBotNetStreamPort_BotNetChargerControl()
  {
    mszName     = (const char*)"BnChgrCntl v01.00.00";
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort_BotNetChargerControl);
  }

  void vAddedToBn(cBotNet* lcBotNet) override
  {
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Status);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetMode);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetAdc);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetSwitch);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDisp);
  }


  void vProcess(void) override
  {
  }
};

cBotNetStreamPort_BotNetChargerControl mcBnPortCharger;


void USART1_IRQHandler(void)
{
}

void I2C2_EV_IRQHandler(void)
{
  mcI2C2_BnMaster.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  mcI2C2_BnMaster.I2C_ER_IRQHandler();
}

void I2C1_EV_IRQHandler(void)
{
  mcI2C1_Disp.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1_Disp.I2C_ER_IRQHandler();
}



void EXTI15_10_IRQHandler(void)
{
  if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_15) != 0x00u)
  {
    __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_15);
    mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}


void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC2;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC3;
  mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
}

void TIM7_IRQHandler(void)
{
  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}


void MAIN_vTick1msHp(void)
{
  mcBn_0x1000->vTickHp1ms();
}

void MAIN_vTick10msLp(void)
{
  mcLipoMon.vUpdateAll(mcOpMode.IsAuto());
  mcBn_0x1000->vTickLp10ms();
  mcI2C1_Disp.bStartNext();
}



void MAIN_vPage1(void)
{
  cPaint::vRectFull(4, 0,  52, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"1-7 Status", &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, mcOpMode.ToString(), &mcScreen1);

  char8  lszStrBuf[32];
  cStr  lszStr(lszStrBuf, 32);
  mClock.toString(lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszStr.ToString(), &mcScreen1);
}

void vInaToString(u16 liIdx, cStr& lszStr)
{
  lszStr.Setf((const char*)"P%d: U=%4d  I=%4d", liIdx + 2, mcLipoMon.mi32VBus[liIdx], mcLipoMon.mi32IShunt[liIdx]);
}

void vSchalterToString(u16 liIdx, u16 liIdx2, cStr& lszStr)
{
  lszStr.Setf((const char*)"S%2d: ", liIdx);

  if (mcLipoMon.lenSmS[liIdx2] == cLipoMon::enSmStOn)
       lszStr += '1';
  else lszStr += '0';
}

void MAIN_vPage2(void)
{
  uint8 lui8t;
  char8  lszStrBuf[24];
  cStr   lszStr(lszStrBuf, 24);

  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"2-7 Charger 1-2", &mcScreen1);

  i16 li16xPos;
  i16 li16yPos;

  li16yPos = 14;
  // Strom und Spannung einlesen
  for (lui8t = (uint8)cLipoMon::enINA_P2; lui8t <= (uint8)cLipoMon::enINA_P4; lui8t++)
  {
    vInaToString(lui8t, lszStr);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, li16yPos, lszStr.ToString(), &mcScreen1);
    li16yPos+= 8;
  }

  li16xPos = 10; li16yPos = 40;
  vSchalterToString(10, cLipoMon::enS10, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);

  li16xPos = 40;
  vSchalterToString(11, cLipoMon::enS11, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);

  li16xPos = 10; li16yPos += 8;
  vSchalterToString(12, cLipoMon::enS12, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);

  li16xPos = 40;
  vSchalterToString(13, cLipoMon::enS13, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);

  li16xPos = 10; li16yPos += 8;
  vSchalterToString(14, cLipoMon::enS14, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);

  li16xPos = 40;
  vSchalterToString(20, cLipoMon::enS20, lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(li16xPos, li16yPos, lszStr.ToString(), &mcScreen1);
}

void MAIN_vPage3(void)
{
  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"3-6 Charger 2-2", &mcScreen1);

  char8  lszStrBuf[24];
  cStr   lszStr(lszStrBuf, 24);

  lszStr.Setf((const char*)"CCnt1: %d", mcLipoMon.mui32LipoChargeCount1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14, lszStr.ToString(),  &mcScreen1);
  lszStr.Setf((const char*)"CCnt2: %d", mcLipoMon.mui32LipoChargeCount2);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 14, lszStr.ToString(),  &mcScreen1);

  lszStr.Setf((const char*)"LCnt: %d", mcLipoMon.mui32LipoLoadCount);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 22, lszStr.ToString(),  &mcScreen1);


  lszStr.Setf((const char*)"Cap: %d", mcLipoMon.mi32LipoCap);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszStr.ToString(),  &mcScreen1);
  lszStr.Setf((const char*)"CapMax: %d", mcLipoMon.mi32LipoCapMax);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 30, lszStr.ToString(),  &mcScreen1);


  switch (mcLipoMon.lenSmLipo)
  {
    case cLipoMon::enSmStNotCharging:
      lszStr.Set((const char*) "NO");
      break;
    case cLipoMon::enSmStChargingSave:
      lszStr.Set((const char*) "SAVE");
      break;
    case cLipoMon::enSmStChargingNormal:
      lszStr.Set((const char*) "NORM");
      break;
    case cLipoMon::enSmStCharged:
      lszStr.Set((const char*) "FULL");
      break;
  }
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 50, lszStr.ToString(),  &mcScreen1);
}


void vPrintLinkToString(u16 liSlaveIdx, cBotNet_LinkBase* lcLink, cStr& lszStr)
{
  if (lcLink != null)
  {
    char8  lszStrBuf2[16];
    cStr   lszStr2(lszStrBuf2, 16);

    lszStr.Setf((const char8*)"%d) %x.%x.%x.%x: ", liSlaveIdx, lcLink->mcDAdr.mAdr.stAdr.AdrD1,
                                                               lcLink->mcDAdr.mAdr.stAdr.AdrD2,
                                                               lcLink->mcDAdr.mAdr.stAdr.AdrD3,
                                                               lcLink->mcDAdr.mAdr.stAdr.AdrD4);

    if (lcLink->mStatus.IsOnline)
    {
      lszStr += (const char8*)"On";
    }
    else
    {
      lszStr += (const char8*)"Off";
    }

    lszStr2.Setf((const char8*)"  %d-%d", lcLink->mcRxComBuf.cnt(),
                                          lcLink->mcTxComBuf.cnt());

    lszStr += lszStr2;
  }
  else
  {
    lszStr.Setf((const char8*)"%d) na", liSlaveIdx);
  }
}


void MAIN_vPage4(void)
{
  char8  lszStrBuf[32];
  cStr   lszStr(lszStrBuf, 32);

  uint8 liSlaveIdx;

  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"4-7 Botnet 1/4", &mcScreen1);

  /* Links */

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14, (const char*)"UL:", &mcScreen1);
  vPrintLinkToString(1, mcBn_0x1000->mcUpLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+8, (const char*)"SL:", &mcScreen1);
  vPrintLinkToString(1, mcBn_0x1000->mcSideLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14+8, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+16, (const char*)"DL:", &mcScreen1);
  for (liSlaveIdx = 0; liSlaveIdx < 4 /*cBotNet_DownLinkCnt*/; liSlaveIdx++)
  {
    vPrintLinkToString(liSlaveIdx + 1, mcBn_0x1000->mcDownLinks[liSlaveIdx], lszStr);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14+16+liSlaveIdx*8, lszStr.ToString(), &mcScreen1);
  }
}


void MAIN_vPrintPort(u8 lu8StartIdx, u8 lu8StopIdx)
{
  char8  lszStrBuf[32];
  cStr   lszStr(lszStrBuf, 32);

  /* Ports */
  uint8 liPortIdx;
  uint8 liCnxIdx;
  uint8 liPosY = 0;

  for (liPortIdx = lu8StartIdx; liPortIdx <= lu8StopIdx /*cBotNet_StreamSysPortsCnt*/; liPortIdx++)
  {
    if (mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      lszStr.Setf((const char8*)"%d): ", liPortIdx);
      lszStr += mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mszName;
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;

      lszStr.Setf((const char8*)"Mem: %d-%d", mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcRxRingBuf.cnt(),
                                              mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcTxRingBuf.cnt());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;


      // externe Connection
      if (mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.u16Adr != 0)
      {
        lszStr.Setf((const char8*)"ext: %x.%x.%x.%x:%d", mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD1,
                                                         mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD2,
                                                         mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD3,
                                                         mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD4,
                                                         mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcBnDestPortIdx);
        cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
        liPosY+=8;
      }

      for (liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
      {
        // interne Connection
        if (mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx] != 0)
        {
          lszStr.Setf((const char8*)"%d int: ", liCnxIdx + 1);
          lszStr += mcBn_0x1000->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mszName;

          cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
          liPosY+=8;
        }
      }
    }
  }
}

void MAIN_vPage5(void)
{
  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"5-7 Botnet 2/4", &mcScreen1);

  MAIN_vPrintPort(0,1);
}


void MAIN_vPage6(void)
{
  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"6-7 Botnet 3/4", &mcScreen1);


  MAIN_vPrintPort(2,3);
}


void MAIN_vPage7(void)
{
  cPaint::vRectFull(4, 0, 70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"7-7 Botnet 3/4", &mcScreen1);


  MAIN_vPrintPort(2, 3);
}


void MAIN_vTick100msLp(void)
{
  mcStatusLed.vTick100ms();

  // Reset Buffer of Offline Links
  mcBn_0x1000->vResetCom(False);

  InAppProg_Platform_vResetWdog();
}


void MAIN_vTick1000msLp(void)
{
  static u8 lu2s    = 0;


  mcScreen1.vFill(0);
  cPaint::vRect(2, 2, 124, 60, 1, &mcScreen1);

  if (mcSSD1306.mStatus.IsInit) // Paint graphic only when Display is available
  {
    switch (mu8DispPage)
    {
      case 0: MAIN_vPage1(); break;
      case 1: MAIN_vPage2(); break;
      case 2: MAIN_vPage3(); break;
      case 3: MAIN_vPage4(); break;
      case 4: MAIN_vPage5(); break;
      case 5: MAIN_vPage6(); break;
      case 6: MAIN_vPage7(); break;
      default: MAIN_vPage1(); break;
    }

    mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  }


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
    if (mcOpMode.IsAuto())
    {
      if (mu8DispPage < 6)
      {
        mu8DispPage++;
      }
      else
      {
        mu8DispPage = 0;
      }
    }
  }

  mcSSD1306.Update();

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
}


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

  mcDigiPower.vPowerOn();

  mcBn_0x1000 = new cBotNet(&mcMyBotNetCfg, &mcBn_MsgProcess_0x1000);

  mcBn_0x1000->bAddLink((cBotNet_LinkBase*)&mcSideLink);
  mcBn_0x1000->bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1100);
  mcBn_0x1000->bAddLink((cBotNet_LinkBase*)&mcDownLinks_0x1200);


  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1000->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  lu16PortIdx = mcBn_0x1000->u16StreamPortAdd(&mcBnMemPort);
  mcBn_0x1000->vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Add LightControl Port (not connected)
  lu16PortIdx = mcBn_0x1000->u16StreamPortAdd(&mcBnPortCharger);

  mClock.vSet(2021, 2, 3, 12, 0, 0);
  mClock.mValid = False;

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  InAppProg_Platform_vResetWdog();

  if (mcI2C1_Disp.bStartNext())
  {
    cClockInfo::Delay_ms(50);
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{

  /*u8 MyMsg[64] = {0x20,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,3,4,5,6,7,
                  0,1,2,0x12,0x11,0x10,0x00,0x80};

  cbArrayExtT<uint16> lcMyMsg(MyMsg, 64);

  lcMyMsg.muiLen = 64;*/

  if (!RomConst_IsValid())
  {
    // RomConst Error
    while (1) {}
  }

  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    //mcDownLinks_0x1200.bPut(&lcMyMsg);

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

