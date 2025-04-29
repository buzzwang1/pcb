
#ifndef _IRQ_H
#define _IRQ_H



extern int miNestedIrqCnt;

// https://groups.google.com/a/groups.riscv.org/g/sw-dev/c/TLKm-64CB-c?pli=1
// __asm__ volatile ("csrw 0x800, %0" : : "r" (0x6000) );  // Disable interrupt (mie = 0)
// mtime_value = *MTIME_PTR;
// __asm__ volatile ("csrw 0x800, %0" : : "r" (0x6088) );  // Enable interrupt (mie = 1)

#ifdef PCB_PROJECTCFG_Test
  #ifdef TESTIRQLOCK
    #include "stm32l433xx.h"
    //enable all interrupts
    #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm__ volatile ("csrw 0x800, %0" : : "r" (0x6088) ); GPIOB->BSRR = (1 << (16+1));} // Test

    //disable all interrupts
    #define _dai() __asm__ volatile ("csrw 0x800, %0" : : "r" (0x6000) ); miNestedIrqCnt++; GPIOB->BSRR = (1 << 1);  // Test
  #else
    //enable all interrupts
    #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm__ volatile ("csrw 0x800, %0" : : "r" (0x6088) );}

    //disable all interrupts
    #define _dai() __asm__ volatile ("csrw 0x800, %0" : : "r" (0x6000) ); miNestedIrqCnt++;
  #endif
#else
  //enable all interrupts
  #define _eai() if (miNestedIrqCnt) miNestedIrqCnt--;if (!miNestedIrqCnt) {__asm__ volatile ("csrw 0x800, %0" : : "r" (0x6088) );}

  //disable all interrupts
  #define _dai() __asm__ volatile ("csrw 0x800, %0" : : "r" (0x6000) ); miNestedIrqCnt++;
#endif

#endif // _IRQ_H