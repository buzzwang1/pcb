

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

#define DISPLAY_X 1024
#define DISPLAY_Y 768


#ifdef WIN32

uint8 mui8Zoom = 1;

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


cUiElement_Root                     mc16GuiRoot(0, 0, DISPLAY_X, DISPLAY_Y, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));

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


class cUiElement_MyWindow: public cUiElement_Window
{
  public:

    /*class cSignalIn
    {
      public:
        i16 mstChnYPts[1000];
        ciGraphY_Channel mcChnY;

        i16  mAmp;
        i16  mMult;

        enum tenType
        {
          nNull = 0,
          nRnd,
          nSin,
          nTri,
          nRect,
          nSaw
        };

        tenType menType;

        cSignalIn()
         : mcChnY(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYPts, 1000, null)
        {
          mAmp     = 100;
          mMult    =   1;
          menType  = cSignalIn::tenType::nNull;

          vSet();
        }

        void vSet()
        {
          switch (menType)
          {
            case cSignalIn::tenType::nNull:
              {
                mcChnY.vClear();
                for (u16 lit = 0; lit < 1000; lit++)
                {
                  mcChnY.vAdd(0);
                }
              }
            break;

            case cSignalIn::tenType::nRnd:
              {
                mcChnY.vClear();
                i16 li16Amp = mAmp;
                for (u16 lit = 0; lit < 1000; lit++)
                {
                  if (li16Amp < 0) li16Amp = -li16Amp;
                  if (li16Amp > 0)
                  {
                    mcChnY.vAdd((rand() % (2 * li16Amp)) - li16Amp);
                  }
                  else
                  {
                    mcChnY.vAdd(0);
                  }
                }
              }
            break;

            case cSignalIn::tenType::nSin:
              {
                i16 li16Amp = mAmp;

                float lfCounter = 0.0f;

                mcChnY.vClear();
                for (u16 lit = 0; lit < 1000; lit++)
                {
                  li16Amp = (i16) (sinf(lfCounter * 3.141592653f / 180.0f) * (mAmp));

                  lfCounter += (float)mMult * (360.0f/1000.0f);
                  if (lfCounter >= 360.0f) lfCounter -= 360.0f;

                  mcChnY.vAdd(li16Amp);
                }
              }
            break;

            case cSignalIn::tenType::nTri:
              {
                mcChnY.vClear();

                for (u16 lit = 0; lit < mMult; lit++)
                {
                  i16 li16Pos1 = (1000 * (lit + 0)) /(mMult * 4);
                  i16 li16Pos2 = (1000 * (lit + 1)) /(mMult * 4);
                  i16 li16Pos3 = (1000 * (lit + 3)) /(mMult * 4);
                  i16 li16Pos4 = (1000 * (lit + 4)) /(mMult * 4);

                  float lfy = 0.0f;

                  for (u16 litx = li16Pos1; litx < li16Pos2; litx++)
                  {
                    float lfm = (float)mAmp / (li16Pos2 - li16Pos1);
                    mcChnY.vAdd((i16)(lfy));
                    lfy += lfm;
                  }
                  for (u16 litx = li16Pos2; litx < li16Pos3; litx++)
                  {
                    float lfm = (float)(-2.0f * mAmp) / (li16Pos3 - li16Pos2);
                    mcChnY.vAdd((i16)(lfy));
                    lfy += lfm;
                  }
                  for (u16 litx = li16Pos3; litx < li16Pos4; litx++)
                  {
                    float lfm = (float)mAmp / (li16Pos4 - li16Pos3);
                    mcChnY.vAdd((i16)(lfy));
                    lfy += lfm;
                  }
                }
              }
            break;

            case cSignalIn::tenType::nRect:
              {
                u16 lu16ToggleValue = 1000 / (2 * mMult);
                u16 lu16ToggleCounter = lu16ToggleValue;

                i16 li16Amp = mAmp;

                mcChnY.vClear();
                for (u16 lit = 0; lit < 1000; lit++)
                {
                  mcChnY.vAdd(li16Amp);

                  lu16ToggleCounter--;
                  if (!lu16ToggleCounter)
                  {
                    lu16ToggleCounter = lu16ToggleValue;
                    li16Amp = -li16Amp;
                  }
                }
              }
            break;

            case cSignalIn::tenType::nSaw:
              {
                mcChnY.vClear();

                for (u16 lit = 0; lit < mMult; lit++)
                {
                  i16 li16Pos1 = (1000 * (lit + 0)) /(mMult * 4);
                  i16 li16Pos2 = (1000 * (lit + 4)) /(mMult * 4);

                  float lfy = (float)-mAmp;

                  for (u16 litx = li16Pos1; litx < li16Pos2; litx++)
                  {
                    float lfm = (float)(2.0f * mAmp) / (li16Pos2 - li16Pos1);
                    mcChnY.vAdd((i16)(lfy));
                    lfy += lfm;
                  }
                }
              }
            break;
          }
        }
    };*/

