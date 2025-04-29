
#include "irq.h"

uint8 mui8IrqLockLevel = 0;

inline void IRQ_vDi(void)
{
  #ifndef _WIN32
  __disable_irq();
  mui8IrqLockLevel++;
  #endif
}


inline void IRQ_vEn(void)
{
  #ifndef _WIN32
  if (mui8IrqLockLevel) mui8IrqLockLevel--;
  if (!mui8IrqLockLevel) __enable_irq();
  #endif
}
