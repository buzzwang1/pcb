#ifndef __CADC_H__
#define __CADC_H__

#include "_Typedef.h"
#include "_stm32f30x.h"
#include "_stm32f30x_adc.h"
#include "_stm32f30x_dma.h"
#include "_stm32f30x_rcc.h"
#include "_stm32f30x_misc.h"


class cADC_Channel
{
  public:

  typedef enum
  {
    enErUnknown  = 0,
  }cADC_Channel_Error;

  typedef enum
  {
    enFree  = 0,
    enUnset,
    enSet,
    enStart,
    enBusy,
    enDone,
    enError,
  }cChannel_Status;

  bool                mbIsInit;

  uint8               mui8AdcChannelIdx;
  uint8               mui8AdcSampleTime;

  uint8               mui8StartRequest;
  cChannel_Status     enStatus;
  uint16              mui16AdcValue;

  virtual ~cADC_Channel() {};

  void virtual vConvDone() = 0;
  void virtual vConvError(cADC_Channel_Error lenError) = 0;

  cADC_Channel(uint8 lui8AdcChannelIdx, uint8 lui8AdcSampleTime)
  {
    mui16AdcValue = 0;
    mui8AdcSampleTime = lui8AdcSampleTime;
    mui8AdcChannelIdx = lui8AdcChannelIdx;
    mbIsInit = true;
  }

  void vStartConv() {mui8StartRequest = 1;}

  void vFree()   {enStatus = enFree;}
  void vUnset()  {enStatus = enUnset;}
  void vSet()    {enStatus = enSet;}
  void vStart()  {enStatus = enStart;}
  void vBusy()   {enStatus = enBusy;}
  void vDone()   {enStatus = enDone;}
  void vError()  {enStatus = enError;}

  bool isFree()  {return (enStatus == enFree);}
  bool isUnSet() {return (enStatus == enUnset);}
  bool isSet()   {return (enStatus == enSet);}
  bool isStart() {return (enStatus == enStart);}
  bool isBusy()  {return (enStatus == enBusy);}
  bool isDone()  {return (enStatus == enDone);}
  bool isError() {return (enStatus == enError);}
  bool isInit()  {return  mbIsInit;}
};


class cADC_ChannelList
{
  public:

  uint32         ui32Size;
  uint32         ui32Used;
  uint32         ui32Pos;
  cADC_Channel  **mpacChannels;

  cADC_ChannelList(uint32 lui32ChannelsCnt);
  ~cADC_ChannelList();

  bool   isEmpty();
  bool   isSlotFree();

  void   vStartAll();

  uint32 ui32GetFreeSlot();
  uint32 ui32GetSlot(cADC_Channel *lpcChannel);
  void vAdd(cADC_Channel *lpcChannel);
  void vRemove(cADC_Channel *lpcChannel);
  cADC_Channel* cGetNext();
};


class cAdc
{
  public:

  typedef enum
  {
    enIdle  = 0,
    enBusy
  }tenADC_States;

  typedef enum
  {
    enStart  = 0,
    enIrq,
    enError
  }tenADC_Events;

  tenADC_States  mSM;

  cADC_ChannelList mcChannels;
  cADC_Channel     *mpcActiveChannel;

  ADC_TypeDef   *mADC;

  uint8         *mpui8ComBuf;
  uint32         mui32ComByteCnt;

  uint16         mui16VRefCal;
  uint16         mui16VRef_mV;

  cAdc(ADC_TypeDef *lstADC, uint32 lui32Channels);
  ~cAdc();

  void ADC_EV_IRQHandler();
  void ADC_ER_IRQHandler();

  void vInitHw();
  void vReInitHw();
  void vError(cADC_Channel::cADC_Channel_Error lenError);

  void Tick();

  void vAddChannel(cADC_Channel *lpcChannel);
  void vRemoveChannel(cADC_Channel *lpcChannel);
  void vSm(tenADC_Events lenEvent);

  void vStartChannel(cADC_Channel *lpcChannel);
  void bStartNext();
};



class cAdc_VBat:cADC_Channel
{
  public:

  cAdc   *mADC;
  uint16 mui16VBat_mv;

  cAdc_VBat(cAdc *lpcADC);
  ~cAdc_VBat();

  void vStartConv();
  void vConvError(cADC_Channel::cADC_Channel_Error lenError);
  void vConvDone();
};


class cAdc_VRef:cADC_Channel
{
  public:

  cAdc   *mADC;
  uint16 mui16VRef_mv;

  cAdc_VRef(cAdc *lpcADC);
  ~cAdc_VRef();


  void vStartConv();
  void vConvError(cADC_Channel::cADC_Channel_Error lenError);
  void vConvDone();
};




class cAdc_Temp:cADC_Channel
{
  public:

  cAdc   *mADC;
  uint16 mui16Temp;

  cAdc_Temp(cAdc *lpcADC);
  ~cAdc_Temp();

  void vStartConv();
  void vConvError(cADC_Channel::cADC_Channel_Error lenError);
  void vConvDone();
};



#endif // __CADC_H__
