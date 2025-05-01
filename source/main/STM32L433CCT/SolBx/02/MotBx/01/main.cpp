
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
//
//  USART2 -> BotCom ComPort (19200)
//  PA02  -> UsartTx
//  PA03  -> UsartRx
//
//
//  PA06  -> Stepper Motor 1 (Turm  Drehung) TIM1_CH1
//  PA07  -> Stepper Motor 1 (Turm  Drehung) TIM1_CH2
//  PB00  -> Stepper Motor 1 (Turm  Drehung) TIM1_CH3
//  PB01  -> Stepper Motor 1 (Turm  Drehung) TIM1_CH4
//
//  PA08  -> Stepper Motor 2 (Turm Neigung) TIM3 B1 (TIM3 nicht vorhanden beim STM32L433)
//  PA09  -> Stepper Motor 2 (Turm Neigung) TIM3 B2
//  PA10  -> Stepper Motor 2 (Turm Neigung) TIM3 A2
//  PA11  -> Stepper Motor 2 (Turm Neigung) TIM3 A1
//
//
//  I2C1  -> LSM303DLHC (Acc & Mag), Display (Optional)
//  PB06  -> I2C1 SCL
//  PB07  -> I2C1 SDA
//  PB05  -> LSM303DLHC G_Int
//  PB04  -> LSM303DLHC G_Rdy
//  PB03  -> LSM303DLHC L_Int1
//  PA15  -> LSM303DLHC L_Int2
//  PA12  -> LSM303DLHC L_Rdy
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
//  PB14  -> Versorgung LSM303DLHC und Display
//  PB15  -> Versorgung LSM303DLHC und Display


// rot     = C:95 R:60 G:16 B:23
// schwarz = C:40 R:10 G:14 B:13
// grün    = C:72 R:14 G:30 B:24
//
//
//



__IO uint32_t TimingDelay = 0;

cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cStatusLed<GPIOB_BASE, 9> mcStatusLed;

// I2C1:
cGpPin mcI2c1_SCL_LSM303DLHC(GPIOB_BASE, 6, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c1_SDA_LSM303DLHC(GPIOB_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

// I2C2:
cGpPin mcI2c2_SCL_Bn(GPIOB_BASE, 10, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c2_SDA_Bn(GPIOB_BASE, 11, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


cI2cMaster   mcI2C1_LSM303DLHC(I2C1, &mcI2c1_SCL_LSM303DLHC, &mcI2c1_SDA_LSM303DLHC, 8);
cSSD1306     mcSSD1306(&mcI2C1_LSM303DLHC, 0x78, 0x38);
//cLSM303DLHC_MAG mcMag(&mcI2C1_LSM303DLHC);
//cLSM303DLHC_ACC mcAcc(&mcI2C1_LSM303DLHC);
cAPDS9960    mcAPDS9960(&mcI2C1_LSM303DLHC, nAPDS9960_I2C_ADDR);

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

u8 mu8DispPage = 0;

cDigiPower       mcDigiPower;
cWorkMode        mcOpMode;

cMot_TurmDrehung mcMot_TurmDrehung;
cMot_TurmNeigung mcMot_TurmNeigung;


u8 mu8LdrR = 0;
u8 mu8LdrL = 0;
u8 mu8LdrO = 0;
u8 mu8LdrU = 0;
u8 mu8LdrH = 0;

i16 mi16LightBoxNeig = 0;
i16 mi16LightBoxKompass = 0;
i16 mi16LightBoxMsgCounter = 0;

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




class cBn_MsgProcess_0x1100 : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    (void)lpcBn;
    // Überprüfen, ob es eine LightBox Nachricht ist
    if ((lcMsg.cGetSAdr() == 0x1000) &&
        (lcMsg.mu16Idx  == 0x50))
    {
      mu8LdrR = lcMsg.mcPayload[0];
      mu8LdrL = lcMsg.mcPayload[1];
      mu8LdrO = lcMsg.mcPayload[2];
      mu8LdrU = lcMsg.mcPayload[3];
      mu8LdrH = lcMsg.mcPayload[4];

      mi16LightBoxNeig    = (i16)((u16)(lcMsg.mcPayload[5] << 8) + (u16)lcMsg.mcPayload[6]);
      mi16LightBoxKompass = (i16)((u16)(lcMsg.mcPayload[7] << 8) + (u16)lcMsg.mcPayload[8]);
      mi16LightBoxMsgCounter++;
    }
  }
};


// BotNet
// I2C2
cI2cSlave mcI2C2_BnSlave(I2C2,  &mcI2c2_SCL_Bn, &mcI2c2_SDA_Bn, 1 << 1, 0);

cBotNet                        *mcBn_0x1100; // Masternode for 011[x] all nodes, e.g. downstream to 0111
                                             // 0 CmdPort
                                             // 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)
                                             // 2 MemPort
                                             // Connections:
                                             //    - CmdPort->Comport
                                             //    - Comport->Cmdport

cBn_MsgProcess_0x1100 mcBn_MsgProcess_0x1100;

// --- 0x1200 UpLink
//cBotNet_UpLinkI2c mcUpLnk_0x1100(0x1000, &mcI2C2_BnSlave);
cBotNet_UpLinkI2c mcUpLnk_0x1100(&mcI2C2_BnSlave);

cBotNetStreamPort_BotNetMemPort    mcBnMemPort(RomConst_Partition_Count, RomConst_stDevice_Info->stPartitions);



class cBotNetStreamPort_BotNetMotControl_Status: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetMotControl_Status():cCliCmd((const char*)"cc_status", (const char*)"Motor Control Status")
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
        lcCli->bPrintLn(lszStr.ToString());
        lszStr.Setf((const char8*)"Neig-Mot: %d-%d", mcMot_TurmNeigung.mui16ActualPos, mcMot_TurmNeigung.mui16TargetPos);
        lcCli->bPrintLn(lszStr.ToString());*/
      return True;
    }
};


