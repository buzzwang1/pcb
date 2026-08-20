#pragma once

#include "cSysDPool.h"
#include "cNrf905Bn.h"


class cCompComIn : public cComponent
{
  public:

  // RF
  cNRF905BnSlave            mcNRF905;
  cBotNet_UpLinknRf905Net   mcSideLink;

  cCompComIn()
    : cComponent(cDepTreeCfg::cComp::nComIn, cDepTreeCfg::cComp::nCom),
      mcNRF905(SPI1, 8, 9, 32, 0),
      mcSideLink(&mcNRF905, cSysDPool::mCom.mpcBn, True)
  {
    cSysDPool::mCom.mpcSideLink = &mcSideLink;
    cSysDPool::mCom.mpcNRF905Bn = &mcNRF905;
  }

  bool bInit() override
  {
    cSysDPool::mCom.mpcBn->bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
    mcSideLink.vSetTiming(15 * 1000, 50); // 15s Ping Interval, 50ms warten auf eine Session nach Ping.
    mcSideLink.vKeepReceiverOnWhileWaiting(True);

    cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    mcNRF905.vTick1ms();
    // return True to signal finished
    return cComponent::bRun();
  };

  bool isError(cStr& lszErrorInfo)
  {
    bool lbRet = False;

    if (!mcSideLink.mStatus.IsInit)
    {
      lszErrorInfo += (rsz)" ErrSideLnk";
      lbRet = True;
    }

    return lbRet;
  }

  bool isReadyForSleep(cStr& lcStatus)
  {
    bool lbRet = True;
    cStr_Create(lszStrBuf, 32);

    // Warten solange active Funk-Verbindung besteht oder Funkverbindung gerade einen Ping durchführt.
    if (((mcSideLink.mStatus.IsOnline) || (!mcSideLink.isReadyForSleep())) &&
         (mcSideLink.mStatus.IsInit))
    {
      lbRet = False;
      if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
      lcStatus += (rsz)"SideLnk";
    }

    return lbRet;
  }
};

#ifdef __cplusplus
extern "C" {
#endif

  extern void EXTI5_IRQHandler(void);
  extern void SPI1_IRQHandler(void);
  extern void TIM7_IRQHandler(void);

  void EXTI5_IRQHandler(void)
  {
    if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_5) != 0x00u)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_5);
      cSysDPool::mCom.mpcNRF905Bn->vIRQHandler(cComNode::tenEvent::enEvUsartExtiP1);
    }
  }

  void SPI1_IRQHandler(void)
  {
    if (SPI1->SR & SPI_FLAG_EOT)
    {
      SPI1->CR1 &= ~(SPI_CR1_SPE);
      SPI1->IFCR = 0xFFFF; // Clear all flags
    }

    cSysDPool::mCom.mpcNRF905Bn->vIRQHandler(cComNode::tenEvent::enEvSpiTc);
  }

  void TIM7_IRQHandler(void)
  {
    if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
    {
      TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
      TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
      cSysDPool::mCom.mpcNRF905Bn->vIRQHandler(cComNode::tenEvent::enEvUsartTimer);
    }
  }
#ifdef __cplusplus
}
#endif


