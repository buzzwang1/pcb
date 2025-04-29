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
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB))
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    lunWuSources->stWakeupSources.isWuStandBy = 1;
  }

  if (__HAL_PWR_GET_FLAG(PWR_FLAG_WUF1))
  {
    /* Clear Standby flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF1);
    lunWuSources->stWakeupSources.isWuPin1 = 1;
  }

  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
  {
    lunWuSources->stWakeupSources.isWuIWdg = 1;
  }

  // RCC_FLAG_LSERDY   LSE oscillator clock ready
  // RCC_FLAG_LSECSSD  Clock security system failure on LSE oscillator detection
  // RCC_FLAG_LSIRDY   LSI oscillator clock ready
  // RCC_FLAG_BORRST   BOR reset
  // RCC_FLAG_OBLRST   OBLRST reset
  // RCC_FLAG_PINRST   Pin reset
  // RCC_FLAG_FWRST    FIREWALL reset
  // RCC_FLAG_SFTRST   Software reset
  // RCC_FLAG_IWDGRST  Independent Watchdog reset
  // RCC_FLAG_WWDGRST  Window Watchdog reset
  // RCC_FLAG_LPWRRST  Low Power reset
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) { lunWuSources->stWakeupSources.isWuPinRst = 1; }
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) { lunWuSources->stWakeupSources.isWuSftRst = 1; }

  __HAL_RCC_CLEAR_RESET_FLAGS();
}


