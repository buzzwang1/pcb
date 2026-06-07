
#include "main.h"


// STM32L433
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 64KB

__IO uint32_t TimingDelay = 0;

LED<GPIOB_BASE, 9> mcLedRed;

cGpPin mcI2c2_SCL_Board(GPIOB_BASE, 13, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin mcI2c2_SDA_Board(GPIOB_BASE, 14, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cI2cMaster   mcI2C2(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 1, 8, 400000, False);

cINA3221      mcINA3221_A(&mcI2C2, INA3221_I2C_ADDRESS_CONF_0, 10, 10, 10);
cINA3221      mcINA3221_B(&mcI2C2, INA3221_I2C_ADDRESS_CONF_1, 10, 10, 10);

int32 mi32IShuntA = 0;
int32 mi32VBusA   = 0;
int32 mi32IShuntB = 0;
int32 mi32VBusB   = 0;



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


void I2C2_EV_IRQHandler(void)
{
  mcI2C2.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  mcI2C2.I2C_ER_IRQHandler();
}


void MAIN_vTick10msHp(void)
{
  if (!mcI2C2.bStartNext())
  {
    mcI2C2.vSetReInitTicks(1000);
  }
}

void MAIN_vTick100msLp(void)
{
  mcINA3221_A.i8ReadVShunt_digit(1);
  mcINA3221_A.i8ReadVBus_digit(1);
  mcINA3221_A.i8ReadVShunt_digit(2);
  mcINA3221_A.i8ReadVBus_digit(2);
  mcINA3221_A.i8ReadVShunt_digit(3);
  mcINA3221_A.i8ReadVBus_digit(3);

  mcINA3221_B.i8ReadVShunt_digit(1);
  mcINA3221_B.i8ReadVBus_digit(1);
  mcINA3221_B.i8ReadVShunt_digit(2);
  mcINA3221_B.i8ReadVBus_digit(2);
  mcINA3221_B.i8ReadVShunt_digit(3);
  mcINA3221_B.i8ReadVBus_digit(3);
}

void MAIN_vTick1000msLp(void)
{

  mcLedRed.Toggle();

  //mi32IShuntA = mcINA3221_A.i32CalcIShunt_uA() / 1000;
  //mi32VBusA   = mcINA3221_A.i32CalcVBus_uV()   / 1000;
}

enum tenLogicPower : u8
{
  nP1 = 0,
  nP2,
  nP3
};

static tenLogicPower menLogicPower = tenLogicPower::nP1;

class cDigiPower
{
public:
  //Digi Power
  static cGpPin mMX22917_S1;
  static cGpPin mMX22917_S2;
};

cGpPin cDigiPower::mMX22917_S1(GPIOC_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
cGpPin cDigiPower::mMX22917_S2(GPIOC_BASE, 4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

cDigiPower    mcDigiPower;

void vInitLevel2()
{
  // S2   S1     R2     VOut
  //  0    0    620K   2V09
  //  0    1    409k   2V75
  //  1    0    447k   2V59
  //  1    1    325k   3V26
  cGpPin mTPS62125_S1(GPIOE_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  cGpPin mTPS62125_S2(GPIOE_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  cGpPin mTPS62125_PG(GPIOE_BASE, 9, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

  menLogicPower = nP1;

  if (mTPS62125_PG.ui8Get()) // 2V09 OK ?
  {
    // Try to set 2V75
    mTPS62125_S1.vSet1();
    for (int i = 0; i < 500000; i++) { __asm("nop"); }
    if (mTPS62125_PG.ui8Get()) // 2V75 OK ?
    {
      // ExtPowerReqCnt = 0
      menLogicPower = nP2;

      // Try to set 3V26
      mTPS62125_S2.vSet1();
      for (int i = 0; i < 500000; i++) { __asm("nop"); }
      if (mTPS62125_PG.ui8Get()) // 3V26 OK ?
      {
        menLogicPower = nP3;
      }
    }
  }

  //if (menLogicPower == nP1)
  //{
    // if (ExtPowerReqCnt < 3)
    //   ExtPowerReqCnt++
    //   Request external power source.
    //   Wait 100ms
    // else
    //  long sleep
  //}

  //if (menLogicPower != nP1)
  {
    mcDigiPower.mMX22917_S1.vSet1();
    mcDigiPower.mMX22917_S2.vSet1();
    for(int i = 0; i < 500000; i++) { __asm("nop"); }
  }
}


void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  vInitLevel2();
  mcI2C2.vInitHw();

  CycCall_Start(NULL /*1ms_HP*/,
                MAIN_vTick10msHp /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);


  if (mcI2C2.bStartNext())
  {
    cClockInfo::Delay_ms(100);
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
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
  PeriphClkInit.PeriphClockSelection = //RCC_PERIPHCLK_USART1|
                                       //RCC_PERIPHCLK_USART2|
                                       //RCC_PERIPHCLK_USART3|
                                       RCC_PERIPHCLK_I2C1|
                                       RCC_PERIPHCLK_I2C2;
                                       //RCC_PERIPHCLK_I2C3;
  //PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  //PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  //PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  //PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI;
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

// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

