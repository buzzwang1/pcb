/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V1.3
 * Date: 08 Feb 2012
 *
 * Copyright (c) 2012, ARM Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the ARM Limited nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARM LIMITED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES//
 * LOSS OF USE, DATA, OR PROFITS// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
    .syntax unified
    .arch armv7-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x400
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0xC00
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    0           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    0     /* MPU Fault Handler */
    .long    0      /* Bus Fault Handler */
    .long    0    /* Usage Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0           /* SVCall Handler */
    .long    0      /* Debug Monitor Handler */
    .long    0                     /* Reserved */
    .long    0        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

             
    .long    0
    .long    0
    .long    0
    .long    0
    .long    0
    .long    0
    .long    0
    .long    0
    .long    0
    .long    DMA_Channel0_IRQHandler
    .long    DMA_Channel1_2_IRQHandler
    .long    DMA_Channel3_4_IRQHandler
    .long    ADC_CMP_IRQHandler
    .long    TIMER0_BRK_UP_TRG_COM_IRQHandler
    .long    TIMER0_Channel_IRQHandler
    .long    TIMER1_IRQHandler
    .long    TIMER2_IRQHandler
    .long    TIMER5_DAC_IRQHandler
    .long    0
    .long    TIMER13_IRQHandler
    .long    TIMER14_IRQHandler
    .long    TIMER15_IRQHandler
    .long    TIMER16_IRQHandler
    .long    I2C0_EV_IRQHandler
    .long    0
    .long    0
    .long    0
    .long    USART0_IRQHandler
    .long    0
    .long    0
    .long    0
    .long    0
    .long    I2C0_ER_IRQHandler

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.flash_to_ram_loop:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .flash_to_ram_loop
#else
    subs    r3, r2
    ble    .flash_to_ram_loop_end
.flash_to_ram_loop:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .flash_to_ram_loop
.flash_to_ram_loop_end:
#endif

#ifndef __NO_SYSTEM_INIT
    ldr    r0, =MainSystemInit
    blx    r0
#endif

    ldr    r0, =_start
    bx    r0
    .pool
    .size Reset_Handler, . - Reset_Handler

/* Our weak _start alternative if we don't use the library _start
 * The zero init section must be cleared, otherwise the librtary is
 * doing that */
    .align 1
    .thumb_func
    .weak _start
    .type _start, %function
_start:

    /* Zero fill the bss segment. */
    ldr   r1, = __bss_start__
    ldr   r2, = __bss_end__
    movs  r3, #0
    b  .fill_zero_bss
.loop_zero_bss:
    str  r3, [r1], #4

.fill_zero_bss:
    cmp  r1, r2
    bcc  .loop_zero_bss

    /* Jump to our main */
    bl main
    b .
    .size    _start, . - _start

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_irq_handler    HardFault_Handler
    def_irq_handler    SysTick_Handler

    def_irq_handler    DMA_Channel0_IRQHandler
    def_irq_handler    DMA_Channel1_2_IRQHandler
    def_irq_handler    DMA_Channel3_4_IRQHandler
    def_irq_handler    ADC_CMP_IRQHandler
    def_irq_handler    TIMER0_BRK_UP_TRG_COM_IRQHandler
    def_irq_handler    TIMER0_Channel_IRQHandler
    def_irq_handler    TIMER1_IRQHandler
    def_irq_handler    TIMER2_IRQHandler
    def_irq_handler    TIMER5_DAC_IRQHandler
    def_irq_handler    TIMER13_IRQHandler
    def_irq_handler    TIMER14_IRQHandler
    def_irq_handler    TIMER15_IRQHandler
    def_irq_handler    TIMER16_IRQHandler
    def_irq_handler    I2C0_EV_IRQHandler
    def_irq_handler    USART0_IRQHandler
    def_irq_handler    I2C0_ER_IRQHandler

    .end
