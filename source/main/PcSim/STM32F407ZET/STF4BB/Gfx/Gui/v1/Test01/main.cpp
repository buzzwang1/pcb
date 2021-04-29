

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
cWin32Display                      *mpcDisplay2;

uint8 maui8KeyState[6];
#else
Screen mstDisplay;     // 120 *176 * 1Bit / Pixel
#endif

cMsgBox                             mcMsgBox(20);
cCursor                             mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp1_1G                     mc1GBm(DISPLAY_X, DISPLAY_Y);
cScreen_Bpp1_1G                     mc1GScreen1(&mc1GBm);
cSprite_Res8b_Bpp1_1G               mc1GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G mc1GSpriteEng(Sprite_nModeOr);;
cRFont_Res8b_Bpp1_1G                mc1GRFont(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc1GSpriteEng);

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y);
cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);
cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);
cRFont_Res8b_Bpp1_1G                     mc16GRFont(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

cUiElement                          mc1GUi1(30, 30, 120, 90, &mcMsgBox);
cUiElement_TextBox                  mc1GUi2(20, 15,  45, 40, &mcMsgBox, 16, &mc1GRFont);
cUiElement                          mc1GUi3(60, 40, 120, 90, &mcMsgBox);
cUiElement                          mc1GUi4(10,  0,  20, 10, &mcMsgBox);
cUiElement                          mc1GUi5(30, 30,  40, 40, &mcMsgBox);

cUiEleTree                          mcUi1GTree(10);

cUiElement                          mc16GUi1(30, 30, 120, 90, &mcMsgBox);
cUiElement_TextBox                  mc16GUi2(20, 15,  45, 40, &mcMsgBox, 16, &mc16GRFont);
cUiElement                          mc16GUi3(60, 40, 120, 90, &mcMsgBox);
cUiElement                          mc16GUi4(10,  0,  20, 10, &mcMsgBox);
cUiElement                          mc16GUi5(30, 30,  40, 40, &mcMsgBox);

cUiEleTree                          mcUi16GTree(10);

#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{
  #ifdef _WIN32
  mpcDisplay1 = new cWin32Display(0, 0,                        DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  mpcDisplay2 = new cWin32Display(0, 0 + DISPLAY_Y * mui8Zoom, DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  #endif

  mcUi1GTree.vAdd(0, &mc1GUi1);
  mcUi1GTree.vAdd(&mc1GUi1, &mc1GUi2);
  mcUi1GTree.vAdd(&mc1GUi1, &mc1GUi3);
  mcUi1GTree.vAdd(&mc1GUi3, &mc1GUi4);
  mcUi1GTree.vAdd(&mc1GUi3, &mc1GUi5);

  mcUi16GTree.vAdd(0, &mc16GUi1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi3);
  mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi4);
  mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi5);

  mc16GUi2.vSetText("TEXT");
}

void CycCall_vMain_10ms()
{
  mcCursor.vTick10ms(MousexPos, MouseyPos, (uint8)MousePressed);
}

void CycCall_vMain_25ms()
{
  cMsg lcMsg;
  while (!mcMsgBox.isEmpty())
  {
    mcMsgBox.vget(&lcMsg);

    mcUi1GTree.vMsgHdl(&lcMsg,  &mc1GScreen1);
    mcUi16GTree.vMsgHdl(&lcMsg, &mc16GScreen1);
  }


  mc1GBm.vFill(0);
  mc1GBm.vPp(160, 100, 1);
  mc1GScreen1.vLine(0, 0, 319, 199, 1);

  cPaint::vRectFull(-90, 53,
                     89, 59,
                      1, &mc1GScreen1);

  mc1GSpriteEng.vSetParam(Sprite_nModeOr, &mc1GSprite, &mc1GScreen1);
  
  mc1GSpriteEng.i8Set(100, 0, SPRTMST_DATA_MainBottom);

  mcUi1GTree.vPaint(&mc1GScreen1);

  mc1GRFont.i8PutStringXY(0, 10, mcCursor.toString(), &mc1GScreen1);

  cBitmapConvert_Bpp32_8R8G8B::i8FromBpp1_1G(mc1GBm, mpcDisplay1->mcBm);

  mpcDisplay1->Show();


  mc16GBm.vFill(BM_BPP16_5R6G5B_RGB(0xFF,0,0));
  mc16GBm.vPp(160, 100, 1);
  mc16GScreen1.vLine(0, 0, 319, 199, BM_BPP16_5R6G5B_RGB(0, 0xFF, 0));

  mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);

  mc16GSpriteEng.i8Set(-10, -10, SPRTMST_DATA_MainBottom);

  mcUi16GTree.vPaint(&mc16GScreen1);

  cBitmapConvert_Bpp32_8R8G8B::i8FromBpp16_5R6G5B(mc16GBm, mpcDisplay2->mcBm);

  mpcDisplay2->Show();
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
      mpcDisplay2->Show();
      EndPaint(hWnd, &ps);
      break;
    case WM_CLOSE:
      KillTimer(hWnd, Timer_10ms);
      KillTimer(hWnd, Timer_25ms);
      delete mpcDisplay1;
      delete mpcDisplay2;
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
    if ( !MakeWindow( nCmdShow, hInstance,0,0,DISPLAY_X * mui8Zoom + 10, DISPLAY_Y * 2 * mui8Zoom + 30) ) return 0;

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


