
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB


RCC_ClocksTypeDef RCC_Clocks;
__IO uint32_t TimingDelay = 0;
//LED<GPIOC_BASE, 13> lcLedRed;

cBotNetCfg mcBnCfg_0x1000((const char8*)"Botnet 0x1000 Master", 16, 0x1000);
cBotNetCfg mcBnCfg_0x1100((const char8*)"Botnet 0x1100 Master", 16, 0x1100);

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay > 0)
  {
    TimingDelay--;
  }
  else
  {
    TimingDelay = 0;
  }
}

void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

class cBn_MsgProcess_0x1000 : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    (void)lpcBn;
    switch (lcMsg.mu16Idx)
    {
      case 80:
        lcMsg.mu16Idx++;
      break;
      default:
      break;
    }
  }
};

class cBn_MsgProcess_0x1100 : public cBotNet_MsgSysProcess
{
  public:

  void vMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    (void)lpcBn;
    switch (lcMsg.mu16Idx)
    {
      case 80:
        lcMsg.mu16Idx++;
      break;
      default:
      break;
    }
  }
};


cI2cMaster                     *mcI2C2_Master;
cI2cSlave                      *mcI2C1_Slave;

cBotNet                        *mcI2cBn_0x1000; // Masternode for 011[x] all nodes, e.g. downstream to 0111
                                                // 0 CmdPort
                                                // 1 ComPort (PA2: USART2_TX; PA3: USART2_RX; 9600)
                                                // 2 MemPort
                                                // Connections:
                                                //    - CmdPort->Comport
                                                //    - Comport->Cmdport
// --- 0x0110 UpLink
cNRF905                  mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905*    mcUpLnk_0x1000Rf;
cBotNet_UpLinknRf905Net* mcUpLnk_0x1000;
// --- 0x0110 DownLinks
cBotNet_DownLinkI2c*  mcDownLinks_0x1000[cBotNet::enCnstSlaveCnt];


cBotNet                        *mcI2cBn_0x1100; // Slavenode of 0110, =>upstream to 0110
                                                // 0 CmdPort
// --- 0x0111 UpLink
cBotNet_UpLinkI2c*    mcUpLnk_0x1100;
// --- 0x0111 DownLinks
cBotNet_DownLinkI2c*  mcDownLinks_0x1100[cBotNet::enCnstSlaveCnt];




cBn_MsgProcess_0x1000 mcBn_MsgProcess_0x1000;
cBn_MsgProcess_0x1100 mcBn_MsgProcess_0x1100;

cGpPin lcSCL_Master(GPIOB_BASE, 6, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);
cGpPin lcSDA_Master(GPIOB_BASE, 7, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);

cGpPin lcSCL_Slave(GPIOA_BASE, 9,  GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);
cGpPin lcSDA_Slave(GPIOA_BASE, 10, GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_NOPULL, GPIO_Speed_10MHz, 0);

tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort2(9600, GPIO_AF_7, 64, 64);

//cBotNetStreamPort_ComPort       mcComPort2(USART2);
//cBotNetStreamPort_ComPort       mcComPort3(USART3);
cBotNetStreamPort_BotNetMemPort mcMemPort(IAP_PARTITION_COUNT, stInAppProg_Platform_Partitions);


void I2C2_EV_IRQHandler(void)
{
  mcI2C2_Master->I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  mcI2C2_Master->I2C_ER_IRQHandler();
}

void I2C1_EV_IRQHandler(void)
{
  mcI2C1_Slave->I2C_EV_IRQHandler();
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C1_Slave->I2C_ER_IRQHandler();
}




void USART2_IRQHandler(void)
{
  //lcLedRed.Toggle();
  mcComPort2.vIRQHandler();
}

void USART3_IRQHandler(void)
{
  //lcLedRed.Toggle();
  //mcComPort3.mcConIo.vIRQHandler_Ev();
}

void EXTI15_10_IRQHandler(void)
{
  // Handle PC15 interrupt
  // Make sure that interrupt flag is set
  if (EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    // Clear interrupt flag
    EXTI_ClearITPendingBit(EXTI_Line15);
    mcUpLnk_0x1000->bEventHandler(cNRF905::NRF905_EvDataReady);
  }
}


void DMA1_Channel2_IRQHandler(void)
{
  // SPI RX
  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC2);
  mcUpLnk_0x1000->bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
}

