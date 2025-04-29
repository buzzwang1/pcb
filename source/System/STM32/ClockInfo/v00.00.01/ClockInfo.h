
#ifndef __ClockInfo_H__
#define __ClockInfo_H__

#include "TypeDef.h"
#include "ClockInfo1.h"


class cClockInfo
{
  public:

  static RCC_ClocksTypeDef mstClocks;

  cClockInfo()
  {
    Update();
  };

  static void Update()
  {
    RCC_GetClocksFreq(&mstClocks);
  };


  // https://developer.arm.com/documentation/dui0489/i/arm-and-thumb-instructions/nop
  // Usage
  //   NOP does nothing.If NOP is not implemented as a specific instruction on your target architecture, the assembler treats it as a pseudo - instruction and generates an alternative instruction that does nothing, such as MOV r0, r0(ARM) or MOV r8, r8(Thumb).
  //   NOP is not necessarily a time - consuming NOP.The processor might remove it from the pipeline before it reaches the execution stage.
  //   You can use NOP for padding, for example to place the following instruction on a 64 - bit boundary in ARM, or a 32 - bit boundary in Thumb.
  //
  // Gemessen auf einen L4 dauert die Routine wesentlich länger als wie erwartet
  // z.B. 500us soll war 800us real
  //      10ms  soll war 18ms  real
  static void Delay_us(volatile uint32 lui32Delay) // __attribute__((optimize("Os")))
  {
    lui32Delay *= (u32GetClockCore() / 1000000);
    lui32Delay /= 8;  // ca. 3 für while + 5 Nops

    //DWT->CTRL |= (1UL << DWT_CTRL_CYCCNTENA_Pos);
    //DWT->CYCCNT = 0;
    //
    // Note: 20.11.2024: Bei L4 und -os braucht die Schleife ohne Nop genau 8 Takte
    //                   Aber weil es noch von Interrupts unterbrochen werden kann
    //                   kann die übergebene Zeit nicht eingehalten werden.

    while (lui32Delay > 0)
    {
      // asm volatile("MOV R8, R8"); // asm volatile ("NOP");
      // asm volatile("MOV R8, R8"); // asm volatile ("NOP");
      // asm volatile("MOV R8, R8"); // asm volatile ("NOP");
      // asm volatile("MOV R8, R8"); // asm volatile ("NOP");
      // asm volatile("MOV R8, R8"); // asm volatile ("NOP");
      lui32Delay--;
    }
    // asm volatile ("NOP");
  }

  static u32 u32GetClockCore()  {return ClockInfo_Clock_Core;}
  static u32 u32GetClockSys()   {return ClockInfo_Clock_Sys;}
  static u32 u32GetClockPort()  {return ClockInfo_Clock_Port;}

  static void Delay_ms(volatile uint32 lui32Delay)
  {
    while (lui32Delay > 0)
    {
      Delay_us(1000);
      lui32Delay--;
    }
  }
};


#endif // __ClockInfo_H__
