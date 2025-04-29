

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

cUiElement_Root                          mc16GuiRoot(10, 10, DISPLAY_X-20, DISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));



class cMyHighMap: public cUiElement
{
  public:

    cHighmapi8 *mcHm; 

    cMyHighMap(cMsgBox *lcMsgBox, cScreen *lpcScreen, cHighmapi8* lcHm)
      : cUiElement(lcMsgBox, lpcScreen)
    {
      mcHm = lcHm;
      //mcHm->vCutSteps(500);
      //mcHm->mcMidStep.vMidPoint(8, 2.5f);
      mcHm->mcCrater.vSetCrater(44, 64, 50);
      mcHm->mcCrater.vSetCrater(84, 64, 50);
    }

    virtual void OnPaint()
    {
      if (isVisible())
      {
        for (u16 lu16Y = 0; lu16Y < mcHm->mu16Height; lu16Y++)
        {
          for (u16 lu16X = 0; lu16X < mcHm->mu16Width; lu16X++)
          {
            i8 li8Val = mcHm->i16Get(lu16X, lu16Y);
            unBmBaseCol lunCol;
            lunCol.stARGB.a = 0;
            lunCol.stARGB.r = li8Val + mcHm->i8GetMinValue();
            lunCol.stARGB.g = li8Val + mcHm->i8GetMinValue();
            lunCol.stARGB.b = li8Val + mcHm->i8GetMinValue();
           
            mcRefScreen->vPp(lu16X, lu16Y, lunCol.u32Col);
          }
        }
      }
    }
};

i8          mai8HighmapArray[129*129];
cHighmapi8  mcHm(129, 129, mai8HighmapArray);
cMyHighMap  mcMyHighMap(&mcMsgBox, &mc16GScreen1, &mcHm);


class cMyBox3Df : public cUiElement_Box3Df
{
public:

  cTexture32_Bpp16_5R6G5B             mcTextures[8];

  Simple3Df_Tex_Light_ObjTemplate_Cube    mcCubeTemplate;
  cSimple3Df_Obj mcCube1;

  cSimple3Df_Face_Data_Tex           mcTestFaceData[129*129*2]; // *2 wegen dreieck
  cSimple3Df_Tex_Light_ObjTemplate   mcTestTemplate;
  cSimple3Df_Obj                     mcTest;

  cWorld_3Df                          mcWorld;

  cWorld_3Df_Viewer                   mcViewer;
  cWorld_3Df_Viewer_Basic_Z           mcViewerBasicZ;

  cHighmapi8                         *mcHm;

