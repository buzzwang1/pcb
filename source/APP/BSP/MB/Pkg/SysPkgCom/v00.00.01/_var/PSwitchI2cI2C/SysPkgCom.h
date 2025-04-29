
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

  // I2C1: UpLink
  cGpPin     mcI2c1_SCL_BnUl;
  cGpPin     mcI2c1_SDA_BnUl;
  cI2cSlave  mcI2C1_BnSlave;

  // I2C2: DownLink
  cGpPin      mcI2c2_SCL_BnDl;
  cGpPin      mcI2c2_SDA_BnDl;
  cI2cMaster  mcI2C2_BnMaster;


  // BotNet
  cBotNet               mcBn;


  cBotNet_UpLinkI2c          mcUpLink;

  cBotNet_DownLinkI2c        mcDownLinks1;
  cBotNet_DownLinkI2c        mcDownLinks2;
  cBotNet_DownLinkI2c        mcDownLinks3;
  cBotNet_DownLinkI2c        mcDownLinks4;
  cBotNet_DownLinkI2c        mcDownLinks5;
  cBotNet_DownLinkI2c        mcDownLinks6;
  cBotNet_DownLinkI2c        mcDownLinks7;
  cBotNet_DownLinkI2c        mcDownLinks8;
  cBotNet_DownLinkI2c        mcDownLinks9;
  cBotNet_DownLinkI2c        mcDownLinksA;
  cBotNet_DownLinkI2c        mcDownLinksB;
  cBotNet_DownLinkI2c        mcDownLinksC;
  cBotNet_DownLinkI2c        mcDownLinksD;
  cBotNet_DownLinkI2c        mcDownLinksE;


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
extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif


#endif // __SYS_PKG_COM_H__
