
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

__IO uint32_t TimingDelay = 0;

LED<GPIOF_BASE, 9> mcLedRed;

cGpPin lcSCL(GPIOB_BASE, 8, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 0);
cGpPin lcSDA(GPIOB_BASE, 9, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_25MHz, 0);

LED<GPIOB_BASE, 4> lcLedI2cDmaTxTc;
LED<GPIOB_BASE, 5> lcLedI2cDmaRxTc;
LED<GPIOB_BASE, 6> lcLedI2cInt;
LED<GPIOB_BASE, 7> lcLedI2cErr;

cI2cMaster   mcI2C1(I2C1, &lcSCL, &lcSDA, 8);
cSSD1306     mcSSD1306(&mcI2C1, 0x78);
cAPDS9960    mcAPDS9960(&mcI2C1, nAPDS9960_I2C_ADDR);

#define MAIN_nDISPLAY_X cSSD1306_WIDTH
#define MAIN_nDISPLAY_Y cSSD1306_HEIGHT

uint8                    mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y / 8];
cBitmap_Bpp1_1G          mcBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp1_1G          mcScreen1(&mcBm);
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mcSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mcSpriteEng);
cRFont_Res8b_Bpp1_1G     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mcSpriteEng);


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
  //1ms at 168Mhs
  uint32 lui32DecValue = 168000000 / cClockInfo::mstClocks.HCLK_Frequency;

  if (TimingDelay > lui32DecValue)
  {
    TimingDelay -= lui32DecValue;
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


void DMA1_Stream5_IRQHandler(void)
{
  // I2C1 Rx Transmission Complete
  lcLedI2cDmaRxTc.vSet1();
  DMA_Cmd(DMA1_Stream5, DISABLE);
  DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
  mcI2C1.I2C_EV_DMAHandler(cComNode::tenEvent::enEvDmaRxTc);
  lcLedI2cDmaRxTc.vSet0();
}


void I2C1_EV_IRQHandler(void)
{
  lcLedI2cInt.vSet1();
  mcI2C1.I2C_EV_IRQHandler();
  lcLedI2cInt.vSet0();
}

void I2C1_ER_IRQHandler(void)
{
  lcLedI2cErr.vSet1();
  mcI2C1.I2C_ER_IRQHandler();
  lcLedI2cErr.vSet0();
}


void MAIN_vTick10msHp(void)
{
  if (!mcI2C1.bStartNext())
  {
    mcI2C1.vSetReInitTicks(1000);
  }
}



void MAIN_vTick100msLp(void)
{
  mcAPDS9960.i8ReadGestureSensor();
}

void MAIN_vItoa_HUD21D(int num, char8* str)
{
  int32 li32Val1 = 0;
  int32 li32Val2 = 0;
  char lszVal1[8] = "";
  char lszVal2[8] = "";
  char lszPkt[] = ",";

  li32Val1 = num / 100;
  li32Val2 = num - (li32Val1 * 100);

  cStrTools::uixItoa(li32Val1, lszVal1, 10);

  if (li32Val2 < 10)
  {
    lszVal2[0] = '0';
    cStrTools::uixItoa(li32Val2, (char8*)(&lszVal2[1]), 10);
  }
  else
  {
    cStrTools::uixItoa(li32Val2, lszVal2, 10);
  }

  cStrTools::szStrCpy(str, lszVal1);
  cStrTools::szStrCat(str, lszPkt);
  cStrTools::szStrCat(str, lszVal2);
}


void MAIN_vTick1000msLp(void)
{
  char lszValue[16] = "";
  mcLedRed.Toggle();

  mcScreen1.vFill(0);

  if (mcAPDS9960.isEnabledAmbientLigth())
  {
    mcAPDS9960.i8ReadAmbientLight();
    if (mcAPDS9960.boGetAmbientLight_Valid())
    {
      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Clear(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 35, (char8*)"C:",   &mcScreen1);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(30, 35, lszValue,       &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Red(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 35, (char8*)"R:",   &mcScreen1);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(90, 35, lszValue,       &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Green(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 45, (char8*)"G:",   &mcScreen1);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(30, 45, lszValue,       &mcScreen1);

      cStrTools::uixItoa(mcAPDS9960.ui16GetAmbientLight_Blue(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 45, (char8*)"B:",   &mcScreen1);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(90, 45, lszValue,       &mcScreen1);
    }
  }

  if (mcAPDS9960.isEnabledProximitySensor())
  {
    mcAPDS9960.i8ReadProximitySensor();
    if (mcAPDS9960.boGetProximitySensor_Valid())
    {
      cStrTools::uixItoa(mcAPDS9960.ui8GetProximitySensor(), lszValue, 10);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(10, 55, (char8*)"P:",   &mcScreen1);
      cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(30, 55, lszValue,       &mcScreen1);
    }
  }

  if (mcAPDS9960.isEnabledGestureSensor())
  {
    mcAPDS9960.i8ReadGestureSensor();
    switch (mcAPDS9960.menDirection)
    {
      case cAPDS9960::nAPDS9960_DIR_DOWN:
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 55, "D",       &mcScreen1);
        break;
      case cAPDS9960::nAPDS9960_DIR_UP:
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 55, "U",       &mcScreen1);
        break;
      case cAPDS9960::nAPDS9960_DIR_RIGHT:
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 55, "R",       &mcScreen1);
        break;
      case cAPDS9960::nAPDS9960_DIR_LEFT:
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 55, "L",       &mcScreen1);
        break;
      default:
        cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(60, 55, "-",       &mcScreen1);
        break;
    }
  }

  mcSSD1306.vShowScreen(mcScreen1.mpcBm->mpui8Data);
  mcSSD1306.Update();
}


void MAIN_vInitSystem(void)
{
  SystemInit();
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  mcAPDS9960.i8EnableAmbientLight();
  //mcAPDS9960.i8EnableProximitySensor();
  //mcAPDS9960.i8EnableGestureSensor();

  if (mcI2C1.bStartNext())
  {
    cClockInfo::Delay_ms(100);
  }
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}