    class cSignalXYBig
    {
      public:
        stPxli16 mstChnXYPts[60 * 60]; // 60s * 60min = 1h
        ciGraphXY_Channel mcChnXY;


        cSignalXYBig()
         : mcChnXY(&cRFont_Res8b_Bpp1_1G_Full, mstChnXYPts, 60 * 60, null)
        {
          mcChnXY.mi16SkalYCnt = 10;
          mcChnXY.mi16SkalYMax = 200;
          mcChnXY.mui32Col     = 0x00FF00;
          vSetRandom();
        }

        void vSetRandom()
        {
          mcChnXY.vClear();
          for (u16 lit = 0; lit < 60 * 60; lit++)
          {
            mcChnXY.vAddXY(lit, 100);
          }
        }
    };

    class cSignalYSmall
    {
      public:
        i16              mstChnYPts[60*4];
        ciGraphY_Channel mcChnY;


        cSignalYSmall()
         : mcChnY(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYPts, 60*4, null)
        {
          mcChnY.mi16SkalYCnt = 10;
          mcChnY.mi16SkalYMax = 200;
          mcChnY.mui32Col     = 0x00FF00;
          vSetRandom();
        }

        void vSetRandom()
        {
          mcChnY.vClear();
          for (u16 lit = 0; lit < 60*4; lit++)
          {
            //mcChnY.vAdd((rand() % (2 * li16Amp)) - li16Amp);
            mcChnY.vAdd(lit * 2);
          }
        }
    };

    cUiElement_VSplit        mc16GuiVSplit_Main;
    cUiElement_HSplit        mc16GuiHSplit_MainLeftMain;
    cUiElement_HSplit        mc16GuiHSplit_MainLeftMainTop;
    cUiElement_HSplit        mc16GuiHSplit_MainLeftMainBottom;

    // mc16GuiVSplit_Main.Left
    // mc16GuiHSplit_MainLeftMainTop.Top
    cUiElement_TextBox       mc16TextHrValue;
    cUiElement_TextBox       mc16TextHrState;

    // mc16GuiVSplit_Main.Left
    // mc16GuiHSplit_MainLeftMainTop.Bot
    cSignalYSmall            mcSigYSmall;
    cUiElement_iGraphYBox    mc16GuiGraphY_HrPreviewSmall;


    // mc16GuiVSplit_Main.Rigth
    cSignalXYBig             mcSigXYBig;
    cUiElement_iGraphXYBox   mc16GuiGraphXY_HrBig;
    cUiElement_Button        mcBtnGraphXY_Reset;
    cUiElement_TextBox       mcTxtGraphXY_Time;

