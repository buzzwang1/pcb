
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB

//  Light 1.2.0.0:
//    LichtStatus: 100ms: An 1.0.0.0: Idx: 0x50 | u8LR u8LL u8LO u8LU u8LH i16Neig i16Kompass
//    LichtStatus: 100ms: An 1.1.0.0: Idx: 0x50 | u8LR u8LL u8LO u8LU u8LH i16Neig i16Kompass
//            Neigung: i16: -1800..1800: -180°..180° von der Horizontalen; Negativ im Uhrzeigersinn
//            Kompass: i16: -1800..1800: -180°..180° von Norden;           Posetiv im Uhrzeigersinn

//  SolarBox 2.0. BotNetId 20 = 0x14
//  Master:   BnAdr: 1.0.0.0
//  MotBx:    BnAdr: 1.1.0.0
//  LightBx:  BnAdr: 1.2.0.0
//
//  PC: BotNetId 7;
//            BnAdr: E.0.0.0
//
//  PA0   -> Wakeup
//
//  USART1 -> BotCom ComPort (19200)
//  PA09  -> UsartTx
//  PA10  -> UsartRx
//
//
//  PA01  -> LDR Links  (1k Widerstand zu 10k LDR and 3,3V)
//  PA02  -> LDR Rechts (1k Widerstand zu 10k LDR and 3,3V)
//  PA03  -> LDR Oben   (1k Widerstand zu 10k LDR and 3,3V)
//  PA04  -> LDR Hinten (1k Widerstand zu 10k LDR and 3,3V)
//  PA05  -> LDR Unten  (1k Widerstand zu 10k LDR and 3,3V)
//  PA06  -> Spannung Solarzellen (10k an 2,35k)
//
//  PB0   -> Mainboard: Schalter S20
//
//  I2C1  -> Display (Optional)
//  PB06  -> I2C1 SCL
//  PB07  -> I2C1 SDA
//
//  I2C2  -> BotNet
//  PB10  -> I2C2 SCL
//  PB11  -> I2C2 SDA
//
//  TIM6  -> CyclicCaller
//  TIM15 -> BotCom
//
//  PB09  -> Status Led
//
//  PA11  -> DigiPower: Versorgung Display
//  PA12  -> DigiPower: Versorgung Display
//  PA15  -> DigiPower: Versorgung Display

//  PB00  -> AnaPower: Versorgung LDRs
//  PB01  -> AnaPower: Versorgung LDRs


//I2C Adresses
// Display:        0x78 => 0x3C (60d)
// LSM303DLHC_MAG: 0x3C => 0x1E (30d)
// LSM303DLHC_ACC: 0x32 => 0x19 (25d)

__IO uint32_t TimingDelay = 0;

cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cStatusLed<GPIOB_BASE, 9> mcStatusLed;

