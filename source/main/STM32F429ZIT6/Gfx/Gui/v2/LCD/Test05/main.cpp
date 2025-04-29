#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;

//cI2cMaster* mcI2C1;
//cAPDS9960* mcAPDS9960;

LED<GPIOF_BASE, 9>   mcLedRed;
cClockInfo mcClkInfo;

#define MAIN_nDISPLAY_X ILI9341_WIDTH
#define MAIN_nDISPLAY_Y ILI9341_HEIGHT

uint8 mDisplayMemory[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y * 2] __attribute__((section(".ExtMem")));
uint8 mDisplayMemory2[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y * 2] __attribute__((section(".ExtMem")));
float mafzBuffer[MAIN_nDISPLAY_X * MAIN_nDISPLAY_Y] __attribute__((section(".ExtMem")));

cDiffTimer                              mcDiffTimer;
cMsgBox                                  mcMsgBox(20);
cCursor                                  mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp16_5R6G5B                     mc16GBm(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
cBitmap_Bpp16_5R6G5B                     mc16GDisplay(MAIN_nDISPLAY_X, MAIN_nDISPLAY_Y, mDisplayMemory2);
cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

stPxli16                            mstChnXYBPts[10];
ciGraphXY_Channel                   mcChnXYB(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnXYBPts, 10, null);

stPxli16                            mstChnXYAPts[10];
ciGraphXY_Channel                   mcChnXYA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnXYAPts, 10, &mcChnXYB);

i16                                 mstChnYAPts[100];
ciGraphY_Channel                    mcChnYA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYAPts, 100, null);