    /*
    cUiElement_SwitchGroup  mcSwitchChnGroup;

    cUiElement_SwitchPressOnly  mcSwitchChn1;
    cUiElement_SwitchPressOnly  mcSwitchChn2;
    cUiElement_SwitchPressOnly  mcSwitchChn3;
    cUiElement_SwitchPressOnly  mcSwitchChn4;
    cUiElement_SwitchPressOnly  mcSwitchChnSum;


    cUiElement_SwitchGroup      mcSwitchSignalGroup;

    cUiElement_SwitchPressOnly  mcSwitchSignal1;
    cUiElement_SwitchPressOnly  mcSwitchSignal2;
    cUiElement_SwitchPressOnly  mcSwitchSignal3;
    cUiElement_SwitchPressOnly  mcSwitchSignal4;
    cUiElement_SwitchPressOnly  mcSwitchSignal5;
    cUiElement_SwitchPressOnly  mcSwitchSignal6;

    cUiElement_Button           mcChnAmpInc0001;
    cUiElement_Button           mcChnAmpInc0010;
    cUiElement_Button           mcChnAmpDec0001;
    cUiElement_Button           mcChnAmpDec0010;
    cUiElement_TextBox          mcChnAmpText;

    cUiElement_Button           mcChnMultInc0001;
    cUiElement_Button           mcChnMultInc0010;
    cUiElement_Button           mcChnMultDec0001;
    cUiElement_Button           mcChnMultDec0010;
    cUiElement_TextBox          mcChnMultText;


    cUiElement                  mcChnFreqBase;
    cUiElement_Button           mcChnFreqInc0001;
    cUiElement_Button           mcChnFreqInc0010;
    cUiElement_Button           mcChnFreqInc0100;
    cUiElement_Button           mcChnFreqInc1000;
    cUiElement_Button           mcChnFreqDec0001;
    cUiElement_Button           mcChnFreqDec0010;
    cUiElement_Button           mcChnFreqDec0100;
    cUiElement_Button           mcChnFreqDec1000;
    cUiElement_TextBox          mcChnFreqText;

    cUiElement_Button           mcChnToDac;*/


    u16  mu16Second;
    u16  mu16Second_Last;

    char mszLogFileName[64];

    cUiElement_MyWindow(cMsgBox *lcMsgBox, cScreen *lpcScreen,
                        uint32 lui32MaxCharacterCount,
                        cRFont *lpcRFontSmall, 
                        cRFont *lpcRFont,
                        cRFont *lpcRFontBig)
      : cUiElement_Window(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont),
        mc16GuiVSplit_Main(lcMsgBox, lpcScreen, 25, 200, -1),
        mc16GuiHSplit_MainLeftMain(lcMsgBox, lpcScreen, 40, -1, -1),
        mc16GuiHSplit_MainLeftMainTop(lcMsgBox, lpcScreen, 40, -1, -1),
        mc16GuiHSplit_MainLeftMainBottom(lcMsgBox, lpcScreen, 50, -1, -1),

        // mc16GuiVSplit_Main.Left
        // mc16GuiHSplit_MainLeftMainTop.Top
        mc16TextHrValue(lcMsgBox, lpcScreen,  32, lpcRFontBig),
        mc16TextHrState(lcMsgBox, lpcScreen, 256, lpcRFontSmall),

        // mc16GuiVSplit_Main.Left
        // mc16GuiHSplit_MainLeftMainTop.Bot
        mc16GuiGraphY_HrPreviewSmall(lcMsgBox, lpcScreen, lpcRFontSmall, &mcSigYSmall.mcChnY),

