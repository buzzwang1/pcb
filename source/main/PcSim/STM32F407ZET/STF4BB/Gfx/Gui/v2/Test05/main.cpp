

#include "windows.h"
#include <time.h>
#include <stdlib.h>

#include "main.h"

#define DISPLAY_X 320
#define DISPLAY_Y 240


#ifdef WIN32

uint8 mui8Zoom = 2;

UINT      Timer_10ms;
UINT      Timer_25ms;
int fps;
#define ID_TIMER_10ms 1
#define ID_TIMER_25ms 2

int16  MousexPos;
int16  MouseyPos;
uint16 MousePressed;


cWin32Display                      *mpcDisplay1;

uint8 maui8KeyState[6];
#else
Screen mstDisplay;     // 120 *176 * 1Bit / Pixel
#endif

cMsgBox                             mcMsgBox(20);
cCursor                             mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
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

cUiElement_Root                     mc16GuiRoot(10, 10, DISPLAY_X-20, DISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));
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
    : mcCubeTemplate(0),
      mcTorusTemplate(0),
      mcSphereTemplate(0),
      mcMonkeyTemplate(0),
      mcCube1(   -9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcCube2(    9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcTorus1(  20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcTorusTemplate),
      mcSphere1(-20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcSphereTemplate),
      mcMonkey1(-80.0f, 4.0f,  -50.0f, 90.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcMonkeyTemplate),
      mcWorld(500, 500, 30),
      mcViewerTexGradZ(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, mcTextures, lafzBuffer, lpcRFont),
      mcViewerFlat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, lpcRFont),
      cUiElement_Box3Df(lcMsgBox, lpcScreen, &mcWorld, (cWorld_3Df_ViewerWorld_Base*)&mcViewerTexGradZ)
  {
    for (int i = 0; i < 8; i++)
    {
      mcTextures[i].Test_Pattern_2x2(BM_BPP16_5R6G5B_RED, BM_BPP16_5R6G5B_BLUE, BM_BPP16_5R6G5B_WHITE);
    }
    mcTextures[0].Test_Pattern_Flat(BM_BPP16_5R6G5B_RGB(15, 31, 15));

    mcWorld.AddObj(&mcCube1);
    mcWorld.AddObj(&mcCube2);
    mcWorld.AddObj(&mcTorus1);
    mcWorld.AddObj(&mcSphere1);
    mcWorld.AddObj(&mcMonkey1);
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


    if (GetAsyncKeyState(L'S')) mcViewer.mcPos = mcViewer.mcPos.cAdd(mcViewer.mcLookDir);
    if (GetAsyncKeyState(L'W')) mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);

    if (GetAsyncKeyState(L'A')) mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookSideDir);
    if (GetAsyncKeyState(L'D')) mcViewer.mcPos = mcViewer.mcPos.cAdd(mcViewer.mcLookSideDir);

    if (GetAsyncKeyState(VK_DOWN))  mcViewer.mcDirA.x -= 1;
    if (GetAsyncKeyState(VK_UP))    mcViewer.mcDirA.x += 1;

    if (GetAsyncKeyState(VK_LEFT))  mcViewer.mcDirA.y += 1;
    if (GetAsyncKeyState(VK_RIGHT)) mcViewer.mcDirA.y -= 1;

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


