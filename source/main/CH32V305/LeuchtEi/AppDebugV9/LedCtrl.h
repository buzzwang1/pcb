#ifndef __WS2812_CONTROL_H__
#define __WS2812_CONTROL_H__

#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_adc.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_tim.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_misc.h"

#include "tGPPin.h"
#include "LED.h"
#include "ClockInfo.h"


#define LEDCTRL_RGB_MEMSIZE(nLedCnt) 3 *  nLedCnt
#define LEDCTRL_LED_MEMSIZE(nLedCnt) 24 * nLedCnt + 1 // + 1 für 0xFF am Ende. Damit high level ausgegeben wird


class cWs2812
{
  public:

  enum
  {
    nV_ntc   = ADC_Channel_5,
    nV_shunt = ADC_Channel_6,
    // VRefInt 1.2V, entspricht 1489 digit @ 3300mV
    nV_vref  = ADC_Channel_Vrefint,


    nIdx_ADC1_ntc1 = 0,
    nIdx_ADC1_ntc2 = 1,
    nIdx_ADC1_ntc3 = 2,
    nIdx_ADC1_ntc4 = 3,
    nIdx_ADC1_ntc5 = 4,
    nIdx_ADC1_vref = 4,

    nIdx_ADC2_shunt1 = 0,
    nIdx_ADC2_shunt2 = 1,
    nIdx_ADC2_shunt3 = 2,
    nIdx_ADC2_shunt4 = 3,
    nIdx_ADC2_shunt5 = 4,
    nChnCount = 5,
  };

  // --- ADC
  u32    mAdcResult[nChnCount]; // Result ADC1 and ADC2
  i16    mi16ADC1_Calibrattion_Val;
  i16    mi16ADC2_Calibrattion_Val;
  i16    mi16ShuntCur;
  i16    mi16Temp;
  i16    mi16TempDigi;
  i16    mi16ShuntDigi;
  i16    mi16ShuntOffset;
  u16    mu16TempAdc1;
  u16    mu16TempAdc2;

  // --- Pins
  cGpPin mcLedSig;
  cGpPin mcEn;

  // --- WS2812
  u16    mu16LedCnt;
  u8     mu8BrigthnessSet; // 0..255 = 0 .. 100%
  u8     mu8Brigthness;    // 0..255 = 0 .. 100%

  u8     mu8StateProcess;

  // raw sensor inputs
  u8 *mu8GfxData; // Gfx Buffer, 3 Byte per Pxl
  u8* mu8LedData; // Led Buffer, 24 Byte per Pxl

  cWs2812(u8* lu8RgbData, u8* lu8LedbData, u16 lu16LedCnt)
    : mcLedSig(GPIOA_BASE,  8, GPIO_Mode_AF_PP,  GPIO_Speed_10MHz, 0),
      mcEn(    GPIOC_BASE,  8, GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 0)
  {
    mu8GfxData  = lu8RgbData;
    mu8LedData  = lu8LedbData;
    mu16LedCnt  = lu16LedCnt;


    // configure analog inputs
    cGpCfg::vSet(GPIOA, 5, cGpCfg::nAnalog, cGpCfg::nInput); // Ntc
    cGpCfg::vSet(GPIOA, 6, cGpCfg::nAnalog, cGpCfg::nInput); // Shunt

    mu8BrigthnessSet = 128; // 50%
    mu8Brigthness    = 0;   // 0%

    for (u16 lu16t = 0; lu16t < LEDCTRL_RGB_MEMSIZE(mu16LedCnt); lu16t++)
    {
      mu8GfxData[lu16t] = 0;
    }

    vShow();

    vTim1_Dma_vInit();
    vTim1_Init_WS2812();

    vADC1_2_DmaInit();
    vADC1_2_Init();

    mi16ShuntCur = 0;
    mi16Temp = 0;
    mu8StateProcess = 0;

    mu16TempAdc1 = 100;
    mu16TempAdc2 =  77;
  }

  void vSetBrigthness(u8 lu8Val)
  {
    mu8BrigthnessSet = lu8Val;
  }

  virtual void vBrigthnessCtrl()
  {
    if (mu8Brigthness < mu8BrigthnessSet) mu8Brigthness++;
    else
    if (mu8Brigthness > mu8BrigthnessSet) mu8Brigthness--;
  }

