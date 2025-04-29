#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;

//cI2cMaster* mcI2C1;
//cAPDS9960* mcAPDS9960;

LED<GPIOF_BASE, 9>   mcLedRed;
cClockInfo mcClkInfo;

#define DISPLAY_X 320
#define DISPLAY_Y 240

uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2] __attribute__((section(".ExtMem")));
float mafzBuffer[320 * 240] __attribute__((section(".ExtMem")));

cDiffTimer                              mcDiffTimer;
cMsgBox                                  mcMsgBox(20);
cCursor                                  mcCursor(200, 800, &mcMsgBox);

cBitmap_Bpp16_5R6G5B                     mc16GBm(DISPLAY_X, DISPLAY_Y, mDisplayMemory);
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

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

#if 0

void DMA1_Channel1_IRQHandler(void)
{
}

void DMA1_Channel2_IRQHandler(void)
{
}

void DMA1_Channel3_IRQHandler(void)
{

}

void DMA1_Channel4_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC4))
  {
    /* Disable the DMA1 Channel 4 */
    DMA_Cmd(DMA1_Stream4, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_FLAG_TC5))
  {
    /* Disable the DMA1 Channel 5 */
    DMA_Cmd(DMA1_Stream5, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
  }
}

void DMA1_Channel6_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Tx

  //if (DMA_GetFlagStatus(DMA1_FLAG_TC6))
  {
    /* Disable the DMA1 Channel 6 */
    DMA_Cmd(DMA1_Stream6, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
  }
}

void DMA1_Channel7_IRQHandler(void)
{
  // DMA1_Channel7_IRQn; I2C1 Rx
  //if (DMA_GetFlagStatus(DMA1_FLAG_TC7))
  {
    /* Disable the DMA1 Channel 7 */
    DMA_Cmd(DMA1_Stream7, DISABLE);
    /* Clear the DMA Transfer complete flag */
    DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
  }
}


void I2C1_EV_IRQHandler(void)
{
  mcI2C1->I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1->I2C_ER_IRQHandler();
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

#endif

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/*
static void Error_Handler(void)
{
  while (1)
  {
  }
}*/


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


#include "math.h"

#define OTT_MAX_TOUCH  5
#define TP_PRES_DOWN 0x80
#define TP_CATH_PRES 0x40

typedef struct
{
  u8(*init)(void);
  u8(*scan)(u8);
  void(*adjust)(void);
  u16 x[OTT_MAX_TOUCH];
  u16 y[OTT_MAX_TOUCH];

  u8  sta;

  float xfac;
  float yfac;
  short xoff;
  short yoff;

  u8 touchtype;
}_m_tp_dev;


#define PEN  		PBin(1)     //T_PEN
#define DOUT 		PBin(2)     //T_MISO
#define TDIN 		PFout(11)   //T_MOSI
#define TCLK 		PBout(0)    //T_SCK
#define TCS  		PCout(13)   //T_CS

u8 TP_Init(void);
u8 TP_Scan(u8 tp);

_m_tp_dev tp_dev =
{
  TP_Init,
  TP_Scan,
  null,
  {0,0,0,0,0},
  {0,0,0,0,0},

  0,

  -0.0891,
  -0.0681,
  340,
  260,

  0,
};

u8 CMD_RDX = 0X90;
u8 CMD_RDY = 0XD0;

void TP_Delay(volatile u32 lu32Num)
{
  lu32Num *= 8;

  while (lu32Num)
  {
    lu32Num--;
  }
}



void TP_Write_Byte(u8 num)
{
  u8 count = 0;
  for (count = 0; count < 8; count++)
  {
    if (num & 0x80)TDIN = 1;
    else TDIN = 0;
    num <<= 1;
    TCLK = 0;
    TP_Delay(1);
    TCLK = 1;
  }
}


u16 TP_Read_AD(u8 CMD)
{
  u8 count = 0;
  u16 Num = 0;
  TCLK = 0;
  TDIN = 0;
  TCS = 0;
  TP_Write_Byte(CMD);
  TP_Delay(6);
  TCLK = 0;
  TP_Delay(1);
  TCLK = 1;
  TP_Delay(1);
  TCLK = 0;
  for (count = 0; count < 16; count++)
  {
    Num <<= 1;
    TCLK = 0;
    TP_Delay(1);
    TCLK = 1;
    if (DOUT)Num++;
  }
  Num >>= 4;
  TCS = 1;
  return(Num);
}




#define READ_TIMES 5
#define LOST_VAL 1
u16 TP_Read_XOY(u8 xy)
{
  u16 i, j;
  u16 buf[READ_TIMES];
  u16 sum = 0;
  u16 temp;
  for (i = 0; i < READ_TIMES; i++)buf[i] = TP_Read_AD(xy);
  for (i = 0; i < READ_TIMES - 1; i++)
  {
    for (j = i + 1; j < READ_TIMES; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  sum = 0;
  for (i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)sum += buf[i];
  temp = sum / (READ_TIMES - 2 * LOST_VAL);
  return temp;
}


u8 TP_Read_XY(u16 *x, u16 *y)
{
  u16 xtemp, ytemp;
  xtemp = TP_Read_XOY(CMD_RDX);
  ytemp = TP_Read_XOY(CMD_RDY);
  //if(xtemp<100||ytemp<100)return 0;
  *x = xtemp;
  *y = ytemp;
  return 1;
}


#define ERR_RANGE 50
u8 TP_Read_XY2(u16 *x, u16 *y)
{
  u16 x1, y1;
  u16 x2, y2;
  u8 flag;
  flag = TP_Read_XY(&x1, &y1);
  if (flag == 0)return(0);
  flag = TP_Read_XY(&x2, &y2);
  if (flag == 0)return(0);
  if (((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE))
      && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
  {
    *x = (x1 + x2) / 2;
    *y = (y1 + y2) / 2;
    return 1;
  }
  else return 0;
}


u8 TP_Scan(u8 tp)
{
  if (PEN == 0)
  {
    if (tp)TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);
    else if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))
    {
      tp_dev.x[0] = tp_dev.xfac*tp_dev.x[0] + tp_dev.xoff;
      tp_dev.y[0] = tp_dev.yfac*tp_dev.y[0] + tp_dev.yoff;
    }
    if ((tp_dev.sta&TP_PRES_DOWN) == 0)
    {
      tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;
      tp_dev.x[4] = tp_dev.x[0];
      tp_dev.y[4] = tp_dev.y[0];
    }

    mcCursor.vTick10ms(tp_dev.x[0], tp_dev.y[0], 1);
  }
  else
  {
    if (tp_dev.sta&TP_PRES_DOWN)
    {
      tp_dev.sta &= ~(1 << 7);
      mcCursor.vTick10ms(tp_dev.x[0], tp_dev.y[0], 0);
    }
    else
    {
      tp_dev.x[4] = 0;
      tp_dev.y[4] = 0;
      tp_dev.x[0] = 0xffff;
      tp_dev.y[0] = 0xffff;
    }
  }
  return tp_dev.sta&TP_PRES_DOWN;
}



u8 TP_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE);

  //GPIOB1,2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOF, &GPIO_InitStructure);


  TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]);

  return 1;
}


