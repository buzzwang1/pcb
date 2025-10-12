#ifndef __OSPI_ZD25WQ32_HW_H__
#define __OSPI_ZD25WQ32_HW_H__

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "MemTools.h"

//  QSPI_P1:
//    PE10 => CLK
//    PE11 => NCS
//    PE12 => IO0
//    PE13 => IO1
//    PE14 => IO2
//    PE15 => IO3

class cZD25WQ32_HW
{
  public:
  OSPI_HandleTypeDef mstOSpi;

  /* OCTOSPI1 init function */
  void vInit(void)
  {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;


    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection   = RCC_OSPICLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PE10    ------> OCTOSPIM_P1_CLK
    PE11    ------> OCTOSPIM_P1_NCS
    PE12    ------> OCTOSPIM_P1_IO0
    PE13    ------> OCTOSPIM_P1_IO1
    PE14    ------> OCTOSPIM_P1_IO2
    PE15    ------> OCTOSPIM_P1_IO3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);


    OSPIM_CfgTypeDef sOspiManagerCfg;
    cMemTools::vMemSet((u8*)&sOspiManagerCfg, 0, sizeof(sOspiManagerCfg));

    // STM32U5 Sys  clock: 160Mhz
    //         OSPI clock:  80Mhz
    //
    // HAL_OSPI WinBond W25Q64JV-DTR 8MB = 2^23 = 8388608
    // Mircon Mode, weil Daten komment als u8 (D0 D1 D2 D3) und nicht als u16 (D1 D0 D3 D2) raus
    //
    //

    mstOSpi.Instance           = OCTOSPI1;
    mstOSpi.Init.DeviceSize    = 22;      // 4MB = 2^22 = 4194304
    mstOSpi.Init.FifoThreshold = 1;
    mstOSpi.Init.DualQuad   = HAL_OSPI_DUALQUAD_DISABLE;
    mstOSpi.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;

    //mstOSpi.Init.ClockPrescaler        = 2;    // 160Mhz / 2 = 80,00Mhz
    //mstOSpi.Init.ClockPrescaler        = 3;    // 160Mhz / 3 = 53,33Mhz
    //mstOSpi.Init.ClockPrescaler        = 4;    // 160Mhz / 4 = 40,00Mhz
    mstOSpi.Init.ClockPrescaler = 0;    // 32Mhz / 1 = 32Mhz
    mstOSpi.Init.ChipSelectHighTime = 1;        // t_SHSL2 = 50ns (DS S.89), 32Mhz / 1(Prescale) = 32Mhz    => 31.25ns; 31.25ns * 2 = 62.5ns

    mstOSpi.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
    mstOSpi.Init.ClockMode        = HAL_OSPI_CLOCK_MODE_0; // Mode 0 = Low Level; Mode 3 = High Level; W25Q64JV supports both (DS S.10)
    mstOSpi.Init.WrapSize         = HAL_OSPI_WRAP_NOT_SUPPORTED;
    mstOSpi.Init.SampleShifting   = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
    mstOSpi.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE; // HAL_OSPI_DHQC_DISABLE;   HAL_OSPI_DHQC_ENABLE is recommended when using DTR-Mode
    mstOSpi.Init.ChipSelectBoundary = 0;
    mstOSpi.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
    mstOSpi.Init.MaxTran          = 0;
    mstOSpi.Init.Refresh          = 0;

    HAL_OSPI_MspInit(&mstOSpi);
    HAL_OSPI_Init(&mstOSpi);

    sOspiManagerCfg.ClkPort = 1;
    sOspiManagerCfg.NCSPort = 1;
    sOspiManagerCfg.IOLowPort  = HAL_OSPIM_IOPORT_1_LOW;
    sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
    HAL_OSPIM_Config(&mstOSpi, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
  }
};



#endif // __OSPI_ZD25WQ32_HW_H__
