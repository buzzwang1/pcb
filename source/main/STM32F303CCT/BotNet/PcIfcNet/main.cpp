
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
LED lcLed1;
LED lcLed2;
LED lcLed3;

cBotNetCfg mcBnCfg((const char8*)"Botnet ", 7, 0xE000);

cHx8347 mcDisplay;

#define MAIN_nDISPLAY_X HX8347A_WIDTH
#define MAIN_nDISPLAY_Y HX8347A_HEIGHT

uint8                     mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];   // 40.960 Bytes
cBitmap_Bpp1_1G           mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G           mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G      cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


tcGpPin<GPIOB_BASE, 0>  mPinBgPwm(GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 1);

// BotNet
cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBotNet gcBn(&mcMyBotNetCfg);

tcUart<USART1_BASE, GPIOB_BASE, 6, GPIOB_BASE, 7> mcComPort2(38400, GPIO_AF_7, 128, 128);
cBotNet_LinkBotCom         mcUpLinkBotCom(&mcComPort2);

cNRF905Master              mcNRF905Master(0x00010110, 0x00010100);
cBotNet_DownLinknRf905Net  gcDownLink(&mcNRF905Master, &gcBn);
bool mbConnected = False;
bool mbDisplayOn = True;


class cTextBox
{
public:
  typedef enum
  {
    nNoFit = 0,
    nFit,
    nFitSmart,
    nNoFitRev,
    nFitRev,
    nFitSmartRev,
  } tenTextFitMode ;

  cRFont *mpcRFont;
  char   *mChText;
  cStr    mszText;
  uint32 mui32MaxCharacterCount;
  uint32 mui32TextCol;
  tenTextFitMode menTextFitMode;

  cTextBox(tenTextFitMode lenTextFitMode,
           uint32 lui32MaxCharacterCount,
           cRFont *lpcRFont)
  : mszText(mChText = new char[lui32MaxCharacterCount + 1], 0, lui32MaxCharacterCount)
  {
    mui32MaxCharacterCount = lui32MaxCharacterCount;
    mpcRFont = lpcRFont;
    mszText = "";
    mui32TextCol = 0xFFFFFF;
    menTextFitMode = lenTextFitMode;
  }

  ~cTextBox()
  {
    delete []mChText;
  }


  void vAddSign(const char8 lChr)
  {
    // Wenn Textbuffer voll, dann ...
    if (mszText.Len() >= mszText.Size())
    {
      // ... versuchen, eine komplette eine Zeile zu löschen
      u16 lu16t;

      lu16t = 0;
      while ((lu16t < mszText.Len()) && (mszText[lu16t] != '\n'))
      {
        lu16t++;
      }

      if (mszText[lu16t] == '\n')
      {
        lu16t++;
      }

      if (lu16t > 0)
      {
        mszText.Cut(0, lu16t);
      }
    }
    mszText += lChr;
  }


  void vAddText(const char8* lszText)
  {
    while (*lszText != 0)
    {
      vAddSign(*lszText);
      lszText++;
    }
  }

