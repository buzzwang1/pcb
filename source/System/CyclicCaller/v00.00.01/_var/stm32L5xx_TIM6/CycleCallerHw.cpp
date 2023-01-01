
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_1ms_HP;


/* TIM6 - 16Bit Timer  */

void TIM6_IRQHandler(void)
{
  if (TIM6->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM6->SR &= ~TIM_SR_UIF; // clear UIF flag
    mptrCycCall_Callback_1ms_HP();
  }
}

void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP)
{
  mptrCycCall_Callback_1ms_HP = lptrCycCall_Callback_1ms_HP;

  /*##-1- Enable peripheral clock #################################*/
  /* TIMx Peripheral clock enable */
  __HAL_RCC_TIM6_CLK_ENABLE();

  /*##-2- Configure the NVIC for TIMx ########################################*/
  /* Set the TIMx priority */
  HAL_NVIC_SetPriority(TIM6_IRQn, 8, 8);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIM6_IRQn);

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM2 input clock (TIM2CLK)  is set to APB1 clock (PCLK1),
    since APB1 prescaler is equal to 1.
      TIM2CLK = PCLK1
      PCLK1 = HCLK
      => TIM2CLK = HCLK = SystemCoreClock
    To get TIM2 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM2CLK / TIM2 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32l4xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */
  TIM_HandleTypeDef TimHandle = {};

  /* Set TIM3 instance */
  TimHandle.Instance = TIM6;

  /* Initialize TIM2 peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/1000000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Init.Period            = 1000-1; // 1ms
  TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) -1;
  //TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  //TimHandle.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)
  {
    /* Starting Error */
    //Error_Handler();
  }
}