  cMyBox3Df(cMsgBox* lcMsgBox, cScreen* lpcScreen, cRFont* lpcRFont, float* lafzBuffer, cHighmapi8  *lcHm)
    : mcCubeTemplate(0),
      mcCube1(   0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcTestTemplate(),
      mcTest(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcTestTemplate),
      mcWorld(128000/*128*128+128*4*/, 129*129*2 + 200, 30),
      mcViewerBasicZ(0.0f, 330.0f, 600.0f, 320.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, lafzBuffer, lpcRFont),
      cUiElement_Box3Df(lcMsgBox, lpcScreen, &mcWorld, (cWorld_3Df_ViewerWorld_Base*)&mcViewerBasicZ)
  {
    mcHm = lcHm;

    for (u16 i = 0; i < 8; i++)
    {
      mcTextures[i].Test_Pattern_2x2(BM_BPP16_5R6G5B_RED, BM_BPP16_5R6G5B_BLUE, BM_BPP16_5R6G5B_WHITE);
    }
    mcTextures[0].Test_Pattern_Flat(BM_BPP16_5R6G5B_RGB(15, 31, 15));

    mcWorld.AddObj(&mcCube1);

    uint16 lui16PlaneY;
    uint16 lui16PlaneX;
    int16  li16OffsetX;
    int16  li16OffsetY;

    uint16 lui16i = mcWorld.mu32PointsCnt;
    uint16 lui16t = 0;

    li16OffsetY = -10 * 64;
    for (lui16PlaneY = 0; lui16PlaneY < 129; lui16PlaneY++)
    {
      li16OffsetX = -10 * 64;
      for (lui16PlaneX = 0; lui16PlaneX < 129; lui16PlaneX++)
      {
        mcWorld.macPoints[lui16i++] = cSimple3Df_Point(li16OffsetX, mcHm->i16Get(lui16PlaneX, lui16PlaneY), li16OffsetY);
        li16OffsetX += 10;
      }
      li16OffsetY += 10;
    }

    lui16i = mcWorld.mu32FacesCnt;
    
    uint16 lui16PntOffset = 0;
    for (lui16PlaneY = 0; lui16PlaneY < 128; lui16PlaneY++)
    {
      for (lui16PlaneX = 0; lui16PlaneX < 128; lui16PlaneX++)
      {
        unBmBaseCol lunCol;
        i8 li8Val = mcHm->i16Get(lui16PlaneX, lui16PlaneY);
        lunCol.stARGB.a = 0;
        lunCol.stARGB.r = li8Val + mcHm->i8GetMinValue();
        lunCol.stARGB.g = li8Val + mcHm->i8GetMinValue();
        lunCol.stARGB.b = li8Val + mcHm->i8GetMinValue();
           
        
        lui16PntOffset = lui16PlaneY * 129 + lui16PlaneX;
        mcWorld.macFaces[lui16i].mau16PntIdx[0] = mcWorld.mu32PointsCnt + 0 +   0 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mau16PntIdx[1] = mcWorld.mu32PointsCnt + 1 +   0 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mau16PntIdx[2] = mcWorld.mu32PointsCnt + 1 + 129 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mpcObjRef = &mcTest; 
        mcTestFaceData[lui16t] = cSimple3Df_Face_Data_Tex(lunCol.u32Col, 31, 0, 31, 31, 0, 31, 0);
        lui16i++;
        lui16t++;
        mcWorld.macFaces[lui16i].mau16PntIdx[0] = mcWorld.mu32PointsCnt + 0 +   0 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mau16PntIdx[1] = mcWorld.mu32PointsCnt + 1 + 129 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mau16PntIdx[2] = mcWorld.mu32PointsCnt + 0 + 129 + lui16PntOffset;
        mcWorld.macFaces[lui16i].mpcObjRef = &mcTest;
        mcTestFaceData[lui16t] = cSimple3Df_Face_Data_Tex(lunCol.u32Col, 0, 31, 0, 0, 31, 0, 0);
        lui16i++;
        lui16t++;
      }
    }

    mcTestTemplate.mu16PointCnt = 129 * 129;
    mcTestTemplate.mu16FaceCnt  = lui16t;
    mcTestTemplate.mcFaces_Data = mcTestFaceData;
    mcTest.cRefTemplateObj = &mcTestTemplate;

    mcWorld.mu32PointsCnt += mcTestTemplate.mu16PointCnt;
    mcWorld.mu32FacesCnt  += mcTestTemplate.mu16FaceCnt;
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

    //---------------------------------------------------------------


    if (GetAsyncKeyState(L'S')) mcViewer.mcPos = mcViewer.mcPos.cAdd(mcViewer.mcLookDir);
    if (GetAsyncKeyState(L'W')) mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookDir);

    if (GetAsyncKeyState(L'A')) mcViewer.mcPos = mcViewer.mcPos.cSub(mcViewer.mcLookSideDir);
    if (GetAsyncKeyState(L'D')) mcViewer.mcPos = mcViewer.mcPos.cAdd(mcViewer.mcLookSideDir);

    if (GetAsyncKeyState(VK_DOWN))  mcViewer.mcDirA.x -= 1;
    if (GetAsyncKeyState(VK_UP))    mcViewer.mcDirA.x += 1;

    if (GetAsyncKeyState(VK_LEFT))  mcViewer.mcDirA.y += 1;
    if (GetAsyncKeyState(VK_RIGHT)) mcViewer.mcDirA.y -= 1;

    mcViewer.vSet(mcViewer.mcPos.x,  mcViewer.mcPos.y,  mcViewer.mcPos.z,
                  mcViewer.mcDirA.x, mcViewer.mcDirA.y, mcViewer.mcDirA.z);

    cUiElement_Box3Df::OnTimer(lu32Timediff_ms);
  }
};


float                       mafzBuffer[320 * 240] = {0};
cUiElement_Window           mc16GuiWindow02_3Df(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cMyBox3Df                   mc16GuiBox3Df(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, mafzBuffer, &mcHm);



#ifdef _WIN32
void MAIN_vInitSystem(HWND hWnd)
#else
void MAIN_vInitSystem(void)
#endif
{
  #ifdef _WIN32
  mpcDisplay1 = new cWin32Display(0, 0,                        DISPLAY_X, DISPLAY_Y, mui8Zoom, hWnd);
  #endif

  mc16GuiRoot.bAdd(15, 15, 240, 200, &mc16GuiWindow02_3Df);
  mc16GuiWindow02_3Df.bAdd(20, 15, 60, 60, &mc16GuiBox3Df);
  mc16GuiBox3Df.vToggleMaximize();
  mc16GuiBox3Df.mstStatus.PaintFrame = 0;

  mc16GuiRoot.bAdd(0, 0, 150, 150, &mcMyHighMap);
}

void CycCall_vMain_10ms()
{
  mcCursor.vTick10ms(MousexPos, MouseyPos, (uint8)MousePressed);
}

void CycCall_vMain_25ms()
{
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

  //li16Value  = (li16LValue + (rand() % 100)) / 2;

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


