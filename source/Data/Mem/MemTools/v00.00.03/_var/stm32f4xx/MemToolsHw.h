#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"

class cMemToolsHw
{
  public:

  DMA_InitTypeDef MemToolsHW_mstDMA;
  uint8           MemToolsHW_mui8Init = 0;

  static void vMemInitHW()
  {
    // Note:When memory - to - memory mode is used, the Circularand direct modes are not allowed.Only the DMA2 controller is able to perform memory - to - memory transfers.

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_StructInit(&MemToolsHW_mstDMA);

    MemToolsHW_mstDMA.DMA_Channel = DMA_Channel_0;

    DMA_ClearFlag(DMA2_Stream0, DMA_FLAG_TCIF0);

    //Configure the channel priority using the PL[1:0] bits in the DMA_CCRx register
    MemToolsHW_mstDMA.DMA_Priority = DMA_Priority_High;

    //5. Configure data transfer direction, circular mode, peripheral & memory incremented mode, peripheral & memory data size, and interrupt after half and/or full transfer in the DMA_CCRx register
    MemToolsHW_mstDMA.DMA_FIFOMode = DMA_FIFOMode_Enable;
    MemToolsHW_mstDMA.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    MemToolsHW_mstDMA.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    MemToolsHW_mstDMA.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    //Src Config
    MemToolsHW_mstDMA.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    MemToolsHW_mstDMA.DMA_MemoryInc  = DMA_MemoryInc_Enable;

    //Dest Config
    MemToolsHW_mstDMA.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    MemToolsHW_mstDMA.DMA_PeripheralInc = DMA_PeripheralInc_Enable;

    // Mode
    MemToolsHW_mstDMA.DMA_DIR = DMA_DIR_MemoryToMemory;
    MemToolsHW_mstDMA.DMA_Mode = DMA_Mode_Normal;

    DMA_Init(DMA2_Stream0, &MemToolsHW_mstDMA);

    MemToolsHW_mui8Init = 1;
  }


  static void vMemSet(uint8* lpui8Dest, uint8  lui8Value, u32 luixSize)
  {
      if (luixSize > 11)
      {
        uint8   lui32Value[4];
        u32     luixCount;

        if (!MemToolsHW_mui8Init) MEMTOOLS_vMemInitHW();

        while (((uint32)lpui8Dest) & 3)  // auf ganzen 32 Bit Adresses ?
        {
          *lpui8Dest++ = lui8Value;
          luixSize--;
        }

        lui32Value[0] = lui8Value;
        lui32Value[1] = lui8Value;
        lui32Value[2] = lui8Value;
        lui32Value[3] = lui8Value;

        luixCount = luixSize >> 2; // 4Byte Blöcke
        luixSize &= 3;

        DMA2_Stream0->PAR  = (uint32)lui32Value;
        DMA2_Stream0->CR &= (~DMA_SxCR_PINC);

        while (luixCount)
        {
          DMA2_Stream0->M0AR = (uint32)lpui8Dest;

          if (luixCount > 64000)
          {
            DMA2_Stream0->NDTR = 64000;
            lpui8Dest += 64000 * 4;
            luixCount -= 64000;
          }
          else
          {
            DMA2_Stream0->NDTR = luixCount;
            lpui8Dest += luixCount * 4;
            luixCount = 0;
          }

          /*Note:   Before setting EN bit to '1' to start a new transfer, the event flags corresponding to the stream in DMA_LISR or DMA_HISR register must be cleared.*/
          DMA2->LIFCR = 0xFF;

          /* Enable the selected DMA2_Stream7 by setting EN bit */
          DMA2_Stream0->CR |= (uint32_t)DMA_SxCR_EN; //Enable the DMA

          /* Wait for the end of Transfer */
          while ((DMA2_Stream0->CR & DMA_SxCR_EN) == DMA_SxCR_EN);
        }
      }


      // Den Rest byteweise
      while (luixSize > 0)
      {
        *lpui8Dest++ = lui8Value;
        luixSize--;
      }
  }


