

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

i16                                 mstChnYAPts[64];
ciGraphY_Channel                    mcChnYA(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYAPts, 64, null);


cUiEleTree                          mcUi16GTree(30);

cUiElement_Box3D_Cube               mcCube;
cUiElement_Box3D_Torus              mcTorus;
cUiElement_Box3D_Sphere             mcSphere;
cUiElement_Box3D_Monkey             mcMonkey;

cUiElement_Window                   mc16GUi1(30, 30, 260, 180, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_TextBox                  mc16GUi2(120, 115, 40, 40, &mcMsgBox, 128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement_Box3D                    mc16GUi2b1(20, 15, 60, 60, &mcMsgBox, &mcCube);
cUiElement_Box3D                    mc16GUi2b2(20, 15, 60, 60, &mcMsgBox, &mcTorus);
cUiElement_Box3D                    mc16GUi2b3(20, 15, 60, 60, &mcMsgBox, &mcSphere);
cUiElement_Box3D                    mc16GUi2b4(20, 15, 60, 60, &mcMsgBox, &mcMonkey);

cUiElement_Window                   mc16GUi3( 60, 40, 150, 100, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphXYBox              mc16GUi3a(20, 15, 250, 200, &mcMsgBox, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnXYA);

cUiElement_Window                   mc16GUi5(60, 40, 150, 100, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement_iGraphYBox               mc16GUi5a(20, 15, 250, 200, &mcMsgBox, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcChnYA);


cUiElement_Window                   mc16GUi4( 60, 40, 60, 50, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_Full);
cUiElement                          mc16GUi4a( 0, 10, 10, 10, &mcMsgBox);

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
        switch (mc16GUi2.menTextFitMode)
        {
          case cUiElement_TextBox::nFit:         mc16GUi2.menTextFitMode = cUiElement_TextBox::nFitSmart;break;
          case cUiElement_TextBox::nFitSmart:    mc16GUi2.menTextFitMode = cUiElement_TextBox::nNoFit;break;
          case cUiElement_TextBox::nNoFit:       mc16GUi2.menTextFitMode = cUiElement_TextBox::nFitRev;break;
          case cUiElement_TextBox::nFitRev:      mc16GUi2.menTextFitMode = cUiElement_TextBox::nFitSmartRev;break;
          case cUiElement_TextBox::nFitSmartRev: mc16GUi2.menTextFitMode = cUiElement_TextBox::nNoFitRev;break;
          case cUiElement_TextBox::nNoFitRev:    mc16GUi2.menTextFitMode = cUiElement_TextBox::nFit;break;
        }
    }
};

cMyButton                   mc16GUi4b(30, 10, 30, 10, &mcMsgBox, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase);


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

  mcUi16GTree.vAdd(0, &mc16GUi1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b1);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b2);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b3);
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi2b4);
  
  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi3);
  mcUi16GTree.vAdd(&mc16GUi3, &mc16GUi3a);
  mc16GUi3a.mstStatus.DragAble  = 0;
  mc16GUi3a.mstStatus.Maximized = 1;
  mcUi16GTree.vUpdateMaximize(&mc16GUi3a);

  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi5);
  mcUi16GTree.vAdd(&mc16GUi5, &mc16GUi5a);
  mc16GUi5a.mstStatus.DragAble = 0;
  mc16GUi5a.mstStatus.Maximized = 1;
  mc16GUi5a.mi16SkalXMax = 70;
  mcUi16GTree.vUpdateMaximize(&mc16GUi5a);


  mcUi16GTree.vAdd(&mc16GUi1, &mc16GUi4);
  mcUi16GTree.vAdd(&mc16GUi4, &mc16GUi4a);
  mcUi16GTree.vAdd(&mc16GUi4, &mc16GUi4b);

  mc16GUi2b1.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GUi2b2.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GUi2b3.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);
  mc16GUi2b4.mu32BaseCol = BM_BPP32_8R8G8B_RGB(128, 128, 128);

  mc16GUi2.vSetText((const char8*)"TEXT aaaa\naaaaaaaa bbbbbbbb 1234 2333 abc xyz bla bla bla");  
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
    mcUi16GTree.vMsgHdl(&lcMsg, &mc16GScreen1);
  }


  mcUi16GTree.vFill(BM_BPP32_8R8G8B_RGB(0xFF,0,0), &mc16GScreen1);
  //mc16GBm.vPp(160, 100, 1);
  //mc16GScreen1.vLine(0, 0, 319, 199, BM_BPP16_5R6G5B_RGB(0, 0xFF, 0));
  //mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
  //mc16GSpriteEng.i8Set(-10, -10, SPRTMST_DATA_MainBottom);
  
  mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);

  char8  lszData[100] = "";

  cStrTools::i8Ftoan(1.25f, lszData, 100, 2);

  cStr   lcStr(lszData, 100);
  lcStr.Setf((const char8*)"Test %d Test %x", 1234, 127);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 20, &lcStr, &mc16GScreen1);

  li16Value  = (li16LValue + (rand() % 100)) / 2;
  li16LValue = li16Value;
  mcChnYA.vAdd(li16Value);

  mcUi16GTree.vPaint(&mc16GScreen1);

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


