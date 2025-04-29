#include "cAdc.h"


cADC_ChannelList::cADC_ChannelList(uint32 lui32ChannelsCnt)
{
  ui32Size =   lui32ChannelsCnt;
  ui32Used =   0;
  ui32Pos  =   0;

  mpacChannels = new cADC_Channel*[ui32Size];


  uint32 lui32ChannelsIdx;
  for (lui32ChannelsIdx = 0; lui32ChannelsIdx < ui32Size; lui32ChannelsIdx++)
  {
    mpacChannels[lui32ChannelsIdx] = NULL;
  }
}

cADC_ChannelList::~cADC_ChannelList()
{
  delete []mpacChannels;
}

void cADC_ChannelList::vStartAll()
{
  uint32 lui32ChannelsIdx;
  for (lui32ChannelsIdx = 0; lui32ChannelsIdx < ui32Size; lui32ChannelsIdx++)
  {
    if (mpacChannels[lui32ChannelsIdx] != NULL)
    {
      mpacChannels[lui32ChannelsIdx]->mui8StartRequest = 1;
    }
  }
}

bool cADC_ChannelList::isEmpty()
{
  uint32 lui32ChannelsIdx;
  for (lui32ChannelsIdx = 0; lui32ChannelsIdx < ui32Size; lui32ChannelsIdx++)
  {
    if (mpacChannels[lui32ChannelsIdx] != NULL)
    {
      if (mpacChannels[lui32ChannelsIdx]->mui8StartRequest)
      {
        return false;
      }
    }
  }

  return true;
}

uint32 cADC_ChannelList::ui32GetFreeSlot()
{
  uint32 lui32ChannelsIdx;
  for (lui32ChannelsIdx = 0; lui32ChannelsIdx < ui32Size; lui32ChannelsIdx++)
  {
    if (mpacChannels[lui32ChannelsIdx] == NULL)
    {
      return lui32ChannelsIdx;
    }
  }
  return -1;
}

uint32 cADC_ChannelList::ui32GetSlot(cADC_Channel *lpcChannel)
{
  uint32 lui32ChannelsIdx;
  for (lui32ChannelsIdx = 0; lui32ChannelsIdx < ui32Size; lui32ChannelsIdx++)
  {
    if (mpacChannels[lui32ChannelsIdx] == lpcChannel)
    {
      return lui32ChannelsIdx;
    }
  }
  return -1;
}



bool cADC_ChannelList::isSlotFree()
{
  return (ui32Used < ui32Size);
}


void cADC_ChannelList::vAdd(cADC_Channel *lpcChannel)
{
  if (isSlotFree())
  {
    uint32 lui32FreeSlot;
    lui32FreeSlot = ui32GetFreeSlot();
    mpacChannels[lui32FreeSlot] = lpcChannel;
    ui32Used++;
  }
}

void cADC_ChannelList::vRemove(cADC_Channel *lpcChannel)
{
  uint32 lui32Slot;

  lui32Slot = ui32GetSlot(lpcChannel);
  mpacChannels[lui32Slot] = NULL;

  ui32Used--;
}

cADC_Channel* cADC_ChannelList::cGetNext()
{
  cADC_Channel *lpcChannel;

  uint32 lui32Cnt;

  lpcChannel = NULL;

  if (!isEmpty())
  {
    for (lui32Cnt = 0; lui32Cnt < ui32Size; lui32Cnt++)
    {
      ui32Pos++;
      if (ui32Pos >= ui32Size)
      {
        ui32Pos = 0;
      }
      if (mpacChannels[ui32Pos] != NULL)
      {
        if (mpacChannels[ui32Pos]->mui8StartRequest)
        {
          return mpacChannels[ui32Pos];
        }
      }
    }
  }
  return lpcChannel;
}




cAdc::cAdc(ADC_TypeDef *lstADC, uint32 lui32Channels)
  : mcChannels(lui32Channels)
{
  uint16* lpui16VRefIntCal;

  mADC = lstADC;
  mSM  = enIdle;
  mpcActiveChannel = NULL;

  lpui16VRefIntCal = (uint16*)0x1FFFF7BA;
  mui16VRefCal = *lpui16VRefIntCal;
  mui16VRef_mV = 3300;


  vInitHw();
}