void FSMC_SRAM_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);//??PD,PE,PF,PG??
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);//??FSMC??


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 ????,????
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOB, &GPIO_InitStructure);//??? //PB15 ????,????

  GPIO_InitStructure.GPIO_Pin = (3 << 0) | (3 << 4) | (0XFF << 8);//PD0,1,4,5,8~15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOD, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (3 << 0) | (0X1FF << 7);//PE0,1,7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOE, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (0X3F << 0) | (0XF << 12);   //PF0~5,12~15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOF, &GPIO_InitStructure);//???

  GPIO_InitStructure.GPIO_Pin = (0X3F << 0) | GPIO_Pin_10;//PG0~5,10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOG, &GPIO_InitStructure);//???


  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);//PD15,AF12

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);//PE15,AF12

  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);//PF0,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);//PF1,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);//PF2,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);//PF3,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);//PF4,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);//PF5,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);//PF13,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);//PF14,AF12
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);//PF15,AF12

  GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);


  readWriteTiming.FSMC_AddressSetupTime = 0x00;   //??????(ADDSET)?1?HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;   //??????(ADDHLD)??A???
  readWriteTiming.FSMC_DataSetupTime = 0x08;     ////??????(DATAST)?9?HCLK 6*9=54ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;   //??A



  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;//  ??????NE3 ,????BTCR[4],[5]?
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//????????16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;  //??????
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; // ?????????
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; //??????

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //???FSMC??

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // ??BANK3
}


void MAIN_vTick10msLp(void)
{
  static uint8 lui8t = 0;
  TP_Scan(0);

  //mcI2C1->bStartNext();

  if (lui8t == 4)
  {
    cMsg lcMsg;

    mcDiffTimer.start();
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

    ILI9341_Show(&mc16GBm);
  }
  else
  {
    lui8t++;
  }
}


void MAIN_vTick1000msLp(void)
{
  static uint8 lui8t = 0;
  mcLedRed.Toggle();
  lui8t++;
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


  mc16GuiRoot.bAdd(10, 10, 250, 150, &mc16GuiWindow01);
  mc16GuiRoot.mstStatus.Sizeable = 0;
  mc16GuiRoot.mstStatus.DragAble = 0;
  mc16GuiRoot.mstStatus.PaintFrame = 0;
  mc16GuiWindow01.vToggleMaximize();

  #ifndef _WIN32

    CycCall_Start(NULL /*1ms_HP*/,
                  NULL /*10ms_HP*/,
                  NULL /*100ms_HP*/,
                  NULL /*1s_HP*/,

                  NULL               /*1ms_LP*/,
                  MAIN_vTick10msLp   /*10ms_LP*/,
                  NULL               /*100ms_LP*/,
                  MAIN_vTick1000msLp /*1s_LP*/);
  #endif
}


/* Main functions ---------------------------------------------------------*/

int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    __asm("wfi");
  }
}