  void vPaint(cScreen *lpcScreen)
  {
    if (lpcScreen != null)
    {
      tstGfxPos2D lstSize;

      lpcScreen->vFill(0);

      mpcRFont->mui32Col = mui32TextCol;

      switch (menTextFitMode)
      {
        case nFitRev:      lstSize = mpcRFont->vFitText(2,      0, &mszText, lpcScreen, True); break;
        case nFitSmartRev: lstSize = mpcRFont->vFitSmartText(2, 0, &mszText, lpcScreen, True); break;
        case nNoFitRev:    lstSize = mpcRFont->i8PutStringXY(2, 0, &mszText, lpcScreen, True); break;
        default:           lstSize.x = 0; lstSize.y = 0;                           break;
      }

      switch (menTextFitMode)
      {
        case nFitRev:      mpcRFont->vFitText(2,      lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nFitSmartRev: mpcRFont->vFitSmartText(2, lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nNoFitRev:    mpcRFont->i8PutStringXY(2, lpcScreen->iGfxHeight() - lstSize.y, &mszText, lpcScreen);break;
        case nFit:         mpcRFont->vFitText(2,      2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);break;
        case nFitSmart:    mpcRFont->vFitSmartText(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);break;
        default:           mpcRFont->i8PutStringXY(2, 2 + mpcRFont->miGfxHeight - mpcRFont->miGfxYOffset, &mszText, lpcScreen);
      }
    }
  }
};

cTextBox mcTextPort(cTextBox::nFitSmartRev, 512, &cRFont_Res8b_Bpp1_1G_5x5Ucase);


class cSwTimer
{
  public:
  u16 mu16DownCounter;
  u16 mu16DownCounter_Reload;

  cSwTimer(u16 lu16DownCounter_Reload)
  {
    mu16DownCounter_Reload = lu16DownCounter_Reload;
    mu16DownCounter = mu16DownCounter_Reload;
  }

  void vReset()
  {
    mu16DownCounter = mu16DownCounter_Reload;
    vReseted();
  }

  void vProcess()
  {
    if (mu16DownCounter) 
    {
      mu16DownCounter--;
      if (!mu16DownCounter)
      {
        vExpired();
      }
    }
  }

  u16 u16Get()
  {
    return mu16DownCounter;
  }

  void vExpired()
  {  
    mPinBgPwm.vSet0();
    mcDisplay.vDeInit();
    mbDisplayOn = False;
  }

  void vReseted()
  {
    if (!mbDisplayOn)
    {
      mcDisplay.vInit();
      mPinBgPwm.vSet1();
      mbDisplayOn = True;
    }
  }
};

cSwTimer mcDisplayTimer(60*5);


class cButtonArray
{
public:
  cGpPin* mBtn[4];
  u8      mu8State[4];

  cButtonArray()
  {
    mBtn[0] = new cGpPin(GPIOA_BASE, 2, GPIO_Mode_IN, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_2MHz, 0);
    mBtn[1] = new cGpPin(GPIOA_BASE, 3, GPIO_Mode_IN, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_2MHz, 0);
    mBtn[2] = new cGpPin(GPIOA_BASE, 0, GPIO_Mode_IN, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_2MHz, 0);
    mBtn[3] = new cGpPin(GPIOA_BASE, 1, GPIO_Mode_IN, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_2MHz, 0);

    mu8State[0] = u8GetState(0);
    mu8State[1] = u8GetState(1);
    mu8State[2] = u8GetState(2);
    mu8State[3] = u8GetState(3);
  }

  u8 u8GetState(u8 lu8Button)
  {
    return (!mBtn[lu8Button]->ui8Get());
  }

  void vProcess()
  {
    u8 lu8BtnIdx;
    
    for (lu8BtnIdx = 0; lu8BtnIdx < 4; lu8BtnIdx++)
    {
     u8 lu8NewState;

     lu8NewState = u8GetState(lu8BtnIdx);

      if (mu8State[lu8BtnIdx] != lu8NewState)
      {
        mu8State[lu8BtnIdx] = lu8NewState;
        // Buuton are Low active, Btn = 0 => Pressed
        if (lu8NewState)
        {
          vReleased(lu8BtnIdx);
        }
        else
        {
          vPressed(lu8BtnIdx);
        }        
      }
    }
  }

  void vPressed(u8 lu8Button)
  {
    switch (lu8Button)
    {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      default:
        break;
    }

    mcDisplayTimer.vReset();
  }

  void vReleased(u8 lu8Button)
  {
    switch (lu8Button)
    {
      case 0:
        break;
      case 1:
        break;
      case 2:
        break;
      default:
        break;
    }
  }
};

cButtonArray mcBtns;


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
  if (TimingDelay > 0)
  {
    TimingDelay--;
  }
  else
  {
    TimingDelay = 0;
  }
}

void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}