        // mc16GuiVSplit_Main.Rigth
        mc16GuiGraphXY_HrBig(lcMsgBox, lpcScreen, lpcRFont, &mcSigXYBig.mcChnXY),
        mcBtnGraphXY_Reset(lcMsgBox, lpcScreen, 16, lpcRFontBig),
        mcTxtGraphXY_Time(lcMsgBox, lpcScreen, 16, lpcRFontBig)/*,

        mcSwitchChnGroup(lcMsgBox, lpcScreen),
        mcSwitchChn1(lcMsgBox, lpcScreen, 8, lpcRFont),
        mcSwitchChn2(lcMsgBox, lpcScreen, 8, lpcRFont),
        mcSwitchChn3(lcMsgBox, lpcScreen, 8, lpcRFont),
        mcSwitchChn4(lcMsgBox, lpcScreen, 8, lpcRFont),
        mcSwitchChnSum(lcMsgBox, lpcScreen, 8, lpcRFont),

        mcSwitchSignalGroup(lcMsgBox, lpcScreen),
        mcSwitchSignal1(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcSwitchSignal2(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcSwitchSignal3(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcSwitchSignal4(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcSwitchSignal5(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcSwitchSignal6(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),

        mcChnAmpInc0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnAmpInc0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnAmpDec0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnAmpDec0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnAmpText(lcMsgBox,   lpcScreen,24, &cRFont_Res8b_Bpp1_1G_5x5Ucase),

        mcChnMultInc0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnMultInc0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnMultDec0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnMultDec0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnMultText(lcMsgBox,   lpcScreen,24, &cRFont_Res8b_Bpp1_1G_5x5Ucase),

        mcChnFreqBase(lcMsgBox, lpcScreen),
        mcChnFreqInc0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqInc0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqInc0100(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqInc1000(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqDec0001(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqDec0010(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqDec0100(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqDec1000(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase),
        mcChnFreqText(lcMsgBox,   lpcScreen,24, &cRFont_Res8b_Bpp1_1G_5x5Ucase),

        mcChnToDac(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase)*/
    {
      mu16Second = 0;
      mu16Second_Last = 0;

      vCreateLogFileName(mszLogFileName);
    }