cAdc::~cAdc()
{
}

void cAdc::vInitHw()
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  NVIC_InitTypeDef      NVIC_InitStructure;

  /* Configure the ADC clock */
  //RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div128);

  /* ADC1 Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

  ADC_StructInit(&ADC_InitStructure);

  /* Calibration procedure */
  ADC_VoltageRegulatorCmd(mADC, ENABLE);

  ADC_SelectCalibrationMode(mADC, ADC_CalibrationMode_Single);
  ADC_StartCalibration(mADC);

  while(ADC_GetCalibrationStatus(mADC) != RESET );
  ADC_GetCalibrationValue(mADC);

  /* Configure the ADC1 in continuous mode */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Clock = ADC_Clock_SynClkModeDiv4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;

  ADC_CommonInit(mADC, &ADC_CommonInitStructure);

  ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;
  ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigInjecConvEvent_0;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;
  ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
  ADC_InitStructure.ADC_NbrOfRegChannel = 1;
  ADC_Init(mADC, &ADC_InitStructure);

  /* Enable End Of Conversion interrupt */
  ADC_ITConfig(mADC, ADC_IT_EOC, ENABLE);

  /* Configure and enable ADC1 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable ADC1 */
  ADC_Cmd(mADC, ENABLE);

  /* wait for ADRDY */
  while(!ADC_GetFlagStatus(mADC, ADC_FLAG_RDY));

}

void cAdc::vReInitHw()
{
  // Reinit I2C & DMA
  ADC_Cmd(mADC, DISABLE);
  ADC_ClearFlag(mADC, 0xFFFF);
  vInitHw();
}



void cAdc::vError(cADC_Channel::cADC_Channel_Error lenError)
{
  mpcActiveChannel->vError();
  mpcActiveChannel->vConvError(lenError);
  mSM = enIdle;
  mpcActiveChannel = NULL;
}

void cAdc::ADC_EV_IRQHandler()
{
  vSm(enIrq);

  if (mSM == enIdle)
  {
    vStartNext();
  }
}

void cAdc::vAddChannel(cADC_Channel *lpcChannel)
{
  mcChannels.vAdd(lpcChannel);
}

void cAdc::vRemoveChannel(cADC_Channel *lpcChannel)
{
  mcChannels.vRemove(lpcChannel);
}


void cAdc::vSm(tenADC_Events lenEvent)
{
  switch (lenEvent)
  {
    case enError:
      vError(cADC_Channel::enErUnknown);
      break;
    default:
      break;
  }


  switch (mSM)
  {
    case enIdle:
      switch (lenEvent)
      {
        case enStart:
          mSM = enBusy;
          ADC_RegularChannelConfig(mADC, mpcActiveChannel->mui8AdcChannelIdx, 1, mpcActiveChannel->mui8AdcSampleTime);
          ADC_ClearFlag(mADC, ADC_FLAG_EOC); //Clear EOC flag
          ADC_StartConversion(mADC);
          break;
        default:
          break;
      }
      break;
    case enBusy:
      switch (lenEvent)
      {
        case enIrq:
          if (ADC_GetFlagStatus(mADC, ADC_FLAG_EOC) == SET)
          {
            /* Get converted value */
            mpcActiveChannel->mui16AdcValue = ADC_GetConversionValue(mADC);
            /* Clear EOC Flag */
            ADC_ClearFlag(mADC, ADC_FLAG_EOC);

            mSM = enIdle;
            mpcActiveChannel->vDone();
            mpcActiveChannel->vConvDone();
          }
          else
          {
            vError(cADC_Channel::enErUnknown);
          }
          break;
        default:
          vError(cADC_Channel::enErUnknown);
          break;
      }
      break;
  }
}

void cAdc::Tick()
{
}


void cAdc::vStartNext()
{
  bool lbStart;

  lbStart = true;

  if (mpcActiveChannel != NULL)
  {
    if (mpcActiveChannel->isBusy())
    {
      lbStart = false;
    }
  }

  if (lbStart)
  {
    lbStart &= (!mcChannels.isEmpty());
  }

  if (lbStart)
  {
    if (ADC_GetFlagStatus(mADC, ADC_FLAG_RDY) == SET)
    {
      mpcActiveChannel = mcChannels.cGetNext();
      mpcActiveChannel->vBusy();
      vSm(enStart);
    }
  }
}