  void vTurnOnOffCtrl(void)
  {
    switch (mu8StateProcess)
    {
      case 1: //Reset WS2812: 10ms '0' mit dem Timer ausgeben
        TIM1->CH1CVR = 0;
        mu8StateProcess = 2;
        break;
      case 2: //DMA Ausgabe starten. Wird von DMA IRQ beendet
        // Start DMA
        DMA1_Channel2->CNTR = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
        DMA1_Channel2->MADDR = (u32)mu8LedData;
        DMA1_Channel2->CFGR |= ((1 << 0)  | (1 << 1)); // EN + TCIE
        TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
        mu8StateProcess = 3;
        break;
      case 3:
        //Wait for DMA
        mu8StateProcess = 0;
        break;
      case 4:
        mu8StateProcess = 0;
        break;
      default:
        mu8StateProcess = 0;
    }
  }

  void vShow()
  {
    u16 lu16LedIdx = 0;
    u16 lu16NewValR;
    u16 lu16NewValG;
    u16 lu16NewValB;

    if (mu8StateProcess == 0)
    {
      for (u16 lu16t = 0; lu16t < mu16LedCnt; lu16t++)
      {
        lu16NewValR = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;
        lu16NewValG = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;
        lu16NewValB = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;

        lu16NewValR /= 256;
        lu16NewValG /= 256;
        lu16NewValB /= 256;

        vSetLedMem(lu16t, lu16NewValR, lu16NewValG, lu16NewValB);
      }
      mu8LedData[LEDCTRL_LED_MEMSIZE(mu16LedCnt) - 1] = 0xFF;

      mu8StateProcess = 1;
    }
  }

  void vTick10msLp(void)
  {
    mi16ShuntCur = i16GetCurrent();
    mi16Temp     = i16GetNtcTemp();

    vBrigthnessCtrl();
    vShow();
    vTurnOnOffCtrl();
  }

  void vSetLedPxl(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 3 * lu16LedIdx;
      
    mu8GfxData[lu16LedIdx] = lu8R; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8G; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8B;
  }


