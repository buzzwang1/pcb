#pragma once

#include "cSysDPool.h"
#include "ili9341.h"
#include "XPT2046.h"
#include "cSPi.h"


//Graphics
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "rfont.h"
#include "paint.h"
#include "uielement_box3df_viewer_txgradz.h"
#include "uielement_box3df_viewer_flat.h"
#include "uielement.h"
#include "cursor.h"
#include "msg.h"

//Resource
#include "ResBase.h"
#include "RFont01_05x05U_1BPP_1G.h"
#include "RFont01_06x08_1BPP_1G.h"

#ifdef __cplusplus
extern "C" {
#endif

  extern void  vTaskAppGfx(void* argument);

#ifdef __cplusplus
}
#endif

class cCompGfx : public cComponent
{
  public:

  cIli9341 mcILI9341;
  cXPT2046 mcXPT2046;

  cMsgBox  mcMsgBox;
  cCursor  mcCursor;

  u8   mu8ShowCnt;
  u8   u8DisplayDim_Percent_old;

  cCompGfx()
    : cComponent(cDepTreeCfg::cComp::nGfx, cDepTreeCfg::cComp::nGfxSpi2),
      mcILI9341(cSysDPool::mBoard.mcSpi2),
      mcXPT2046(cSysDPool::mBoard.mcSpi2),
      mcMsgBox(20),
      mcCursor(200, 800, &mcMsgBox)
  {
    cSysDPool::mBoard.mcGfx.u16TouchRawXMax = 0;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMax = 0;
    cSysDPool::mBoard.mcGfx.u16TouchRawXMin = 0xFFFF;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMin = 0xFFFF;

    u8DisplayDim_Percent_old = cSysDPool::mBoard.mcGfx.u8DisplayDim_Percent = 25;

    cSysDPool::mBoard.mcGfx.mpcILI9341 = &mcILI9341;
    cSysDPool::mBoard.mcGfx.mpcMsgBox  = &mcMsgBox;
    cSysDPool::mBoard.mcGfx.mpcCursor  = &mcCursor;

    cSysDPool::mGfx.pu8GfxRam = mcILI9341.pGRAM();

    mu8ShowCnt = 0;
  }

