#include "WufDef.h"

void WufDef_vCheckWuReason(tunWakeupSources* lunWuSources)
{
  lunWuSources->u32WakeupSources = 0;

  // Wakeup Timer RTC
  if (LL_RTC_IsActiveFlag_WUT(RTC))
  {
    /* Reset RTC Internal Wake up flag */
    LL_RTC_DisableWriteProtection(RTC);

    LL_RTC_ClearFlag_WUT(RTC);
    /* Disable wake up timer to modify it */
    LL_RTC_WAKEUP_Disable(RTC);
    LL_RTC_DisableIT_WUT(RTC);

    LL_RTC_EnableWriteProtection(RTC);

    lunWuSources->stWakeupSources.isWuTimerRtc = 1;
  }

  /* Check if the system was resumed from Standby mode */
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SBF))
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SBF);
    lunWuSources->stWakeupSources.isWuStandBy = 1;
  }

  if (__HAL_PWR_GET_FLAG(PWR_WAKEUP_FLAG1))
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_FLAG1);
    lunWuSources->stWakeupSources.isWuPin1 = 1;
  }

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
  {
    lunWuSources->stWakeupSources.isWuIWdg = 1;
  }


  // RCC_FLAG_RMVF     Remove reset flag
  // RCC_FLAG_OBLRST   Option Byte Loader reset flag
  // RCC_FLAG_PINRST   PIN reset flag
  // RCC_FLAG_BORRST   BOR reset flag
  // RCC_FLAG_SFTRST   Software Reset flag
  // RCC_FLAG_IWDGRST  Independent Watchdog reset flag
  // RCC_FLAG_WWDGRST  Window watchdog reset flag
  // RCC_FLAG_LPWRRST  Low-Power reset flag
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) { lunWuSources->stWakeupSources.isWuPinRst = 1; }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) { lunWuSources->stWakeupSources.isWuSftRst = 1; }

  __HAL_RCC_CLEAR_RESET_FLAGS();
}


