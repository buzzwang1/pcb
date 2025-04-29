
#include "main.h"


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB



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
}


template <uintptr_t mpstPort>
class tcPort
{
public:
  tcPort(uint32 lui32PortRCC)
  {
    /* GPIO Periph clock enable */
    RCC_AHBPeriphClockCmd(lui32PortRCC, ENABLE);
  }

  inline volatile GPIO_TypeDef* Port()
  {
    return (volatile GPIO_TypeDef*)mpstPort;
  }
};

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class tcGPIN : public tcPort<mpstPort>
{
public:
  tcGPIN(uint32 lui32PortRCC,
         GPIOMode_TypeDef lenMode,
         GPIOOType_TypeDef lenOType,
         GPIOPuPd_TypeDef lenPuPd,
         GPIOSpeed_TypeDef lenSpeed,
         uint8 lui8InitValue)
    : tcPort<mpstPort>(lui32PortRCC)
  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    vSet(lui8InitValue);
    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin   = PinBitMask();
    GPIO_InitStructure.GPIO_Mode  = lenMode;
    GPIO_InitStructure.GPIO_OType = lenOType;
    GPIO_InitStructure.GPIO_PuPd  = lenPuPd;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init((GPIO_TypeDef*)this->Port(), &GPIO_InitStructure);
  }

  inline uint8 ui8Get()
  {
    if (this->Port()->IDR & PinBitMask())
    {
      return 1;
    }
    return 0;
  }

  inline void vSet(uint8 lui8Value)
  {
    if (lui8Value == 0)
    {
      vSet0();
    }
    else
    {
      vSet1();
    }
  }

  inline void vSet1(void)
  {
    this->Port()->BSRR = PinBitMask();
  }

  inline void vSet0(void)
  {
    this->Port()->BRR = PinBitMask();
  }

  inline void vToggle(void)
  {
    this->Port()->ODR ^= PinBitMask();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};




class cGPPIN
{
public:

  cGPPIN(uint32 lui32PortRCC,
    GPIO_TypeDef* lpstPort,
    uint16 lui16Pin,
    GPIOMode_TypeDef lenMode,
    GPIOOType_TypeDef lenOType,
    GPIOPuPd_TypeDef lenPuPd,
    GPIOSpeed_TypeDef lenSpeed,
    uint8 lui8InitValue)
  {
    GPIO_InitTypeDef  GPIO_InitStructure;

    mpstPort = lpstPort;
    mui16Pin = lui16Pin;

    /* GPIO Periph clock enable */
    RCC_AHBPeriphClockCmd(lui32PortRCC, ENABLE);

    vSet(lui8InitValue);

    /* Configure GPIO_Pin */
    GPIO_InitStructure.GPIO_Pin   = lui16Pin;
    GPIO_InitStructure.GPIO_Mode  = lenMode;
    GPIO_InitStructure.GPIO_OType = lenOType;
    GPIO_InitStructure.GPIO_PuPd  = lenPuPd;
    GPIO_InitStructure.GPIO_Speed = lenSpeed;
    GPIO_Init(lpstPort, &GPIO_InitStructure);
  }

  ~cGPPIN()
  {
  }

  inline uint8 ui8Get()
  {
    if (mpstPort->IDR & mui16Pin)
      return 1;
    else
      return 0;
  }

  inline void vSet(uint8 lui8Value)
  {
    if (lui8Value == 0)
    {
      vSet0();
    }
    else
    {
      vSet1();
    }
  }

  inline void vSet1(void)
  {
    mpstPort->BSRR = mui16Pin;
  }

  inline void vSet0(void)
  {
    mpstPort->BRR = mui16Pin;
  }

  inline void vToggle(void)
  {
    mpstPort->ODR ^= mui16Pin;
  }

private:
  GPIO_TypeDef* mpstPort;
  uint16        mui16Pin;
};


cGPPIN mcLed(RCC_AHBPeriph_GPIOE, GPIOE, GPIO_Pin_14, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0);
tcGPIN<GPIOE_BASE, 14> mtyLed(RCC_AHBPeriph_GPIOE, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_50MHz, 0);


int main(void)
{

  while (1)
  {
    asm( "nop" );
    asm( "nop" );
    asm( "nop" );
    asm( "nop" );

    mcLed.vSet0();
    mcLed.vSet1();

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    mtyLed.vSet0();
    mtyLed.vSet1();

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");

    GPIOE->BRR  = GPIO_Pin_14;
    GPIOE->BSRR = GPIO_Pin_14;

    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