    virtual void OnLoaded() override
    {
      cUiElement_Window::OnLoaded();

      this->bAdd(0, 0, 0, 0, &mc16GuiVSplit_Main);
      mc16GuiVSplit_Main.mcElementLeft.bAdd(0, 0, 0, 0, &mc16GuiHSplit_MainLeftMain);
      mc16GuiHSplit_MainLeftMain.mcElementTop.bAdd(0, 0, 0, 0, &mc16GuiHSplit_MainLeftMainTop);
      mc16GuiHSplit_MainLeftMain.mcElementBot.bAdd(0, 0, 0, 0, &mc16GuiHSplit_MainLeftMainBottom);

      mc16GuiVSplit_Main.vToggleMaximize();
      mc16GuiHSplit_MainLeftMain.vToggleMaximize();
      mc16GuiHSplit_MainLeftMainTop.vToggleMaximize();
      mc16GuiHSplit_MainLeftMainBottom.vToggleMaximize();


      // mc16GuiVSplit_Main.Left
      // mc16GuiHSplit_MainLeftMainTop.Top
      mc16GuiHSplit_MainLeftMainTop.mcElementTop.bAdd(4, 0,     130, 80, &mc16TextHrValue);
      mc16GuiHSplit_MainLeftMainTop.mcElementTop.bAdd(4, 80,    130, 12, &mc16TextHrState);
      mc16TextHrValue.mstStatus.PaintFrame = False;
      mc16TextHrState.mstStatus.PaintFrame = False;
      mc16TextHrValue.vSetText((rsz)"---");
      mc16TextHrState.vSetText((rsz)"---");
      
      // mc16GuiVSplit_Main.Left
      // mc16GuiHSplit_MainLeftMainTop.Bot
      mc16GuiHSplit_MainLeftMainTop.mcElementBot.bAdd(0, 0, 10, 10, &mc16GuiGraphY_HrPreviewSmall);
      mc16GuiGraphY_HrPreviewSmall.vToggleMaximize();
      mc16GuiGraphY_HrPreviewSmall.mi16LabelXDiv = 4;
      mc16GuiGraphY_HrPreviewSmall.mi16SkalXCnt = 6;
      mc16GuiGraphY_HrPreviewSmall.mi16SkalXMax = 240;


      // mc16GuiVSplit_Main.Rigth
      mc16GuiVSplit_Main.mcElementRight.bAdd(0, 0, 10, 10, &mc16GuiGraphXY_HrBig);
      mc16GuiGraphXY_HrBig.vToggleMaximize();
      mc16GuiGraphXY_HrBig.mi16LabelXDiv = 60;
      mc16GuiGraphXY_HrBig.mi16SkalXCnt  = 12;
      mc16GuiGraphXY_HrBig.mi16SkalXMax  = 60*60;

      mc16GuiGraphXY_HrBig.bAdd(30, 20, 220, 70, &mcBtnGraphXY_Reset);
      mcBtnGraphXY_Reset.vSetText((rsz)"Reset");

      mc16GuiGraphXY_HrBig.bAdd(300, 20, 300, 70, &mcTxtGraphXY_Time);
      mcTxtGraphXY_Time.vSetText((rsz)"00:00");
      mcTxtGraphXY_Time.mstStatus.PaintFrame = False;
      mcTxtGraphXY_Time.mstStatus.PaintBg    = False;


      /*
      for (u16 lii = 0; lii < 4; lii++)
      {
        mcSigIn[lii].mcChnY.mi16SkalYMin = -130;
        mcSigIn[lii].mcChnY.mi16SkalYMax =  130;
        mcSigIn[lii].mcChnY.mi16SkalYCnt =   13;
        mcSigIn[lii].mcChnY.mui32Col     = 0x00FF00;
      }

      mcSigOut.mcChnY.mi16SkalYMin = -130;
      mcSigOut.mcChnY.mi16SkalYMax =  130;
      mcSigOut.mcChnY.mi16SkalYCnt =   13;
      mcSigOut.mcChnY.mui32Col     = 0x0000FF;


      mc16GuiVSplit_Main.mcElementLeft.bAdd(0, 0, 0, 0, &mcSwitchChnGroup);
      mcSwitchChnGroup.vToggleMaximize();

      mcSwitchChn1.vSetText((const char*)"Chn1");
      mcSwitchChn2.vSetText((const char*)"Chn2");
      mcSwitchChn3.vSetText((const char*)"Chn3");
      mcSwitchChn4.vSetText((const char*)"Chn4");
      mcSwitchChnSum.vSetText((const char*)"Sum");

      mcSwitchChnGroup.bAdd(2,  2,      30, 14, &mcSwitchChn1);
      mcSwitchChnGroup.bAdd(2,  4 + 14, 30, 14, &mcSwitchChn2);
      mcSwitchChnGroup.bAdd(2,  6 + 28, 30, 14, &mcSwitchChn3);
      mcSwitchChnGroup.bAdd(2,  8 + 42, 30, 14, &mcSwitchChn4);
      mcSwitchChnGroup.bAdd(2, 10 + 56, 30, 14, &mcSwitchChnSum);

      mc16GuiHSplit_MainRight.mcElementBot.bAdd(0, 0, 0, 0, &mc16GuiHSplit_MainLeft);
      mc16GuiHSplit_MainLeft.vToggleMaximize();

      mc16GuiHSplit_MainLeft.mcElementLeft.bAdd(0,0,0,0, &mcSwitchSignalGroup);
      mcSwitchSignalGroup.vToggleMaximize();

      mcSwitchSignal1.vSetText((const char*)"Null");
      mcSwitchSignal2.vSetText((const char*)"Rnd");
      mcSwitchSignal3.vSetText((const char*)"Sin");
      mcSwitchSignal4.vSetText((const char*)"Tri");
      mcSwitchSignal5.vSetText((const char*)"Rect");
      mcSwitchSignal6.vSetText((const char*)"Saw");

      mcSwitchSignalGroup.bAdd(2,           2, 20, 10, &mcSwitchSignal1);
      mcSwitchSignalGroup.bAdd(2 + 24,      2, 20, 10, &mcSwitchSignal2);
      mcSwitchSignalGroup.bAdd(2 + 48,      2, 20, 10, &mcSwitchSignal3);

      mcSwitchSignalGroup.bAdd(2,      2 + 12, 20, 10, &mcSwitchSignal4);
      mcSwitchSignalGroup.bAdd(2 + 24, 2 + 12, 20, 10, &mcSwitchSignal5);
      mcSwitchSignalGroup.bAdd(2 + 48, 2 + 12, 20, 10, &mcSwitchSignal6);

      mcSwitchChn1.vSetState(1);
      mcSwitchSignal1.vSetState(1);

      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      mcChnAmpInc0001.vSetText((rsz)"+");
      mcChnAmpInc0010.vSetText((rsz)"+");
      mcChnAmpDec0001.vSetText((rsz)"-");
      mcChnAmpDec0010.vSetText((rsz)"-");
      lszStr.Setf((rsz)"Amp: %d", mcSigIn[0].mAmp);
      mcChnAmpText.vSetText(lszStr.ToString());
      mcChnAmpText.mu32FrameCol = 0;

      mcChnMultInc0001.vSetText((rsz)"+");
      mcChnMultInc0010.vSetText((rsz)"+");
      mcChnMultDec0001.vSetText((rsz)"-");
      mcChnMultDec0010.vSetText((rsz)"-");
      lszStr.Setf((rsz)"Mult: %d", mcSigIn[0].mMult);
      mcChnMultText.vSetText(lszStr.ToString());
      mcChnMultText.mu32FrameCol = 0;

      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(41,       4, 8, 8, &mcChnAmpInc0001);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(41 + 9,   4, 8, 8, &mcChnAmpInc0010);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(41,      12, 8, 8, &mcChnAmpDec0001);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(41 + 9,  12, 8, 8, &mcChnAmpDec0010);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(2,        8,38,10, &mcChnAmpText);

      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(110,       4, 8, 8, &mcChnMultInc0001);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(110 + 9,   4, 8, 8, &mcChnMultInc0010);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(110,      12, 8, 8, &mcChnMultDec0001);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(110 + 9 , 12, 8, 8, &mcChnMultDec0010);
      mc16GuiHSplit_MainLeft.mcElementRight.bAdd(60,        8,48,10, &mcChnMultText);

      mc16GuiHSplit_MainRight.mcElementBot.bAdd(0, 0, 0, 0, &mcChnFreqBase);
      mcChnFreqBase.vToggleMaximize();
      mcChnFreqBase.mstStatus.Visible = 0;
      mcChnFreqBase.bAdd(61,       4, 8, 8, &mcChnFreqInc0001);
      mcChnFreqBase.bAdd(61 + 9,   4, 8, 8, &mcChnFreqInc0010);
      mcChnFreqBase.bAdd(61,      12, 8, 8, &mcChnFreqDec0001);
      mcChnFreqBase.bAdd(61 + 9,  12, 8, 8, &mcChnFreqDec0010);
      mcChnFreqBase.bAdd(61 +18,   4, 8, 8, &mcChnFreqInc0100);
      mcChnFreqBase.bAdd(61 +27,   4, 8, 8, &mcChnFreqInc1000);
      mcChnFreqBase.bAdd(61 +18,  12, 8, 8, &mcChnFreqDec0100);
      mcChnFreqBase.bAdd(61 +27,  12, 8, 8, &mcChnFreqDec1000);
      mcChnFreqBase.bAdd(2,        8,58,10, &mcChnFreqText);

      mcChnFreqInc0001.vSetText((rsz)"+");
      mcChnFreqInc0010.vSetText((rsz)"+");
      mcChnFreqInc0100.vSetText((rsz)"+");
      mcChnFreqInc1000.vSetText((rsz)"+");
      mcChnFreqDec0001.vSetText((rsz)"-");
      mcChnFreqDec0010.vSetText((rsz)"-");
      mcChnFreqDec0100.vSetText((rsz)"-");
      mcChnFreqDec1000.vSetText((rsz)"-");

      lszStr.Setf((rsz)"Freq: %d", mcSigOut.mFreq);
      mcChnFreqText.vSetText(lszStr.ToString());
      mcChnFreqText.mu32FrameCol = 0;

      mcChnToDac.vSetText((rsz)"DAC");
      mcChnFreqBase.bAdd(110,        8, 20, 10, &mcChnToDac);*/
    }