class cBotNetStreamPort_BotNetMotControl_SetMode: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetMotControl_SetMode():cCliCmd((const char*)"mc_setmode", (const char*)"a=auto m=manual")
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


class cBotNetStreamPort_BotNetMotControl_SetDreh: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetMotControl_SetDreh():cCliCmd((const char*)"mc_SetDreh", (const char*)"x=angle to set")
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
        lcCli->bPrintLn((const char8*)"Not in auto");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        if (lcParam.Len() != 0)
        {
          i16 li16Value = lcParam.Atoi();
          mcMot_TurmDrehung.vSetPosGrad((u16)li16Value);
        }
        else
        {
          mcMot_TurmDrehung.vNull();
          lcCli->bPrintLn((const char8*)"Nulled");
        }
      }

      return True;
    }
};

class cBotNetStreamPort_BotNetMotControl_SetNeig: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetMotControl_SetNeig():cCliCmd((const char*)"mc_SetNeig", (const char*)"x=angle to set")
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
        lcCli->bPrintLn((const char8*)"Not in auto");
      }
      else
      {
        mcOpMode.vSet(cWorkMode::nStManuel);
        if (lcParam.Len() != 0)
        {
          i16 li16Value = lcParam.Atoi();
          mcMot_TurmNeigung.vSetPosGrad((u16)li16Value);
        }
        else
        {
          mcMot_TurmNeigung.vNull();
          lcCli->bPrintLn((const char8*)"Nulled");
        }
      }

      return True;
    }
};

class cBotNetStreamPort_BotNetMotControl_SetDisp: public cCliCmd
{
  public:
    cBotNetStreamPort_BotNetMotControl_SetDisp():cCliCmd((const char*)"mc_SetDisp", (const char*)"x=display page")
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


class cBotNetStreamPort_BotNetMotControl: public cBotNetStreamPort
{
  public:
  cBotNetStreamPort_BotNetMotControl_Status    mcBnCliCmd_Status;
  cBotNetStreamPort_BotNetMotControl_SetMode   mcBnCliCmd_SetMode;
  cBotNetStreamPort_BotNetMotControl_SetDreh   mcBnCliCmd_SetDreh;
  cBotNetStreamPort_BotNetMotControl_SetNeig   mcBnCliCmd_SetNeig;
  cBotNetStreamPort_BotNetMotControl_SetDisp   mcBnCliCmd_SetDisp;

