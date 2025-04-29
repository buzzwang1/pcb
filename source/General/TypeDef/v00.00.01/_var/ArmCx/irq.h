
#ifndef _IRQ_H
#define _IRQ_H



extern int miNestedIrqCnt;

#ifdef PCB_PROJECTCFG_Test
  #ifdef TESTIRQLOCK
    #include "stm32l433xx.h"
    //enable all interrupts
    #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm volatile ("cpsie i" : : : "memory"); GPIOA->BRR = (1 << 5);} // Test

    //disable all interrupts
    #define _dai() __asm volatile ("cpsid i" : : : "memory"); miNestedIrqCnt++; GPIOA->BSRR = (1 << 5);  // Test
  #else
    //enable all interrupts
    #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm volatile ("cpsie i" : : : "memory");}

    //disable all interrupts
    #define _dai() __asm volatile ("cpsid i" : : : "memory"); miNestedIrqCnt++;
  #endif
#else
  //enable all interrupts
  #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm volatile ("cpsie i" : : : "memory");}

  //disable all interrupts
  #define _dai() __asm volatile ("cpsid i" : : : "memory"); miNestedIrqCnt++;
#endif

#endif // _IRQ_H