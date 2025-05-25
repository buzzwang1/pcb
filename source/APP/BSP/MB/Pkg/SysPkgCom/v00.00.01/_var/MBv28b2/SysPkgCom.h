
#ifndef __SYS_PKG_COM_H__
#define __SYS_PKG_COM_H__


#include "TypeDef.h"
#include "cStrT.h"

  // Driver
#include "tGPPin.h"
#include "cI2c.h"
#include "cNrf905.h"

//Data
#include "cRingBufT.h"
#include "cBnSpop.h"
#include "cBnLinkI2c.h"
#include "cBnLinkUsartMpHd.h"
#include "cBnLinkVLink.h"
#include "cBnLinkNrf905.h"
#include "cBnLinkNrf905Net.h"
#include "cBnLinkBotCom.h"
#include "cBnStreamSysPortBase.h"
#include "cBnStreamSysPortCom.h"
#include "cBnStreamSysPortCmd.h"
#include "cBotnet.h"


class cSysPkgCom
{
  public:

  cBotNetCfg mcMyBotNetCfg;

  // DownLink I2C3:
  cGpPin     mcI2c3_SCL_Bn;
  cGpPin     mcI2c3_SDA_Bn;
  cI2cMaster mcI2C3_BnMaster;

  // Debug Port U1
  tcUart<USART1_BASE, GPIOA_BASE, 9, GPIOA_BASE, 10> mcComPort1;


  // BotNet
  cBotNet               mcBn;


  // --- 0xE000 SideLink => PC
  // RF 
  cNRF905                   mcNRF905;
  cBotNet_UpLinknRf905      mcSideLinkRf;
  cBotNet_UpLinknRf905Net   mcSideLink;
    
  // Uart for developing
  cBotNet_LinkBotCom         mcSideLinkBotCom;

  // --- 0x1000 DownLinks
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1100;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1200;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1300;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1400;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1500;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1600;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1700;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1800;

  // DownLink U2:
  cGpPin                     mcU2TxRx;
  cUartMpHdMaster            mcMasterUartMpHdU2;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1900;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1A00;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1B00;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1C00;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1D00;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1000_to_0x1E00;


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


// ---------------------------- UpLinks -----------------------------
// 
// ---------------------------- RF-UpLink ---------------------------

extern void EXTI5_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void TIM7_IRQHandler(void);

// ---------------------------- I2C-UpLink ---------------------------

extern void I2C4_EV_IRQHandler(void);
extern void I2C4_ER_IRQHandler(void);

// ---------------------------- DownLinks -----------------------------
// 
// ---------------------------- I2C-DownLink ---------------------------

extern void I2C3_EV_IRQHandler(void);
extern void I2C3_ER_IRQHandler(void);


// ---------------------------- U2 ---------------------------

extern void USART2_IRQHandler(void);
extern void GPDMA1_Channel5_IRQHandler(void); // DMA Rx/TX
extern void TIM16_IRQHandler(void);  //Botnet USART2 Timer

// ---------------------------- U1 ---------------------------

extern void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif


#endif // __SYS_PKG_COM_H__
