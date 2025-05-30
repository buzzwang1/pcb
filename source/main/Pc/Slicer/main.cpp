

#include "windows.h"
#include <time.h>
#include <stdlib.h>
#include <io.h>

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


cUiElement_Root                     mc16GuiRoot(10, 10, DISPLAY_X-20, DISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));
cUiElement                          mc16GuiBase01(&mcMsgBox, &mc16GScreen1);
cUiElement_TextBox                  mc16GuiText01(&mcMsgBox, &mc16GScreen1, 128, &cRFont_Res8b_Bpp1_1G_5x5Ucase);
cUiElement                          mc16GuiBase02(&mcMsgBox, &mc16GScreen1);



class cStl
{
  public: 
    typedef struct
    {
      float Norm[3];
      float V1[3];
      float V2[3];
      float V3[3];
      u16   Attribute;
    }tstTriData;


    char mszHead[80];
    u32  mu32TriCnt;
    tstTriData* mfTri;


    cStl()
    {
      mu32TriCnt = 0;
    }

    cStl(char* lszFile)
    {
      mu32TriCnt = 0;
      vLoad(lszFile);
    }

    void vLoad(char* lszFile)
    {
      if (_access(lszFile, 0) == 0)
      {
        unsigned char buffer[10];
        FILE* lFilePtr;

        fopen_s(&lFilePtr, lszFile, "rb");  // r for read, b for binary

        fread(mszHead,     sizeof(mszHead),    1, lFilePtr);
        fread(&mu32TriCnt, sizeof(mu32TriCnt), 1, lFilePtr);

        mfTri = (tstTriData*) new tstTriData[mu32TriCnt];

        fread(mfTri, sizeof(tstTriData) * mu32TriCnt, 1, lFilePtr);

        fclose(lFilePtr);
      }
    }


};


class cMyBox3Df : public cUiElement_Box3Df
{
public:

  cTexture32_Bpp16_5R6G5B               mcTextures[8];
  cWorld_3Df                            mcWorld;
  Simple3Df_Tex_Light_ObjTemplate_Plane mcPlaneTemplate;
  Simple3Df_Tex_Light_ObjTemplate_Cube  mcCubeTemplate;

  cSimple3Df_Obj mcFloor;
  cSimple3Df_Obj mcSlicerPlane;
  cSimple3Df_Obj mcCube;

  cWorld_3Df_Viewer                   mcViewer;
  cWorld_3Df_Viewer_Basic_Z           mcViewerBasicZ;


  cMyBox3Df(cMsgBox* lcMsgBox, cScreen* lpcScreen, cRFont* lpcRFont, float* lafzBuffer)
    : mcWorld(500, 500, 30), 
      mcPlaneTemplate(0),
      mcCubeTemplate(0),
      mcFloor(   0.0f, 0.0f,    0.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcPlaneTemplate),
      mcSlicerPlane( 0.0f, 2.0f/10.0f,-9.0f/10.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcPlaneTemplate),
      mcCube(    0.0f, 4.0f,   -9.0f,  0.0f, 0.0f, 0.0f, (cSimple3Df_ObjTemplate*)&mcCubeTemplate),
      mcViewerBasicZ(0.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, &mcViewer, &mcWorld, lafzBuffer, lpcRFont),
      cUiElement_Box3Df(lcMsgBox, lpcScreen, &mcWorld, (cWorld_3Df_ViewerWorld_Base*)&mcViewerBasicZ)
  {
    mcWorld.AddObj(&mcFloor);

    u32 lit = 0;
    for (lit = 0; lit < mcPlaneTemplate.mu16PointCnt; lit++)
    {
      mcWorld.macPoints[mcWorld.macObj[0]->PointIdx + lit].x *= 100;
      mcWorld.macPoints[mcWorld.macObj[0]->PointIdx + lit].y *= 100;
      mcWorld.macPoints[mcWorld.macObj[0]->PointIdx + lit].z *= 100;
    }

    mcWorld.AddObj(&mcSlicerPlane);
    for (lit = 0; lit < mcPlaneTemplate.mu16PointCnt; lit++)
    {
      mcWorld.macPoints[mcWorld.macObj[1]->PointIdx + lit].x *= 10;
      mcWorld.macPoints[mcWorld.macObj[1]->PointIdx + lit].y *= 10;
      mcWorld.macPoints[mcWorld.macObj[1]->PointIdx + lit].z *= 10;
    }

    mcWorld.AddObj(&mcCube);
  }

  virtual void OnTimer(u32 lu32Timediff_ms) override
  {
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


float                       mafzBuffer[320 * 240];
cUiElement_Window           mc16GuiWindow02_3Df(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);
cMyBox3Df                   mc16GuiBox3Df(&mcMsgBox, &mc16GScreen1, &cRFont_Res8b_Bpp1_1G_5x5Ucase, mafzBuffer);
cStl                        mStlDach("U://E2//Projekte//SW//Code//pcb//source//main//Pc//Slicer//Dach.stl");

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



  mc16GuiRoot.bAdd(10, 10, 50, 50, &mc16GuiBase01);
  mc16GuiBase01.bAdd(10, 10, 20, 20, &mc16GuiText01);

  mc16GuiRoot.bAdd(10, 70, 50, 50, &mc16GuiBase02);
  mc16GuiBase02.bAdd(10, 10, 30, 10, &mc16GuiBtn01);

  mc16GuiText01.vSetText("Text acbfgr-tdfgd fdgdee 2134");


  mc16GuiRoot.bAdd(80, 15, 200, 140, &mc16GuiWindow02_3Df);
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
    // zun�chst wird eine eigene Window-Klasse f�r 
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