float                       mafzBuffer[320 * 240];
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
        mc16GuiBox3D_Cube.mboWireFrame = False;
        mc16GuiBox3D_Monkey.mboWireFrame = False;
        mc16GuiBox3D_Sphere.mboWireFrame = False;
        mc16GuiBox3D_Torus.mboWireFrame = False;
      }
      else
      {
        mc16GuiBox3D_Cube.mboWireFrame = True;
        mc16GuiBox3D_Monkey.mboWireFrame = True;
        mc16GuiBox3D_Sphere.mboWireFrame = True;
        mc16GuiBox3D_Torus.mboWireFrame = True;
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




#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{
  #ifdef _WIN32
  mpcDisplay1 = new cWin32Display(0, 0,                        DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  #endif

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
}

void CycCall_vMain_10ms()
{
  mcCursor.vTick10ms(MousexPos, MouseyPos, (uint8)MousePressed);
}

void CycCall_vMain_25ms()
{
  static i16 li16LValue = 0;
  static i16 li16Value = 0;

  cMsg lcMsg;

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

  cBitmapConvert_Bpp32_8R8G8B::i8FromBpp16_5R6G5B(mc16GBm, mpcDisplay1->mcBm);
  mpcDisplay1->Show();
}


/* Main functions ---------------------------------------------------------*/

LRESULT CALLBACK WindowProc(HWND   hWnd, UINT   message,
  WPARAM wParam, LPARAM lParam)
{

  HDC hDC;
  PAINTSTRUCT ps;

  switch (message)
  {
    case WM_CREATE:
      {
        MAIN_vInitSystem(hWnd);

        Timer_10ms = (UINT)SetTimer(hWnd, ID_TIMER_10ms, 10, 0); // 10ms
        Timer_25ms = (UINT)SetTimer(hWnd, ID_TIMER_25ms, 25, 0); // 25ms
      }
      break;

    case WM_PAINT:
      hDC = BeginPaint(hWnd, &ps);
      mpcDisplay1->Show();
      EndPaint(hWnd, &ps);
      break;
    case WM_CLOSE:
      KillTimer(hWnd, Timer_10ms);
      KillTimer(hWnd, Timer_25ms);
      delete mpcDisplay1;
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_CHAR:
      //Taste = (TCHAR) wParam;
      //TasteData = lParam;    
      break;


    case WM_KEYDOWN:
      {
        uint8 lpui8Taste[2];
        uint8 lui8Result;
        uint8 lpui8GetKeyboardState[256];

        GetKeyboardState(lpui8GetKeyboardState);

        lui8Result = ToAscii((uint8)wParam,
          MapVirtualKey((uint8)wParam, 0),
          lpui8GetKeyboardState,
          (LPWORD)lpui8Taste, 0);

        if (lui8Result == 1)
        {
          if (lpui8Taste[0] == '1') maui8KeyState[0] = 1;
          if (lpui8Taste[0] == '2') maui8KeyState[1] = 1;
          if (lpui8Taste[0] == '3') maui8KeyState[2] = 1;
          if (lpui8Taste[0] == '4') maui8KeyState[3] = 1;
          if (lpui8Taste[0] == 'p') maui8KeyState[4] = 1;
          if (lpui8Taste[0] == 'd') maui8KeyState[5] = 1;
        }
      }
      break;

    case WM_KEYUP:
      {
        uint8 lpui8Taste[2];
        uint8 lui8Result;
        uint8 lpui8GetKeyboardState[256];

        GetKeyboardState(lpui8GetKeyboardState);

        lui8Result = ToAscii((uint8)wParam,
          MapVirtualKey((uint8)wParam, 0),
          lpui8GetKeyboardState,
          (LPWORD)lpui8Taste, 0);

        if (lui8Result == 1)
        {
          if (lpui8Taste[0] == '1') maui8KeyState[0] = 0;
          if (lpui8Taste[0] == '2') maui8KeyState[1] = 0;
          if (lpui8Taste[0] == '3') maui8KeyState[2] = 0;
          if (lpui8Taste[0] == '4') maui8KeyState[3] = 0;
          if (lpui8Taste[0] == 'p') maui8KeyState[4] = 0;
          if (lpui8Taste[0] == 'd') maui8KeyState[5] = 0;
        }
      }
      break;

    /*
    case WM_RBUTTONUP:
    case WM_RBUTTONDOWN:*/

    case WM_LBUTTONUP:
      MousePressed = 0;
      break;
    case WM_LBUTTONDOWN:
      MousePressed = 1;
      break;

    case WM_MOUSEMOVE: //Maus Position relative zum Fenster
      {
        uint8 fwKeys;
        POINT lstPoint;
        cMsg  lcMsg;

        GetCursorPos(&lstPoint);
        ScreenToClient(hWnd, &lstPoint);

        fwKeys = (uint8)wParam;    // key flags 
        MousexPos = (int16)lstPoint.x/2;  // horizontal position of cursor 
        MouseyPos = (int16)lstPoint.y/2;  // vertical position of cursor
      }
      break;

    case WM_TIMER:
      {
        if (wParam == ID_TIMER_10ms)
        {
          CycCall_vMain_10ms();
        }
        else
        {
          CycCall_vMain_25ms();
        }
      }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}


// Erzeugen des Fensters und anderer Windows spezifischer
// Initialisierungen
BOOL MakeWindow(int nCmdShow, HINSTANCE instance, int x1,int y1,int width,int height)
{
    HWND         hwnd;
    // zunächst wird eine eigene Window-Klasse für 
    // unsere Ausgabe erzeugt.
    WNDCLASS        wc;
    unsigned long   style;
    unsigned long   styleex;

    wc.style         = CS_BYTEALIGNCLIENT;
    wc.lpfnWndProc   = WindowProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance;
    wc.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor (NULL, IDC_ARROW);

    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "demo";
    RegisterClass( &wc );

              
    style     = WS_OVERLAPPED | WS_SYSMENU;
    styleex   = 0;
 
    hwnd       =  CreateWindowEx( styleex , "demo", "Applikation",
                                  style   , x1, y1, x1+width + 20,
                                  y1+height + 20, 0, 0, instance, 0);

    
    if ( !hwnd ) return 0;

    ShowWindow( hwnd, nCmdShow );  // Fenster anzeigen
    UpdateWindow(hwnd);

    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    //THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_HIGHEST
    return TRUE;
}

// Hauptprogramm
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst,
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG         message;


    // Erzeugt ein Fenster
    if ( !MakeWindow( nCmdShow, hInstance,0,0,DISPLAY_X * mui8Zoom + 10, DISPLAY_Y * mui8Zoom + 30) ) return 0;

    // https://stackoverflow.com/questions/33948837/win32-application-with-high-cpu-usage
    /*
    while (TRUE)
    {
        if ( PeekMessage( &message, NULL, 0, 0,PM_REMOVE ) )
        {
            if (message.message == WM_QUIT) return (int)message.wParam;
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }*/

    while (GetMessage(&message, NULL, 0, 0))
    {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
    
    return (int) message.wParam;
}