  void vSetLedMem(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 24 * lu16LedIdx;
  
    if (lu8G & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8G &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8G &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8G &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8G &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8G &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8G &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8G &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8R & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8R &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8R &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8R &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8R &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8R &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8R &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8R &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8B & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8B &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8B &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8B &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8B &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8B &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8B &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8B &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;
  }

  void vEnable()
  {
    mcEn.vSet1();
  }

  void vDisable()
  {
    mcEn.vSet0();
  }

  i16 i16GetAdc1(u8 lu8Idx)
  {
    i16 li16AdcValue = mAdcResult[lu8Idx] & 0xffff;
    li16AdcValue    += mi16ADC1_Calibrattion_Val;
    if (li16AdcValue < 0)    return 0;
    if (li16AdcValue > 4095) return 4095;
    return (li16AdcValue);
  }

  i16 i16GetAdc2(u8 lu8Idx)
  {

    i16 li16AdcValue = mAdcResult[lu8Idx] >> 16;
    u16 u16Gain = 16;
    li16AdcValue    += (mi16ADC2_Calibrattion_Val * u16Gain);
    if (li16AdcValue < 0)    return 0;
    if (li16AdcValue > 4095) return 4095;
    return (li16AdcValue);
  }

  i16 i16GetCurrent()
  {
    i32 li32Zwerg;

    li32Zwerg  = i16GetAdc2(nIdx_ADC2_shunt1);
    li32Zwerg += i16GetAdc2(nIdx_ADC2_shunt2);
    li32Zwerg += i16GetAdc2(nIdx_ADC2_shunt3);
    li32Zwerg += i16GetAdc2(nIdx_ADC2_shunt4);
    li32Zwerg += i16GetAdc2(nIdx_ADC2_shunt5);
    li32Zwerg /= 5;

    mi16ShuntDigi = li32Zwerg;
    
    // VRef = 1200mV => 4096/3300*1200 = 1489d (0x5D2)

    i32 li32VRef, li32VRef3300;
    li32VRef = i16GetAdc1(nIdx_ADC1_vref);
    li32VRef3300 = (4096 * 1200) / li32VRef;

    // 50mOhm Shunt LED Strom:  ADC2 ADC_IN6 PGA 16x 
    // ADC_Ref = 3300mV / 16 ~= 206mV => 206mV / 50mOhm = 4.125A (max)
    // ADC_Res = 4096 => ~1,01mA/digit
    // 
    //     16 * 50 * 4096     3276800         3277 
    // => ---------------- = ---------- ~ ------------ [mA]
    //      3300 * 1000       3300000     (3300 * ADC)
    //

    li32Zwerg = (li32Zwerg * 3277) / li32VRef3300;

    return (i16)li32Zwerg;
  }

  i16 i16GetNtcTemp()
  {
    i32 li32Zwerg;

    // ADC-Werte für 10kOhm in Reihe mit 10KOhm NTC, B = 3380K
    
    // z.B. bei  20°C NTC =  8,23k => U = 1,81V => 2243
    //           25°C NTC = 10,00k => U = 1,65V => 2048
    //           30°C NTC = 12,00k => U = 1,50V => 1859
                   
    const u16 lu16NtcRomTable[30] = {3897, 3837, 3764, 3676, 3572,  // -40°C, -35°C, -30°C, -25°C, -20°C,
                                     3452, 3316, 3163, 3995, 2817,  // -15°C, -10°C,  -5°C,   0°C,   5°C,
                                     2629, 2436, 2241, 2048, 1860,  //  10°C,  15°C,  20°C,  25°C,  30°C,
                                     1679, 1509, 1350, 1204, 1070,  //  35°C,  40°C,  45°C,  50°C,  55°C,
                                      949,  842,  746,  661,  586,  //  60°C,  65°C,  70°C,  75°C,  80°C,
                                      519,  461,  409,  364,  324}; //  85°C,  90°C,  95°C, 100°C, 105°C, 

                                              //Shunt Spannung abziehen
    li32Zwerg =  i16GetAdc1(nIdx_ADC1_ntc1) - i16GetAdc2(nIdx_ADC2_shunt1) / 16;
    li32Zwerg += i16GetAdc1(nIdx_ADC1_ntc2) - i16GetAdc2(nIdx_ADC2_shunt2) / 16;
    li32Zwerg += i16GetAdc1(nIdx_ADC1_ntc3) - i16GetAdc2(nIdx_ADC2_shunt3) / 16;
    li32Zwerg += i16GetAdc1(nIdx_ADC1_ntc4) - i16GetAdc2(nIdx_ADC2_shunt4) / 16;
    li32Zwerg /= 4;

    mi16TempDigi = li32Zwerg;

    //i32 li32VRef;
    //li32VRef   = i16GetAdc1(nIdx_ADC1_vref);
    

    i16 li16AdcNtc = (i16)li32Zwerg;

    if (li16AdcNtc >= lu16NtcRomTable[0])  return (-40 * mu16TempAdc1) / mu16TempAdc2;
    if (li16AdcNtc <= lu16NtcRomTable[29]) return (105 * mu16TempAdc1) / mu16TempAdc2;

    for (u8 lu8Idx = 1; lu8Idx < sizeof(lu16NtcRomTable) / 2; lu8Idx++)
    {
      if (li16AdcNtc > lu16NtcRomTable[lu8Idx])
      {
         i16 li16DiffDigit = lu16NtcRomTable[lu8Idx-1] - lu16NtcRomTable[lu8Idx];
         i16 li16DiffTemp  = -5;
         i16 li16IdxTemp   = -40 - li16DiffTemp * lu8Idx;

         li32Zwerg = li16IdxTemp + ((li16AdcNtc - lu16NtcRomTable[lu8Idx]) * li16DiffTemp) / li16DiffDigit;
         li32Zwerg = (li32Zwerg * mu16TempAdc1) / mu16TempAdc2;
         return (i16)li32Zwerg;
      }
    }
    return 0;
  }

  void vADC1_2_DmaInit()
  {
    // dma_ch0 config used for ADC
    // enable the DMA clock
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(mAdcResult);
    DMA_InitStructure.DMA_DIR            = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize     = nChnCount;
    DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc      = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode           = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority       = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M            = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
  }

  void vADC1_2_Init()
  {
    // ------------ ADC_Config
    // enable the DMA clock
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8); // => 96 / 8 => 12Mhz

    cClockInfo::mstClocks.ADCCLK_Frequency = cClockInfo::mstClocks.HCLK_Frequency / 8;

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult; // Regular simultaneous mode
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = nChnCount;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &ADC_InitStructure);