class cBnMsgHandlerApp : public cBotNet_MsgSysProcess
{
public:
  cBnMsgHandlerApp()
    : cBotNet_MsgSysProcess(&gcBn)
  {
  }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    // Überprüfen,obes eine Session Start/Stop Nachricht ist
    //
    //                 0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
    // Wait & Start:  11 | DH DL | FE | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || DA DA E0 00 80 : Versucht mit DH.DL:DA.DA zu verbinden
    // Disconnect:    11 | DH DL | FF | XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || DA DA E0 00 80
    u8* lu8RxPayload = lcMsg.mpu8Data;
    if (lcMsg.u8Len() == 25)
    {
      // Info Nachricht ?
      // Nachricht vom PC
      if ((lu8RxPayload[24] == 0x80) &&
           (lu8RxPayload[0] == 0x11))
      {
        //Start
        if ((lu8RxPayload[3] == 0xFE) && (mbConnected == False))
        {
          gcDownLink.mu16SessionDAdr  = (lu8RxPayload[ 1] << 8) + lu8RxPayload[2];
          gcDownLink.mu16SessionBnAdr = (lu8RxPayload[20] << 8) + lu8RxPayload[21];

          gcBn.mcAdr.Set((lu8RxPayload[22] << 8) + lu8RxPayload[23]);
          gcBn.mcDeviceID.Set((lu8RxPayload[4] << 8) + lu8RxPayload[5]);

          gcDownLink.mbSessionStart = True;
          gcDownLink.mbSessionStop  = False;
          //lcLed2.On();
          //mcTextPort.vAddText((const char8*)"Wait for Connection\r\n");
          //mcDisplayTimer.vReset();
          return True;
        }

        //Stop
        if (lcMsg[3] == 0xFF)
        {
          gcDownLink.mbSessionStop = True;
          //mcTextPort.vAddText((const char8*)"Abort Connection\r\n");
          //mcDisplayTimer.vReset();
          //lcLed2.Off();
          return True;
        }
      }
    }
    return False;
  }
};

cBnMsgHandlerApp gcBnMsgHandlerApp;

void I2C2_EV_IRQHandler(void)
{
}

void I2C2_ER_IRQHandler(void)
{
}

void I2C1_EV_IRQHandler(void)
{
}

void I2C1_ER_IRQHandler(void)
{
}

//Display
void DMA1_Channel5_IRQHandler(void)
{
  // SPI TX
  DMA1_Channel5->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA1_FLAG_GL5 | DMA1_FLAG_HT5 | DMA1_FLAG_TC5;
  mcDisplay.vEvDmaTc();
}

void EXTI15_10_IRQHandler(void)
{
  // Handle PC15 interrupt
  // Make sure that interrupt flag is set
  if (EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    // Clear interrupt flag
    EXTI_ClearITPendingBit(EXTI_Line15);
    mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvUsartExtiP1);
  }
}

//nrf905
void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC2);
  mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvDmaRxTc);
}

//nrf905
void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA_Cmd(DMA1_Channel3, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC3);
  mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvDmaTxTc);
}


void TIM4_IRQHandler(void)
{
  if(TIM4->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM_Cmd(TIM4, DISABLE);
    mcNRF905Master.IrqHandler(cComNode::tenEvent::enEvUsartTimer);
  }
}


void USART1_IRQHandler(void)
{
  lcLed1.Toggle();
  mcUpLinkBotCom.mcUart->vIRQHandler();

  //mcComPort2.mcConIo.vIRQHandler_Ev();
}

void USART3_IRQHandler(void)
{
  lcLed1.Toggle();
  //mcComPort3.mcConIo.vIRQHandler_Ev();
}


void MAIN_vTick1msLp(void)
{
  gcBn.vProcess(1000);
  mcNRF905Master.vTick1ms();
}

