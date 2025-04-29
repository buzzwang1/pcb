
#ifndef __ST7735__HW_H__
#define __ST7735__HW_H__


#include "TypeDef.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_spi.h"

#include "ClockInfo.h"
#include "tGpPin.h"

class cST7735_HW
{
  public:

  SPI_TypeDef* mSpi;
  //DMA_HandleTypeDef mh_GPDMA1;
  //LL_DMA_LinkNodeTypeDef mpDmaNode;

  bool isDmaBusy;

  //  LCD_SDA: MOSI: PE14: SPI4: AF5
  //  LCD_AD:  GPIO: PE13
  //  LCD_SCL: SCK:  PE12: SPI4: AF5
  //  LCD_CS:  GPIO: PE11
  //  LCD_LED: Tim1: PE10: Tim1 Chnl2N: AF1
  //
  //
  //

  tcGpPin<GPIOE_BASE, 12> mSCK;    //
  tcGpPin<GPIOE_BASE, 14> mMOSI;   //
  tcGpPin<GPIOE_BASE, 13> mPinA0;  // ST7735 A0 (Data/Command select) pin
  tcGpPinDummy            mPinRst; // ST7735 Reset is done in HW
  tcGpPin<GPIOE_BASE, 11> mPinCs;  // ST7735 CS (Chip Select) pin
  tcGpPin<GPIOE_BASE, 10> mPinLed; // ST7735 Background Led

