

#include "windows.h"
#include <time.h>
#include <stdlib.h>

#include "main.h"

#include "console.h"
#include "garminhr.h"
#include "libantplus.h"


// when trying each of the ant+ keys
// each key is tried CONNECTION_TIMEOUT times before moving on to the next
// then repeated CONNECTION_RETRIES times
#define CONNECTION_TIMEOUT  (2)    // per key retry on failure
#define CONNECTION_RETRIES  (5)    // all key retry
#define LIBUSB_DISCONNECT  (-5)
#define LIBUSB_TIMEOUT    (-116)

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

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_96x96U(SPRTMST_FontLut_RFont01_96x96U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_96x96U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);


cUiElement_Root                     mc16GuiRoot(10, 10, DISPLAY_X-20, DISPLAY_Y-20, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));
cUiElement_TextBox                  mc16GuiText01(&mcMsgBox, &mc16GScreen1, 256, &cRFont_Res8b_Bpp1_1G_96x96U);
cUiElement_TextBox                  mc16GuiText02(&mcMsgBox, &mc16GScreen1, 256, &cRFont_Res8b_Bpp1_1G_5x5Ucase);

THR *hr;
int readOK;
int keyIdx;

typedef enum
{
  nStHr_UnConnected = 0,
  nStHr_Connected_HR_Search,
  nStHr_Connected_HR_Found
}tenHR_States;

tenHR_States menHrSm;


GMN_tenConstants getRunState (THR *hr)
{
  return hr->cstates->enRunning;
}

static void setRunState (THR *hr, GMN_tenConstants enState)
{
  hr->cstates->enRunning = enState;
}


static int antOpen (THR *hr)
{
  int status = libantplus_Open(hr->ant, 0);
  if (status > 0)
  {
    hr->cstates->openCt = 1;
  }
  return status;
}

static void antClose (THR *hr)
{
  if (hr->cstates->openCt)
  {
    libantplus_Close(hr->ant);
  }
  hr->cstates->openCt = 0;
}

static int antGetOpenCt (THR *hr)
{
  return hr->cstates->openCt;
}

static void antCloseChannel (THR *hr)
{
  if ((hr->cstates->channelStatus) && (antGetOpenCt(hr)))
  {
    libantplus_CloseChannel(hr->ant, hr->dcfg->channel);
  }
}

THR *new_HR()
{
  THR *hr = (THR*)calloc(1, sizeof(THR));
  if (hr)
  {
    hr->dcfg = (TDCONFIG *)calloc(1, sizeof(TDCONFIG));
    if (hr->dcfg)
    {
      hr->cstates = (TCONNSTATES *)calloc(1, sizeof(TCONNSTATES));
      if (hr->cstates)
      {
        hr->rate = (THRBUFFER *)calloc(1, sizeof(THRBUFFER));
        if (hr->rate)
        {
          hr->dev = (TDEVICET *)calloc(1, sizeof(TDEVICET));
          if (hr->dev)
          {
            hr->ant = libantplus_Init();
            if (hr->ant)
            {
              setRunState(hr, GMN_nenRunning);
              return hr;
            }
            free(hr->dev);
          }
          free(hr->rate);
        }
        free(hr->cstates);
      }
      free(hr->dcfg);
    }
    free(hr);
  }
  return NULL;
}




void delete_HR (THR *hr)
{
  if (hr)
  {
    free(hr->ant);
    free(hr->dcfg);
    free(hr->cstates);
    free(hr->dev);
    free(hr->rate);
    free(hr);
  }
}

static void setKey (THR *hr, const int keyIdx)
{
  hr->dcfg->keyIdx = keyIdx;
  hr->cstates->chanIdOnce = 0;
  hr->cstates->channelStatus = 0;
  hr->rate->low = 255;
  hr->rate->high = 0;
  libantplus_ResetSystem(hr->ant);
}



HANDLE         mHrThread = 0;

