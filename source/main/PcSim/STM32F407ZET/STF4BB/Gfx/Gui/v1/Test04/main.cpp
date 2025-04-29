

#include "windows.h"

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


cMsgBox                                  mcMsgBox(20);
cCursor                                  mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

cUiEleTree                          mcUi16GTree(25);

Simple3Df_Tex_Light_ObjTemplate_Cube    cCubeTemplate(0);
Simple3Df_Tex_Light_ObjTemplate_Torus   cTorusTemplate(0);
Simple3Df_Tex_Light_ObjTemplate_Sphere  cSphereTemplate(0);
Simple3Df_Tex_Light_ObjTemplate_Monkey  cMonkeyTemplate(0);

cSimple3Df_Obj cCube1(   -9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&cCubeTemplate);
cSimple3Df_Obj cCube2(    9.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&cCubeTemplate);
cSimple3Df_Obj cTorus1(  20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&cTorusTemplate);
cSimple3Df_Obj cSphere1(-20.0f, 4.0f, -100.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&cSphereTemplate);
cSimple3Df_Obj cMonkey1(-80.0f, 4.0f,  -50.0f, 90.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&cMonkeyTemplate);

float          mafzBuffer[320 * 240];

cTexture32_Bpp16_5R6G5B mcTextures[8];

cWorld_3Df                   cWorld(500, 500, 30);