    void vIncSecond()
    {      
      if (mu16Second < (60 * 60 * 4)) //4h
      {
        mu16Second++;

        char lszString[32];
        cStr lcString(lszString, 32);
        lcString.Setf("%02d:%02d", mu16Second / 60, mu16Second % 60);
        mcTxtGraphXY_Time.vSetText(lcString.ToString());
      }
    }

    void vSetHrStatus(rsz lszText)
    {
      mc16TextHrState.vSetText(lszText);
    }

    void vSetHrValue(u16 lu16Value)
    {
      char lszHrBmp[32];
      cStrTools::uixItoa(lu16Value, lszHrBmp, 10);
      mc16TextHrValue.vSetText(lszHrBmp);
      mcSigYSmall.mcChnY.vAdd(lu16Value);
      mc16GuiGraphY_HrPreviewSmall.vRepaint();

      if (mu16Second_Last != mu16Second)
      {
        mu16Second_Last = mu16Second;
        mcSigXYBig.mcChnXY.vAddXY(mu16Second, lu16Value);
        mc16GuiGraphXY_HrBig.vRepaint();
        LogFile_Add(mszLogFileName, lu16Value);
      }
    }


    void vActivateChannel(u8 lu8ChannelIdx)
    {
      /*mu16SelectedChannel = lu8ChannelIdx;
      if (lu8ChannelIdx < 4)
      {
        mc16GuiGraphY.mcGraphChn = &mcSigIn[mu16SelectedChannel].mcChnY;

        switch (mcSigIn[mu16SelectedChannel].menType)
        {
          case cSignalIn::tenType::nNull:
            mcSwitchSignal1.vSetState(1);
          break;

          case cSignalIn::tenType::nRnd:
            mcSwitchSignal2.vSetState(1);
          break;

          case cSignalIn::tenType::nSin:
            mcSwitchSignal3.vSetState(1);
          break;

          case cSignalIn::tenType::nTri:
            mcSwitchSignal4.vSetState(1);
          break;

          case cSignalIn::tenType::nRect:
            mcSwitchSignal5.vSetState(1);
          break;

          case cSignalIn::tenType::nSaw:
            mcSwitchSignal6.vSetState(1);
          break;
        }

        char8  lszStrBuf[32];
        cStr  lszStr(lszStrBuf, 32);

        lszStr.Setf((rsz)"Amp: %d", mcSigIn[mu16SelectedChannel].mAmp);
        mcChnAmpText.vSetText(lszStr.ToString());

        lszStr.Setf((rsz)"Mult: %d", mcSigIn[mu16SelectedChannel].mMult);
        mcChnMultText.vSetText(lszStr.ToString());

        mc16GuiVSplit2.mstStatus.Visible = 1;
        mcChnFreqBase.mstStatus.Visible = 0;
      }
      else
      {
        char8  lszStrBuf[32];
        cStr  lszStr(lszStrBuf, 32);

        mc16GuiGraphY.mcGraphChn = &mcSigOut.mcChnY;
        mcSigOut.vSet(mcSigIn, 4);
        mc16GuiVSplit2.mstStatus.Visible = 0;
        mcChnFreqBase.mstStatus.Visible = 1;

        lszStr.Setf((rsz)"Freq: %d", mcSigOut.mFreq);
        mcChnFreqText.vSetText(lszStr.ToString());
        mcChnFreqText.mu32FrameCol = 0;
      }*/
    }