void DMA1_Channel3_IRQHandler(void)
{
  // SPI TX
  DMA_Cmd(DMA1_Channel3, DISABLE);
  DMA_ClearFlag(DMA1_FLAG_TC3);
  mcUpLnk_0x1000->bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
}

void TIM4_IRQHandler(void)
{
  if(TIM4->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM4->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM_Cmd(TIM4, DISABLE);
    mcUpLnk_0x1000->bEventHandler(cNRF905::NRF905_EvTimer);
  }
}

void MAIN_vTick1msHp(void)
{
  mcI2cBn_0x1000->vTickHp1ms();
  mcI2cBn_0x1100->vTickHp1ms();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vTick10msLp(void)
{
  mcI2cBn_0x1000->vTickLp10ms();
  mcI2cBn_0x1100->vTickLp10ms();
}

void MAIN_vTick100msLp(void)
{
  //lcLedRed.Toggle();
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  u8 lu8t;

  // Construct function
  SystemInit();
  // SysTick end of count event each 10ms
  RCC_GetClocksFreq(&RCC_Clocks);
  SystemCoreClockUpdate();
  cClockInfo::Update();

  //mcComPort2.mcConIo.vHwInit();
  //mcComPort3.mcConIo.vHwInit();

  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);


  // --------------- 0x0111 ------------------
  mcI2C2_Master  = new cI2cMaster(I2C1, &lcSCL_Master, &lcSDA_Master, 16);
  mcI2cBn_0x1000 = new cBotNet(&mcBnCfg_0x1000, &mcBn_MsgProcess_0x1000);

  // --- 0x0100 UpLink
  mcUpLnk_0x1000Rf = (cBotNet_UpLinknRf905*) new cBotNet_UpLinknRf905(0xE000, &mcNRF905);
  mcUpLnk_0x1000   = (cBotNet_UpLinknRf905Net*) new cBotNet_UpLinknRf905Net(mcUpLnk_0x1000Rf);

  // --- 0x0110 DownLinks
  cBotNetAdress mcBnAdr_0x1000;
  mcBnAdr_0x1000.Set(0x0110);
  for (lu8t = 0; lu8t < cBotNet::enCnstSlaveCnt; lu8t++)
  {
    uint16 lui16Adr = mcBnAdr_0x1000.GetSlaveAdr(lu8t + 1);
    mcDownLinks_0x1000[lu8t] = (cBotNet_DownLinkI2c*) new cBotNet_DownLinkI2c(lui16Adr, mcI2C2_Master);
    mcI2cBn_0x1000->bAddLink((cBotNet_LinkBase*)mcDownLinks_0x1000[lu8t]);
  }

  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcI2cBn_0x1000->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  lu16PortIdx = mcI2cBn_0x1000->u16StreamPortAdd(&mcMemPort);
  mcI2cBn_0x1000->vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  lu16PortIdx = mcI2cBn_0x1000->u16StreamPortAdd(&mcMemPort);
  mcI2cBn_0x1000->vStreamPortConnect(lu16PortIdx, 0xE000, 1);


  // --------------- 0x0111 ------------------
  mcI2C1_Slave    = new cI2cSlave(I2C2,  &lcSCL_Slave,  &lcSDA_Slave,   1 << 1, 0);
  mcI2cBn_0x1100  = new cBotNet(&mcBnCfg_0x1100, &mcBn_MsgProcess_0x1100);

  // --- 0x0111 UpLink
  mcUpLnk_0x1100 = (cBotNet_UpLinkI2c*) new cBotNet_UpLinkI2c(0x0110, mcI2C1_Slave);
  mcI2cBn_0x1100->bAddLink((cBotNet_LinkBase*)mcUpLnk_0x1100);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcI2cBn_0x1100->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);



  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                MAIN_vTick1000msHp /*1s_HP*/,

                NULL               /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


void vCleanUp()
{
  delete mcUpLnk_0x1000;
  delete mcUpLnk_0x1100;
  for (int t = 0; t < cBotNet::enCnstSlaveCnt; t++)
  {
    delete mcDownLinks_0x1000[t];
    delete mcDownLinks_0x1100[t];
  }

  delete mcI2C1_Slave;
  delete mcI2C2_Master;
  delete mcI2cBn_0x1000;
  delete mcI2cBn_0x1100;
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

