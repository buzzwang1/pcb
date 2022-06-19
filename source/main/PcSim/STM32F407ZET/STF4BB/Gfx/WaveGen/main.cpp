#include "windows.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
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

cUiElement_Root                     mc16GuiRoot(0, 0, DISPLAY_X, DISPLAY_Y, &mcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF,0,0));


class cUiElement_MyWindow: public cUiElement_Window
{
  public:

    class cSignalIn
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
                  /*#ifdef USE_ARM_MATH
                    li16Amp = (u16) (arm_sin_f32(lfCounter * 3.141592653f / 180.0f) * (mAmp));
                  #else
                    li16Amp = (i16) (sinf(lfCounter * 3.141592653f / 180.0f) * (mAmp));
                  #endif*/
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
    };

    class cSignalOut
    {
      public:
        i16 mstChnYPts[1000];
        ciGraphY_Channel mcChnY;

        i32  mFreq;

        cSignalOut()
         : mcChnY(&cRFont_Res8b_Bpp1_1G_5x5Ucase, mstChnYPts, 1000, null)
        {
          mFreq   = 1000;
        }

        void vSet(cSignalIn lcSig[], u8 lu8SigCnt)
        {
          i16 li16Amp;

          mcChnY.vClear();
          for (u16 lit = 0; lit < 1000; lit++)
          {
            li16Amp = 0;
            for (u16 lii = 0; lii < lu8SigCnt; lii++)
            {
              li16Amp += lcSig[lii].mstChnYPts[lit];
            }

            if (li16Amp >  127) li16Amp =  127;
            if (li16Amp < -127) li16Amp = -127;

            mcChnY.vAdd(li16Amp);
          }
        }
    };

    cSignalIn                mcSigIn[4];
    cSignalOut               mcSigOut;

    cUiElement_iGraphYBox    mc16GuiGraphY;

    cUiElement_VSplit        mc16GuiVSplit;
    cUiElement_HSplit        mc16GuiHSplit;
    cUiElement_VSplit        mc16GuiVSplit2;


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

    cUiElement_Button           mcChnToDac;

    u16  mu16SelectedChannel;

    u8   mau8DacDmaBuf[1000];

    cUiElement_MyWindow(cMsgBox *lcMsgBox, cScreen *lpcScreen,
                        uint32 lui32MaxCharacterCount,
                        cRFont *lpcRFont)
      : cUiElement_Window(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont),
        mc16GuiGraphY(lcMsgBox, lpcScreen, &cRFont_Res8b_Bpp1_1G_5x5Ucase, &mcSigIn[0].mcChnY),
        mc16GuiVSplit(lcMsgBox, lpcScreen, 25, 34, -1),
        mc16GuiHSplit(lcMsgBox, lpcScreen, 75, -1, 26),
        mc16GuiVSplit2(lcMsgBox, lpcScreen, 65, 71, -1),
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

        mcChnToDac(lcMsgBox, lpcScreen, 8, &cRFont_Res8b_Bpp1_1G_5x5Ucase)
    {
      mu16SelectedChannel = 0;
    }


    virtual void OnLoaded() override
    {
      cUiElement_Window::OnLoaded();

      this->bAdd(0, 0, 0, 0, &mc16GuiVSplit);
      mc16GuiVSplit.mcElementRight.bAdd(0, 0, 0, 0, &mc16GuiHSplit);

      mc16GuiVSplit.vToggleMaximize();
      mc16GuiHSplit.vToggleMaximize();

      mc16GuiHSplit.mcElementTop.bAdd(20, 20, 150, 100, &mc16GuiGraphY);
      mc16GuiGraphY.vToggleMaximize();

      mc16GuiGraphY.mi16SkalXCnt = 10;
      mc16GuiGraphY.mi16SkalXMax = 1000;


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


      mc16GuiVSplit.mcElementLeft.bAdd(0,0,0,0, &mcSwitchChnGroup);
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

      mc16GuiHSplit.mcElementBot.bAdd(0, 0, 0, 0, &mc16GuiVSplit2);
      mc16GuiVSplit2.vToggleMaximize();

      mc16GuiVSplit2.mcElementLeft.bAdd(0,0,0,0, &mcSwitchSignalGroup);
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

      mc16GuiVSplit2.mcElementRight.bAdd(41,       4, 8, 8, &mcChnAmpInc0001);
      mc16GuiVSplit2.mcElementRight.bAdd(41 + 9,   4, 8, 8, &mcChnAmpInc0010);
      mc16GuiVSplit2.mcElementRight.bAdd(41,      12, 8, 8, &mcChnAmpDec0001);
      mc16GuiVSplit2.mcElementRight.bAdd(41 + 9,  12, 8, 8, &mcChnAmpDec0010);
      mc16GuiVSplit2.mcElementRight.bAdd(2,        8,38,10, &mcChnAmpText);

      mc16GuiVSplit2.mcElementRight.bAdd(110,       4, 8, 8, &mcChnMultInc0001);
      mc16GuiVSplit2.mcElementRight.bAdd(110 + 9,   4, 8, 8, &mcChnMultInc0010);
      mc16GuiVSplit2.mcElementRight.bAdd(110,      12, 8, 8, &mcChnMultDec0001);
      mc16GuiVSplit2.mcElementRight.bAdd(110 + 9 , 12, 8, 8, &mcChnMultDec0010);
      mc16GuiVSplit2.mcElementRight.bAdd(60,        8,48,10, &mcChnMultText);

      mc16GuiHSplit.mcElementBot.bAdd(0, 0, 0, 0, &mcChnFreqBase);
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
      mcChnFreqBase.bAdd(110,        8, 20, 10, &mcChnToDac);
    }


    void vActivateChannel(u8 lu8ChannelIdx)
    {
      mu16SelectedChannel = lu8ChannelIdx;
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
      }
    }

    void vStartDac()
    {
      // ----  Stop Dac
      #ifndef _WIN32
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

        DMA_DeInit(DMA1_Stream5);
        TIM_DeInit(TIM5);
        DAC_DeInit();
      #endif

      // ---- Load values into the Buffer
      for (u16 lit = 0; lit < 1000; lit++)
      {
        mau8DacDmaBuf[lit] = this->mcSigOut.mstChnYPts[lit] + 127;
      }

      // ---- Start Dac
      // Dac auf 8Bit
      // DMA

      //GPIO
      #ifndef _WIN32
        cGpPin lcPortA4(GPIOA_BASE, 4, GPIO_Mode_AN, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0);


        //TIM5 / 32Bit
        TIM_TimeBaseInitTypeDef TIM5_Init_Struct;


        TIM5_Init_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM5_Init_Struct.TIM_CounterMode   = TIM_CounterMode_Up;
        TIM5_Init_Struct.TIM_Period        = (21000/this->mcSigOut.mFreq)-1;
        if (TIM5_Init_Struct.TIM_Period < 1) TIM5_Init_Struct.TIM_Period = 1;
        TIM5_Init_Struct.TIM_Prescaler     = (cClockInfo::mstClocks.PCLK2_Frequency / 21000000) - 1;  // 0.25us;

        TIM_TimeBaseInit(TIM5, &TIM5_Init_Struct);
        TIM_SelectOutputTrigger(TIM5, TIM_TRGOSource_Update);
        TIM_Cmd(TIM5, ENABLE);

        //DAC
        DAC_InitTypeDef DAC_CH1;
        DAC_CH1.DAC_OutputBuffer   = DAC_OutputBuffer_Enable;
        DAC_CH1.DAC_Trigger        = DAC_Trigger_T5_TRGO;
        DAC_CH1.DAC_WaveGeneration = DAC_WaveGeneration_None;
        DAC_CH1.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
        DAC_Init(DAC_Channel_1, &DAC_CH1);

        DAC_ITConfig(DAC_Channel_1, DAC_IT_DMAUDR, DISABLE);


        //DMA
        DMA_InitTypeDef DMA1_STR5_CH7;

        DMA1_STR5_CH7.DMA_BufferSize         = 1000;
        DMA1_STR5_CH7.DMA_Channel            = DMA_Channel_7;
        DMA1_STR5_CH7.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
        DMA1_STR5_CH7.DMA_FIFOMode           = DMA_FIFOMode_Disable;
        DMA1_STR5_CH7.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
        DMA1_STR5_CH7.DMA_Memory0BaseAddr    = (u32)mau8DacDmaBuf;
        DMA1_STR5_CH7.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
        DMA1_STR5_CH7.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
        DMA1_STR5_CH7.DMA_MemoryInc          = DMA_MemoryInc_Enable;
        DMA1_STR5_CH7.DMA_Mode               = DMA_Mode_Circular;
        DMA1_STR5_CH7.DMA_PeripheralBaseAddr = (u32)&DAC->DHR8R1; //-----??????????------
        DMA1_STR5_CH7.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
        DMA1_STR5_CH7.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
        DMA1_STR5_CH7.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
        DMA1_STR5_CH7.DMA_Priority           = DMA_Priority_High;

        DMA_Init(DMA1_Stream5, &DMA1_STR5_CH7);
        DMA_Cmd(DMA1_Stream5, ENABLE);
        DAC_Cmd(DAC_Channel_1, ENABLE);
        DAC_DMACmd(DAC_Channel_1, ENABLE);
      #endif
    }

    virtual void vParentCallback(cUiElement *lpcChild, tenEvent lenEvent) override
    {

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
      }

      if (lenEvent == cUiElement::tenEvent::nBtnPressStart)
      {
        if (lpcChild == &mcChnToDac)
        {
          vStartDac();
        }
        else
        {
          i16 li16Amp  = 0;
          i16 li16Mult = 0;
          i16 li16Freq = 0;

          if (lpcChild == &mcChnAmpInc0001)
          {
            li16Amp = 1;
          }
          else if (lpcChild == &mcChnAmpInc0010)
          {
            li16Amp = 10;
          }
          else if (lpcChild == &mcChnAmpDec0001)
          {
            li16Amp -= 1;
          }
          else if (lpcChild == &mcChnAmpDec0010)
          {
            li16Amp = -10;
          }


          // -----------------------------------
          else if (lpcChild == &mcChnMultInc0001)
          {
            li16Mult = 1;
          }
          else if (lpcChild == &mcChnMultInc0010)
          {
            li16Mult = 10;
          }
          else if (lpcChild == &mcChnMultDec0001)
          {
            li16Mult = -1;
          }
          else if (lpcChild == &mcChnMultDec0010)
          {
            li16Mult = -10;
          }


          // -----------------------------------
          else if (lpcChild == &mcChnFreqInc0001)
          {
            li16Freq = 1;
          }
          else if (lpcChild == &mcChnFreqInc0010)
          {
            li16Freq = 10;
          }
          else if (lpcChild == &mcChnFreqInc0100)
          {
            li16Freq = 100;
          }
          else if (lpcChild == &mcChnFreqInc1000)
          {
            li16Freq = 1000;
          }
          else if (lpcChild == &mcChnFreqDec0001)
          {
            li16Freq = -1;
          }
          else if (lpcChild == &mcChnFreqDec0010)
          {
            li16Freq = -10;
          }
          else if (lpcChild == &mcChnFreqDec0100)
          {
            li16Freq = -100;
          }
          else if (lpcChild == &mcChnFreqDec1000)
          {
            li16Freq = -1000;
          }

          if (mu16SelectedChannel < 4)
          {
            char8  lszStrBuf[32];
            cStr  lszStr(lszStrBuf, 32);

            mcSigIn[mu16SelectedChannel].mAmp += li16Amp;
            if (mcSigIn[mu16SelectedChannel].mAmp >  127) mcSigIn[mu16SelectedChannel].mAmp =  127;
            if (mcSigIn[mu16SelectedChannel].mAmp < -127) mcSigIn[mu16SelectedChannel].mAmp = -127;

            mcSigIn[mu16SelectedChannel].mMult+= li16Mult;
            if (mcSigIn[mu16SelectedChannel].mMult >  100)  mcSigIn[mu16SelectedChannel].mMult =  100;
            if (mcSigIn[mu16SelectedChannel].mMult <     1) mcSigIn[mu16SelectedChannel].mMult =  1;

            lszStr.Setf((rsz)"Amp: %d", mcSigIn[mu16SelectedChannel].mAmp);
            mcChnAmpText.vSetText(lszStr.ToString());

            lszStr.Setf((rsz)"Mult: %d", mcSigIn[mu16SelectedChannel].mMult);
            mcChnMultText.vSetText(lszStr.ToString());

            mcSigIn[mu16SelectedChannel].vSet();
          }
          else
          {
            char8  lszStrBuf[32];
            cStr  lszStr(lszStrBuf, 32);

            mcSigOut.mFreq += li16Freq;
            if (mcSigOut.mFreq >  1000000) mcSigOut.mFreq = 10000;
            if (mcSigOut.mFreq <        1) mcSigOut.mFreq = 1;

            lszStr.Setf((rsz)"Freq: %d", mcSigOut.mFreq);
            mcChnFreqText.vSetText(lszStr.ToString());
            mcChnFreqText.mu32FrameCol = 0;
          }
        }
      }

      cUiElement_Window::vParentCallback(lpcChild, lenEvent);
    }
};


cUiElement_MyWindow  mc16GuiWindow01(&mcMsgBox, &mc16GScreen1, 16, &cRFont_Res8b_Bpp1_1G_Full);



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