// I2C1:
cGpPin mcI2c1_SCL_Disp(GPIOB_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c1_SDA_Disp(GPIOB_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

// I2C2:
cGpPin mcI2c2_SCL_Bn(GPIOB_BASE, 10, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c2_SDA_Bn(GPIOB_BASE, 11, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


cI2cMaster   mcI2C1_Disp(I2C1, &mcI2c1_SCL_Disp, &mcI2c1_SDA_Disp, 8);
cSSD1306     mcSSD1306(&mcI2C1_Disp, 0x78, 0x38);
cLSM303DLHC_MAG mcMag(&mcI2C1_Disp);
cLSM303DLHC_ACC mcAcc(&mcI2C1_Disp);

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
cWorkMode      mcOpMode;
cDigiPower    mcDigiPower;
cAnaPower     mcAnaPower;
cLightControl mcLightControl;


#define ABS(x)         (x < 0) ? (-x) : x

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */
#define PI                         (float)     3.14159265f

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

#define LSM303DLHC_M_SENSITIVITY_XY_1_3Ga     1100  /*!< magnetometer X Y axes sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_1_9Ga     855   /*!< magnetometer X Y axes sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_2_5Ga     670   /*!< magnetometer X Y axes sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_4Ga       450   /*!< magnetometer X Y axes sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_4_7Ga     400   /*!< magnetometer X Y axes sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_5_6Ga     330   /*!< magnetometer X Y axes sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_XY_8_1Ga     230   /*!< magnetometer X Y axes sensitivity for 8.1 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_1_3Ga      980   /*!< magnetometer Z axis sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_1_9Ga      760   /*!< magnetometer Z axis sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_2_5Ga      600   /*!< magnetometer Z axis sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_4Ga        400   /*!< magnetometer Z axis sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_4_7Ga      355   /*!< magnetometer Z axis sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_5_6Ga      295   /*!< magnetometer Z axis sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303DLHC_M_SENSITIVITY_Z_8_1Ga      205   /*!< magnetometer Z axis sensitivity for 8.1 Ga full scale [LSB/Ga] */

float HeadingValue = 0.0f;
float MagBuffer[3] = {0.0f}, AccBuffer[3] = {0.0f};

float fNormAcc,fSinRoll,fCosRoll,fSinPitch,fCosPitch = 0.0f, RollAng = 0.0f, PitchAng = 0.0f;
float fTiltedX,fTiltedY = 0.0f;


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




class cBn_MsgProcess_0x1200 : public cBotNet_MsgSysProcess
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


// BotNet
// I2C2
cI2cSlave mcI2C2_BnSlave(I2C2,  &mcI2c2_SCL_Bn, &mcI2c2_SDA_Bn, 2 << 1, 0);

cBotNet                        *mcBn_0x1200; // Masternode for 011[x] all nodes, e.g. downstream to 0111
                                             // 0 CmdPort
                                             // 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)
                                             // 2 MemPort
                                             // Connections:
                                             //    - CmdPort->Comport
                                             //    - Comport->Cmdport

cBn_MsgProcess_0x1200 mcBn_MsgProcess_0x1200;

// --- 0x1200 UpLink
cBotNet_UpLinkI2c    mcUpLnk_0x1200(0x1000, &mcI2C2_BnSlave);


tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcUart1(19200, GPIO_AF7_USART2, 64, 64);

cBotNetStreamPort_ComPort       mcBnComPort1(&mcUart1);
cBotNetStreamPort_BotNetMemPort mcBnMemPort(RomConst_Partition_Count, RomConst_stDevice_Info->stPartitions);



class cBnCliCmd_LightControl_Status: public cCliCmd
{
  public:
    cBnCliCmd_LightControl_Status():cCliCmd((const char*)"lc_Status", (const char*)"Light Control Status")
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


class cBnCliCmd_LightControl_SetMode: public cCliCmd
{
  public:
    cBnCliCmd_LightControl_SetMode():cCliCmd((const char*)"lc_SetMode", (const char*)"a=auto m=manual")
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


class cBnCliCmd_LightControl_SetDisp: public cCliCmd
{
  public:
    cBnCliCmd_LightControl_SetDisp():cCliCmd((const char*)"lc_SetDisp", (const char*)"x=display page")
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


class cBotNetStreamPort_BotNetLightControl: public cBotNetStreamPort
{
  public:
  cBnCliCmd_LightControl_Status     mcBnCliCmd_Status;
  cBnCliCmd_LightControl_SetMode    mcBnCliCmd_SetMode;
  cBnCliCmd_LightControl_SetDisp    mcBnCliCmd_SetDisp;

  cBotNetStreamPort_BotNetLightControl()
  {
    mszName     = (const char*)"BnLigthCntl v01.00.00";
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort_BotNetLightControl);
  }

  void vAddedToBn(cBotNet *lcBotNet) override
  {
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Status);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetMode);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDisp);
  }


  void vProcess(void) override
  {
  }
};

cBotNetStreamPort_BotNetLightControl mcBnPortLight;


void USART1_IRQHandler(void)
{
  mcStatusLed.Toggle();
  mcUart1.vIRQHandler();
}
void I2C2_EV_IRQHandler(void)
{
  mcI2C2_BnSlave.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  mcI2C2_BnSlave.I2C_ER_IRQHandler();
}

void I2C1_EV_IRQHandler(void)
{
  mcI2C1_Disp.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1_Disp.I2C_ER_IRQHandler();
}




void MAIN_vTick1msHp(void)
{
  mcBn_0x1200->vTickHp1ms();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vTick10msLp(void)
{
  mcLightControl.vMain_SmAdc(cLightControl::nEvDummy);

  mcI2C1_Disp.bStartNext();
  mcBn_0x1200->vTickLp10ms();
}



void MAIN_vPage1(void)
{
  cPaint::vRectFull(4, 0,  52, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"1-7 Status", &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, mcOpMode.ToString(), &mcScreen1);
}

/*void vInaToString(u16 liIdx, cStr& lszStr)
{
  lszStr.Setf((const char*)"P%d: U=%4d  I=%4d", liIdx + 2, mcLipoMon.mi32VBus[liIdx], mcLipoMon.mi32IShunt[liIdx]);
}

void vSchalterToString(u16 liIdx, u16 liIdx2, cStr& lszStr)
{
  lszStr.Setf((const char*)"S%2d: ", liIdx);

  if (mcLipoMon.lenSmS[liIdx2] == cLipoMon::enSmStOn)
       lszStr += '1';
  else lszStr += '0';
}*/

void MAIN_vPage2(void)
{
  char8  lszStrBuf[24];
  cStr   lszStr(lszStrBuf, 24);

  cPaint::vRectFull(4, 0,  70, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"2-7 LDR", &mcScreen1);

  lszStr.Setf((const char*)"L:%d", mcLightControl.u32GetLdrL_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszStr.ToString(), &mcScreen1);

  lszStr.Setf((const char*)"R:%d", mcLightControl.u32GetLdrR_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 30, lszStr.ToString(), &mcScreen1);

  lszStr.Setf((const char*)"O:%d", mcLightControl.u32GetLdrO_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(55, 20, lszStr.ToString(), &mcScreen1);

  lszStr.Setf((const char*)"U:%d", mcLightControl.u32GetLdrU_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(55, 40, lszStr.ToString(), &mcScreen1);

  lszStr.Setf((const char*)"H:%d", mcLightControl.u32GetLdrH_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(55, 30, lszStr.ToString(), &mcScreen1);

  lszStr.Setf((const char*)"Sol:%d", mcLightControl.u32GetU_Sol_mv());
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 50, lszStr.ToString(), &mcScreen1);
}

void MAIN_vPage3(void)
{
  char8 lszValue[12];

  cPaint::vRectFull(4, 0,  40, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"3-7 Immu", &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, (const char*)"Mag", &mcScreen1);
  cStrTools::uixItoa(mcMag.i16GetMagX()*1000/230, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMag.i16GetMagY()*1000/230, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 40, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMag.i16GetMagZ()*1000/205, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 50, lszValue, &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 20, (const char*)"Acc", &mcScreen1);
  cStrTools::uixItoa(mcAcc.i16GetAccX()/16, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcAcc.i16GetAccY()/16, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 40, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcAcc.i16GetAccZ()/16, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(50, 50, lszValue, &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 20, (const char*)"Pointer", &mcScreen1);
  cStrTools::uixItoa((i16)HeadingValue, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa((i16)RollAng, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 40, lszValue, &mcScreen1);
  cStrTools::uixItoa((i16)PitchAng, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 50, lszValue, &mcScreen1);
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
  vPrintLinkToString(1, mcBn_0x1200->mcUpLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+8, (const char*)"SL:", &mcScreen1);
  vPrintLinkToString(1, mcBn_0x1200->mcSideLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14+8, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+16, (const char*)"DL:", &mcScreen1);
  for (liSlaveIdx = 0; liSlaveIdx < 4 /*cBotNet_DownLinkCnt*/; liSlaveIdx++)
  {
    vPrintLinkToString(liSlaveIdx + 1, mcBn_0x1200->mcDownLinks[liSlaveIdx], lszStr);
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
    if (mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      lszStr.Setf((const char8*)"%d): ", liPortIdx);
      lszStr += mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mszName;
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;

      lszStr.Setf((const char8*)"Mem: %d-%d", mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcRxRingBuf.cnt(),
                                              mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcTxRingBuf.cnt());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;


      // externe Connection
      if (mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.u16Adr != 0)
      {
        lszStr.Setf((const char8*)"ext: %x.%x.%x.%x:%d", mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD1,
                                                         mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD2,
                                                         mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD3,
                                                         mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD4,
                                                         mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcBnDestPortIdx);
        cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
        liPosY+=8;
      }

      for (liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
      {
        // interne Connection
        if (mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx] != 0)
        {
          lszStr.Setf((const char8*)"%d int: ", liCnxIdx + 1);
          lszStr += mcBn_0x1200->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mszName;

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





void vGetAcc(float* pfData)
{
  i16 pnRawData[3];
  u8  cDivider;
  u8  i = 0;
  float LSM_Acc_Sensitivity = LSM_Acc_Sensitivity_2g;

  cDivider=16;

  /* Big Endian Mode */
  pnRawData[0]= mcAcc.i16GetAccX()/cDivider;
  pnRawData[1]= mcAcc.i16GetAccY()/cDivider;
  pnRawData[2]= mcAcc.i16GetAccZ()/cDivider;

  /* Obtain the mg value for the three axis */
  for (i=0; i < 3; i++)
  {
    pfData[i]=(float)pnRawData[i] / LSM_Acc_Sensitivity;
  }
}


void vGetMag(float* pfData)
{
  u16 Magn_Sensitivity_XY = 0, Magn_Sensitivity_Z = 0;

  Magn_Sensitivity_XY = LSM303DLHC_M_SENSITIVITY_XY_8_1Ga;
  Magn_Sensitivity_Z = LSM303DLHC_M_SENSITIVITY_Z_8_1Ga;

  pfData[0]=(float)(mcMag.i16GetMagX()*1000)/Magn_Sensitivity_XY;
  pfData[1]=(float)(mcMag.i16GetMagY()*1000)/Magn_Sensitivity_XY;
  pfData[2]=(float)(mcMag.i16GetMagZ()*1000)/Magn_Sensitivity_Z;
}


void MAIN_vTick100msLp(void)
{
  u8  i = 0;

  mcMag.vReadMag();
  mcAcc.vReadAcc();

  vGetAcc(AccBuffer);
  vGetMag(MagBuffer);

  for (i = 0; i < 3; i++) AccBuffer[i] /= 100.0f;

  fNormAcc = sqrtf((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));

  fSinRoll =  -AccBuffer[1]/fNormAcc;
  fCosRoll =   sqrtf(1.0f-(fSinRoll * fSinRoll));
  fSinPitch =  AccBuffer[0]/fNormAcc;
  fCosPitch =  sqrtf(1.0f-(fSinPitch * fSinPitch));

  if (AccBuffer[2] < 0 ) 
  {
    fCosPitch = -fCosPitch;
    fCosRoll = -fCosRoll;
  }

  RollAng  = acosf(fCosRoll)*180.0f/PI;

  if (fSinRoll < 0)
  {
    RollAng = -RollAng;
  }

  PitchAng = acosf(fCosPitch)*180.0f/PI;

  if (fSinPitch < 0)
  {
    PitchAng = -PitchAng;
  }

  // https://www.jameco.com/jameco/products/prodds/2150248.pdf
  // page 23
  fTiltedX = MagBuffer[0]* fCosPitch +
             MagBuffer[1]* fSinRoll*fSinPitch -
             MagBuffer[2]* fCosRoll*fSinPitch;
  fTiltedY = 0 +
             MagBuffer[1]* fCosRoll +
             MagBuffer[2]* fSinRoll;

  //Roll sollte immer 0° sein Cos(0) = 1; Sin(0) = 0;
  /*fTiltedX = MagBuffer[0] * fCosPitch +
             0 -
             MagBuffer[2]* 1*fSinPitch;
  fTiltedY = 0 +
             MagBuffer[1] * 1 +
             MagBuffer[2] * 0;*/

  if (fTiltedX == 0.0f)
  {
    if (fTiltedY < 0.0f) HeadingValue = -90.0f;
                    else HeadingValue =  90.0f;
  }
  else
  {
    HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;
  }

  mcStatusLed.vTick100ms();

  // Von mir an den Master
  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgLightStatus, 32, 0x1200, 0x1000, 0x50);

  lcMsgLightStatus.mcPayload[0] = (u8)(mcLightControl.u32GetLdrR_mv() >> 8);
  lcMsgLightStatus.mcPayload[1] = (u8)(mcLightControl.u32GetLdrL_mv() >> 8);
  lcMsgLightStatus.mcPayload[2] = (u8)(mcLightControl.u32GetLdrO_mv() >> 8);
  lcMsgLightStatus.mcPayload[3] = (u8)(mcLightControl.u32GetLdrU_mv() >> 8);
  lcMsgLightStatus.mcPayload[4] = (u8)(mcLightControl.u32GetLdrH_mv() >> 8);

  i16 liKompass = (i16)HeadingValue;
  i16 liNeig    = (i16)PitchAng;

  lcMsgLightStatus.mcPayload[5] = (u8)(liNeig >> 8);
  lcMsgLightStatus.mcPayload[6] = (u8)liNeig;

  lcMsgLightStatus.mcPayload[7] = (u8)(liKompass >> 8);
  lcMsgLightStatus.mcPayload[8] = (u8)liKompass;

  lcMsgLightStatus.vEncode();

  mcBn_0x1200->bSendMsg(&lcMsgLightStatus);

  // Reset Buffer of Offline Links
  mcBn_0x1200->vResetCom(False);

  InAppProg_Platform_vResetWdog();
}


void MAIN_vTick1000msLp(void)
{
  static u8 lu2s    = 0;


  mcScreen1.vFill(0);
  cPaint::vRect(2, 2, 124, 60, 1, &mcScreen1);

  if (mcSSD1306.mStatus.IsInit) // Paint graphic only when Display is available
  {
    mu8DispPage = 2;
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
  mcAnaPower.vPowerOn();


  cClockInfo::Delay_ms(50); // Nach Power On warten
  // Warten bis Init ausgefürht ist (Display und Sensor)
  mcI2C1_Disp.bStartNext();
  cClockInfo::Delay_ms(50);

  // Config setzen (Display und Sensor)
  mcMag.vSetConfig(LSM303DLHC_MAG_TEMPSENSOR_ENABLE, LSM303DLHC_MAG_ODR_15_HZ, LSM303DLHC_MAG_FS_8_1_GA, LSM303DLHC_MAG_CONTINUOS_CONVERSION);
  mcAcc.vSetControl(LSM303DLHC_ACC_ODR_25_HZ | LSM303DLHC_ACC_NORMAL_MODE | LSM303DLHC_ACC_X_ENABLE | LSM303DLHC_ACC_Y_ENABLE | LSM303DLHC_ACC_Z_ENABLE,
                    LSM303DLHC_ACC_HPM_NORMAL_MODE | LSM303DLHC_ACC_HPFCF_32,
                    0, // No Ints
                    LSM303DLHC_ACC_BlockUpdate_Continous | LSM303DLHC_ACC_FULLSCALE_2G,
                    0,
                    0
                    );
  // Warten bis Config gesetzt ist (Display und Sensor)
  mcI2C1_Disp.bStartNext();
  cClockInfo::Delay_ms(10);


  /* ADC */
  mcLightControl.vMain_SmAdc(cLightControl::nEvReset);
  mcLightControl.vMain_SmAdc(cLightControl::nEvStart);


  mcBn_0x1200 = new cBotNet(&mcMyBotNetCfg, &mcBn_MsgProcess_0x1200);

  mcBn_0x1200->bAddLink((cBotNet_LinkBase*)&mcUpLnk_0x1200);


  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1200->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  lu16PortIdx = mcBn_0x1200->u16StreamPortAdd(&mcBnMemPort);
  mcBn_0x1200->vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Add Comport and connect the Comport's output to internal Port (to internal CmdPort)
  // Should be Index 2
  lu16PortIdx = mcBn_0x1200->u16StreamPortAdd(&mcBnComPort1);
  mcBn_0x1200->vStreamPortConnect(lu16PortIdx, 0, cBotNet_CmdPortIdx);

  // Connect the CmdPort's output to internal Port (to internal ComPort)
  mcBn_0x1200->vStreamPortConnect(cBotNet_CmdPortIdx, 0, lu16PortIdx);

  // Add LightControl Port (not connected)
  lu16PortIdx = mcBn_0x1200->u16StreamPortAdd(&mcBnPortLight);

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,


                NULL  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

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