static DWORD WINAPI Hr_Thread(LPVOID lpStart)
{
  // a4 01 6f | 20 | ea // MESG_STARTUP_MESG_ID
  // 
  // a4 03 40 | 00 46 00 | a1 // MESG_RESPONSE_EVENT_ID
  // 
  // a4 03 40 | 00 42 00 | a5 // MESG_RESPONSE_EVENT_ID
  // 
  // a4 09 4e | 00 84 00 03 18 d1 1b f5 40 | 03 // MESG_BROADCAST_DATA_ID Puls Daten
  // 
  // 
  // a4 02 52 | 00 03 | f7 // MESG_CHANNEL_STATUS_ID
  // a4 06 54 | 08 03 00 ba 36 00 | 71 // MESG_CAPABILITIES_ID
  // a4 09 4e | 00 84 00 10 a7 fe aa 58 3c | e0 // MESG_BROADCAST_DATA_ID Puls Daten

  int timeoutcounter;
  while (1)
  {
    if ((getRunState(hr) == GMN_nenRunning))
    {
      if ((readOK > 0) && (getRunState(hr) == GMN_nenRunning))
      {
        switch (menHrSm)
        {
          case nStHr_UnConnected:
          {
            //mc16GuiText02.vSetText((const char*)"Connecting ...");

            dbprintf("searching for Ant+ stick (0x%.4X/0x%.4X)\n", ANTSTICK_VID, ANTSTICK_PID);
    
            int dfound = libantplus_Discover(hr->ant, 0);
            if (dfound)
            {
              dbprintf("Ant+ USB stick found");
              if (hr->ant->strings[0][0])
              {
                dbprintf(" %s ",hr->ant->strings[0]);
              }
              if (hr->ant->strings[1][0] && hr->ant->strings[2][0])
              {
                dbprintf(" %s - %s ",hr->ant->strings[1], hr->ant->strings[2]);
              }
              else if (hr->ant->strings[1][0])
              {
                dbprintf(" %s ",hr->ant->strings[1]);    
              }
            }
    
            if ((dfound) && (getRunState(hr) == GMN_nenRunning))
            {
              antClose(hr);
              dfound  = antOpen(hr);
              menHrSm = nStHr_Connected_HR_Search;
              keyIdx = 0;
              timeoutcounter = 0;
            }
          }
          break;

          case nStHr_Connected_HR_Search:
            if (getRunState(hr) == GMN_nenRunning)
            {
              timeoutcounter++;
              if (timeoutcounter > CONNECTION_TIMEOUT)
              {
                timeoutcounter = 0;
                //keyIdx++;
              }

              if (keyIdx < KEY_TOTAL)
              {
                setKey(hr, keyIdx);
                readOK = libantplus_HandleMessages(hr->ant);

                if (readOK != LIBUSB_DISCONNECT) 
                {
                  if (hr->dev->scidDeviceNumber)
                  {
                    menHrSm = nStHr_Connected_HR_Found;
                  }
                }
              }
              else
              {
                readOK = LIBUSB_DISCONNECT;
              }
            }

            {
              //char lszStr[32];
              //cStrTools::uixItoa(keyIdx, lszStr, 10);
              //mc16GuiText01.vSetText(lszStr);
              //mc16GuiText02.vSetText((const char*)"HR searching ...");
            }
          break;

          case nStHr_Connected_HR_Found:
            //mc16GuiText02.vSetText((const char*)"HR ok");

            if ((getRunState(hr) == GMN_nenRunning) /* && (hr->cstates->channelStatus) */)
            {
              readOK = libantplus_HandleMessages(hr->ant);
            
              if (readOK > 0)
              {
                //char lszHrBmp[32];
                //cStrTools::uixItoa(hr->rate->currentBmp, lszHrBmp, 10);
                //mc16GuiText01.vSetText(lszHrBmp);
              }
            }
          break;
        }
      }


      if ((readOK < 0) && (getRunState(hr) == GMN_nenRunning))
      {
        libantplus_ResetSystem(hr->ant);
        antClose(hr);
        hr->dev->scidDeviceNumber = 0;
        readOK = 1;
        menHrSm = nStHr_UnConnected;
      }
      else if (readOK < 0)
      {
        dbprintf("read error, retrying...");
        Sleep(1000);
      }
    }
    Sleep(500); 
  }
}

void Start_Hr_Thread()
{
  hr = new_HR();
  if (hr == NULL)
  {
     //dbprintf("startup failed allocating memory\n");
     return; // EXIT_FAILURE;
  }

  hr->marquee = marqueeNew(14, MARQUEE_LEFT);

  if (hr->ant)
  {
    hr->dcfg->deviceNumber = 0;    // 0 
    hr->dcfg->deviceType = 0x78;   // 1
    hr->dcfg->transType = 0;       // 5
    hr->dcfg->channelType = 0;    
    hr->dcfg->networkNumber = 0;
    hr->dcfg->channel = 0;
    hr->dcfg->channelPeriod = 8070; /*4096*/
    hr->dcfg->RFFreq = 0x39; /*0x32*/
    hr->dcfg->searchTimeout = 255;
    hr->dcfg->searchWaveform = 0x53;
    libantplus_SetEventFunction(hr->ant, EVENTI_MESSAGE, messageEventCb, hr);
                    
    readOK = 1;
    menHrSm = nStHr_UnConnected;

    if (mHrThread==0)
    {
      DWORD dwThreadId, dwThrdParam = 1;
      mHrThread=CreateThread(0, 0, Hr_Thread, 0, 0, &dwThreadId);
      SetThreadPriority(mHrThread, THREAD_PRIORITY_NORMAL);
    }
  }
}

void Terminate_Hr_Thread()
{


  if (mHrThread != 0)
  {  
    antCloseChannel(hr);
    antClose(hr);
    marqueeDelete(hr->marquee);

    TerminateThread(mHrThread, 0);
    mHrThread = 0;
  }  
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
    mc16GuiRoot.vMsgHdl(&lcMsg);
  }


  switch (menHrSm)
  {
    case nStHr_UnConnected:
    {
      mc16GuiText02.vSetText((const char*)"Connecting ...");
    }
    break;

    case nStHr_Connected_HR_Search:
      {
        char lszStr[32];
        cStrTools::uixItoa(keyIdx, lszStr, 10);
        mc16GuiText01.vSetText(lszStr);
        mc16GuiText02.vSetText((const char*)"HR searching ...");
      }
    break;

    case nStHr_Connected_HR_Found:
      mc16GuiText02.vSetText((const char*)"HR ok");
      {
        char lszHrBmp[32];
        cStrTools::uixItoa(hr->rate->currentBmp, lszHrBmp, 10);
        mc16GuiText01.vSetText(lszHrBmp);
      }
    break;
  }

  mc16GuiRoot.vPaint();

  mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
  cRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(0, 10, mcCursor.toString(), &mc16GScreen1);

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
  #endif

  mc16GuiRoot.bAdd(10, 10, 130, 80, &mc16GuiText01);
  mc16GuiText01.vSetText((const char*)"---");
  mc16GuiText01.bAdd(0, 80-12, 130, 12, &mc16GuiText02);

  Start_Hr_Thread();
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
      Terminate_Hr_Thread();
      KillTimer(hWnd, Timer_10ms);
      KillTimer(hWnd, Timer_25ms);
      delete mpcDisplay1;

      delete_HR(hr);
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


