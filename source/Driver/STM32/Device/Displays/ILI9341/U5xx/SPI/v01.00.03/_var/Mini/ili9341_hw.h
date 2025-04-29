
#ifndef _ILI9341_HW_H
#define _ILI9341_HW_H


/* Includes ------------------------------------------------------------------*/
#include "TYPEDEF.h"
#include "bitmap_Bpp16_5R6G5B.h"

#include "TypeDef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_gpio.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_spi.h"

#include "tGPPin.h"


class cILI9341_HW
{
public:
  cGpPin mSCK;
  cGpPin mMISO;
  cGpPin mMOSI;
  cGpPin mPinA0;
  cGpPin mPinRst;
  cGpPin mPinCs;

  cILI9341_HW()
    : mSCK(GPIOB_BASE,    3, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMISO(GPIOB_BASE,   4, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIOB_BASE,   5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinA0(GPIOB_BASE, 15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinRst(GPIOA_BASE,11, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinCs(GPIOA_BASE, 12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    LL_RCC_SetSPIClockSource(LL_RCC_SPI1_CLKSOURCE_SYSCLK);

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    mSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);  // MOSI

    vInitSpi();
  }

  void vInitSpi()
  {
    LL_SPI_InitTypeDef SPI_InitStruct;

    LL_SPI_Disable(SPI1);

    // SPI1_Init 1
    // SPI1 parameter configuration
    // Baudrate: 160Mhz / 4 = 40Mhz = 25 ns/Bit
    // => 320*240*2 = 153.600 Byte = 1.228.800 Bit
    // 0,025 us/Bit * 1.228.800 Bit = 30.720us =~ 31ms
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 0x7;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_EnableNSSPulseMgt(SPI1);

    LL_SPI_SetTransferSize(SPI1, 1);

    LL_SPI_Enable(SPI1);
  }


  bool isReady()
  {
    return  ((!LL_SPI_IsActiveFlag_TXP(SPI1)) && (LL_SPI_IsActiveFlag_EOT(SPI1)));
  }

  void fill(u16 data, u32 count)
  {
    u32 i;
    u8  CH, CL;

    CH = data >> 8;
    CL = (u8)data;

    LL_SPI_Disable(SPI1);
    LL_SPI_SetTransferSize(SPI1, 0); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(SPI1);

    LL_SPI_StartMasterTransfer(SPI1);
    SPI1->IFCR = 0x10;

    for (i = 0; i < count; i++)
    {
      while (!LL_SPI_IsActiveFlag_TXP(SPI1)); // Tx-Packet space available
      LL_SPI_TransmitData8(SPI1, CH);
      while (!LL_SPI_IsActiveFlag_TXP(SPI1)); // Tx-Packet space available
      LL_SPI_TransmitData8(SPI1, CL);
    }
    while (!LL_SPI_IsActiveFlag_TXC(SPI1)); // Wait for Fifo
    CLEAR_BIT(SPI1->CR1, SPI_CR1_CSTART);

    LL_SPI_Disable(SPI1);
    LL_SPI_SetTransferSize(SPI1, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(SPI1);
  }

  void write(u8 data)
  {
    // Master transfer start
    SET_BIT(SPI1->CR1, SPI_CR1_CSTART);
    SPI1->IFCR = 0xFFFF;

    while (!LL_SPI_IsActiveFlag_TXP(SPI1)); // Tx-Packet space available
    LL_SPI_TransmitData8(SPI1, data);
    while (!LL_SPI_IsActiveFlag_EOT(SPI1)); // End of Transmission
  }


  void write(uint8* lui8Data, uint32 lui32Len)
  {
    while (lui32Len >= 1)
    {
      write(*lui8Data++);
      lui32Len--;
    }
    while (!LL_SPI_IsActiveFlag_EOT(SPI1));
  }

  void cmd(u8 cmd)
  {
    mPinA0.vSet0();
    write(cmd);
    while (!LL_SPI_IsActiveFlag_EOT(SPI1));
  }

  void data(u8 data) {
    mPinA0.vSet1();
    write(data);
    while (!LL_SPI_IsActiveFlag_EOT(SPI1));
  }

  void cmd2(u8 cmd)
  {
    mPinCs.vSet0();
    mPinA0.vSet0();
    write(cmd);
    while (!LL_SPI_IsActiveFlag_EOT(SPI1));
    mPinCs.vSet1();
  }

  void data2(u8 data) {
    mPinCs.vSet0();
    mPinA0.vSet1();
    write(data);
    while (!LL_SPI_IsActiveFlag_EOT(SPI1));
    mPinCs.vSet1();
  }
};



#endif // _ILI9341_HW_H
