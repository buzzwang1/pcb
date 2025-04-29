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
	.long	__StackTop
  .long  Reset_Handler
  .long  NMI_Handler
  .long  HardFault_Handler
  .long  MemManage_Handler
  .long  BusFault_Handler
  .long  UsageFault_Handler
  .long  0
  .long  0
  .long  0
  .long  0
  .long  SVC_Handler
  .long  DebugMon_Handler
  .long  0
  .long  PendSV_Handler
  .long  SysTick_Handler

  // External Interrupts
    .long   POWER_CLOCK_IRQHandler
    .long   RADIO_IRQHandler
    .long   UARTE0_UART0_IRQHandler
    .long   SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
    .long   SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
    .long   NFCT_IRQHandler
    .long   GPIOTE_IRQHandler
    .long   SAADC_IRQHandler
    .long   TIMER0_IRQHandler
    .long   TIMER1_IRQHandler
    .long   TIMER2_IRQHandler
    .long   RTC0_IRQHandler
    .long   TEMP_IRQHandler
    .long   RNG_IRQHandler
    .long   ECB_IRQHandler
    .long   CCM_AAR_IRQHandler
    .long   WDT_IRQHandler
    .long   RTC1_IRQHandler
    .long   QDEC_IRQHandler
    .long   COMP_LPCOMP_IRQHandler
    .long   SWI0_EGU0_IRQHandler
    .long   SWI1_EGU1_IRQHandler
    .long   SWI2_EGU2_IRQHandler
    .long   SWI3_EGU3_IRQHandler
    .long   SWI4_EGU4_IRQHandler
    .long   SWI5_EGU5_IRQHandler
    .long   TIMER3_IRQHandler
    .long   TIMER4_IRQHandler
    .long   PWM0_IRQHandler
    .long   PDM_IRQHandler
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   MWU_IRQHandler
    .long   PWM1_IRQHandler
    .long   PWM2_IRQHandler
    .long   SPIM2_SPIS2_SPI2_IRQHandler
    .long   RTC2_IRQHandler
    .long   I2S_IRQHandler
    .long   FPU_IRQHandler
    .long   USBD_IRQHandler
    .long   UARTE1_IRQHandler
    .long   QSPI_IRQHandler
    .long   CRYPTOCELL_IRQHandler
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   PWM3_IRQHandler
    .long   0                           /*Reserved */
    .long   SPIM3_IRQHandler
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */
    .long   0                           /*Reserved */

    .size    __isr_vector, . - __isr_vector

/* Reset Handler */

    .text
    .thumb
    .thumb_func
    .align 2
    .globl   Reset_Handler
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

    /* Call CTORS of static objects */
    bl __libc_init_array
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

  def_irq_handler  NMI_Handler
  def_irq_handler  HardFault_Handler
  def_irq_handler  MemManage_Handler
  def_irq_handler  BusFault_Handler
  def_irq_handler  UsageFault_Handler
  def_irq_handler  SVC_Handler
  def_irq_handler  DebugMon_Handler
  def_irq_handler  PendSV_Handler
  def_irq_handler  SysTick_Handler

  def_irq_handler  POWER_CLOCK_IRQHandler
  def_irq_handler  RADIO_IRQHandler
  def_irq_handler  UARTE0_UART0_IRQHandler
  def_irq_handler  SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
  def_irq_handler  SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
  def_irq_handler  NFCT_IRQHandler
  def_irq_handler  GPIOTE_IRQHandler
  def_irq_handler  SAADC_IRQHandler
  def_irq_handler  TIMER0_IRQHandler
  def_irq_handler  TIMER1_IRQHandler
  def_irq_handler  TIMER2_IRQHandler
  def_irq_handler  RTC0_IRQHandler
  def_irq_handler  TEMP_IRQHandler
  def_irq_handler  RNG_IRQHandler
  def_irq_handler  ECB_IRQHandler
  def_irq_handler  CCM_AAR_IRQHandler
  def_irq_handler  WDT_IRQHandler
  def_irq_handler  RTC1_IRQHandler
  def_irq_handler  QDEC_IRQHandler
  def_irq_handler  COMP_LPCOMP_IRQHandler
  def_irq_handler  SWI0_EGU0_IRQHandler
  def_irq_handler  SWI1_EGU1_IRQHandler
  def_irq_handler  SWI2_EGU2_IRQHandler
  def_irq_handler  SWI3_EGU3_IRQHandler
  def_irq_handler  SWI4_EGU4_IRQHandler
  def_irq_handler  SWI5_EGU5_IRQHandler
  def_irq_handler  TIMER3_IRQHandler
  def_irq_handler  TIMER4_IRQHandler
  def_irq_handler  PWM0_IRQHandler
  def_irq_handler  PDM_IRQHandler
  def_irq_handler  MWU_IRQHandler
  def_irq_handler  PWM1_IRQHandler
  def_irq_handler  PWM2_IRQHandler
  def_irq_handler  SPIM2_SPIS2_SPI2_IRQHandler
  def_irq_handler  RTC2_IRQHandler
  def_irq_handler  I2S_IRQHandler
  def_irq_handler  FPU_IRQHandler
  def_irq_handler  USBD_IRQHandler
  def_irq_handler  UARTE1_IRQHandler
  def_irq_handler  QSPI_IRQHandler
  def_irq_handler  CRYPTOCELL_IRQHandler
  def_irq_handler  PWM3_IRQHandler
  def_irq_handler  SPIM3_IRQHandler

.end