  cBotNetStreamPort_BotNetMotControl()
  {
    mszName     = (const char*)"BnMotCntl v01.00.00";
  }

  inline uint32 SizeOf() override
  {
    return sizeof(cBotNetStreamPort_BotNetMotControl);
  }

  void vAddedToBn(cBotNet *lcBotNet) override
  {
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Status);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetMode);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDreh);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetNeig);
    lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetDisp);
  }


  void vProcess(void) override
  {
  }
};

cBotNetStreamPort_BotNetMotControl mcBnMotControl;


void USART2_IRQHandler(void)
{
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
  mcI2C1_LSM303DLHC.I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1_LSM303DLHC.I2C_ER_IRQHandler();
}


void MAIN_vTick1msHp(void)
{
  mcBn_0x1100->vTickHp1ms();
}

void MAIN_vTick1msLp(void)
{
  mcMot_TurmDrehung.vTick1ms();
  mcMot_TurmNeigung.vTick1ms();
}

void MAIN_vTick10msLp(void)
{
  mcBn_0x1100->vTickLp10ms();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vPage1(void)
{
  cPaint::vRectFull(4, 0,  52, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"1-6 Status", &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, mcOpMode.ToString(), &mcScreen1);
}

void MAIN_vPage2(void)
{
  char8 lszValue[12];

  cPaint::vRectFull(4, 0,  40, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"2-6 Immu", &mcScreen1);

  /*cStrTools::uixItoa(mcMag.i16GetMagX(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMag.i16GetMagY(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMag.i16GetMagZ(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 40, lszValue, &mcScreen1);

  cStrTools::uixItoa(mcAcc.i16GetAccX(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 20, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcAcc.i16GetAccY(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcAcc.i16GetAccZ(), lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 40, lszValue, &mcScreen1);*/

  if (mcAPDS9960.isEnabledAmbientLigth())
  {
    if (mcAPDS9960.boGetAmbientLight_Valid())
    {
      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Clear(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 35, (const char8*)"C:", &mcScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 35, lszValue, &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Red(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 35, (const char8*)"R:", &mcScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 35, lszValue, &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Green(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 45, (const char8*)"G:", &mcScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 45, lszValue, &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Blue(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(60, 45, (const char8*)"B:", &mcScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(90, 45, lszValue, &mcScreen1);

      cStrTools::uixItoa(mcMot_TurmDrehung.miSensor, lszValue, 10);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 55, (const char8*)"Als:", &mcScreen1);
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 55, lszValue, &mcScreen1);
    }
  }



}

void MAIN_vPage3(void)
{
  char8 lszValue[12];

  cPaint::vRectFull(4, 0,  38, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"3-6 Mot", &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 20, (const char*)"Dreh:       - ", &mcScreen1);
  cStrTools::uixItoa(mcMot_TurmDrehung.mi16ActualPos, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(40, 20, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMot_TurmDrehung.mi16TargetPos, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(70, 20, lszValue, &mcScreen1);


  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 30, (const char*)"Neig:       - ", &mcScreen1);
  cStrTools::uixItoa(mcMot_TurmNeigung.mi16ActualPos, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(40, 30, lszValue, &mcScreen1);
  cStrTools::uixItoa(mcMot_TurmNeigung.mi16TargetPos, lszValue, 10);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(70, 30, lszValue, &mcScreen1);

}


void vPrintLinkToString(u16 liSlaveIdx, cBotNet_LinkBase* lcLink, cStr& lszStr)
{
  if (lcLink != null)
  {
    char8  lszStrBuf2[16];
    cStr   lszStr2(lszStrBuf2, 16);

    lszStr.Setf((const char8*)"%d) %x.%x.%x.%x: ", liSlaveIdx, lcLink->mcAdr.mAdr.stAdr.AdrD1,
                                                               lcLink->mcAdr.mAdr.stAdr.AdrD2,
                                                               lcLink->mcAdr.mAdr.stAdr.AdrD3,
                                                               lcLink->mcAdr.mAdr.stAdr.AdrD4);

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

  cPaint::vRectFull(4, 0,  60, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"4-6 Botnet 1/3", &mcScreen1);

  /* Links */

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14, (const char*)"UL:", &mcScreen1);
  vPrintLinkToString(1, mcBn_0x1100->mcUpLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+8, (const char*)"SL:", &mcScreen1);
  vPrintLinkToString(1, mcBn_0x1100->mcSideLink , lszStr);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(30, 14+8, lszStr.ToString(), &mcScreen1);

  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+16, (const char*)"DL:", &mcScreen1);
  for (liSlaveIdx = 0; liSlaveIdx < 4 /*cBotNet_DownLinkCnt*/; liSlaveIdx++)
  {
    vPrintLinkToString(liSlaveIdx + 1, mcBn_0x1100->mcDownLinks[liSlaveIdx], lszStr);
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
    if (mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      lszStr.Setf((const char8*)"%d): ", liPortIdx);
      lszStr += mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mszName;
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(10, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;

      lszStr.Setf((const char8*)"Mem: %d-%d", mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcRxRingBuf.cnt(),
                                              mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcTxRingBuf.cnt());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
      liPosY+=8;


      // externe Connection
      if (mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.u16Adr != 0)
      {
        lszStr.Setf((const char8*)"ext: %x.%x.%x.%x:%d", mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD1,
                                                         mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD2,
                                                         mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD3,
                                                         mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestAdr.mAdr.stAdr.AdrD4,
                                                         mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcBnDestPortIdx);
        cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
        liPosY+=8;
      }

      for (liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
      {
        // interne Connection
        if (mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx] != 0)
        {
          lszStr.Setf((const char8*)"%d int: ", liCnxIdx + 1);
          lszStr += mcBn_0x1100->mcStreamSys.mcPorts[liPortIdx]->mcConnection[liCnxIdx]->mszName;

          cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(20, 14+liPosY, lszStr.ToString(), &mcScreen1);
          liPosY+=8;
        }
      }
    }
  }
}

void MAIN_vPage5(void)
{
  cPaint::vRectFull(4, 0,  60, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"5-6 Botnet 2/3", &mcScreen1);

  MAIN_vPrintPort(0,1);
}


void MAIN_vPage6(void)
{
  cPaint::vRectFull(4, 0,  60, 10, 0, &mcScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(6, 4, (const char*)"6-6 Botnet 3/3", &mcScreen1);


  MAIN_vPrintPort(2,3);
}


bool MAIN_bRefLaufDrehReady;
bool MAIN_bRefLaufNeigReady;
bool MAIN_bRefLaufNorthReady;

i16 MAIN_i16RefLaufDrehStart;
i16 MAIN_i16RefLaufNeigStart;
i16 MAIN_i16RefLaufNorthStart;

i16  MAIN_i16NorthPos;

typedef enum
{
  nNotDone = 0,

  nWaitForNeigAndDreh,
  nWaitForNorth,
  nWaitForNull,
  nError,
  nDone
}MAIN_tenRefLauf;

MAIN_tenRefLauf menRefLauf = MAIN_tenRefLauf::nNotDone;


void vSm_ReferenzLauf(bool lbStart)
{
  if (lbStart)
  {
    menRefLauf = MAIN_tenRefLauf::nNotDone;
    MAIN_i16NorthPos        = 0;
    MAIN_bRefLaufDrehReady  = False;
    MAIN_bRefLaufNeigReady  = False;
    MAIN_bRefLaufNorthReady = False;
  }

  if (!mcAPDS9960.isEnabledAmbientLigth())
  {
    menRefLauf = MAIN_tenRefLauf::nError;
  }


  switch (menRefLauf)
  {
    case MAIN_tenRefLauf::nNotDone:

      MAIN_i16RefLaufDrehStart  = mcMot_TurmDrehung.miSensor;

      MAIN_i16RefLaufNeigStart  = mi16LightBoxNeig;
      if (MAIN_i16RefLaufNeigStart >  1) MAIN_i16RefLaufNeigStart =  1;
      if (MAIN_i16RefLaufNeigStart < -1) MAIN_i16RefLaufNeigStart = -1;

       if (mcMot_TurmDrehung.miSensor > 0)
      {
        mcMot_TurmDrehung.vSetSpeed(100);
      }
      else if (mcMot_TurmDrehung.miSensor < 0)
      {
        mcMot_TurmDrehung.vSetSpeed(-100);
      }

      if (mi16LightBoxNeig > 0)
      {
        mcMot_TurmNeigung.vSetSpeed(100);
      }
      else if (mi16LightBoxNeig < 0)
      {
        mcMot_TurmNeigung.vSetSpeed(-100);
      }

      menRefLauf = MAIN_tenRefLauf::nWaitForNeigAndDreh;
      break;
    case MAIN_tenRefLauf::nWaitForNeigAndDreh:
      {
        if (((mcMot_TurmDrehung.miSensor != MAIN_i16RefLaufDrehStart) || (mcMot_TurmDrehung.miSensor == 0)) && (!MAIN_bRefLaufDrehReady))
        {
          mcMot_TurmDrehung.vSetSpeed(0);
          MAIN_bRefLaufDrehReady = True;
          mcMot_TurmDrehung.vNull();
        }

        i16 li16Zwerg;
        li16Zwerg  = mi16LightBoxNeig;
        if (li16Zwerg >  1) li16Zwerg =  1;
        if (li16Zwerg < -1) li16Zwerg = -1;
        if (((li16Zwerg != MAIN_i16RefLaufNeigStart) || (mi16LightBoxNeig == 0)) && (!MAIN_bRefLaufNeigReady))
        {
          mcMot_TurmNeigung.vSetSpeed(0);
          MAIN_bRefLaufNeigReady = True;
          mcMot_TurmNeigung.vNull();
        }

        if (MAIN_bRefLaufDrehReady && MAIN_bRefLaufNeigReady)
        {
          menRefLauf = MAIN_tenRefLauf::nWaitForNorth;

          MAIN_i16RefLaufNorthStart = mi16LightBoxKompass;
          if (MAIN_i16RefLaufNorthStart >  1) MAIN_i16RefLaufNorthStart =  1;
          if (MAIN_i16RefLaufNorthStart < -1) MAIN_i16RefLaufNorthStart = -1;

          if (MAIN_i16RefLaufNorthStart > 0)
          {
            mcMot_TurmDrehung.vSetSpeed(-100);
          }
          else if (MAIN_i16RefLaufNorthStart < 0)
          {
            mcMot_TurmDrehung.vSetSpeed(+100);
          }
        }
      }
      break;
    case MAIN_tenRefLauf::nWaitForNorth:
      { 
        i16 li16Zwerg;
        li16Zwerg  = mi16LightBoxKompass;
        if (li16Zwerg >  1) li16Zwerg =  1;
        if (li16Zwerg < -1) li16Zwerg = -1;
        if (li16Zwerg != MAIN_i16RefLaufNorthStart)
        {
          mcMot_TurmDrehung.vSetPosGrad(0);
          MAIN_bRefLaufNorthReady = True;
          MAIN_i16NorthPos = mcMot_TurmDrehung.i16GetPosGrad();
          menRefLauf = MAIN_tenRefLauf::nWaitForNull;
        }
      }
      break;
    case MAIN_tenRefLauf::nWaitForNull:

      if (mcMot_TurmDrehung.isRdy())
      {
        menRefLauf = MAIN_tenRefLauf::nDone;
      }
      break;
    default:
      break;
  }

}

void MAIN_vTick100msLp(void)
{
  mcStatusLed.vTick100ms();

  // Reset Buffer of Offline Links
  //mcBn_0x1100->vResetCom(False);

  InAppProg_Platform_vResetWdog();

  if (mcAPDS9960.isEnabledAmbientLigth())
  {
    mcAPDS9960.i8ReadAmbientLight();
    mcMot_TurmDrehung.vCheckPositionAls(mcAPDS9960.ui16GetAmbientLight_Clear(), mcAPDS9960.ui16GetAmbientLight_Red(), mcAPDS9960.ui16GetAmbientLight_Green(), mcAPDS9960.ui16GetAmbientLight_Blue());
  }

  if ((mcMot_TurmDrehung.miSensor == 0) && 
      ((mcMot_TurmDrehung.i16GetPosGrad() > 30) || (mcMot_TurmDrehung.i16GetPosGrad() < -30))
      )
  {
    mcMot_TurmDrehung.vNull();
  }

  if (mi16LightBoxNeig == 0)
  {
    mcMot_TurmNeigung.vNull();
  }

  if (mcOpMode.IsAuto())
  {
    if ((menRefLauf != MAIN_tenRefLauf::nDone) && (mi16LightBoxMsgCounter > 3))
    {
      if (menRefLauf == MAIN_tenRefLauf::nNotDone)
      {
        vSm_ReferenzLauf(True);
      }
      else
      {
        vSm_ReferenzLauf(False);
      }
    }
  }

  mcI2C1_LSM303DLHC.bStartNext();
}


void MAIN_vTick1000msLp(void)
{
  //static u8 lu2s    = 0;

  mcScreen1.vFill(0);
  cPaint::vRect(2, 2, 124, 60, 1, &mcScreen1);

  if (mcSSD1306.mStatus.IsInit)
  {
    mu8DispPage = 1;
    switch (mu8DispPage)
    {
      case 0: MAIN_vPage1(); break;
      case 1: MAIN_vPage2(); break;
      case 2: MAIN_vPage3(); break;
      case 3: MAIN_vPage4(); break;
      case 4: MAIN_vPage5(); break;
      case 5: MAIN_vPage6(); break;
      default: MAIN_vPage1(); break;
    }
  }


  /*if (lu2s < 2)
  {
    lu2s++;

    if (mcOpMode.IsAuto())
    {
      mcStatusLed.vSetMode(cStatusLed<GPIOB_BASE, 9>::nStToggle500ms);
      if (mcMot_TurmDrehung.isRdy())
      {
        if (mcMot_TurmDrehung.mi16ActualPos == 0)
          mcMot_TurmDrehung.vSetPos(345/2);
        else
          mcMot_TurmDrehung.vSetPos(0);
      }

      if (mcMot_TurmNeigung.isRdy())
      {
        if (mcMot_TurmNeigung.mi16ActualPos == 0)
          mcMot_TurmNeigung.vSetPos(345);
        else
          mcMot_TurmNeigung.vSetPos(0);
      }
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
      if (mu8DispPage < 5)
      {
        mu8DispPage++;
      }
      else
      {
        mu8DispPage = 0;
      }
    }
  }*/


  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
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

  /*mcMag.vSetConfig(LSM303DLHC_MAG_TEMPSENSOR_ENABLE, LSM303DLHC_MAG_ODR_15_HZ, LSM303DLHC_MAG_FS_8_1_GA, LSM303DLHC_MAG_CONTINUOS_CONVERSION);
  mcAcc.vSetControl(LSM303DLHC_ACC_ODR_25_HZ | LSM303DLHC_ACC_NORMAL_MODE | LSM303DLHC_ACC_X_ENABLE | LSM303DLHC_ACC_Y_ENABLE | LSM303DLHC_ACC_Z_ENABLE,
                    LSM303DLHC_ACC_HPM_NORMAL_MODE | LSM303DLHC_ACC_HPFCF_32,
                    0, // No Ints
                    LSM303DLHC_ACC_BlockUpdate_Continous | LSM303DLHC_ACC_FULLSCALE_2G,
                    0,
                    0
                    );*/

  mcAPDS9960.i8EnableAmbientLight();

  cClockInfo::Delay_ms(50);
  mcI2C1_LSM303DLHC.bStartNext();
  cClockInfo::Delay_ms(50);

  mcBn_0x1100 = new cBotNet(&mcMyBotNetCfg, &mcBn_MsgProcess_0x1100);

  mcBn_0x1100->bAddLink((cBotNet_LinkBase*)&mcUpLnk_0x1100);

  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1100->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  lu16PortIdx = mcBn_0x1100->u16StreamPortAdd(&mcBnMemPort);
  mcBn_0x1100->vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Connect the CmdPort's output to internal Port (to internal ComPort)
  mcBn_0x1100->vStreamPortConnect(cBotNet_CmdPortIdx, 0, lu16PortIdx);

  // Add MotorControl Port (not connected)
  lu16PortIdx = mcBn_0x1100->u16StreamPortAdd(&mcBnMotControl);

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,


                MAIN_vTick1msLp  /*1ms_LP*/,
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
