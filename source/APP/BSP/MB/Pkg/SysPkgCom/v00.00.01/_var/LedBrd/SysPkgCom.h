
#ifndef __SYS_PKG_COM_H__
#define __SYS_PKG_COM_H__


#include "TypeDef.h"
#include "cStrT.h"

  // Driver
#include "tGPPin.h"
#include "cI2c.h"

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

  cGpPin     mcI2c1_SCL_BnUl;
  cGpPin     mcI2c1_SDA_BnUl;
  cI2cSlave  mcI2C1_BnSlave;


  // BotNet
  cBotNet               mcBn;

  // I2C1: UpLink
  cBotNet_UpLinkI2c     mcUpLink;


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

extern void I2C1_EV_IRQHandler(void);
extern void I2C1_ER_IRQHandler(void);


#ifdef __cplusplus
}
#endif


#endif // __SYS_PKG_COM_H__
