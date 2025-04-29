/* Cortex-M4 stm32L43x.S
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
  .long  WWDG_IRQHandler
  .long  PVD_PVM_IRQHandler
  .long  TAMP_STAMP_IRQHandler
  .long  RTC_WKUP_IRQHandler
  .long  FLASH_IRQHandler
  .long  RCC_IRQHandler
  .long  EXTI0_IRQHandler
  .long  EXTI1_IRQHandler
  .long  EXTI2_IRQHandler
  .long  EXTI3_IRQHandler
  .long  EXTI4_IRQHandler
  .long  DMA1_Channel1_IRQHandler
  .long  DMA1_Channel2_IRQHandler
  .long  DMA1_Channel3_IRQHandler
  .long  DMA1_Channel4_IRQHandler
  .long  DMA1_Channel5_IRQHandler
  .long  DMA1_Channel6_IRQHandler
  .long  DMA1_Channel7_IRQHandler
  .long  ADC1_IRQHandler
  .long  CAN1_TX_IRQHandler
  .long  CAN1_RX0_IRQHandler
  .long  CAN1_RX1_IRQHandler
  .long  CAN1_SCE_IRQHandler
  .long  EXTI9_5_IRQHandler
  .long  TIM1_BRK_TIM15_IRQHandler
  .long  TIM1_UP_TIM16_IRQHandler
  .long  TIM1_TRG_COM_IRQHandler
  .long  TIM1_CC_IRQHandler
  .long  TIM2_IRQHandler
  .long  0
  .long  0
  .long  I2C1_EV_IRQHandler
  .long  I2C1_ER_IRQHandler
  .long  I2C2_EV_IRQHandler
  .long  I2C2_ER_IRQHandler
  .long  SPI1_IRQHandler
  .long  SPI2_IRQHandler
  .long  USART1_IRQHandler
  .long  USART2_IRQHandler
  .long  USART3_IRQHandler
  .long  EXTI15_10_IRQHandler
  .long  RTC_Alarm_IRQHandler
  .long  0
  .long  0
  .long  0
  .long  0
  .long  0
  .long  0
  .long  0
  .long  SDMMC1_IRQHandler
  .long  0
  .long  SPI3_IRQHandler
  .long  0
  .long  0
  .long  TIM6_DAC_IRQHandler
  .long  TIM7_IRQHandler
  .long  DMA2_Channel1_IRQHandler
  .long  DMA2_Channel2_IRQHandler
  .long  DMA2_Channel3_IRQHandler
  .long  DMA2_Channel4_IRQHandler
  .long  DMA2_Channel5_IRQHandler
  .long  0
  .long  0
  .long  0
  .long  COMP_IRQHandler
  .long  LPTIM1_IRQHandler
  .long  LPTIM2_IRQHandler
  .long  USB_IRQHandler
  .long  DMA2_Channel6_IRQHandler
  .long  DMA2_Channel7_IRQHandler
  .long  LPUART1_IRQHandler
  .long  QUADSPI_IRQHandler
  .long  I2C3_EV_IRQHandler
  .long  I2C3_ER_IRQHandler
  .long  SAI1_IRQHandler
  .long  0
  .long  SWPMI1_IRQHandler
  .long  TSC_IRQHandler
  .long  LCD_IRQHandler
  .long  0
  .long  RNG_IRQHandler
  .long  FPU_IRQHandler
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
  def_irq_handler  WWDG_IRQHandler
  def_irq_handler  PVD_PVM_IRQHandler
  def_irq_handler  TAMP_STAMP_IRQHandler
  def_irq_handler  RTC_WKUP_IRQHandler
  def_irq_handler  FLASH_IRQHandler
  def_irq_handler  RCC_IRQHandler
  def_irq_handler  EXTI0_IRQHandler
  def_irq_handler  EXTI1_IRQHandler
  def_irq_handler  EXTI2_IRQHandler
  def_irq_handler  EXTI3_IRQHandler
  def_irq_handler  EXTI4_IRQHandler
  def_irq_handler  DMA1_Channel1_IRQHandler
  def_irq_handler  DMA1_Channel2_IRQHandler
  def_irq_handler  DMA1_Channel3_IRQHandler
  def_irq_handler  DMA1_Channel4_IRQHandler
  def_irq_handler  DMA1_Channel5_IRQHandler
  def_irq_handler  DMA1_Channel6_IRQHandler
  def_irq_handler  DMA1_Channel7_IRQHandler
  def_irq_handler  ADC1_IRQHandler
  def_irq_handler  CAN1_TX_IRQHandler
  def_irq_handler  CAN1_RX0_IRQHandler
  def_irq_handler  CAN1_RX1_IRQHandler
  def_irq_handler  CAN1_SCE_IRQHandler
  def_irq_handler  EXTI9_5_IRQHandler
  def_irq_handler  TIM1_BRK_TIM15_IRQHandler
  def_irq_handler  TIM1_UP_TIM16_IRQHandler
  def_irq_handler  TIM1_TRG_COM_IRQHandler
  def_irq_handler  TIM1_CC_IRQHandler
  def_irq_handler  TIM2_IRQHandler
  def_irq_handler  I2C1_EV_IRQHandler
  def_irq_handler  I2C1_ER_IRQHandler
  def_irq_handler  I2C2_EV_IRQHandler
  def_irq_handler  I2C2_ER_IRQHandler
  def_irq_handler  SPI1_IRQHandler
  def_irq_handler  SPI2_IRQHandler
  def_irq_handler  USART1_IRQHandler
  def_irq_handler  USART2_IRQHandler
  def_irq_handler  USART3_IRQHandler
  def_irq_handler  EXTI15_10_IRQHandler
  def_irq_handler  RTC_Alarm_IRQHandler
  def_irq_handler  SDMMC1_IRQHandler
  def_irq_handler  SPI3_IRQHandler
  def_irq_handler  TIM6_DAC_IRQHandler
  def_irq_handler  TIM7_IRQHandler
  def_irq_handler  DMA2_Channel1_IRQHandler
  def_irq_handler  DMA2_Channel2_IRQHandler
  def_irq_handler  DMA2_Channel3_IRQHandler
  def_irq_handler  DMA2_Channel4_IRQHandler
  def_irq_handler  DMA2_Channel5_IRQHandler
  def_irq_handler  COMP_IRQHandler
  def_irq_handler  LPTIM1_IRQHandler
  def_irq_handler  LPTIM2_IRQHandler
  def_irq_handler  USB_IRQHandler
  def_irq_handler  DMA2_Channel6_IRQHandler
  def_irq_handler  DMA2_Channel7_IRQHandler
  def_irq_handler  LPUART1_IRQHandler
  def_irq_handler  QUADSPI_IRQHandler
  def_irq_handler  I2C3_EV_IRQHandler
  def_irq_handler  I2C3_ER_IRQHandler
  def_irq_handler  SAI1_IRQHandler
  def_irq_handler  SWPMI1_IRQHandler
  def_irq_handler  TSC_IRQHandler
  def_irq_handler  LCD_IRQHandler
  def_irq_handler  RNG_IRQHandler
  def_irq_handler  FPU_IRQHandler
  def_irq_handler  CRS_IRQHandler

.end
