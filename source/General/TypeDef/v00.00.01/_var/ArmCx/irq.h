
#ifndef _IRQ_H
#define _IRQ_H

//#include "stm32l433xx.h"

extern int miNestedIrqCnt;

//enable all interrupts
#define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm volatile ("cpsie i" : : : "memory");} //  GPIOB->BRR = (1 << 3);  // Test

//disable all interrupts
#define _dai() __asm volatile ("cpsid i" : : : "memory"); miNestedIrqCnt++; //  GPIOB->BSRR = (1 << 3);  // Test

#endif // _IRQ_H