cUiElement_Root                     mc16GuiRoot(10, 10, MAIN_nDISPLAY_X-20, MAIN_nDISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));
cUiElement                          mc16GuiBase01(&mcMsgBox, &mc16GScreen1);
cUiElement_TextBox                  mc16GuiText01(&mcMsgBox, &mc16GScreen1, 128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement                          mc16GuiBase02(&mcMsgBox, &mc16GScreen1);


cUiElement_Box3D_Cube               mcCube;
cUiElement_Box3D_Torus              mcTorus;
cUiElement_Box3D_Sphere             mcSphere;
cUiElement_Box3D_Monkey             mcMonkey;

cUiElement_Box3D                    mc16GuiBox3D_Cube(&mcMsgBox, &mc16GScreen1, &mcCube);
cUiElement_Box3D                    mc16GuiBox3D_Torus(&mcMsgBox, &mc16GScreen1, &mcTorus);
cUiElement_Box3D                    mc16GuiBox3D_Sphere(&mcMsgBox, &mc16GScreen1, &mcSphere);
cUiElement_Box3D                    mc16GuiBox3D_Monkey(&mcMsgBox, &mc16GScreen1, &mcMonkey);

cUiElement_Window                   mc16GuiWindow01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphXYBox              mc16GuiGraphXY(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnXYA);

cUiElement_Window                   mc16GuiWindow02(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphYBox               mc16GuiGraphY(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnYA);

class cMyBox3Df : public cUiElement_Box3Df 
{
public:

  cTexture32_Bpp16_5R6G5B             mcTextures[8];

  cWorld_3Df                          mcWorld;

  Simple3Df_Tex_Light_ObjTemplate_Cube    mcCubeTemplate;
  Simple3Df_Tex_Light_ObjTemplate_Torus   mcTorusTemplate;
  Simple3Df_Tex_Light_ObjTemplate_Sphere  mcSphereTemplate;
  Simple3Df_Tex_Light_ObjTemplate_Monkey  mcMonkeyTemplate;

  cSimple3Df_Obj mcCube1;
  cSimple3Df_Obj mcCube2;
  cSimple3Df_Obj mcTorus1;
  cSimple3Df_Obj mcSphere1;
  cSimple3Df_Obj mcMonkey1;

  cWorld_3Df_Viewer                   mcViewer;
  cWorld_3Df_Viewer_Tex_Grad_Z        mcViewerTexGradZ;
  cWorld_3Df_Viewer_Flat              mcViewerFlat;

  cMyBox3Df(cMsgBox* lcMsgBox, cScreen* lpcScreen, cRFont* lpcRFont, float* lafzBuffer)    
    : cUiElement_Box3Df(lcMsgBox, lpcScreen, &mcWorld, (cWorld_3Df_ViewerWorld_Base*)&mcViewerTexGradZ), 
      mcWorld(300, 500, 30),
      mcCubeTemplate(0),
      mcTorusTemplate(0),
      mcSphereTemplate(0),
      mcMonkeyTemplate(0),
      mcCube1(   -9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcCube2(    9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcTorus1(  20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcTorusTemplate),
      mcSphere1(-20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcSphereTemplate),
      mcMonkey1(-80.0f, 4.0f,  -50.0f, 90.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcMonkeyTemplate),
      mcViewerTexGradZ(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, mcTextures, lafzBuffer, lpcRFont),
      mcViewerFlat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, lpcRFont)
  {
    for (int i = 0; i < 8; i++)
    {
      mcTextures[i].Test_Pattern_2x2(BM_BPP16_5R6G5B_RED, BM_BPP16_5R6G5B_BLUE, BM_BPP16_5R6G5B_WHITE);
    }
    mcTextures[0].Test_Pattern_Flat(BM_BPP16_5R6G5B_RGB(15, 31, 15));

    // This is done later, because static data were still only 0 ?
    /*mcWorld.AddObj(&mcCube1);
    mcWorld.AddObj(&mcCube2);
    mcWorld.AddObj(&mcTorus1);
    mcWorld.AddObj(&mcSphere1);
    mcWorld.AddObj(&mcMonkey1);*/
  }

  virtual void OnTimer(u32 lu32Timediff_ms) override
  {
    static int liTextureCnt = 0;
    int liTextureCnt2 = 0;
    static u8  lu8T100ms = 0;

    static int liViewerState = 0;
    static int liViewerCounter = 0;

    if (lu8T100ms == 2)
    {
      lu8T100ms = 0;

      liTextureCnt++;
      liTextureCnt &= 15;
    }
    else
    {
      lu8T100ms++;
    }

    // Texture[1] überzeichnen zeichnen
    cScreen_Bpp16_5R6G5B lcTexScreen(&mcTextures[1]);
    lcTexScreen.vFill(0xFFFFFF);

    liTextureCnt2 = liTextureCnt;
    cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2 * liTextureCnt2, 31 - 2 * liTextureCnt2, 0x0FF0000, &lcTexScreen);

    liTextureCnt2 += 4;
    liTextureCnt2 &= 15;
    cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2 * liTextureCnt2, 31 - 2 * liTextureCnt2, 0x000FF00, &lcTexScreen);

    liTextureCnt2 += 4;
    liTextureCnt2 &= 15;
    cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2 * liTextureCnt2, 31 - 2 * liTextureCnt2, 0x00000FF, &lcTexScreen);

    liTextureCnt2 += 4;
    liTextureCnt2 &= 15;
    cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2 * liTextureCnt2, 31 - 2 * liTextureCnt2, 0x0FF00FF, &lcTexScreen);


    //---------------------------------------------------------------

    if (mui8Play)
    {
      switch (liViewerState)
      {
      case 0:
        mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);
        mcViewer.mcDirA.z += 360.0f / 70.0f;

        liViewerCounter++;
        if (liViewerCounter >= 70)
        {
          liViewerState++;
          liViewerCounter = 0;
          mcViewer.mcDirA.z = 0.0f;
        }
        break;
      case 1:
        mcViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 2:
        mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 40)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 3:
        mcViewer.mcDirA.x += 1;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 4:
        mcViewer.mcDirA.x -= 1;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 5:
        mcViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 6:
        mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 70)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 7:
        mcViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 8:
        mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 40)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 9:
        mcViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          mcViewer.mcDirA.y = 0;
          liViewerState = 0;
          liViewerCounter = 0;
        }
        break;
      }
    }

    /*mcCube1.rx += 3.0f;
    if (mcCube1.rx > 360) mcCube1.rx -= 360.0f;
    mcCube1.ry += 3.0f;
    if (mcCube1.ry > 360) mcCube1.ry -= 360.0f;
    mcWorld.UpdateObj(&mcCube1);

    mcCube2.rz += 3.0f;
    if (mcCube2.rz > 360.0f) mcCube2.rz -= 360.0f;
    mcWorld.UpdateObj(&mcCube2);*/

    mcTorus1.rx += 3.0f;
    if (mcTorus1.rx > 360.0f) mcTorus1.rx -= 360.0f;
    mcWorld.UpdateObj(&mcTorus1);

    /*mcSphere1.rx += 3.0f;
    if (mcSphere1.rx > 360.0f) mcSphere1.rx -= 360.0f;
    mcWorld.UpdateObj(&mcSphere1);*/

    mcMonkey1.rz += 2.0f;
    if (mcMonkey1.rz > 360.0f) mcMonkey1.rz -= 360.0f;
    mcWorld.UpdateObj(&mcMonkey1);

    mcViewer.vSet(mcViewer.mcPos.x,  mcViewer.mcPos.y,  mcViewer.mcPos.z,
                  mcViewer.mcDirA.x, mcViewer.mcDirA.y, mcViewer.mcDirA.z);

    cUiElement_Box3Df::OnTimer(lu32Timediff_ms);
  }
};