void MAIN_vTick10msLp(void)
{
  // Check Connected Status
  // Überprüfen,obes eine Session Start/Stop Nachricht ist
  //
  //                 0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
  // Wait & Start:  11 | DH DL | FE | SH.SL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || E0 00 DA DA 80
  // Disconnect:    11 | DH DL | FF | SH.SL.XX.XX XX.XX.XX.XX XX.XX.XX.XX XX.XX.XX.XX || E0 00 DA DA 80

  if ((!mbConnected) && (gcDownLink.IsOnline()))
  {
    mbConnected = True;
    lcLed2.Off();
    mcTextPort.vAddText((const char8*)"Connected\r\n");
    mcDisplayTimer.vReset();

    // Send Connection Acknowledge
    u8 u8MsgData[] = { 0x11, (u8)(gcDownLink.mu16SessionDAdr >> 8), (u8)gcDownLink.mu16SessionDAdr, 0xFE,
                             (u8)(gcBn.mcDeviceID.Get() >> 8), (u8)gcBn.mcDeviceID.Get(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             (u8)(gcBn.mcAdr.Get() >> 8), (u8)gcBn.mcAdr.Get(), (u8)(gcDownLink.mu16SessionBnAdr >> 8), (u8)gcDownLink.mu16SessionBnAdr, 0x80 };

    u8 lu8PoolIdx;
    cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, sizeof(u8MsgData));
    if (lu8PoolIdx)
    {
      lcMsgTx.Set(u8MsgData, sizeof(u8MsgData));
      cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.Len());
      mcUpLinkBotCom.bPut(lu8PoolIdx);
      cBnMsgPool::vReleaseMsg(lu8PoolIdx);
    }
  }


  if ((mbConnected) && (!gcDownLink.IsOnline()))
  {
    mbConnected = False;
    lcLed2.Off();
    mcTextPort.vAddText((const char8*)"Disconnected\r\n");
    mcDisplayTimer.vReset();

    // Send Dis-Connection Acknowledge
    u8 u8MsgData[] = { 0x11, (u8)(gcDownLink.mu16SessionDAdr >> 8), (u8)gcDownLink.mu16SessionDAdr, 0xFF,
                             (u8)(gcBn.mcDeviceID.Get() >> 8), (u8)gcBn.mcDeviceID.Get(), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             (u8)(gcBn.mcAdr.Get() >> 8), (u8)gcBn.mcAdr.Get(), (u8)(gcDownLink.mu16SessionBnAdr >> 8), (u8)gcDownLink.mu16SessionBnAdr, 0x80 };

    u8 lu8PoolIdx;
    cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, sizeof(u8MsgData));
    if (lu8PoolIdx)
    {
      lcMsgTx.Set(u8MsgData, sizeof(u8MsgData));
      cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.Len());
      mcUpLinkBotCom.bPut(lu8PoolIdx);
      cBnMsgPool::vReleaseMsg(lu8PoolIdx);
    }
  }
}

