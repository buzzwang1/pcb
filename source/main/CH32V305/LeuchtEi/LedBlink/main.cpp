
#include "main.h"


// CH32V305 => D8C, Core V4F IMAFC
// RiscV 32b MCU, (144 MHz max)
// Rom 128KB
// 
// PA0: LED:          Tim2_Ch2(AF1); Tim0_Ch1_On(AF2)
// 


//LED<GPIOB_BASE, 14> mcTestLed; // v0
LED<GPIOC_BASE, 2> mcTestLed; // v9


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}


void MAIN_vTick1000msLp(void)
{
  mcTestLed.Toggle();
}


void MAIN_vInitSystem(void)
{
  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __WFI();
  }
}


/*static void MainSetSysClockToHSE24Mhz(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);

  // Wait till HSE is ready and if Time out is reached exit
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;
  } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }
  if (HSEStatus == (uint32_t)0x01)
  {
    // HCLK = SYSCLK
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
    // PCLK2 = HCLK
    RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
    // PCLK1 = HCLK
    RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV1;

    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_PLLSRC | RCC_PLLXTPRE | RCC_PLLMULL));

    #ifdef CH32V30x_D8
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL3);
    #else
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL3_EXTEN);
    #endif
    // Enable PLL
    RCC->CTLR |= RCC_PLLON;

    // Wait till PLL is ready
    while ((RCC->CTLR & RCC_PLLRDY) == 0)
    {
    }
    // Select PLL as system clock source
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    // Wait till PLL is used as system clock source
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
    // If HSE fails to start-up, the application will have wrong clock
    // configuration. User can add here some code to deal with this error

  }
}*/


void MainSystemRccInit(void)
{
  #ifdef CH32V30x_D8C
    RCC->CFGR0 &= (uint32_t)0xF8FF0000;
  #else
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
  #endif 

  RCC->CTLR &= (uint32_t)0xFEF6FFFF;
  RCC->CTLR &= (uint32_t)0xFFFBFFFF;
  RCC->CFGR0 &= (uint32_t)0xFF80FFFF;

  #ifdef CH32V30x_D8C
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;
  #else
    RCC->INTR = 0x009F0000;
  #endif
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  MainSystemRccInit();

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  SystemCoreClockUpdate();
  cClockInfo::Update();
}