cWorld_3Df_Viewer            cViewer;
cWorld_3Df_Viewer_Tex_Grad_Z cViewerTexGradZ(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &cViewer, &cWorld, mcTextures, mafzBuffer, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cWorld_3Df_Viewer_Flat       cViewerFlat(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, &cViewer, &cWorld, &cRFont_Res8b_Bpp1_1G_5x5Ucase);

cUiElement_Window  mc16GUi1(30, 30, 260, 180, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_Box3Df  mc16GUi2b4(0, 0, 160, 100, &mcMsgBox, &cWorld, (cWorld_3Df_ViewerWorld_Base*)&cViewerTexGradZ);

class cMyButton: public cUiElement_Button
{
  public:

    cMyButton(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cMsgBox *lcMsgBox,
              uint32 lui32MaxCharacterCount,
              cRFont* lpcRFont)
      : cUiElement_Button(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcMsgBox,
                          lui32MaxCharacterCount,
                          lpcRFont)
    {
    }

    virtual void vPressEnd() override
    {
      if (mc16GUi2b4.cViewer == &cViewerTexGradZ)
      {
        mc16GUi2b4.cViewer = &cViewerFlat;
      }
      else
      {
        mc16GUi2b4.cViewer = &cViewerTexGradZ;
      }
    }
};

cMyButton                   mc16GUiButton01(5, 40, 30, 10, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);

void CycCall_vMain_10ms()
{
  mcCursor.vTick10ms(MousexPos, MouseyPos, (uint8)MousePressed);
}

void CycCall_vMain_25ms()
{
  static int liTextureCnt = 0;
  int liTextureCnt2 = 0;
  static u8  lu8T100ms = 0;

  static int liViewerState = 0;
  static int liViewerCounter = 0;

  cMsg lcMsg;

  lcMsg.mID         =  cMsg::tenMsgIDs::nTimestamp;
  lcMsg.mSubID      =  0;
  lcMsg.mui8Data[7] = 25;
  mcMsgBox.vput(&lcMsg);

  while (!mcMsgBox.isEmpty())
  {
    mcMsgBox.vget(&lcMsg);
    mcUi16GTree.vMsgHdl(&lcMsg, &mc16GScreen1);
  }

  
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
  cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2*liTextureCnt2,31 - 2*liTextureCnt2, 0x0FF0000, &lcTexScreen);

  liTextureCnt2 += 4;
  liTextureCnt2 &= 15;
  cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2*liTextureCnt2,31 - 2*liTextureCnt2, 0x000FF00, &lcTexScreen);

  liTextureCnt2 += 4;
  liTextureCnt2 &= 15;
  cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2*liTextureCnt2,31 - 2*liTextureCnt2, 0x00000FF, &lcTexScreen);

  liTextureCnt2 += 4;
  liTextureCnt2 &= 15;
  cPaint::vRect(liTextureCnt2, liTextureCnt2, 31 - 2*liTextureCnt2,31 - 2*liTextureCnt2, 0x0FF00FF, &lcTexScreen);


  mcUi16GTree.vFill(BM_BPP32_8R8G8B_RGB(0xFF,0,0), &mc16GScreen1);
  
  //mc16GBm.vPp(160, 100, 1);
  //mc16GScreen1.vLine(0, 0, 319, 199, BM_BPP16_5R6G5B_RGB(0, 0xFF, 0));
  //mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
  //mc16GSpriteEng.i8Set(-10, -10, SPRTMST_DATA_MainBottom);
  
  mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);


  if (GetAsyncKeyState(L'S')) cViewer.mcPos = cViewer.mcPos.cAdd(cViewer.mcLookDir);
  if (GetAsyncKeyState(L'W')) cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookDir);

  if (GetAsyncKeyState(L'A')) cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookSideDir);
  if (GetAsyncKeyState(L'D')) cViewer.mcPos = cViewer.mcPos.cAdd(cViewer.mcLookSideDir);

  if (GetAsyncKeyState(VK_DOWN))  cViewer.mcDirA.x -= 1;
  if (GetAsyncKeyState(VK_UP))    cViewer.mcDirA.x += 1;

  if (GetAsyncKeyState(VK_LEFT))  cViewer.mcDirA.y += 1;
  if (GetAsyncKeyState(VK_RIGHT)) cViewer.mcDirA.y -= 1;


  if (mc16GUi2b4.mui8Play)
  {
    switch (liViewerState)
    {
      case 0:
        cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookDir);
        cViewer.mcDirA.z += 360.0f / 70.0f;

        liViewerCounter++;
        if (liViewerCounter >= 70)
        {
          liViewerState++;
          liViewerCounter = 0;
          cViewer.mcDirA.z = 0.0f;
        }
        break;
      case 1:
        cViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 2:
        cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 40)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 3:
        cViewer.mcDirA.x += 1;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 4:
        cViewer.mcDirA.x -= 1;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 5:
        cViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 6:
        cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 70)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 7:
        cViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 8:
        cViewer.mcPos = cViewer.mcPos.cSub(cViewer.mcLookDir);
        liViewerCounter++;
        if (liViewerCounter >= 40)
        {
          liViewerState++;
          liViewerCounter = 0;
        }
        break;
      case 9:
        cViewer.mcDirA.y += 2;
        liViewerCounter++;
        if (liViewerCounter >= 45)
        {
          cViewer.mcDirA.y = 0;
          liViewerState = 0;
          liViewerCounter = 0;
        }
        break;
    }
  }

  /*cCube1.rx += 3.0f;
  if (cCube1.rx > 360) cCube1.rx -= 360.0f;
  cCube1.ry += 3.0f;
  if (cCube1.ry > 360) cCube1.ry -= 360.0f;
  cWorld.UpdateObj(&cCube1);

  cCube2.rz += 3.0f;
  if (cCube2.rz > 360.0f) cCube2.rz -= 360.0f;
  cWorld.UpdateObj(&cCube2);*/

  cTorus1.rx += 3.0f;
  if (cTorus1.rx > 360.0f) cTorus1.rx -= 360.0f;
  cWorld.UpdateObj(&cTorus1);

  /*cSphere1.rx += 3.0f;
  if (cSphere1.rx > 360.0f) cSphere1.rx -= 360.0f;
  cWorld.UpdateObj(&cSphere1);*/

  cMonkey1.rz += 2.0f;
  if (cMonkey1.rz > 360.0f) cMonkey1.rz -= 360.0f;
  cWorld.UpdateObj(&cMonkey1);

  cViewer.vSet(cViewer.mcPos.x,  cViewer.mcPos.y,  cViewer.mcPos.z,
               cViewer.mcDirA.x, cViewer.mcDirA.y, cViewer.mcDirA.z);

  mcUi16GTree.vPaint(&mc16GScreen1);

  cBitmapConvert_Bpp32_8R8G8B::i8FromBpp16_5R6G5B(mc16GBm, mpcDisplay1->mcBm);

  mpcDisplay1->Show();
}
#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{

  #ifdef _WIN32
    mpcDisplay1 = new cWin32Display(0, 0,                        DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  #else
    // Construct function
    SystemInit();

    mcClkInfo.Update();

    FSMC_SRAM_Init();
    ILI9341_Init();
    TP_Init();
  #endif

  mcUi16GTree.vAdd(0, &mc16GUi1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b4);
  mcUi16GTree.vAdd(&mc16GUi2b4, &mc16GUiButton01);
  
  for (int i = 0; i < 8; i++)
  {
    mcTextures[i].Test_Pattern_2x2(BM_BPP16_5R6G5B_RED, BM_BPP16_5R6G5B_BLUE, BM_BPP16_5R6G5B_WHITE);
  }
  mcTextures[0].Test_Pattern_Flat(BM_BPP16_5R6G5B_RGB(15, 31, 15));


  cWorld.AddObj(&cCube1);
  cWorld.AddObj(&cCube2);
  cWorld.AddObj(&cTorus1);
  cWorld.AddObj(&cSphere1);
  cWorld.AddObj(&cMonkey1);

  #ifndef _WIN32
    mcLedRed = new LED(RCC_AHB1Periph_GPIOF, GPIOF, GPIO_Pin_9);

    CycCall_Start(NULL /*500us_HP*/,
                  NULL /*1ms_HP*/,
                  NULL /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*500us_LP*/,
                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
  #endif
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
    if ( !MakeWindow( nCmdShow, hInstance, 0, 0, DISPLAY_X * mui8Zoom + 10, DISPLAY_Y * mui8Zoom + 30) ) return 0;

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


