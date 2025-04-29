
#include "main.h"
#include "stdio.h"
#include "string.h"

// CH32V305
// RiscV 32b MCU, (144 MHz max)
// Rom 128KB, Ram 32kb

//
// TIM1(AT), TIM2(GP), TIM3(GP), TIM4(GP), TIM5(GP), TIM6(BT), TIM7(BT), TIM8(AT), TIM9(AT), TIM10(AT)


LED<GPIOA_BASE, 0>    mcPA00;
LED<GPIOA_BASE, 1>    mcPA01;


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    volatile u32 lu32MCause;
    CSRR_READ(lu32MCause, 0x342);
    if ((lu32MCause & 0x80000000) == 0) // Is it an Exception ?
    {
      switch (lu32MCause & 0xFF)
      {
        case 0: // Synchronous Instruction address misalignment
          asm volatile ("NOP");
          break;
        case 1 : // Synchronous Fetch command access error
          asm volatile ("NOP");
          break;
        case 2 : // Synchronous Illegal instructions
          asm volatile ("NOP");
          break;
        case 3 : // Synchronous Breakpoints
          asm volatile ("NOP");
          break;
        case 4 : // Synchronous Load instruction access address misalignment
          asm volatile ("NOP");
          break;
        case 5 : // Non - precision asynchronous Load command access error
          asm volatile ("NOP");
          break;
        case 6 : // Synchronous Store / AMO instruction access address misalignment
          asm volatile ("NOP");
          break;
        case 7 : // Non - precision asynchronous Store / AMO command access error
          asm volatile ("NOP");
          break;
        case 8 : // Synchronous Environment call in User mode
          asm volatile ("NOP");
          break;
        case 11 : // Synchronous Environment call in Machine mode
          asm volatile ("NOP");
          break;
      }
    }
  #endif

  while (1)
  {
  }
}

static void vMemToolInit()
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

  DMA_InitTypeDef      DMA_InitStructure;
  // configure the USART TX DMA channel
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)0;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)0;
  DMA_InitStructure.DMA_BufferSize = 0;

  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;

  DMA_InitStructure.DMA_Mode = DMA_Priority_Low;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
  DMA_Init(DMA2_Channel1, &DMA_InitStructure);
}

static void vMemCpyDma(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
{
  GPIOA->OUTDR |= (1 << 0);
  DMA2->INTFCR = DMA2_IT_TC1 | DMA2_IT_TE1;

  DMA2_Channel1->CFGR &= ~(1 << 0); // ~EN
  DMA2_Channel1->PADDR = (u32)lpui8Source;
  DMA2_Channel1->MADDR = (u32)lpui8Dest;
  DMA2_Channel1->CNTR = luixSize;
  DMA2_Channel1->CFGR |= (1 << 0); // EN

  while ((DMA_GetFlagStatus(DMA2_FLAG_TC1) == RESET) &&
         (DMA_GetFlagStatus(DMA2_FLAG_TE1) == RESET))
  {
  }
  GPIOA->OUTDR &= ~(1 << 0);
}

static void vMemCpySw(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)  __attribute__((optimize("-Ofast")));
static void vMemCpySw(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
{
  GPIOA->OUTDR |= (1 << 1);  
  while (luixSize > 7)
  {
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    *lpui8Dest++ = *lpui8Source++;
    luixSize -= 8;
  }

  switch (luixSize)
  { 
    case 7: 
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 6:
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 5:
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 4:
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 3:
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 2:
      *lpui8Dest++ = *lpui8Source++;
      *lpui8Dest++ = *lpui8Source++;
      break;
    case 1:
      *lpui8Dest++ = *lpui8Source++;
      break;
  }
  GPIOA->OUTDR &= ~(1 << 1);
}

static void vMemCpySw1(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)  __attribute__((optimize("-Ofast")));
static void vMemCpySw1(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
{
  GPIOA->OUTDR |= (1 << 1);
  while (luixSize)
  {
    *lpui8Dest++ = *lpui8Source++;
    luixSize--;
  }
  GPIOA->OUTDR &= ~(1 << 1);
}

static void vMemCpySw2(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
{
  GPIOA->OUTDR |= (1 << 1);
  memcpy(lpui8Dest, lpui8Source, luixSize);
  GPIOA->OUTDR &= ~(1 << 1);
}


void MAIN_vInitSystem(void)
{
  vMemToolInit();
}

u8 BufSrc[130];
u8 BufDst[130];

int main(void)
{
  MAIN_vInitSystem();


  while (1)
  {
    // Aligned
    vMemCpyDma(BufDst, BufSrc, 8);
    vMemCpyDma(BufDst, BufSrc, 16);
    vMemCpyDma(BufDst, BufSrc, 32);
    vMemCpyDma(BufDst, BufSrc, 64);
    vMemCpyDma(BufDst, BufSrc, 128);

    vMemCpySw(BufDst, BufSrc, 8);
    vMemCpySw(BufDst, BufSrc, 16);
    vMemCpySw(BufDst, BufSrc, 32);
    vMemCpySw(BufDst, BufSrc, 64);
    vMemCpySw(BufDst, BufSrc, 128);

    vMemCpySw1(BufDst, BufSrc, 8);
    vMemCpySw1(BufDst, BufSrc, 16);
    vMemCpySw1(BufDst, BufSrc, 32);
    vMemCpySw1(BufDst, BufSrc, 64);
    vMemCpySw1(BufDst, BufSrc, 128);

    vMemCpySw2(BufDst, BufSrc, 8);
    vMemCpySw2(BufDst, BufSrc, 16);
    vMemCpySw2(BufDst, BufSrc, 32);
    vMemCpySw2(BufDst, BufSrc, 64);
    vMemCpySw2(BufDst, BufSrc, 128);

    // Unaligned
    vMemCpyDma(BufDst + 1, BufSrc + 1, 8);
    vMemCpyDma(BufDst + 1, BufSrc + 1, 16);
    vMemCpyDma(BufDst + 1, BufSrc + 1, 32);
    vMemCpyDma(BufDst + 1, BufSrc + 1, 64);
    vMemCpyDma(BufDst + 1, BufSrc + 1, 128);

    vMemCpySw(BufDst + 1, BufSrc + 1, 8);
    vMemCpySw(BufDst + 1, BufSrc + 1, 16);
    vMemCpySw(BufDst + 1, BufSrc + 1, 32);
    vMemCpySw(BufDst + 1, BufSrc + 1, 64);
    vMemCpySw(BufDst + 1, BufSrc + 1, 128);

    vMemCpySw1(BufDst + 1, BufSrc + 1, 8);
    vMemCpySw1(BufDst + 1, BufSrc + 1, 16);
    vMemCpySw1(BufDst + 1, BufSrc + 1, 32);
    vMemCpySw1(BufDst + 1, BufSrc + 1, 64);
    vMemCpySw1(BufDst + 1, BufSrc + 1, 128);

    vMemCpySw2(BufDst + 1, BufSrc + 1, 8);
    vMemCpySw2(BufDst + 1, BufSrc + 1, 16);
    vMemCpySw2(BufDst + 1, BufSrc + 1, 32);
    vMemCpySw2(BufDst + 1, BufSrc + 1, 64);
    vMemCpySw2(BufDst + 1, BufSrc + 1, 128);


    //__asm("wfi");
  }
}



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
  //cMemTools::vMemToolInit();
}



