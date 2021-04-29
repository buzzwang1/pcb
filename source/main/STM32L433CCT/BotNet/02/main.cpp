
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB


__IO uint32_t TimingDelay = 0;
LED<GPIOC_BASE, 13> lcLedRed;

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
      case 81: // Zum Test für direkte Master-Slave Kommunikation
      {
        u8                  lu8MsgData[20];
        cbArrayExtT<uint16> lpcMsgData(lu8MsgData, 20);
        lpcMsgData.muiLen = 6;
        cBotNetMsg_MsgProt lcMsg(&lpcMsgData);

        //lcMsg.vPrepare(False, False, True, cBotNetAdress::u16GetAdr(0,1,1,0), cBotNetAdress::u16GetAdr(0,1,1,1), (u16)82, (u8)0);
        lcMsg.vPrepare(cBotNetAdress::u16GetAdr(0,1,1,0), cBotNetAdress::u16GetAdr(0,1,1,1), (u16)82);
        lcMsg.mcPayload.Add(1);
        lcMsg.mcPayload.Add(2);
        lcMsg.mcPayload.Add(3);
        lcMsg.mcPayload.Add(4);
        lcMsg.mcPayload.Add(5);
        lcMsg.mcPayload.Add(6);

        lcMsg.vEncode();

        lpcBn->mcUpLink->mcRxComBuf.put(&lcMsg);
      }
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
cBotNet_LinkBotCom* mcUpLnk_0x1000;
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

// I2C1: PA9:  I2C1_SCL; PA10: I2C1_SDA
// I2C1: PB6:  I2C1_SCL; PB7:  I2C1_SDA
// I2C2: PB10: I2C2_SCL; PB11: I2C2_SDA
// I2C2: PB13: I2C2_SCL; PB14: I2C2_SDA
// I2C1:
cGpPin lcSCL_Master(GPIOB_BASE, 6, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSDA_Master(GPIOB_BASE, 7, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

// I2C2:
cGpPin lcSCL_Slave(GPIOB_BASE, 10, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSDA_Slave(GPIOB_BASE, 11, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


tcUart<USART2_BASE, GPIOA_BASE,  2, GPIOA_BASE,  3> mcComPort2(9600, GPIO_AF7_USART2, 64, 64);

//cBotNetStreamPort_ComPort       mcComPort2(USART2);
//cBotNetStreamPort_ComPort       mcComPort3(USART3);
cBotNetStreamPort_BotNetMemPort mcMemPort(IAP_PARTITION_COUNT, stInAppProg_Platform_Partitions);


// Für Performance Test
cGpPin lcSCL_Test1(GPIOB_BASE, 12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSCL_Test2(GPIOB_BASE, 13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSCL_Test3(GPIOB_BASE,  9, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin lcSCL_Test4(GPIOB_BASE, 15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);


void TIM1_BRK_TIM15_IRQHandler(void)
{
  GPIOB->BSRR = (1 << 12);

  if (TIM15->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM15->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcI2C2_Master->TIM_EV_IRQHandler();
  }  

  GPIOB->BRR = (1 << 12);
}

void I2C1_EV_IRQHandler(void)
{
  GPIOB->BSRR = (1 << 9);
  mcI2C2_Master->I2C_EV_IRQHandler();
  GPIOB->BRR = (1 << 9);
}

void I2C1_ER_IRQHandler(void)
{
  mcI2C2_Master->I2C_ER_IRQHandler();
}



void I2C2_EV_IRQHandler(void)
{
  GPIOB->BSRR = (1 << 15);
  mcI2C1_Slave->I2C_EV_IRQHandler();
  GPIOB->BRR = (1 << 15);
}

void I2C2_ER_IRQHandler(void)
{
  mcI2C1_Slave->I2C_ER_IRQHandler();
}



void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  mcComPort2.vIRQHandler();
}

void USART3_IRQHandler(void)
{
  //lcLedRed.Toggle();
  //mcComPort3.mcConIo.vIRQHandler_Ev();
}


void MAIN_vTick1msHp(void)
{
  mcI2cBn_0x1000->vSync();
  mcI2C2_Master->vStartNext();

  mcI2cBn_0x1100->vSync();
}

void MAIN_vTick10msHp(void)
{
}

void MAIN_vTick10msLp(void)
{
  mcI2cBn_0x1000->vProcess();
  mcI2cBn_0x1000->vTick10ms();

  mcI2cBn_0x1100->vProcess();
  mcI2cBn_0x1100->vTick10ms();
}

void MAIN_vTick100msLp(void)
{
  lcLedRed.Toggle();
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
  mcI2C2_Master  = new cI2cMaster(I2C1, &lcSCL_Master, &lcSDA_Master, 16);
  mcI2cBn_0x1000 = new cBotNet(&mcBnCfg_0x1000, &mcBn_MsgProcess_0x1000);

  // --- 0x0110 UpLink
  mcUpLnk_0x1000 = (cBotNet_LinkBotCom*) new cBotNet_LinkBotCom(0xE000, &mcComPort2);
  mcI2cBn_0x1000->bAddLink((cBotNet_LinkBase*)mcUpLnk_0x1000);

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


  // --- 0x0111 ----
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

