#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_dma.h"


class cMemToolsHw
{
  public:

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

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA2_Channel1, &DMA_InitStructure);
  }

  static void vMemCpyDma(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
  {
    //GPIOA->OUTDR &= ~(1 << 2);
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
    //GPIOA->OUTDR |= (1 << 2);
  }

  static void vMemCpy(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
  {
    if (luixSize > 7)
    {
      while (luixSize >= 65535)
      {
        vMemCpyDma(lpui8Dest, lpui8Source, 65535);
        luixSize -= 65535;
      }

      if (luixSize)
      {
        vMemCpyDma(lpui8Dest, lpui8Source, luixSize);
      }
    }
    else
    {
      while (luixSize)
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }
    }
  }


  static void vMemSet(uint8* lpui8Dest, const uint8  lui8Value, size_t luixSize)
  {
    while (luixSize > 7)
    {
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      *lpui8Dest++ = lui8Value;
      luixSize -= 8;
    }

    while (luixSize)
    {
      *lpui8Dest++ = lui8Value;
      luixSize--;
    }
  }

  static void vMemSet16(uint16* lpui16Dest, const uint16 lui16Value, size_t luixSize)
  {
    while (luixSize)
    {
      *lpui16Dest++ = lui16Value;
      luixSize--;
    }
  }

  static void vMemSet32(uint32* lpui32Dest, const uint32 lui32Value, size_t luixSize)
  {
    while (luixSize)
    {
      *lpui32Dest++ = lui32Value;
      luixSize--;
    }
  }
};

#endif /* _MEMTOOLSHW_H */