  bool bInit() override
  {
    cSysDPool::mBoard.mcGfx.u16TouchRawXMax = 3800;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMax = 3800;
    cSysDPool::mBoard.mcGfx.u16TouchRawXMin = 260;
    cSysDPool::mBoard.mcGfx.u16TouchRawYMin = 380;


    mcILI9341.vInitHw();
    mcXPT2046.vInitHw();

    // Mit AddNode wird auch die Init Sequenzen requested

    cSysDPool::mBoard.mcSpi2->vAddNode(&mcILI9341);
    cSysDPool::mBoard.mcSpi2->vAddNode(&mcXPT2046);

    vWaitCom();

    mcILI9341.vSetLigth(cSysDPool::mBoard.mcGfx.u8DisplayDim_Percent);


    mcILI9341.vClearGRAM();
    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ07WriteGRAMCfg);
    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ08WriteGRAMData);

    vGfxStart();

    cComponentList::mcList16ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    vGfxTerminate();

    cComponentList::mcList16ms.vRemove(this->mu8Idx);

    mcILI9341.vClearAllRequest();
    mcXPT2046.vClearAllRequest();

    vWaitCom();

    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ05Disable);
    mcXPT2046.vRequest(cIli9341_RegisterMap::nSEQ05Disable);

    vWaitCom();

    mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ06DeInit);
    mcXPT2046.vRequest(cIli9341_RegisterMap::nSEQ06DeInit);

    vWaitCom();

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    u8 lu8Touch = 0;

    if (mcXPT2046.mIrq.ui8Get() == 0)
    {
      if (mcXPT2046.isDone((u8)cXPT2046_RegisterMap::nSEQ07_ReadXYZ))
      {
        u16 lu16Xa;
        u16 lu16Xb;
        u16 lu16Ya;
        u16 lu16Yb;
        u16 lu16XDiff;
        u16 lu16YDiff;


        lu16Xa = (((u16)mcXPT2046.stRegs.unCtrlXa.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlXa.au8Byte[1]);
        lu16Xb = (((u16)mcXPT2046.stRegs.unCtrlXb.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlXb.au8Byte[1]);
        lu16Ya = (((u16)mcXPT2046.stRegs.unCtrlYa.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlYa.au8Byte[1]);
        lu16Yb = (((u16)mcXPT2046.stRegs.unCtrlYb.au8Byte[0] << 8)  + mcXPT2046.stRegs.unCtrlYb.au8Byte[1]);

        lu16Xa = (lu16Xa & 0x7FFF) >> 3;
        lu16Xb = (lu16Xb & 0x7FFF) >> 3;
        lu16Ya = (lu16Ya & 0x7FFF) >> 3;
        lu16Yb = (lu16Yb & 0x7FFF) >> 3;


        if (lu16Xa > lu16Xb) lu16XDiff = lu16Xa - lu16Xb;
                        else lu16XDiff = lu16Xb - lu16Xa;

        if (lu16Ya > lu16Yb) lu16YDiff = lu16Ya - lu16Yb;
                        else lu16YDiff = lu16Yb - lu16Ya;


        if ((lu16XDiff < 25) && (lu16YDiff < 25))
        {
          u16 lu16X = (lu16Xa + lu16Xb) / 2;
          u16 lu16Y = (lu16Ya + lu16Yb) / 2;


          cSysDPool::mBoard.mcGfx.u16TouchRawX = lu16X;
          cSysDPool::mBoard.mcGfx.u16TouchRawY = lu16Y;

          //if (lu16X > cSysDPool::mBoard.mcGfx.u16TouchRawXMax) cSysDPool::mBoard.mcGfx.u16TouchRawXMax = lu16X;
          //if (lu16Y > cSysDPool::mBoard.mcGfx.u16TouchRawYMax) cSysDPool::mBoard.mcGfx.u16TouchRawYMax = lu16Y;
          //if (lu16X < cSysDPool::mBoard.mcGfx.u16TouchRawXMin) cSysDPool::mBoard.mcGfx.u16TouchRawXMin = lu16X;
          //if (lu16Y < cSysDPool::mBoard.mcGfx.u16TouchRawYMin) cSysDPool::mBoard.mcGfx.u16TouchRawYMin = lu16Y;

          float lfXDiffMinMax = cSysDPool::mBoard.mcGfx.u16TouchRawXMax - cSysDPool::mBoard.mcGfx.u16TouchRawXMin;
          float lfYDiffMinMax = cSysDPool::mBoard.mcGfx.u16TouchRawYMax - cSysDPool::mBoard.mcGfx.u16TouchRawYMin;

          // X und Y tauchen für landscape
          // und noch spiegel
          cSysDPool::mBoard.mcGfx.u16TouchX = 319 - (float)(lu16Y - cSysDPool::mBoard.mcGfx.u16TouchRawYMin) * 320.0f / lfYDiffMinMax;
          cSysDPool::mBoard.mcGfx.u16TouchY = 239 - (float)(lu16X - cSysDPool::mBoard.mcGfx.u16TouchRawXMin) * 240.0f / lfXDiffMinMax;

          lu8Touch = 1;
        }
      }
      mcXPT2046.vRequest(cXPT2046_RegisterMap::nSEQ07_ReadXYZ);
    }

    mcCursor.vTick_ms(cSysDPool::mBoard.mcGfx.u16TouchX, cSysDPool::mBoard.mcGfx.u16TouchY, lu8Touch, 16);
    cSysDPool::mBoard.mcGfx.u8TouchIrq = lu8Touch;

    mu8ShowCnt++;
    // Maximial alle 48ms, ca. 20 Bilder / s
    if (mu8ShowCnt >= 3)
    {
      mu8ShowCnt = 0;

      if (cSysDPool::mBoard.mcGfx.bShowScreen)
      {
        cSysDPool::mBoard.mcGfx.bShowScreen = False;
        mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ07WriteGRAMCfg);
        mcILI9341.vRequest(cIli9341_RegisterMap::nSEQ08WriteGRAMData);
      }

      if (u8DisplayDim_Percent_old != cSysDPool::mBoard.mcGfx.u8DisplayDim_Percent)
      {
        u8DisplayDim_Percent_old = cSysDPool::mBoard.mcGfx.u8DisplayDim_Percent;
        mcILI9341.vSetLigth(cSysDPool::mBoard.mcGfx.u8DisplayDim_Percent);
      }
    }

    if (mcILI9341.isSeqDone(cIli9341_RegisterMap::nSEQ08WriteGRAMData))
    {
      mcILI9341.vDoneClear(cIli9341_RegisterMap::nSEQ08WriteGRAMData);
      osThreadResume(cSysDPool::mSys.mcTasks.Gfx.Handle);
    }

    // return True to signal finished
    return cComponent::bRun();
  };


  void vWaitCom()
  {
    while (1)
    {
      // Warten bis Com Sequenz fertig ist
      // Bei Error wird auch Done gesetzt, von daher braucht man das hier nicht extra testen
      if ((mcILI9341.isAnySeqPending()) ||
          (mcXPT2046.isAnySeqPending()))
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
      else
      {
        break;
      }
    }
  }


  void vGfxStart()
  {
    const osThreadAttr_t TaskGfx_attributes = {
      .name       = "cAppGfx",                               ///< name of the thread
      .attr_bits  = 0,                                       ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.Gfx.Tcb,        ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.Gfx.Tcb), ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.Gfx.Stack,       ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.Gfx.StackSize(), ///< size of stack
      .priority   = (osPriority_t)osPriorityLow,             ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };

    cSysDPool::mSys.mcTasks.Gfx.Handle = osThreadNew(vTaskAppGfx, (void*)null, &TaskGfx_attributes);
  }

  void vGfxTerminate()
  {
    osThreadTerminate(cSysDPool::mSys.mcTasks.Gfx.Handle);
  }


  static void vGfxTask()
  {
    cIli9341* lcILI9341 = cSysDPool::mBoard.mcGfx.mpcILI9341;

    cMsgBox*  lpcMsgBox = cSysDPool::mBoard.mcGfx.mpcMsgBox;
    cCursor*  lpcCursor = cSysDPool::mBoard.mcGfx.mpcCursor;

    cBitmap_Bpp16_5R6G5B                     mc16GBm(lcILI9341->mu16Height, lcILI9341->mu16Width, lcILI9341->pGRAM());
    cScreen_Bpp16_5R6G5B                     mc16GScreen1(&mc16GBm);

    cSprite_Res8b_Bpp1_1G                    mc16GSprite;
    cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

    cRFont_Res8b_Bpp1_1G                     mcRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
    cRFont_Res8b_Bpp1_1G                     mcRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

    cUiElement_Root                          mc16GuiRoot(0, 0, 320, 240, lpcMsgBox, &mc16GScreen1, BM_BPP32_8R8G8B_RGB(0xFF, 0, 0));

    class cUiElement_MyWindow: public cUiElement_Window
    {
      public:
        cUiElement_VSplit        mc16GuiVSplit;

        cUiElement_SwitchGroup  mcSwitchChnGroup;

        cUiElement_SwitchPressOnly  mcSwitchChn1;
        cUiElement_SwitchPressOnly  mcSwitchChn2;
        cUiElement_SwitchPressOnly  mcSwitchChn3;
        cUiElement_SwitchPressOnly  mcSwitchChn4;

        cUiElement_TextBox          mcText;

        u8 miSelectionIdx;

        cUiElement_MyWindow(cMsgBox *lcMsgBox, cScreen *lpcScreen,
                            uint32 lui32MaxCharacterCount,
                            cRFont *lpcRFont)
          : cUiElement_Window(lcMsgBox, lpcScreen, lui32MaxCharacterCount, lpcRFont),
            mc16GuiVSplit(lcMsgBox, lpcScreen, 25, 40, -1),
            mcSwitchChnGroup(lcMsgBox, lpcScreen),
            mcSwitchChn1(lcMsgBox, lpcScreen, 8, lpcRFont),
            mcSwitchChn2(lcMsgBox, lpcScreen, 8, lpcRFont),
            mcSwitchChn3(lcMsgBox, lpcScreen, 8, lpcRFont),
            mcSwitchChn4(lcMsgBox, lpcScreen, 8, lpcRFont),

            mcText(lcMsgBox,   lpcScreen, 1024, lpcRFont)
        {
          miSelectionIdx = 0;
        }


        virtual void OnLoaded() override
        {
          cUiElement_Window::OnLoaded();

          this->vSetTitle((const char*)"Monitor");

          this->bAdd(0, 0, 0, 0, &mc16GuiVSplit);
          mc16GuiVSplit.mcElementRight.bAdd(0, 0, 0, 0, &mcText);
          mc16GuiVSplit.vToggleMaximize();
          mcText.vToggleMaximize();

          mc16GuiVSplit.mcElementLeft.bAdd(0,0,0,0, &mcSwitchChnGroup);
          mcSwitchChnGroup.vToggleMaximize();

          mcSwitchChn1.vSetText((const char*)"V-Mon");
          mcSwitchChn2.vSetText((const char*)"Bq");
          mcSwitchChn3.vSetText((const char*)"Chn3");
          mcSwitchChn4.vSetText((const char*)"Chn4");

          mcSwitchChnGroup.bAdd(2,  2,      35, 14, &mcSwitchChn1);
          mcSwitchChnGroup.bAdd(2,  4 + 14, 35, 14, &mcSwitchChn2);
          mcSwitchChnGroup.bAdd(2,  6 + 28, 35, 14, &mcSwitchChn3);
          mcSwitchChnGroup.bAdd(2,  8 + 42, 35, 14, &mcSwitchChn4);

          mcText.vSetText((const char*)"");
        }

        void vUpdateText()
        {
          cStr_Create(lszStr, 64);

          switch (miSelectionIdx)
          {
            case 1:
              {
                cStr lcTextBox((const char8*)mcText.mChText, 0, mcText.mui32MaxCharacterCount);
                lcTextBox += lszStr.Setf(" SysIn:   %dmV %dmA\n", (i16)cSysDPool::mBoard.mcMonitor.mfSysInV_mV, (i16)cSysDPool::mBoard.mcMonitor.mfSysInI_mA);
                lcTextBox += lszStr.Setf(" PomoIn:  %dmV %dmA\n", (i16)cSysDPool::mBoard.mcMonitor.mfPomoInV_mV, (i16)cSysDPool::mBoard.mcMonitor.mfPomoInI_mA);
                lcTextBox += lszStr.Setf(" PomoOut: %dmV %dmA\n\n", (i16)cSysDPool::mBoard.mcMonitor.mfPomoOutV_mV, (i16)cSysDPool::mBoard.mcMonitor.mfPomoOutI_mA);

                cBQ25798* lpcBQ25798 = cSysDPool::mBoard.mcCharger.mpcBQ25798;
                lcTextBox += lszStr.Setf(" Bus:   %dmV %dmA\n", (u16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG35_VBUS_ADC), (i16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG31_IBUS_ADC));
                lcTextBox += lszStr.Setf(" Bat:   %dmV %dmA\n", (u16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG3B_VBAT_ADC), (i16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG33_IBAT_ADC));
                lcTextBox += lszStr.Setf(" VAC:   %dmV %dmV\n", (u16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG37_VAC1_ADC), (u16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG39_VAC2_ADC));

                lcTextBox += lszStr.Setf(" Sys:   %dmV\n", (u16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG3D_VSYS_ADC));

                lcTextBox += lszStr.Setf(" Tmp:   %d°C %d°C\n\n", (i16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG3F_TS_ADC), (i16)lpcBQ25798->fGetAdc(cBQ25798_RegisterMap::nREG41_TDIE_ADC));


                lcTextBox += lszStr.Setf(" CS0: PG: %d, AC1: %d, AC2: %d, VBus: %d\n", lpcBQ25798->tunREG1B_Charger_Status_0.stBits.PG_STAT,
                                                                                       lpcBQ25798->tunREG1B_Charger_Status_0.stBits.AC1_PRESENT_STAT,
                                                                                       lpcBQ25798->tunREG1B_Charger_Status_0.stBits.AC2_PRESENT_STAT,
                                                                                       lpcBQ25798->tunREG1B_Charger_Status_0.stBits.VBUS_PRESENT_STAT);
                lcTextBox += lszStr.Setf(" CS1: CHG_S: %d, VBUS_S: %d\n", lpcBQ25798->tunREG1C_Charger_Status_1.stBits.CHG_STAT,
                                                                          lpcBQ25798->tunREG1C_Charger_Status_1.stBits.VBUS_STAT);
                lcTextBox += lszStr.Setf(" CS2: ICO: %d, TREG: %d, VBAT: %d\n", lpcBQ25798->tunREG1D_Charger_Status_2.stBits.ICO_STAT,
                                                                                lpcBQ25798->tunREG1D_Charger_Status_2.stBits.TREG_STAT,
                                                                                lpcBQ25798->tunREG1D_Charger_Status_2.stBits.VBAT_PRESENT_STAT);
                lcTextBox += lszStr.Setf(" CS3: T1: %d, T2: %d, VSYS: %d, VBus: %d\n", lpcBQ25798->tunREG1E_Charger_Status_3.stBits.ACRB1_STAT,
                                                                                       lpcBQ25798->tunREG1E_Charger_Status_3.stBits.ACRB2_STAT,
                                                                                       lpcBQ25798->tunREG1E_Charger_Status_3.stBits.VSYS_STAT);
                /*lcTextBox +=*/ lszStr.Setf(" CS4: COLD: %d, COOL: %d, WARM: %d, HOT: %d\n\n", lpcBQ25798->tunREG1F_Charger_Status_4.stBits.TS_COLD_STAT,
                                                                                            lpcBQ25798->tunREG1F_Charger_Status_4.stBits.TS_COOL_STAT,
                                                                                            lpcBQ25798->tunREG1F_Charger_Status_4.stBits.TS_WARM_STAT,
                                                                                            lpcBQ25798->tunREG1F_Charger_Status_4.stBits.TS_HOT_STAT);

                lcTextBox += lszStr;

                lcTextBox += lszStr.Setf(" FS0: IBAT: %d, BUS_OV: %d, BAT_OV: %d, BUS_OC: %d\n", lpcBQ25798->tunREG20_FAULT_Status_0.stBits.IBAT_REG_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.VBUS_OVP_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.VBAT_OVP_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.IBUS_OCP_STAT);
                lcTextBox += lszStr.Setf(" FS0: BAT_OC: %d, CON_OC: %d, V2_OV: %d, V1_OV: %d\n", lpcBQ25798->tunREG20_FAULT_Status_0.stBits.IBAT_OCP_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.CONV_OCP_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.VAC2_OVP_STAT,
                                                                                                 lpcBQ25798->tunREG20_FAULT_Status_0.stBits.VAC1_OVP_STAT);
                lcTextBox += lszStr.Setf(" FS1: SYS_SH: %d, SYS_OV: %d, TS_OV: %d\n\n", lpcBQ25798->tunREG21_FAULT_Status_1.stBits.VSYS_SHORT_STAT,
                                                                                        lpcBQ25798->tunREG21_FAULT_Status_1.stBits.VSYS_OVP_STAT,
                                                                                        lpcBQ25798->tunREG21_FAULT_Status_1.stBits.TSHUT_STAT);


                lcTextBox.ToString();
                mcText.vRepaint();
              }
              break;
            case 2:
              {
                cStr lcTextBox((const char8*)mcText.mChText, 0, mcText.mui32MaxCharacterCount);
                cBQ25798* lpcBQ25798 = cSysDPool::mBoard.mcCharger.mpcBQ25798;

                lcTextBox += lszStr.Setf(" R00: VSYSMIN: %dmV\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG00_MIN_SYS_VOLTAGE, 0));
                lcTextBox += lszStr.Setf(" R01: BAT_LIM: %dmV\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG01_CHG_VOLTAGE_LIMIT, 0));
                lcTextBox += lszStr.Setf(" R03: CHG_LIM: %dmA\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG03_CHG_CURRENT_LIMIT, 0));
                lcTextBox += lszStr.Setf(" R05: IN_LIM: %dmV\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG05_INPUT_VOLTAGE_LIMIT, 0));
                lcTextBox += lszStr.Setf(" R06: IN_LIM: %dmA\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG06_INPUT_CURRENT_LIMIT, 0));
                lcTextBox += lszStr.Setf(" R08: BAT_LOW: %dmV, PRECHG: %dmA\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG08_PRECHARGE_CTRL, 0), (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG08_PRECHARGE_CTRL, 1));
                lcTextBox += lszStr.Setf(" R09: TERM: %dmA\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG09_TERM_CTRL, 0));
                lcTextBox += lszStr.Setf(" R0A: CELL: %ds, RECHG: %dms, RECHG: %dmV\n\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG0A_RECHARGE_CTRL, 0),
                                                                                          (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG0A_RECHARGE_CTRL, 1),
                                                                                          (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG0A_RECHARGE_CTRL, 2));
                lcTextBox += lszStr.Setf(" R0F: CHG: %d, ICO: %d, HIZ: %d, TERM: %d\n", lpcBQ25798->tunREG0F_Charger_Control_0.stBits.EN_CHG,
                                                                                        lpcBQ25798->tunREG0F_Charger_Control_0.stBits.EN_ICO,
                                                                                        lpcBQ25798->tunREG0F_Charger_Control_0.stBits.EN_HIZ,
                                                                                        lpcBQ25798->tunREG0F_Charger_Control_0.stBits.EN_TERM);
                lcTextBox += lszStr.Setf(" R10: BUS_BK: %dmV, AC_OVP: %dmV\n", (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG10_CHARGER_CTRL_1, 0),
                                                                               (u16)lpcBQ25798->fGetSetting(cBQ25798_RegisterMap::nREG10_CHARGER_CTRL_1, 1));

                lcTextBox += lszStr.Setf(" R13: FREQ: %dMhz, DISSTAT: %d\n", (lpcBQ25798->tunREG13_Charger_Control_4.stBits.PWM_FREQ+1) * 750,
                                                                              lpcBQ25798->tunREG13_Charger_Control_4.stBits.DIS_STAT);
                lcTextBox += lszStr.Setf(" R14: EN_IBAT: %d\n\n", (lpcBQ25798->tunREG14_Charger_Control_5.stBits.EN_IBAT));
                lcTextBox += lszStr.Setf(" R14: EN_MPPT: %d\n\n", (lpcBQ25798->tunREG15_MPPT_Control.stBits.EN_MPPT));


                lcTextBox.ToString();
                mcText.vRepaint();
              }
              break;
            default:
              mcText.vSetText((const char*)" abcf4: 500\n xyz: 500");
              break;
          }
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
              miSelectionIdx = 1;
            }
            else if (lpcChild == &mcSwitchChn2)
            {
              miSelectionIdx = 2;
            }
            else if (lpcChild == &mcSwitchChn3)
            {
              miSelectionIdx = 3;
            }
            else if (lpcChild == &mcSwitchChn4)
            {
              miSelectionIdx = 4;
            }
          }

          if (lenEvent == cUiElement::tenEvent::nBtnPressStart)
          {
          }

          cUiElement_Window::vParentCallback(lpcChild, lenEvent);
        }
    };

    cUiElement_MyWindow  mc16GuiWindow01(lpcMsgBox, &mc16GScreen1, 16, &mcRFont_Res8b_Bpp1_1G_Full);

    mc16GuiRoot.bAdd(10, 10, 250, 150, &mc16GuiWindow01);
    mc16GuiRoot.mstStatus.Sizeable = 0;
    mc16GuiRoot.mstStatus.DragAble = 0;
    mc16GuiRoot.mstStatus.PaintFrame = 0;
    mc16GuiWindow01.vToggleMaximize();

    while (1)
    {
      //if (cSysDPool::mBoard.mcGfx.u8TouchIrq)
      {
        cMsg lcMsg;

        while (!lpcMsgBox->isEmpty())
        {
          lpcMsgBox->vget(&lcMsg);
          mc16GuiRoot.vMsgHdl(&lcMsg);
        }
        mc16GuiWindow01.vUpdateText();
        mc16GuiRoot.vPaint();

        mc16GSpriteEng.vSetParam(Sprite_nModeOr, &mc16GSprite, &mc16GScreen1);
        mcRFont_Res8b_Bpp1_1G_5x5Ucase.mui32Col = mc16GScreen1.u32GetCol(0xFFFFFF);
        mcRFont_Res8b_Bpp1_1G_5x5Ucase.i8PutStringXY(140, 10, lpcCursor->toString(), &mc16GScreen1);

        cPaint::vRect(cSysDPool::mBoard.mcGfx.u16TouchX - 4, cSysDPool::mBoard.mcGfx.u16TouchY - 4, 8, 8, BM_BPP32_8R8G8B_RGB(255, 0, 0), &mc16GScreen1);

        cSysDPool::mBoard.mcGfx.bShowScreen = True;
      }

      vTaskSuspend(NULL);
      //vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
};



void vTaskAppGfx(void* argument)
{
  UNUSED(argument);
  while (1)
  {
    cCompGfx::vGfxTask();
  }
}

