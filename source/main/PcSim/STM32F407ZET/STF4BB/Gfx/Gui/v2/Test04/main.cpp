

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


class cMyButton: public cUiElement_Button
{
  public:

    cMyButton(cMsgBox *lcMsgBox, cScreen *lpcScreen, uint32 lui32MaxCharacterCount, cRFont* lpcRFont)
      : cUiElement_Button(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont) {}

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
}

void CycCall_vMain_10ms()
{
  mcCursor.vTick10ms(MousexPos, MouseyPos, (uint8)MousePressed);
}

void CycCall_vMain_25ms()
{
  cMsg lcMsg;
  static i16 li16LValue = 0;
  static i16 li16Value = 0;

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
  mc16GuiRoot.vRepaint();

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