    virtual void vParentCallback(cUiElement *lpcChild, tenEvent lenEvent) override
    {
      /*
      if (lenEvent == cUiElement::tenEvent::nSwitchStateChanged)
      {
        if (((cUiElement_SwitchPressOnly*)lpcChild)->mu8State)
        {
          lpcChild->mu32BaseCol = 0x000080;
        }
        else
        {
          lpcChild->mu32BaseCol = 0;
        }

        // ------------------------
        if (lpcChild == &mcSwitchChn1)
        {
          vActivateChannel(0);
        }
        else if (lpcChild == &mcSwitchChn2)
        {
          vActivateChannel(1);
        }
        else if (lpcChild == &mcSwitchChn3)
        {
          vActivateChannel(2);
        }
        else if (lpcChild == &mcSwitchChn4)
        {
          vActivateChannel(3);
        }
        else if (lpcChild == &mcSwitchChnSum)
        {
          vActivateChannel(4);
        }
        // --------------------------
        else if (lpcChild == &mcSwitchSignal1)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nNull;
          mcSigIn[mu16SelectedChannel].vSet();
        }
        else if (lpcChild == &mcSwitchSignal2)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nRnd;
          mcSigIn[mu16SelectedChannel].vSet();
        }
        else if (lpcChild == &mcSwitchSignal3)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nSin;
          mcSigIn[mu16SelectedChannel].vSet();
        }
        else if (lpcChild == &mcSwitchSignal4)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nTri;
          mcSigIn[mu16SelectedChannel].vSet();
        }
        else if (lpcChild == &mcSwitchSignal5)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nRect;
          mcSigIn[mu16SelectedChannel].vSet();
        }
        else if (lpcChild == &mcSwitchSignal6)
        {
          mcSigIn[mu16SelectedChannel].menType = cSignalIn::tenType::nSaw;
          mcSigIn[mu16SelectedChannel].vSet();
        }
      }*/

      if (lenEvent == cUiElement::tenEvent::nBtnPressEnd)
      {
        if (lpcChild == &mcBtnGraphXY_Reset)
        {
          mcSigXYBig.mcChnXY.vClear();
          mu16Second_Last = mu16Second = 0;
          vCreateLogFileName(mszLogFileName);
        }
      }

      cUiElement_Window::vParentCallback(lpcChild, lenEvent);
    }

