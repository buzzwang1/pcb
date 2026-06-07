#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcComPort2(38400, GPIO_AF7_USART2, 16, 16),
    mcBn(&mcMyBotNetCfg),
    mcUpLinkBotCom(&mcComPort2),
    mcNRF905Master(0x00010110, 0x00010100),
    mcDownLink(&mcNRF905Master, &mcBn) // 1x10ms warten bis zum Start vom Ping, weil NRF905 erst in bAddedToBn initialisiert wird
  {
  }

void cSysPkgCom::vInit1()
{
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 7, 8);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  //Überprüfen, ob ein externes EEPROM in der Partitionstabelle konfiguriert ist
  const tstMemPart_Partition* lpPartition;
  for (u8 lu8MemIdx = 0; lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt; lu8MemIdx++)
  {
    RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);
    if ((lpPartition->ui16Sort == RomConst_Sort_ExtEep) ||
        (lpPartition->ui16Sort == RomConst_Sort_IntEep))
    {
      //mcBn.mcSpop.mcEep = &mcSys.mcBoard.mcEep;
      break;
    }
  }

  mcUpLinkBotCom.menType = cBotNet_LinkBase::tenType::enUpLink;
  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLinkBotCom);

  // Nrf905
  mcBn.bAddLink((cBotNet_LinkBase*)&mcDownLink);
}

void cSysPkgCom::vInit2()
{
}

bool cSysPkgCom::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  if (!mcUpLinkBotCom.mStatus.IsInit)
  {
    lszErrorInfo += (rsz)" ErrSideLnk";
    lbRet = True;
  }

  if (!mcDownLink.mStatus.IsInit)
  {
    lszErrorInfo += (rsz)" ErrSideLnk";
    lbRet = True;
  }

  return lbRet;
}


bool cSysPkgCom::isReadyForSleep(cStr& lcStatus)
{
  UNUSED(lcStatus);
  return False; // Nie schlafen gehen
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



// ---------------------------- RF ---------------------------

  void EXTI15_10_IRQHandler(void)
  {
    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTRFIRQ
    ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
    ////    u32 lu32TimEnd; mu8IntLvl++;
    ////    mcPA05.vSet1();
    ////  #endif
    ////#endif

    if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_13) != 0x00u)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_13);
      mcSys.mcCom.mcDownLink.bEventHandler(cNRF905::NRF905_EvDataReady);
    }

    ////#ifdef PCB_PROJECTCFG_Test
    ////  #ifdef TESTRFIRQ
    ////    mcPA05.vSet0();
    ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
    ////    if (lu32TimEnd > lu32TimStart)
    ////    {
    ////      mcTestClassMaxCyc[4].vSetMaxTimer(lu32TimEnd - lu32TimStart);
    ////    }
    ////    mcTestClassMaxCyc[4].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
    ////  #endif
    ////#endif
  }


void DMA1_Channel2_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // SPI RX
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC2;
  mcSys.mcCom.mcDownLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[5].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[5].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void DMA1_Channel3_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  // SPI TX
  DMA1_Channel3->CCR &= ~DMA_CCR_EN;
  DMA1->IFCR          = DMA_FLAG_TC3;
  mcSys.mcCom.mcDownLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[6].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[6].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void TIM7_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSys.mcCom.mcDownLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTRFIRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[7].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[7].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}


//---------------------------------- U2 --------------------------------

void USART2_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU2IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcCom.mcComPort2.vIRQHandler();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTU2IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[11].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[11].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