  cST7735_HW()
    : mSCK( GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mMOSI(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),

      mPinA0( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinRst(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mPinCs( GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),

      mPinLed(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    isDmaBusy = False;

    mSpi = SPI4;

    if (mSpi == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    else
    if (mSpi == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    else
    if (mSpi == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();
    else
    if (mSpi == SPI4) __HAL_RCC_SPI4_CLK_ENABLE();

    mSCK.vSetAF(GPIO_MODE_AF_PP,  GPIO_AF5_SPI4);  // SCK
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI4);  // MOSI
    mPinLed.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM1);

    // Configure and enable SPI
    cClockInfo::Update();

    LL_SPI_InitTypeDef SPI;
    SPI.Mode              = LL_SPI_MODE_MASTER;
    SPI.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
    SPI.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
    SPI.ClockPolarity     = LL_SPI_POLARITY_LOW;
    SPI.ClockPhase        = LL_SPI_PHASE_1EDGE;
    SPI.NSS               = LL_SPI_NSS_SOFT;
    SPI.BitOrder          = LL_SPI_MSB_FIRST;
    SPI.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
    SPI.CRCPoly           = 7;

    /* 80*160 = 20480 Pxl = 25600 Byte
    *  25600 Byte/frame => 204800 Bit/frame => 4.096.000 Bit/s @ 20frames/s
    */
    switch (cClockInfo::mstClocks.PCLK1_Frequency)
    {
      case 280000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_16; break;    //280Mhz  / 16 =>  17.5 Mhz
      case 140000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_8;  break;    //140Mhz  /  8 =>  17.5 Mhz
      case  80000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_4;  break;    // 80Mhz  /  4 =>  20.0 Mhz
      case  64000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_4;  break;    // 64Mhz  /  4 =>  16.0 Mhz
      case  16000000: SPI.BaudRate = SPI_BAUDRATEPRESCALER_2;  break;    // 16Mhz  /  2 =>   8.0 Mhz
      default: SPI.BaudRate = SPI_BAUDRATEPRESCALER_8; break;
    }

    LL_SPI_Init(mSpi, &SPI);
    LL_SPI_SetTransferSize(mSpi, 1);
    LL_SPI_Enable(mSpi);

    vInitDMA();
    vInitTimer();
  };

  bool isDisplayInversionOn() {return True;}
  bool u8BGR()                {return 1;}
  i16 i16GetWidth()  {return 80;}
  i16 i16GetHeight() {return 160;}
  i16 i16GetOftX()   {return 26;}
  i16 i16GetOftY()   {return 1;}


  void vFlipByte(u8* lpui8Bm)
  {
    UNUSED(lpui8Bm);
    // Wird nur bei SPI = 8Bit benötigt
    // High und Low Byte order drehen
    //u16 i;
    //u8  d;
    //
    //for (i = 0; i < (i16GetWidth() * i16GetHeight() * 2); i += 2)
    //{
    //  d = lpui8Bm[i];
    //  lpui8Bm[i] = lpui8Bm[i + 1];
    //  lpui8Bm[i + 1] = d;
    //}
  }

  void vInitTimer()
  {
    TIM_HandleTypeDef htim1;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

    __HAL_RCC_TIM1_CLK_ENABLE();

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 14 - 1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000 - 1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim1);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);


    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 50);
  }

  void vInitDMA(void)
  {
    /* (1) Enable the clock of DMA2 */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* (2) Configure the DMA functional parameters */
    /* Configuration of the DMA parameters can be done using unitary functions or using the specific configure function */
    /* Unitary Functions */

    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetStreamPriorityLevel(DMA1,   LL_DMA_STREAM_2, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode(DMA1,                  LL_DMA_STREAM_2, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1,         LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1,         LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1,            LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1,            LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_HALFWORD);

    /* Configure Function */
    //LL_DMA_SetDataLength(DMA1,            LL_DMA_STREAM_2, i16GetWidth() * i16GetHeight());
    //LL_DMA_ConfigAddresses(DMA1,          LL_DMA_STREAM_2, (uint32_t)0, (u32) &(mSpi->TXDR), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_2));

    /* (3) Configure NVIC for DMA transfer complete/error interrupts */
    //LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_2);
    //NVIC_SetPriority(DMA2_Stream2_IRQn, 0);
    //NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  }

  bool isReady()
  {
    //return ((DMA1_Channel3->CNDTR == 0) && (!LL_SPI_IsActiveFlag_BSY(mSpi)));
    return (!isDmaBusy);
  }




  void writeDma(uint8* lui8Data, uint32 lui32Len)
  {
    if (!isDmaBusy)
    {
      isDmaBusy = True;

      LL_SPI_Disable(mSpi);
      LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_16BIT); lui32Len /= 2;
      LL_SPI_SetTransferSize(mSpi, lui32Len);
      LL_SPI_Enable(mSpi);


      //Control register
      //GPDMA1_Channel6->CCR;
      // Clear Flag register
      LL_DMA_ClearFlag_TC2(DMA1);

      /* Configure Function */
      LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_2, lui32Len);
      LL_DMA_ConfigAddresses(DMA1, LL_DMA_STREAM_2, (u32)lui8Data, (u32) & (mSpi->TXDR), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_2));


      LL_DMAMUX_SetRequestID(DMAMUX1, LL_DMAMUX_CHANNEL_2, LL_DMAMUX1_REQ_SPI4_TX);


      /* (3) Configure NVIC for DMA transfer complete/error interrupts */
      LL_SPI_EnableIT_EOT(mSpi);
      NVIC_SetPriority(SPI4_IRQn, 0);
      NVIC_EnableIRQ(SPI4_IRQn);

      LL_SPI_EnableDMAReq_TX(mSpi);
      SET_BIT(mSpi->CR1, SPI_CR1_CSTART);
      LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);
    }
  }

  void vStopDma()
  {
    isDmaBusy = False;
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
    LL_DMA_ClearFlag_TC2(DMA1);

    LL_SPI_DisableIT_EOT(mSpi);
    mSpi->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    LL_SPI_DisableDMAReq_TX(mSpi);
    LL_SPI_Disable(mSpi);
    LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetTransferSize(mSpi, 1);
    LL_SPI_Enable(mSpi);
  }


  void fill(u16 data, u32 count)
  {
    u32 i;
    u8  CH, CL;

    CH = data >> 8;
    CL = (u8)data;

    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, 0); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(mSpi);

    LL_SPI_StartMasterTransfer(mSpi);
    mSpi->IFCR = 0x10;

    for (i = 0; i < count; i++)
    {
      while (!LL_SPI_IsActiveFlag_TXP(mSpi)); // Tx-Packet space available
      LL_SPI_TransmitData8(mSpi, CH);
      while (!LL_SPI_IsActiveFlag_TXP(mSpi)); // Tx-Packet space available
      LL_SPI_TransmitData8(mSpi, CL);
    }
    while (!LL_SPI_IsActiveFlag_TXC(mSpi)); // Wait for Fifo
    CLEAR_BIT(mSpi->CR1, SPI_CR1_CSTART);

    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(mSpi);
  }

  void write(uint8_t data)
  {
    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(mSpi);

    // Master transfer start
    while (!LL_SPI_IsActiveFlag_TXP(mSpi)); // Tx-Packet space available

    SET_BIT(mSpi->CR1, SPI_CR1_CSTART);
    LL_SPI_TransmitData8(mSpi, data);
    while (!LL_SPI_IsActiveFlag_EOT(mSpi)); // End of Transmission

    mSpi->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    LL_SPI_Disable(mSpi);
    LL_SPI_Enable(mSpi);

    // Master data transfer stall at system clock much faster than SCK
    //   Description
    //   With the system clock(spi_pclk) substantially faster than SCK(spi_ker_ck divided by a
    //     prescaler), SPI / I2S master data transfer can stall upon setting the CSTART bit within one
    //   SCK cycle after the EOT event(EOT flag raise) signaling the end of the previous transfer.
    //   Workaround
    //   Apply one of the following measures :
    //     • Disable then enable SPI / I2S after each EOT event.
    //     • Upon EOT event, wait for at least one SCK cycle before setting CSTART.
    //     • Prevent EOT events from occurring, by setting transfer size to undefined(TSIZE = 0)
    //         and by triggering transmission exclusively by TXFIFO writes.
  }

  void write(uint8* lui8Data, uint32 lui32Len)
  {

    // SPI4: Maximum configurable data and CRC size[bits] => 16Bit
    LL_SPI_Disable(mSpi);
    LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_16BIT); lui32Len /= 2;
    LL_SPI_SetTransferSize(mSpi, lui32Len);
    LL_SPI_Enable(mSpi);

    // Master transfer start
    SET_BIT(mSpi->CR1, SPI_CR1_CSTART);

    while (lui32Len >= 1)
    {
      while (!LL_SPI_IsActiveFlag_TXP(mSpi)); // Tx-Packet space available
      LL_SPI_TransmitData16(mSpi, *(u16*)lui8Data);
      lui8Data += 2;
      lui32Len--;
    }
    while (!LL_SPI_IsActiveFlag_EOT(mSpi));

    mSpi->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    LL_SPI_Disable(mSpi);
    LL_SPI_SetTransferSize(mSpi, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_SetDataWidth(mSpi, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_Enable(mSpi);
  }

  void cmd(uint8_t cmd)
  {
    mPinA0.vSet0();
    write(cmd);
  }

  void data(uint8_t data)
  {
    mPinA0.vSet1();
    write(data);
  }

  void cmd2(uint8_t cmd) // __attribute__((optimize("O0")))
  {
    mPinCs.vSet0();
    mPinA0.vSet0();
    write(cmd);
    mPinCs.vSet1();
  }

  void data2(uint8_t data) // __attribute__((optimize("O0")))
  {
    mPinCs.vSet0();
    mPinA0.vSet1();
    write(data);
    mPinCs.vSet1();
  }
};



#endif // __ST7735__HW_H__
