#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg1((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcComPortU2(38400, GPIO_AF7_USART2, 16, 16),
    mcSideLinkBotCom(&mcComPortU2),
    
    //mcU3_BnMaster(USART3, 16, 200000),
    //mcBn(&mcMyBotNetCfg1),
    //mcDownLinks1(&mcU3_BnMaster, True),
    //mcDownLinks2(&mcU3_BnMaster, True),
    //mcDownLinks3(&mcU3_BnMaster, True),
    //mcDownLinks4(&mcU3_BnMaster, True),
    //mcDownLinks5(&mcU3_BnMaster, True),
    //mcDownLinks6(&mcU3_BnMaster, True),
    //mcDownLinks7(&mcU3_BnMaster, True),
    //mcDownLinks8(&mcU3_BnMaster, True),
    //mcDownLinks9(&mcU3_BnMaster, True),
    //mcDownLinksA(&mcU3_BnMaster, True),
    //mcDownLinksB(&mcU3_BnMaster, True),
    //mcDownLinksC(&mcU3_BnMaster, True),
    //mcDownLinksD(&mcU3_BnMaster, True),
    //mcDownLinksE(&mcU3_BnMaster, True)

    mcI2c2_SCL_BnDl(GPIOB_BASE, 13, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c2_SDA_BnDl(GPIOB_BASE, 14, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C2_BnMaster(I2C2, &mcI2c2_SCL_BnDl, &mcI2c2_SDA_BnDl, DMA1, DMA1_Channel5, DMA1_Channel4, 16, u16GetRomConstBaudDownLink1() * 1000),
    mcBn(&mcMyBotNetCfg1),
    mcDownLinks1(&mcI2C2_BnMaster),
    mcDownLinks2(&mcI2C2_BnMaster),
    mcDownLinks3(&mcI2C2_BnMaster),
    mcDownLinks4(&mcI2C2_BnMaster),
    mcDownLinks5(&mcI2C2_BnMaster),
    mcDownLinks6(&mcI2C2_BnMaster),
    mcDownLinks7(&mcI2C2_BnMaster),
    mcDownLinks8(&mcI2C2_BnMaster),
    mcDownLinks9(&mcI2C2_BnMaster),
    mcDownLinksA(&mcI2C2_BnMaster),
    mcDownLinksB(&mcI2C2_BnMaster),
    mcDownLinksC(&mcI2C2_BnMaster),
    mcDownLinksD(&mcI2C2_BnMaster),
    mcDownLinksE(&mcI2C2_BnMaster)
{
}

void cSysPkgCom::vInit1()
{
  //Überprüfen, ob ein externes EEPROM in der Partitionstabelle konfiguriert ist
  const tstMemPart_Partition* lpPartition;
  for (u8 lu8MemIdx = 0; lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt; lu8MemIdx++)
  {
    RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);
    if ((lpPartition->ui16Sort == RomConst_Sort_ExtEep) ||
        (lpPartition->ui16Sort == RomConst_Sort_IntEep))
    {
      mcBn.mcSpop.mcEep = &mcSys.mcBoard.mcEep;
      break;
    }
  }

  mcBn.bAddLink((cBotNet_LinkBase*)&mcSideLinkBotCom, 0xE000);

  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks1);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks2);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks3);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks4);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks5);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks6);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks7);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks8);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinks9);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinksA);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinksB);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinksC);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinksD);
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLinksE);
  mcBn.vSetHandleHardware(1, True);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);
}

void cSysPkgCom::vInit2()
{
}

bool cSysPkgCom::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  if (!mcSideLinkBotCom.mStatus.IsInit)
  {
    lszErrorInfo += (rsz)" ErrSideLnk";
    lbRet = True;
  }

  //if (mcU3_BnMaster.mSm == cComNode::enStError)
  //{
  //  lszErrorInfo += (rsz)" ErrI2c2Bn";
  //  lbRet = True;
  //}


  if (mcI2C2_BnMaster.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2c2Bn";
    lbRet = True;
  }


  return lbRet;
}


bool cSysPkgCom::isReadyForSleep(cStr& lcStatus)
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  // Warten bis SPOP fertig ist  
  if (mcBn.mcSpop.isBusy())
  {
    lszStrBuf.Setf((rsz)"Spop");
    if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
    lcStatus += lszStrBuf;
  }

  return lbRet;
}

void cSysPkgCom::vTick1msHp()
{
  mcBn.vTickHp1ms();
}

void cSysPkgCom::vTick1msLp(void)
{
  mcBn.vProcess(1000);
}

void cSysPkgCom::vTick10msLp(void)
{
}

void cSysPkgCom::vTick100msLp(void)
{
}

void cSysPkgCom::vTick1000msLp(void)
{
}


//void DMA1_Channel2_IRQHandler(void)
//{
//  // USART1 TX
//  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
//  DMA1->IFCR = DMA_FLAG_TC2;
//  mcSys.mcCom.mcU3_BnMaster.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
//}
//
//
//void DMA1_Channel3_IRQHandler(void)
//{
//  // USART1 RX
//  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
//  DMA1->IFCR = DMA_FLAG_TC3;
//  mcSys.mcCom.mcU3_BnMaster.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
//}
//
//void USART3_IRQHandler(void)
//{
//  if (USART3->ISR & LL_USART_ISR_TC)
//  {
//    USART3->ICR = LL_USART_ISR_TC;
//    mcSys.mcCom.mcU3_BnMaster.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
//  }
//  if (USART3->ISR & 0xF)
//  {
//    USART3->ICR = LL_USART_ISR_ORE;
//    mcSys.mcCom.mcU3_BnMaster.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
//  }
//}
//
//void TIM1_BRK_TIM15_IRQHandler(void)
//{
//  if (TIM15->SR & TIM_SR_UIF) // if UIF flag is set
//  {
//    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
//    TIM15->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
//    mcSys.mcCom.mcU3_BnMaster.TIM_EV_IRQHandler();
//  }
//}


void I2C2_EV_IRQHandler(void)
{
  mcSys.mcCom.mcI2C2_BnMaster.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  mcSys.mcCom.mcI2C2_BnMaster.I2C_ER_IRQHandler();
}

// ---------------------------- U2 ---------------------------

void USART2_IRQHandler(void)
{
  mcSys.mcCom.mcComPortU2.vIRQHandler();
}