void MAIN_vTick100msLp(void)
{
  static u8 lu8Cnt = 0;
  if ((mbConnected) || (lu8Cnt == 0))
  {
    lcLed1.Toggle();
  }

  lu8Cnt++;
  if (lu8Cnt > 5) lu8Cnt = 0;

  if (mbDisplayOn)
  {
    if (mcDisplay.isReady())
    {
      mcScreen1.vFill(0);

      cPaint::vRect(1, 1, MAIN_nDISPLAY_X-2, MAIN_nDISPLAY_Y-2-16, 1, &mcScreen1);

      cScreen_Bpp1_1G lcMid(2, 2, MAIN_nDISPLAY_X - 3, MAIN_nDISPLAY_Y - 2-16-2, &mcScreen1);
      mcTextPort.vPaint(&lcMid);


      cPaint::vRect(1, MAIN_nDISPLAY_Y - 2-16, 16, 16, 1, &mcScreen1);
      if (mcBtns.u8GetState(0) == 1) cPaint::vRectFull( 3, MAIN_nDISPLAY_Y - 2 - 14, 4, 4, 1, &mcScreen1);
      if (mcBtns.u8GetState(1) == 1) cPaint::vRectFull(11, MAIN_nDISPLAY_Y - 2 - 14, 4, 4, 1, &mcScreen1);
      if (mcBtns.u8GetState(2) == 1) cPaint::vRectFull( 3, MAIN_nDISPLAY_Y - 2 - 14 + 8, 4, 4, 1, &mcScreen1);
      if (mcBtns.u8GetState(3) == 1) cPaint::vRectFull(11, MAIN_nDISPLAY_Y - 2 - 14 + 8, 4, 4, 1, &mcScreen1);

      cPaint::vRect(1+16, MAIN_nDISPLAY_Y - 2-16, 16, 16, 1, &mcScreen1);
      if (lcLed1 == LED::nOn) cPaint::vRectFull(1+16+2, MAIN_nDISPLAY_Y - 2 - 16 + 2, 12, 12, 1, &mcScreen1);

      cPaint::vRect(1+16*2, MAIN_nDISPLAY_Y - 2-16, 16, 16, 1, &mcScreen1);
      if (lcLed2 == LED::nOn) cPaint::vRectFull(1+16*2+2, MAIN_nDISPLAY_Y - 2 - 16 + 2, 12, 12, 1, &mcScreen1);

      cPaint::vRect(1+16*3, MAIN_nDISPLAY_Y - 2-16, 16, 16, 1, &mcScreen1);
      if (lcLed3 == LED::nOn) cPaint::vRectFull(1+16*3+2, MAIN_nDISPLAY_Y - 2 - 16 + 2, 12, 12, 1, &mcScreen1);

      cPaint::vRect(1+16*4, MAIN_nDISPLAY_Y - 2-16, 54, 16, 1, &mcScreen1);


      cBotNetAdress lBnAdr(gcDownLink.mu16SessionBnAdr);
      char8         lszStrBuf[32];
      cStr          lszStr(lszStrBuf, 32);

      lszStr.Setf((const char8*)"%04x %s", gcBn.mcDeviceID.Get(), gcBn.mcAdr.ToString());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(1+16*4 + 4, MAIN_nDISPLAY_Y - 2-16 + 6 , lszStr.ToString(), &mcScreen1);

      lszStr.Setf((const char8*)"%04x %s", gcDownLink.mu16SessionDAdr, lBnAdr.ToString());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(1+16*4 + 4, MAIN_nDISPLAY_Y - 2-16 + 6*2+1, lszStr.ToString(), &mcScreen1);

    
      lszStr.Setf((const char8*)"%3d", mcDisplayTimer.u16Get());
      cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(MAIN_nDISPLAY_X-3*4-2-2, 8, lszStr.ToString(), &mcScreen1);

      mcDisplay.vShowScreenDma(mcScreen1.mpcBm->mpui8Data);
    }
  }
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
  mcDisplayTimer.vProcess();
}


void MAIN_vInitSystem(void)
{
  // Construct function
  SystemInit();
  // SysTick end of count event each 10ms
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

  cBnMsgPool::vInit();


  mcUpLinkBotCom.menType = cBotNet_LinkBase::tenType::enUpLink;
  gcBn.bAddLink((cBotNet_LinkBase*)&mcUpLinkBotCom);

  // Nrf905
  // AddLink initialisiert beim Slave auch direkt die HW
  // Wenn man das zu schnell nach Reset macht, dann kann sein dass der 3 nrf905 noch nicht bereit ist.
  // Device Switching Times: PWR_DWN -> ST_BY mode: 3 ms
  gcBn.bAddLink((cBotNet_LinkBase*)&gcDownLink);
  gcBnMsgHandlerApp.vAddMsgSys();


  mcTextPort.vAddText((const char8*)"Radio ");
  if (gcDownLink.mStatus.IsInit)
  {
    mcTextPort.vAddText((const char8*)"OK\r\n");
  }
  else
  {
    mcTextPort.vAddText((const char8*)"not OK !!!\r\n");
  }

  mcTextPort.vAddText((const char8*)"Init done\r\n");

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp    /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

