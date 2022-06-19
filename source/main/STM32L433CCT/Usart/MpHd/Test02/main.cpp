
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;
//LED<GPIOC_BASE, 13> lcLedRed;

cBotNetCfg mcBnCfg_0x1000((const char8*)"Botnet 0x1000 Master", 16, 0x1000);
cBotNetCfg mcBnCfg_0x1100((const char8*)"Botnet 0x1100 Slave", 16, 0x1100);

LED<GPIOB_BASE, 9> lcLedU2DmaTxTc;
LED<GPIOB_BASE, 8> lcLedU2DmaRxTc;
LED<GPIOB_BASE, 7> lcLedU2Tc;
LED<GPIOB_BASE, 6> lcLedU3DmaTxTc;
LED<GPIOB_BASE, 5> lcLedU3DmaRxTc;
LED<GPIOB_BASE, 4> lcLedU3Tc;

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
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

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



cBotNet                        *mcBn_0x1000; // Masternode for 011[x] all nodes, e.g. downstream to 0111
                                                // 0 CmdPort
                                                // 1 MemPort

// --- 0x1000 DownLinks
cGpPin                     mcU2TxRx(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cUartMpHdMaster            mcMasterUartMpHdU2(USART2, cBotNet::enCnstSlaveCnt, 115200);
cBotNet_DownLinkUsartMpHd *mcDownLinks_0x1000[cBotNet::enCnstSlaveCnt];


cBotNet                   *mcBn_0x1100; // Slavenode of 0110, =>upstream to 0110
                                                // 0 CmdPort
// --- 0x1100 UpLink
cGpPin                   mcU3TxRx(GPIOB_BASE,10, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cUartMpHdSlave           mcSlaveUartMpHdU3(USART3, 115200);
cBotNet_UpLinkUsartMpHd *mcUpLnk_0x1100;
// --- 0x0111 DownLinks
//cBotNet_DownLinkUsartMpHd *mcDownLinks_0x1100[cBotNet::enCnstSlaveCnt];


cBn_MsgProcess_0x1000 mcBn_MsgProcess_0x1000;
cBn_MsgProcess_0x1100 mcBn_MsgProcess_0x1100;


//tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort2(9600, GPIO_AF7_USART2, 64, 64);

//cBotNetStreamPort_ComPort       mcComPort2(USART2);
//cBotNetStreamPort_ComPort       mcComPort3(USART3);
cBotNetStreamPort_BotNetMemPort mcMemPort(IAP_PARTITION_COUNT, stInAppProg_Platform_Partitions);


void DMA1_Channel2_IRQHandler(void)
{
  // USART3 TX
  DMA1_Channel2->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC2;
  lcLedU3DmaTxTc.Toggle();
  mcSlaveUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
}

void DMA1_Channel3_IRQHandler(void)
{
  // USART3 RX
  DMA1_Channel3->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC3;
  lcLedU3DmaRxTc.Toggle();
  mcSlaveUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
}

void USART3_IRQHandler(void)
{
  if (USART3->ISR & LL_USART_ISR_TC)
  {
    USART3->ICR = LL_USART_ISR_TC;
    lcLedU3Tc.Toggle();
    mcSlaveUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
  }
  if (USART3->ISR & 0xF)
  {
    USART3->ICR = LL_USART_ISR_ORE;
    mcSlaveUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
  }
}


/*
void DMA1_Channel4_IRQHandler(void)
{
  // USART1 TX
  DMA1_Channel4->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC4;
  //mcUartMpHdU1.USART_DMA_IRQHandler();
}

void DMA1_Channel5_IRQHandler(void)
{
  // USART1 RX
  DMA1_Channel5->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC5;
  //mcUartMpHdU1.USART_DMA_IRQHandler();
}


void USART1_IRQHandler(void)
{
  if (USART2->ISR & LL_USART_ISR_TC)
  {
    USART2->ICR = LL_USART_ISR_TC;
    mcMasterUartMpHdU2.ComIrqHandler(cUartMpHd::enEvTyIrq, cUartMpHd::enEvUsartTc);
  }
}
*/


void DMA1_Channel6_IRQHandler(void)
{
  // USART2 RX
  DMA1_Channel6->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC6;
  lcLedU2DmaRxTc.Toggle();
  mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
}

void DMA1_Channel7_IRQHandler(void)
{
  // USART2 TX
  DMA1_Channel7->CCR &=  ~DMA_CCR_EN;
  DMA1->IFCR = DMA_FLAG_TC7;
  lcLedU2DmaTxTc.Toggle();
  mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
}

void USART2_IRQHandler(void)
{
  if (USART2->ISR & LL_USART_ISR_TC)
  {
    USART2->ICR = LL_USART_ISR_TC;
    lcLedU2Tc.Toggle();
    mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
  }
  if (USART2->ISR & 0xF)
  {
    USART2->ICR = LL_USART_ISR_ORE;
    mcMasterUartMpHdU2.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
  }
}


void TIM1_UP_TIM16_IRQHandler(void)
{
  if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM16->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcMasterUartMpHdU2.TIM_EV_IRQHandler();
    mcSlaveUartMpHdU3.TIM_EV_IRQHandler();
  }
}


void MAIN_vTick1msHp(void)
{
  mcBn_0x1100->vTickHp1ms();
  mcBn_0x1000->vTickHp1ms();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vTick10msLp(void)
{
  lcLedU2DmaTxTc.vSet0();
  lcLedU2DmaRxTc.vSet0();
  lcLedU2Tc.vSet0();
  lcLedU3DmaTxTc.vSet0();
  lcLedU3DmaRxTc.vSet0();
  lcLedU3Tc.vSet0();

  mcBn_0x1100->vTickLp10ms();
  mcBn_0x1000->vTickLp10ms();
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

  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  // --------------- 0x0111 ------------------
  mcBn_0x1000 = new cBotNet(&mcBnCfg_0x1000, &mcBn_MsgProcess_0x1000);

  // --- 0x0100 UpLink
  //mcUpLnk_0x1000Rf = (cBotNet_UpLinknRf905*) new cBotNet_UpLinknRf905(0xE000, &mcNRF905);
  //mcUpLnk_0x1000   = (cBotNet_UpLinknRf905Net*) new cBotNet_UpLinknRf905Net(mcUpLnk_0x1000Rf);

  // --- 0x0100 DownLinks
  cBotNetAdress mcBnAdr_0x1000;
  mcBnAdr_0x1000.Set(0x1000);
  for (lu8t = 0; lu8t < cBotNet::enCnstSlaveCnt; lu8t++)
  {
    uint16 lui16Adr = mcBnAdr_0x1000.GetSlaveAdr(lu8t + 1);
    mcDownLinks_0x1000[lu8t] = (cBotNet_DownLinkUsartMpHd*) new cBotNet_DownLinkUsartMpHd(lui16Adr, &mcMasterUartMpHdU2);
    mcBn_0x1000->bAddLink((cBotNet_LinkBase*)mcDownLinks_0x1000[lu8t]);
  }

  u16 lu16PortIdx;
  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn_0x1000->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  lu16PortIdx = mcBn_0x1000->u16StreamPortAdd(&mcMemPort);
  mcBn_0x1000->vStreamPortConnect(lu16PortIdx, 0xE000, 1);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  //lu16PortIdx = mcBn_0x1000->u16StreamPortAdd(&mcMemPort);
  //mcBn_0x1000->vStreamPortConnect(lu16PortIdx, 0xE000, 1);


  // --------------- 0x1100 ------------------
  mcBn_0x1100  = new cBotNet(&mcBnCfg_0x1100, &mcBn_MsgProcess_0x1100);

  // --- 0x0111 UpLink
  mcUpLnk_0x1100 = (cBotNet_UpLinkUsartMpHd*) new cBotNet_UpLinkUsartMpHd(0x1100, &mcSlaveUartMpHdU3);
  mcBn_0x1100->bAddLink((cBotNet_LinkBase*)mcUpLnk_0x1100);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  //mcBn_0x1100->vStreamPortConnect(cBotNet_CmdPortIdx, 0xE000, cBotNet_CmdPortIdx);


  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg, 64, 0x1000, 0x1100, 0x50);

  u8 t;

  for (t=0;t<64;t++)
  {
    lcMsgTestMsg.mcPayload[t] = t;
  }
  lcMsgTestMsg.vEncode();

  mcBn_0x1000->mcDownLinks[0]->mStatus.IsOnline = 1;
  mcBn_0x1000->bSendMsg(&lcMsgTestMsg);

  
  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg2, 64, 0x1100, 0x1000, 0x50);

  for (t=0;t<64;t++)
  {
    lcMsgTestMsg2.mcPayload[t] = t | 0x80;
  }
  lcMsgTestMsg2.vEncode();

  mcBn_0x1100->mcUpLink->mStatus.IsOnline = 1;
  mcBn_0x1100->bSendMsg(&lcMsgTestMsg2);



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
  /*delete mcUpLnk_0x1000;
  delete mcUpLnk_0x1100;
  for (int t = 0; t < cBotNet::enCnstSlaveCnt; t++)
  {
    delete mcDownLinks_0x1000[t];
    delete mcDownLinks_0x1100[t];
  }

  delete mcBn_0x1000;
  delete mcBn_0x1100;*/
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

void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|
                                       RCC_PERIPHCLK_USART2|
                                       RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1|
                                       RCC_PERIPHCLK_I2C2|
                                       RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

