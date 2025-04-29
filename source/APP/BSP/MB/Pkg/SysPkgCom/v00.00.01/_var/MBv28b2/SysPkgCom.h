
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

  cBotNetCfg mcMyBotNetCfg1;
  cBotNetCfg mcMyBotNetCfg2;

  // I2C1:
  cGpPin     mcI2c1_SCL_Bn;
  cGpPin     mcI2c1_SDA_Bn;
  cI2cMaster mcI2C1_BnMaster;

  tcUart<USART2_BASE, GPIOA_BASE, 2, GPIOA_BASE, 3> mcComPort2;


  // BotNet
  cBotNet               mcBn_0x1000;
  //  0x1000 Masternode for 011[x] all nodes, e.g. downstream to 0111
  // 0 CmdPort
  // 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)


  // --- 0xE000 SideLink => PC
  // RF 
  cNRF905                   mcNRF905;
  cBotNet_UpLinknRf905      mcSideLinkRf;
  cBotNet_UpLinknRf905Net   mcSideLink;
    
  // Uart for developing
  cBotNet_LinkBotCom        mcSideLinkBotCom;

  // --- 0x1000 DownLinks
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1100;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1200;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1300;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1400;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1500;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1600;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1700;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1800;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1900;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1A00;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1B00;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1C00;
  cBotNet_DownLinkI2c        mcDownLinks_0x1000_to_0x1D00;
  cBotNet_LinkVLink_DownLink mcDownLinks_0x1000_to_0x1E00;



  // --- 0x1E00 -----
  cBotNet                    mcBn_0x1E00;

  // --- 0x1E00 Uplink
  cBotNet_LinkVLink_UpLink   mcUpLinks_0x1E00_to_0x1000;

  // --- 0x1E00 DownLinks
  cGpPin                     mcU1TxRx;
  cUartMpHdMaster            mcMasterUartMpHdU1;

  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E10;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E20;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E30;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E40;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E50;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E60;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E70;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E80;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1E90;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EA0;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EB0;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EC0;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1ED0;
  cBotNet_DownLinkUsartMpHd  mcDownLinks_0x1E00_to_0x1EE0;

  
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

// ---------------------------- RF ---------------------------

extern void EXTI15_10_IRQHandler(void);
extern void DMA1_Channel2_IRQHandler(void); // SPI1 RX
extern void DMA1_Channel3_IRQHandler(void); // SPI1 TX
extern void TIM7_IRQHandler(void);

// ---------------------------- U1 ---------------------------

extern void USART1_IRQHandler(void);
extern void DMA2_Channel6_IRQHandler(void); // DMA USART1 TX
extern void DMA2_Channel7_IRQHandler(void); // DMA USART1 RX
extern void TIM1_UP_TIM16_IRQHandler(void);  //Botnet USART2 Timer

// ---------------------------- U2 ---------------------------

extern void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif


#endif // __SYS_PKG_COM_H__
