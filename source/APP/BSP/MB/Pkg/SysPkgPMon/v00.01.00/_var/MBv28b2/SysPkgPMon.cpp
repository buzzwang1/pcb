
#include "SysPkgPMon.h"


cSysPkgPMon::cSysPkgPMon()
{
  mu8IntLvl = 0;
};

void cSysPkgPMon::vInit1(void)
{
}

void cSysPkgPMon::vInit2(void)
{
}

void cSysPkgPMon::vTick1msHp(void)
  {
  }

void cSysPkgPMon::vTick1msLp(void)
{
}

void cSysPkgPMon::vTick10msLp(void)
{
}

void cSysPkgPMon::vTick100msLp(void)
{
}

void cSysPkgPMon::vTick1000msLp(void)
{
}

void cSysPkgPMon::vInitTimer(u32 lu32InitTime_u32)
{
  UNUSED(lu32InitTime_u32);
  #ifdef PCB_PROJECTCFG_Test
    cDiffTimerHw::InitTimer(lu32InitTime_u32);
  #endif
}

void cSysPkgPMon::vPB00_Init()
{
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
  //GPIOB->MODER &= ~(3 << (2 * 0));
  //GPIOB->MODER |= (1 << (2 * 0));
}

void cSysPkgPMon::vPB00_Set1()
{
  //GPIOB->BSRR = (1 << 0);
}

void cSysPkgPMon::vPB00_Set0()
{
  //GPIOB->BSRR = (1 << (16 + 0));
}

void cSysPkgPMon::vPA01_Init()
{
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  //GPIOA->MODER &= ~(3 << (2 * 1));
  //GPIOA->MODER |= (1 << (2 * 1));
}

void cSysPkgPMon::vPA01_Set1()
{
  //GPIOA->BSRR = (1 << 1);
}

void cSysPkgPMon::vPA01_Set0()
{
  //GPIOA->BSRR = (1 << (16 + 1));
}

void cSysPkgPMon::vPA05_Init()
{
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  //GPIOA->MODER &= ~(3 << (2 * 5));
  //GPIOA->MODER |= (1 << (2 * 5));
}

void cSysPkgPMon::vPA05_Set1()
{
  //GPIOA->BSRR = (1 << 5);
}

void cSysPkgPMon::vPA05_Set0()
{
  //GPIOA->BSRR = (1 << (16 + 5));
}


void cSysPkgPMon::vPA08_Init()
{
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  //GPIOA->MODER &= ~(3 << (2 * 8));
  //GPIOA->MODER |= (1 << (2 * 8));
}

void cSysPkgPMon::vPA08_Set1()
{
  //GPIOA->BSRR = (1 << 8);
}

void cSysPkgPMon::vPA08_Set0()
{
  //GPIOA->BSRR = (1 << (16 + 8));
}

