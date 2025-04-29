#include "Irq.h"

static uint8_t miNestedIrqCnt = 0;

//enable all interrupts
void _eai(void)
{
  if (miNestedIrqCnt) miNestedIrqCnt--;
}

//disable all interrupts
void _dai(void)
{
  miNestedIrqCnt++;
}