cUiElement_Window           mc16GuiWindow02_3Df(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cMyBox3Df                   mc16GuiBox3Df(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, mafzBuffer);

class cMyButton: public cUiElement_Button
{
  public:

    cMyButton(cMsgBox *lcMsgBox, cScreen *lpcScreen, uint32 lui32MaxCharacterCount, cRFont* lpcRFont)
      : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont)
    {
      this->mu32BaseCol = 0x101080;
    }

    virtual void OnPressEnd() override
    {
      switch (mc16GuiText01.menTextFitMode)
      {
        case cUiElement_TextBox::nFit:         mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitSmart;break;
        case cUiElement_TextBox::nFitSmart:    mc16GuiText01.menTextFitMode = cUiElement_TextBox::nNoFit;break;
        case cUiElement_TextBox::nNoFit:       mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitRev;break;
        case cUiElement_TextBox::nFitRev:      mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFitSmartRev;break;
        case cUiElement_TextBox::nFitSmartRev: mc16GuiText01.menTextFitMode = cUiElement_TextBox::nNoFitRev;break;
        case cUiElement_TextBox::nNoFitRev:    mc16GuiText01.menTextFitMode = cUiElement_TextBox::nFit;break;
      }

      if (mc16GuiBox3D_Cube.mboWireFrame)
      {
        mc16GuiBox3D_Cube.mboWireFrame   = False;
        mc16GuiBox3D_Monkey.mboWireFrame = False;
        mc16GuiBox3D_Sphere.mboWireFrame = False;
        mc16GuiBox3D_Torus.mboWireFrame  = False;
      }
      else
      {
        mc16GuiBox3D_Cube.mboWireFrame   = True;
        mc16GuiBox3D_Monkey.mboWireFrame = True;
        mc16GuiBox3D_Sphere.mboWireFrame = True;
        mc16GuiBox3D_Torus.mboWireFrame  = True;
      }


      if (mc16GuiBox3Df.mcpViewer == &mc16GuiBox3Df.mcViewerTexGradZ)
      {
        mc16GuiBox3Df.mcpViewer = &mc16GuiBox3Df.mcViewerFlat;
      }
      else
      {
        mc16GuiBox3Df.mcpViewer = &mc16GuiBox3Df.mcViewerTexGradZ;
      }

      mc16GuiRoot.vRepaint();
    }
};


cMyButton                   mc16GuiBtn01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);

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

#if 0

void DMA1_Channel1_IRQHandler(void)
{
}

void DMA1_Channel2_IRQHandler(void)
{
}

void DMA1_Channel3_IRQHandler(void)
{

}

void DMA1_Channel4_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    /* Disable the DMA1 Channel 4 */
    DMA_Cmd(DMA1_Stream4, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    /* Disable the DMA1 Channel 5 */
    DMA_Cmd(DMA1_Stream5, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
  }
}

void DMA1_Channel6_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Tx

  //if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
  {
    /* Disable the DMA1 Channel 6 */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
  }
}

void DMA1_Channel7_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Rx
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    /* Disable the DMA1 Channel 7 */
    DMA_Cmd(DMA1_Stream7, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
  }
}


void I2C1_EV_IRQHandler(void)
{
  mcI2C1->I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1->I2C_ER_IRQHandler();
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}
#endif

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
}

/*
static void Error_Handler(void)
{
  while (1)
  {
  }
}*/


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



void MAIN_vTick10msLp(void)
{
  static i16 li16LValue = 0;
  static i16 li16Value = 0;
  static uint8 lui8t = 0;

  TP_STATE *lstpCursor;

  //mcI2C1->bStartNext();

  lstpCursor = IOE_TP_GetState();

  if (lstpCursor->TouchDetected)
    mcCursor.vTick10ms(lstpCursor->X, lstpCursor->Y, 1);
  else
    mcCursor.vTick10ms(lstpCursor->X, lstpCursor->Y, 0);


  if (lui8t == 4)
  {
    cMsg lcMsg;

    mcDiffTimer.start();
    lcMsg.mID         =  cMsg::tenMsgIDs::nTimestamp;
    lcMsg.mSubID      =  0;
    lcMsg.mui8Data[7] = 25;
    mcMsgBox.vput(&lcMsg);

    while (!mcMsgBox.isEmpty())
    {
      mcMsgBox.vget(&lcMsg);
      mc16GuiRoot.vMsgHdl(&lcMsg);
    }

    li16Value  = (li16LValue + (rand() % 100)) / 2;
    li16LValue = li16Value;
    mcChnYA.vAdd(li16Value);
    mc16GuiGraphY.vRepaint();

    mc16GuiRoot.vPaint();

    mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
    cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);

    mc16GBm.vCopy(mc16GDisplay);
  }
  else
  {
    lui8t++;
  }
}