    // For the 12-bit resolution, the total conversion time is sampling time + 12.5” ADC_CLK cycles
    //
    //  1.5cycles: Total conversion time:  1.5 + 12.5 = 14cycles =  1,12us
    //  7.5cycles: Total conversion time:  7.5 + 12.5 = 20cycles =  1,66us
    // 13.5cycles: Total conversion time: 13.5 + 12.5 = 26cycles =  2,16us
    // 28.5cycles: Total conversion time: 28.5 + 12.5 = 41cycles =  3,41us
    // 41.5cycles: Total conversion time: 41.5 + 12.5 = 54cycles =  4,50us
    // 55.5cycles: Total conversion time: 55.5 + 12.5 = 68cycles =  5,60us
    // 71.5cycles: Total conversion time: 71.5 + 12.5 = 84cycles =  7,00us
    //239.5cycles: Total conversion time:239.5 + 12.5 =252cycles = 21,00us

    // ADC regular channel config
    // Alle 5 Kanäle sollte innerhalb 50us gesammpled werden,
    // weil Timer0 mit 20khz läuft.
    ADC_RegularChannelConfig(ADC1, nV_ntc,  nIdx_ADC1_ntc1 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, nV_ntc,  nIdx_ADC1_ntc2 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, nV_ntc,  nIdx_ADC1_ntc3 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, nV_ntc,  nIdx_ADC1_ntc4 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, nV_vref, nIdx_ADC1_vref + 1, ADC_SampleTime_239Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    u8 lit;
    for (lit = 0; lit < 10; lit++);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer, for Calibration
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    mi16ADC1_Calibrattion_Val = Get_CalibrationValue(ADC1);
    ADC_BufferCmd(ADC1, ENABLE);


    // ----------------------- ADC2 
    ADC_Init(ADC2, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC2, nV_shunt, nIdx_ADC2_shunt1 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC2, nV_shunt, nIdx_ADC2_shunt2 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC2, nV_shunt, nIdx_ADC2_shunt3 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC2, nV_shunt, nIdx_ADC2_shunt4 + 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC2, nV_shunt, nIdx_ADC2_shunt5 + 1, ADC_SampleTime_239Cycles5);

    ADC_Cmd(ADC2, ENABLE);

    // Note: Before starting calibration, you must ensure that the ADC module is in the power - on status(ADON = 1)
    // for more than 2 ADC clock cycles at least
    for (lit = 0; lit < 10; lit++);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer, for Calibration
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2));
    mi16ADC2_Calibrattion_Val = Get_CalibrationValue(ADC2);

    // At this time, the ADC sampling time is not recommended to be less than 7.5T
    ADC_BufferCmd(ADC2, ENABLE);   //disable buffer, not disable when PGA > 1
    
    // ADC_Buffer needs to be enabled to use this function
    ADC2->CTLR1 |= ADC_Pga_16;

    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
  }


  void vTim1_Init_WS2812(void)
  {
    TIM_OCInitTypeDef       TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


    // 96 Mhz / 8Mhz = 12
    TIM_TimeBaseInitStructure.TIM_Prescaler     = (cClockInfo::mstClocks.PCLK2_Frequency / 8000000) - 1; // 0,125 us = 8Mhz
    TIM_TimeBaseInitStructure.TIM_Period        = 10 - 1; // = > 1,25us is = 800khz
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCNIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
  }

  void Dma_vIrqTc()
  {
    TIM_DMACmd(TIM1, TIM_DMA_CC1, DISABLE);
    DMA1_Channel2->CFGR &= ~(1 << 0); // EN
    DMA1->INTFCR = DMA1_IT_TC2;
    mu8StateProcess = 4;
    vTurnOnOffCtrl();
  }

  void vTim1_Dma_vInit(void)
  {
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&TIM1->CH1CVR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)mu8LedData;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    NVIC_SetPriority(DMA1_Channel2_IRQn, (2 << 5));
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  }
};


#endif  //__WS2812_CONTROL_H__
