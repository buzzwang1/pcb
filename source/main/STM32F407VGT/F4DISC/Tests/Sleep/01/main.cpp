
#include "main.h"

#define RTC_CLOCK_SOURCE_LSI


static volatile uint32 gui32BkpRamMagicNumber __attribute__ ((section (".BkpRam")));


USART_InitTypeDef USART_InitStructure;
RTC_InitTypeDef   RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;
RTC_TimeTypeDef   RTC_TimeStructure;
__IO uint32_t uwTimingDelay = 0;
__IO uint32_t AsynchPrediv = 0;
__IO uint32_t SynchPrediv = 0;

/* Private function prototypes -----------------------------------------------*/
static void RTC_Config_Standby(void);
static void RTC_Config_Stop(void);
static void SysTick_Configuration(void);
static void Delay(__IO uint32_t nTime);
static void SYSCLKConfig_STOP(void);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void RTC_Config_Standby(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Clear Wakeup flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Check if the StandBy flag is set */
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB);

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
    RTC_WaitForSynchro();
    /* No need to configure the RTC as the RTC config(clock source, enable,
       prescaler,...) are kept after wake-up from STANDBY */
  }
  else
  {
    /* RTC Configuration ******************************************************/
    /* Reset Backup Domain */
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
    RTC_WaitForSynchro();

    /* Set the RTC time base to 1s */
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0x00FF;

    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      /* Turn on LED6 */
      STM_EVAL_LEDOn(LED6);

      /* User can add here some code to deal with this error */
      while(1);
    }

    /* Set the time to 01h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x01;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;

    if(RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure) == ERROR)
    {
      /* Turn on LED6 */
      STM_EVAL_LEDOn(LED6);

      /* User can add here some code to deal with this error */
      while(1);
    }
  }

  /* Clear RTC Alarm Flag */
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}

static void RTC_Config_Stop(void)
{
  EXTI_InitTypeDef  EXTI_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  __IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* RTC Wakeup Interrupt Generation: Clock Source: RTCCLK_Div16, Wakeup Time Base: ~4s

     Wakeup Time Base = (16 / (LSE or LSI)) * WakeUpCounter
  */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(0x1FFF);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Connect EXTI_Line22 to the RTC Wakeup event */
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures system clock after wake-up from STOP: enable HSE, PLL
  *         and select PLL as system clock source.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }

  /* Enable PLL */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}

/**
  * @brief  Configures the SysTick to generate an interrupt each 250 millisecond.
  * @param  None
  * @retval None
  */
static void SysTick_Configuration(void)
{
  /* SysTick interrupt each 250 ms */
  if (SysTick_Config((SystemCoreClock/8) / 4))
  {
    /* Capture error */
    while (1);
  }

  /* Select AHB clock(HCLK) divided by 8 as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

  /* Set SysTick Preemption Priority to 1 */
  NVIC_SetPriority(SysTick_IRQn, 0x04);
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, with a base of 250 milliseconds.
  * @retval None
  */
static void Delay(__IO uint32_t nTime)
{
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);

}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);

  return ch;
}


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

void SysTick_Handler(void)
{
  uwTimingDelay--;
  /* Toggle LED6 */
  STM_EVAL_LEDToggle(LED6);
}

void RTC_WKUP_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    /* Toggle LED5 */
    STM_EVAL_LEDToggle(LED5);
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI_ClearITPendingBit(EXTI_Line22);
  }
}

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LED4 */
    STM_EVAL_LEDToggle(LED4);

    /* Clear the SEL Button EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);

    /* Disable the Alarm A */
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

    /* Disable RTC Alarm A Interrupt  */
    RTC_ITConfig(RTC_IT_ALRA, DISABLE);

    /* Clear Power WakeUp (CWUF) pending flag */
    PWR_ClearFlag(PWR_FLAG_WU);

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

    /* Set the alarm to current time + 5s */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_TimeStructure.RTC_H12;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = RTC_TimeStructure.RTC_Hours;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = RTC_TimeStructure.RTC_Minutes;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = (RTC_TimeStructure.RTC_Seconds + 0x5) % 60;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes;
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

    /* Enable RTC Alarm A Interrupt: this Interrupt will wake-up the system from
       STANDBY mode (RTC Alarm IT not enabled in NVIC) */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* Enable the Alarm A */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    /* Clear RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    //RTC_ClearFlag(RTC_FLAG_WUTF);

    /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
    PWR_EnterSTANDBYMode();
  }
}

int main(void)
{
  uint8 lui8Reset;
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

  /* USARTx configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  /* Output a message on Hyperterminal using printf function */
  printf("\r\nActivated by:\r\n");
  lui8Reset = 1;
  if (PWR_GetFlagStatus(PWR_FLAG_WU))
  {
    printf(" - PWR_FLAG_WU->Wakeup Event\r\n");

    if (RTC_GetFlagStatus(RTC_FLAG_ALRAF))
    {
      printf("   - by ALARM A\r\n");
    }
    else
    {
      printf("   - by Pin\r\n");
    }

    lui8Reset = 0;
  }

  if (PWR_GetFlagStatus(PWR_FLAG_SB))
  {
    printf(" - PWR_FLAG_SB->Standy by\r\n");
    lui8Reset = 0;
  }

  if (lui8Reset) printf(" - Reset\r\n");


  // Enable Backup SRAM
  {
    //Enable the PWR clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    //Enable access to the backup domain
    PWR_BackupAccessCmd(ENABLE);

    //Enable backup SRAM Clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

    // Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode
    PWR_BackupRegulatorCmd(ENABLE);

    if (gui32BkpRamMagicNumber == 0x12345678)
    {
      printf("Magic number found !\r\n");
    }
    else
    {
      gui32BkpRamMagicNumber = 0x12345678;
      printf("Magic number not found !\r\n");
    }
  }


  /* Initialize LEDs mounted on EVAL board */
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED4);

  /* Initialize Key Button mounted on EVAL board */
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* RTC configuration */
  RTC_Config_Standby();
  //RTC_Config_Stop();


  /* Enable WKUP pin  */
  PWR_WakeUpPinCmd(ENABLE);

  /* Configure the SysTick to generate an interrupt each 250 ms */
  SysTick_Configuration();

  while (1)
  {
    int ch;

    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET);
    ch = USART_ReceiveData(EVAL_COM1);
    printf("%c", ch&0xff);
  }
}

