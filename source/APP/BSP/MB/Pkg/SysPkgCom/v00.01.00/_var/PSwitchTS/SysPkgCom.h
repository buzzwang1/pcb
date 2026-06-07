
#ifndef __SYS_PKG_COM_H__
#define __SYS_PKG_COM_H__


#include "TypeDef.h"
#include "cStrT.h"

  // Driver
#include "tGPPin.h"
#include "cI2c.h"
#include "cUsart_MpHd.h"

//Data
#include "cRingBufT.h"
#include "cBnSpop.h"
#include "cBnLinkI2c.h"
#include "cBnLinkUsartMpHd.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortBase.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortCmd.h"
#include "cBotnet.h"


class cSysPkgCom
{
  public:

  cBotNetCfg mcMyBotNetCfg1;

  // I2C1: UpLink
  cUartMpHdSlave  mcU1_BnSlave;

  // BotNet
  cBotNet         mcBn;


  //cBotNet_UpLinkUsartMpHd          mcUpLink;
  cBotNet_UpLinkUsartMpHdNoCheck   mcUpLink;



  cSysPkgCom();

  void vInit1();
  void vInit2();

  bool isError(cStr& lszErrorInfo);
  bool isReadyForSleep(cStr& lcStatus);

  void vTick1msHp();
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};


#ifdef __cplusplus
extern "C" {
#endif

extern void DMA2_Channel6_IRQHandler(void);
extern void DMA2_Channel7_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void TIM1_BRK_TIM15_IRQHandler(void);


#ifdef __cplusplus
}
#endif


#endif // __SYS_PKG_COM_H__