    void vCreateLogFileName(char* lszFilename)
    {
       time_t t = time(NULL);
       struct tm tm = *localtime(&t);
       sprintf(lszFilename, "%d%02d%02d_%02d%02d%02d_%d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, GetTickCount()); 
    }

    void LogFile_Add(char* lszFilename, int iPulse)
    {
      FILE *pFile;
      pFile = fopen(lszFilename, "a"); 

      if (pFile != NULL)
      {
        fprintf(pFile, "%i;%i\n", mu16Second, iPulse);
        fclose(pFile);
      }
    }
};


cUiElement_MyWindow  mc16GuiWindow01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &cRFont_Res8b_Bpp1_1G_Full, &cRFont_Res8b_Bpp1_1G_96x96U);




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
  static u16 lu16Tick250ms  = 0;
  static u16 lu16Tick1000ms = 0;

  cMsg lcMsg;
  while (!mcMsgBox.isEmpty())
  {
    mcMsgBox.vget(&lcMsg);
    mc16GuiRoot.vMsgHdl(&lcMsg);
  }

  lu16Tick250ms++;
  lu16Tick1000ms++;
  if (lu16Tick1000ms >= 40)
  {
    lu16Tick1000ms = 0;
    mc16GuiWindow01.vIncSecond();
  }

  if (lu16Tick250ms >= 10)
  {
    lu16Tick250ms = 0;
    switch (menHrSm)
    {
      case nStHr_UnConnected:
      {
        mc16GuiWindow01.vSetHrStatus((rsz)"Connecting ...");
      }
      break;

      case nStHr_Connected_HR_Search:
        {
          mc16GuiWindow01.vSetHrStatus((rsz)"HR searching ...");

          char lszStr[32];
          cStrTools::uixItoa(keyIdx, lszStr, 10);
          mc16GuiWindow01.mc16TextHrValue.vSetText(lszStr);
        }
      break;

      case nStHr_Connected_HR_Found:
        mc16GuiWindow01.vSetHrStatus((rsz)"HR ok");
        mc16GuiWindow01.vSetHrValue(hr->rate->currentBmp);
      break;
    }
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

  mc16GuiRoot.bAdd(10, 10, 250, 150, &mc16GuiWindow01);
  mc16GuiRoot.mstStatus.Sizeable = 0;
  mc16GuiRoot.mstStatus.DragAble = 0;
  mc16GuiRoot.mstStatus.PaintFrame = 0;
  mc16GuiWindow01.vToggleMaximize();

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
        MousexPos = (int16)lstPoint.x / mui8Zoom;  // horizontal position of cursor 
        MouseyPos = (int16)lstPoint.y / mui8Zoom;  // vertical position of cursor
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
    if ( !MakeWindow( nCmdShow, hInstance, 0, 0, DISPLAY_X * mui8Zoom, DISPLAY_Y * mui8Zoom + 20) ) return 0;

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