//-----------------------------------------------------
//---------------cAdc_VBat-----------------------------
//-----------------------------------------------------

cAdc_VBat::cAdc_VBat(cAdc *lpcADC)
  :cADC_Channel(17, ADC_SampleTime_19Cycles5)
{
  mADC   = lpcADC;
  mADC->vAddChannel((cADC_Channel*)this);

  if (mADC->mADC->CR & ADC_CR_ADEN)
  {
    ADC_Cmd(mADC->mADC, DISABLE);
    ADC_VbatCmd(mADC->mADC, ENABLE);
    ADC_Cmd(mADC->mADC, ENABLE);
  }
  else
  {
    ADC_VbatCmd(mADC->mADC, ENABLE);
  }
};

cAdc_VBat::~cAdc_VBat()
{
  mADC->vRemoveChannel((cADC_Channel*)this);
}

void cAdc_VBat::vStartConv()
{
  cADC_Channel::vStartConv();
}


void cAdc_VBat::vConvError(cADC_Channel::cADC_Channel_Error lenError)
{
}

void cAdc_VBat::vConvDone()
{
  uint32 lui32Temp;
  lui32Temp  = this->mui16AdcValue * 2 * mADC->mui16VRef_mV;
  lui32Temp /= 4096;
  mui16VBat_mv = (uint16)lui32Temp;
}

//-----------------------------------------------------
//---------------cAdc_VRef-----------------------------
//-----------------------------------------------------

cAdc_VRef::cAdc_VRef(cAdc *lpcADC)
  :cADC_Channel(18, ADC_SampleTime_19Cycles5)
{
  mADC   = lpcADC;
  mADC->vAddChannel((cADC_Channel*)this);

  if (mADC->mADC->CR & ADC_CR_ADEN)
  {
    ADC_Cmd(mADC->mADC, DISABLE);
    ADC_VrefintCmd(mADC->mADC, ENABLE);
    ADC_Cmd(mADC->mADC, ENABLE);
  }
  else
  {
    ADC_VrefintCmd(mADC->mADC, ENABLE);
  }
};

cAdc_VRef::~cAdc_VRef()
{
  mADC->vRemoveChannel((cADC_Channel*)this);
}

void cAdc_VRef::vStartConv()
{
  cADC_Channel::vStartConv();
}

void cAdc_VRef::vConvError(cADC_Channel::cADC_Channel_Error lenError)
{
}

void cAdc_VRef::vConvDone()
{
  uint32 lui32Temp;
  lui32Temp  = mADC->mui16VRefCal * 3300;
  lui32Temp /= this->mui16AdcValue;
  mui16VRef_mv = (uint16)lui32Temp;
  mADC->mui16VRef_mV = mui16VRef_mv;
}

//-----------------------------------------------------
//---------------cAdc_Temp-----------------------------
//-----------------------------------------------------

cAdc_Temp::cAdc_Temp(cAdc *lpcADC)
  :cADC_Channel(16, ADC_SampleTime_19Cycles5)
{
  mADC   = lpcADC;
  mADC->vAddChannel((cADC_Channel*)this);

  if (mADC->mADC->CR & ADC_CR_ADEN)
  {
    ADC_Cmd(mADC->mADC, DISABLE);
    ADC_TempSensorCmd(mADC->mADC, ENABLE);
    ADC_Cmd(mADC->mADC, ENABLE);
  }
  else
  {
    ADC_TempSensorCmd(mADC->mADC, ENABLE);
  }
};

void cAdc_Temp::vStartConv()
{
  cADC_Channel::vStartConv();
}

cAdc_Temp::~cAdc_Temp()
{
  mADC->vRemoveChannel((cADC_Channel*)this);
}

void cAdc_Temp::vConvError(cADC_Channel::cADC_Channel_Error lenError)
{
}

void cAdc_Temp::vConvDone()
{
  mui16Temp = this->mui16AdcValue;
}