  static void vMemSet16(uint16* lpui16Dest, uint16 lui16Value, u32 luixSize)
  {
    if (luixSize > 11)
    {
      uint16  lui32Value[2];
      u32     luixCount;

      if (!MemToolsHW_mui8Init) MEMTOOLS_vMemInitHW();

      if (((uint32)lpui16Dest) & 3)  // auf ganzen 32 Bit Adresses ?
      {
        *lpui16Dest++ = lui16Value;
        luixSize--;
      }

      lui32Value[0] = lui16Value;
      lui32Value[1] = lui16Value;

      luixCount = luixSize >> 1; // 4Byte Blöcke
      luixSize &= 1;

      DMA2_Stream0->PAR = (uint32)lui32Value; // Source Memory
      DMA2_Stream0->CR &= (~DMA_SxCR_PINC);   // Not Inc Source Pointer

      while (luixCount)
      {
        DMA2_Stream0->M0AR = (uint32)lpui16Dest; // Destination Memory

        if (luixCount > 64000)
        {
          DMA2_Stream0->NDTR = 64000;
          lpui16Dest += (64000 * 2);
          luixCount  -= 64000;
        }
        else
        {
          DMA2_Stream0->NDTR = luixCount;
          lpui16Dest += (luixCount * 2);
          luixCount = 0;
        }

        /*Note:   Before setting EN bit to '1' to start a new transfer, the event flags corresponding to the stream in DMA_LISR or DMA_HISR register must be cleared.*/
        DMA2->LIFCR = 0xFF;

        /* Enable the selected DMA2_Stream7 by setting EN bit */
        DMA2_Stream0->CR |= (uint32_t)DMA_SxCR_EN; //Enable the DMA

        /* Wait for the end of Transfer */
        while ((DMA2_Stream0->CR & DMA_SxCR_EN) == DMA_SxCR_EN);
      }
    }


    // Den Rest byteweise
    while (luixSize > 0)
    {
      *lpui16Dest++ = lui16Value;
      luixSize--;
    }
  }


  static void vMemSet32(uint32* lpui32Dest, uint32 lui32Value, u32 luixSize)
  {
    if (!MemToolsHW_mui8Init) MEMTOOLS_vMemInitHW();

    DMA2_Stream0->M0AR = (uint32)lpui32Dest;
    DMA2_Stream0->PAR = (uint32)(&lui32Value);
    DMA2_Stream0->CR &=  (~DMA_SxCR_PINC);

    while (luixSize)
    {
      if (luixSize > 64000)
      {
        DMA2_Stream0->NDTR = 64000;
        luixSize -= 64000;
      }
      else
      {
        DMA2_Stream0->NDTR = luixSize;
        luixSize = 0;
      }

      /*Note:   Before setting EN bit to '1' to start a new transfer, the event flags corresponding to the stream in DMA_LISR or DMA_HISR register must be cleared.*/
      DMA2->LIFCR = 0xFF;

      /* Enable the selected DMA2_Stream7 by setting EN bit */
      DMA2_Stream0->CR |= (uint32_t)DMA_SxCR_EN; //Enable the DMA

      /* Wait for the end of Transfer */
      while ((DMA2_Stream0->CR & DMA_SxCR_EN) == DMA_SxCR_EN);
    }
  }


  static void vMemCpy(uint8* lpui8Dest, uint8* lpui8Source, u32 luixSize)
  {
    if (luixSize > 15) // 16Byte Blöcke
    {
      u32     luixCount;

      if (!MemToolsHW_mui8Init) MEMTOOLS_vMemInitHW();

      while (((uint32)lpui8Dest) & 3)  // auf ganzen 32 Bit Adresses ?
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }


      luixCount = luixSize >> 2; // 4Byte Blöcke
      luixSize &= 3;
    
      DMA2_Stream0->CR |= (DMA_SxCR_PINC);

      while (luixCount)
      {
        DMA2_Stream0->PAR  = (uint32)lpui8Source;
        DMA2_Stream0->M0AR = (uint32)lpui8Dest;

        if (luixCount > 64000)
        {
          DMA2_Stream0->NDTR = 64000;
          lpui8Dest   += 64000 * 4;
          lpui8Source += 64000 * 4;
          luixCount -= 64000;
        }
        else
        {
          DMA2_Stream0->NDTR = luixCount;
          lpui8Source += luixCount * 4;
          lpui8Dest   += luixCount * 4;
          luixCount = 0;
        }

        /*Note:   Before setting EN bit to '1' to start a new transfer, the event flags corresponding to the stream in DMA_LISR or DMA_HISR register must be cleared.*/
        DMA2->LIFCR = 0xFF;

        /* Enable the selected DMA2_Stream7 by setting EN bit */
        DMA2_Stream0->CR |= (uint32_t)DMA_SxCR_EN; //Enable the DMA

        /* Wait for the end of Transfer */
        while ((DMA2_Stream0->CR & DMA_SxCR_EN) == DMA_SxCR_EN);
      }
    }

    // Den Rest byteweise
    while (luixSize > 0)
    {
      *lpui8Dest++ = *lpui8Source++;
      luixSize--;
    }
  }
};

#endif /* _MEMTOOLSHW_H */