void MAIN_vTick1000msLp(void)
{
  static uint8 lui8t = 0;
  mcLedRed.Toggle();
  lui8t++;
}


#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{

 // Construct function
  SystemInit();

  mcClkInfo.Update();

  SDRAM_Init();

  mc16GDisplay.vFill(BM_BPP16_5R6G5B_GREEN);
  ILI9341_Init(&mc16GDisplay);

  IOE_Config();

  mcChnXYA.vAddXY(10, 10);
  mcChnXYA.vAddXY(20, 20);
  mcChnXYA.vAddXY(30, 10);
  mcChnXYA.vAddXY(40, 10);
  mcChnXYA.vAddXY(50, 10);
  mcChnXYA.vAddXY(60, 20);
  mcChnXYA.vAddXY(70, 20);
  mcChnXYA.vAddXY(80, 10);
  mcChnXYA.mui32Col = 0xFF0000;

  mcChnXYB.vAddXY(10, 30);
  mcChnXYB.vAddXY(20, 20);
  mcChnXYB.vAddXY(30, 30);
  mcChnXYB.vAddXY(40, 40);
  mcChnXYB.vAddXY(50, -50);
  mcChnXYB.vAddXY(60, 60);
  mcChnXYB.vAddXY(70, 90);
  mcChnXYB.vAddXY(80, 80);
  mcChnXYB.mui32Col = 0x00FF00;

  mc16GuiRoot.bAdd(10, 10, 50, 50, &mc16GuiBase01);
  mc16GuiBase01.bAdd(10, 10, 20, 20, &mc16GuiText01);

  mc16GuiRoot.bAdd(70, 70, 50, 50, &mc16GuiBase02);
  mc16GuiBase02.bAdd(10, 10, 30, 10, &mc16GuiBtn01);

  mc16GuiText01.vSetText("Text acbfgr-tdfgd fdgdee 2134");

  mc16GuiRoot.bAdd(60, 40, 150, 100, &mc16GuiWindow01);
  mc16GuiWindow01.bAdd(20, 20, 150, 100, &mc16GuiGraphXY);
  mc16GuiGraphXY.vToggleMaximize();

  mc16GuiRoot.bAdd(60, 40, 60, 50, &mc16GuiWindow02);
  mc16GuiWindow02.bAdd(20, 20, 150, 100, &mc16GuiGraphY);
  mc16GuiGraphY.vToggleMaximize();


  mc16GuiRoot.bAdd(20, 15, 60, 60, &mc16GuiBox3D_Cube);
  mc16GuiRoot.bAdd(20, 15, 60, 60, &mc16GuiBox3D_Torus);
  mc16GuiRoot.bAdd(20, 15, 60, 60, &mc16GuiBox3D_Sphere);
  mc16GuiRoot.bAdd(20, 15, 60, 60, &mc16GuiBox3D_Monkey);

  mc16GuiBox3D_Cube.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Torus.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Sphere.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GuiBox3D_Monkey.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);


  mc16GuiRoot.bAdd(100, 15, 60, 60, &mc16GuiWindow02_3Df);
  mc16GuiWindow02_3Df.bAdd(20, 15, 60, 60, &mc16GuiBox3Df);
  mc16GuiBox3Df.vToggleMaximize();
  mc16GuiBox3Df.mstStatus.PaintFrame = 0;

  // This is done later, because static data were still only 0 ?
  mc16GuiBox3Df.mcWorld.AddObj(&mc16GuiBox3Df.mcCube1);
  mc16GuiBox3Df.mcWorld.AddObj(&mc16GuiBox3Df.mcCube2);
  mc16GuiBox3Df.mcWorld.AddObj(&mc16GuiBox3Df.mcTorus1);
  mc16GuiBox3Df.mcWorld.AddObj(&mc16GuiBox3Df.mcSphere1);
  mc16GuiBox3Df.mcWorld.AddObj(&mc16GuiBox3Df.mcMonkey1);

  #ifndef _WIN32

    CycCall_Start(NULL /*1ms_HP*/,
                  NULL /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
  #endif
}

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    __asm("wfi");
  }
}

