#pragma once

#include "cSysDPool.h"

#include "board_api.h"
#include "tusb.h"

class cCompAddOnUsb : public cComponent
{
  public:

  cCompAddOnUsb()
    : cComponent(cDepTreeCfg::cComp::nUSB, { cDepTreeCfg::cComp::nAddOn })
  {
  }

  // Erzwingt, dass der PC das Gerät neu erkennt
  // USB Reset für STM32U575 PHY/Transceiver
  void usb_hardware_reset(void)
  {
    // Schaltet den USB PHY temporär aus und wieder ein
    HAL_PWREx_DisableVddUSB();
    vTaskDelay(pdMS_TO_TICKS(50)); // 50ms reichen aus
    HAL_PWREx_EnableVddUSB();
  }

  bool bInit() override
  {
    // USB-Taktquelle auf HSI48 (48 MHz) festlegen
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInit.Clk48ClockSelection = RCC_CLK48CLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      while (1);
    }

    LL_PWR_EnableVDDUSB();

    LL_RCC_HSI48_Enable();
    while (!LL_RCC_HSI48_IsReady())
    {
      vTaskDelay(pdMS_TO_TICKS(1));
    }

    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_HSI48);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_USBFS);

    RCC_CRSInitTypeDef RCC_CRSInitStruct;

    // Clock Recovery System (CRS) für HSI48 aktivieren (Synchronisation via USB-SOF)
    __HAL_RCC_CRS_CLK_ENABLE();
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
    RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;
    RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;
    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

    usb_hardware_reset();

    // TinyUSB Stack initialisieren
    board_init();
    tusb_init();

    cComponentList::mcList1ms.Add(this->mu8Idx);
    
    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);
    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    tud_task();
    // return True to signal finished
    return cComponent::bRun();
  };
};






