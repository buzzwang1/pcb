#include "SysPkgCom.h"
#include "System.h"


cSysPkgCom::cSysPkgCom()
  : mcMyBotNetCfg1((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr),
    mcU1_BnSlave(USART1, 200000),
    mcBn(&mcMyBotNetCfg1),
    mcUpLink(&mcU1_BnSlave)
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

  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);


  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);
}

void cSysPkgCom::vInit2()
{
}

bool cSysPkgCom::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  if (mcU1_BnSlave.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2c1Bn";
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



void DMA2_Channel7_IRQHandler(void)
{
  // USART1 RX
  mcSys.mcCom.mcU1_BnSlave.IrqHandler(cComNode::tenEvent::enEvDmaRxTc);
}

void USART1_IRQHandler(void)
{
  mcSys.mcCom.mcU1_BnSlave.IrqHandler(cComNode::tenEvent::enEvUsartTc);
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
  if (TIM15->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM15->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSys.mcCom.mcU1_BnSlave.TIM_EV_IRQHandler();
  }
}

