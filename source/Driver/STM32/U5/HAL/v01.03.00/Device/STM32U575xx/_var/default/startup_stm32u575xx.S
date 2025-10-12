  .syntax unified
  .cpu cortex-m33
  .thumb

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
  .long  __StackTop
  .long	Reset_Handler
  .long	NMI_Handler
  .long	HardFault_Handler
  .long	MemManage_Handler
  .long	BusFault_Handler
  .long	UsageFault_Handler
  .long	SecureFault_Handler
  .long	0
  .long	0
  .long	0
  .long	SVC_Handler
  .long	DebugMon_Handler
  .long	0
  .long	PendSV_Handler
  .long	SysTick_Handler
  .long	WWDG_IRQHandler
  .long	PVD_AVD_IRQHandler
  .long	RTC_IRQHandler
  .long	RTC_S_IRQHandler
  .long	TAMP_IRQHandler
  .long	RAMCFG_IRQHandler
  .long	FLASH_IRQHandler
  .long	FLASH_S_IRQHandler
  .long	GTZC_IRQHandler
  .long	RCC_IRQHandler
  .long	RCC_S_IRQHandler
  .long	EXTI0_IRQHandler
  .long	EXTI1_IRQHandler
  .long	EXTI2_IRQHandler
  .long	EXTI3_IRQHandler
  .long	EXTI4_IRQHandler
  .long	EXTI5_IRQHandler
  .long	EXTI6_IRQHandler
  .long	EXTI7_IRQHandler
  .long	EXTI8_IRQHandler
  .long	EXTI9_IRQHandler
  .long	EXTI10_IRQHandler
  .long	EXTI11_IRQHandler
  .long	EXTI12_IRQHandler
  .long	EXTI13_IRQHandler
  .long	EXTI14_IRQHandler
  .long	EXTI15_IRQHandler
  .long	IWDG_IRQHandler
  .long	0
  .long	GPDMA1_Channel0_IRQHandler
  .long	GPDMA1_Channel1_IRQHandler
  .long	GPDMA1_Channel2_IRQHandler
  .long	GPDMA1_Channel3_IRQHandler
  .long	GPDMA1_Channel4_IRQHandler
  .long	GPDMA1_Channel5_IRQHandler
  .long	GPDMA1_Channel6_IRQHandler
  .long	GPDMA1_Channel7_IRQHandler
  .long	ADC1_IRQHandler
  .long	DAC1_IRQHandler
  .long	FDCAN1_IT0_IRQHandler
  .long	FDCAN1_IT1_IRQHandler
  .long	TIM1_BRK_IRQHandler
  .long	TIM1_UP_IRQHandler
  .long	TIM1_TRG_COM_IRQHandler
  .long	TIM1_CC_IRQHandler
  .long	TIM2_IRQHandler
  .long	TIM3_IRQHandler
  .long	TIM4_IRQHandler
  .long	TIM5_IRQHandler
  .long	TIM6_IRQHandler
  .long	TIM7_IRQHandler
  .long	TIM8_BRK_IRQHandler
  .long	TIM8_UP_IRQHandler
  .long	TIM8_TRG_COM_IRQHandler
  .long	TIM8_CC_IRQHandler
  .long	I2C1_EV_IRQHandler
  .long	I2C1_ER_IRQHandler
  .long	I2C2_EV_IRQHandler
  .long	I2C2_ER_IRQHandler
  .long	SPI1_IRQHandler
  .long	SPI2_IRQHandler
  .long	USART1_IRQHandler
  .long	USART2_IRQHandler
  .long	USART3_IRQHandler
  .long	UART4_IRQHandler
  .long	UART5_IRQHandler
  .long	LPUART1_IRQHandler
  .long	LPTIM1_IRQHandler
  .long	LPTIM2_IRQHandler
  .long	TIM15_IRQHandler
  .long	TIM16_IRQHandler
  .long	TIM17_IRQHandler
  .long	COMP_IRQHandler
  .long	OTG_FS_IRQHandler
  .long	CRS_IRQHandler
  .long	FMC_IRQHandler
  .long	OCTOSPI1_IRQHandler
  .long	PWR_S3WU_IRQHandler
  .long	SDMMC1_IRQHandler
  .long	SDMMC2_IRQHandler
  .long	GPDMA1_Channel8_IRQHandler
  .long	GPDMA1_Channel9_IRQHandler
  .long	GPDMA1_Channel10_IRQHandler
  .long	GPDMA1_Channel11_IRQHandler
  .long	GPDMA1_Channel12_IRQHandler
  .long	GPDMA1_Channel13_IRQHandler
  .long	GPDMA1_Channel14_IRQHandler
  .long	GPDMA1_Channel15_IRQHandler
  .long	I2C3_EV_IRQHandler
  .long	I2C3_ER_IRQHandler
  .long	SAI1_IRQHandler
  .long	SAI2_IRQHandler
  .long	TSC_IRQHandler
  .long	0
  .long	RNG_IRQHandler
  .long	FPU_IRQHandler
  .long	HASH_IRQHandler
  .long	0
  .long	LPTIM3_IRQHandler
  .long	SPI3_IRQHandler
  .long	I2C4_ER_IRQHandler
  .long	I2C4_EV_IRQHandler
  .long	MDF1_FLT0_IRQHandler
  .long	MDF1_FLT1_IRQHandler
  .long	MDF1_FLT2_IRQHandler
  .long	MDF1_FLT3_IRQHandler
  .long	UCPD1_IRQHandler
  .long	ICACHE_IRQHandler
  .long	0
  .long	0
  .long	LPTIM4_IRQHandler
  .long	DCACHE1_IRQHandler
  .long	ADF1_IRQHandler
  .long	ADC4_IRQHandler
  .long	LPDMA1_Channel0_IRQHandler
  .long	LPDMA1_Channel1_IRQHandler
  .long	LPDMA1_Channel2_IRQHandler
  .long	LPDMA1_Channel3_IRQHandler
  .long	DMA2D_IRQHandler
  .long	DCMI_PSSI_IRQHandler
  .long	OCTOSPI2_IRQHandler
  .long	MDF1_FLT4_IRQHandler
  .long	MDF1_FLT5_IRQHandler
  .long	CORDIC_IRQHandler
  .long	FMAC_IRQHandler

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
  def_irq_handler  SecureFault_Handler
  def_irq_handler  SVC_Handler
  def_irq_handler  DebugMon_Handler
  def_irq_handler  PendSV_Handler
  def_irq_handler  SysTick_Handler
  def_irq_handler  WWDG_IRQHandler
  def_irq_handler  PVD_AVD_IRQHandler
  def_irq_handler  RTC_IRQHandler
  def_irq_handler  RTC_S_IRQHandler
  def_irq_handler  TAMP_IRQHandler
  def_irq_handler  RAMCFG_IRQHandler
  def_irq_handler  FLASH_IRQHandler
  def_irq_handler  FLASH_S_IRQHandler
  def_irq_handler  GTZC_IRQHandler
  def_irq_handler  RCC_IRQHandler
  def_irq_handler  RCC_S_IRQHandler
  def_irq_handler  EXTI0_IRQHandler
  def_irq_handler  EXTI1_IRQHandler
  def_irq_handler  EXTI2_IRQHandler
  def_irq_handler  EXTI3_IRQHandler
  def_irq_handler  EXTI4_IRQHandler
  def_irq_handler  EXTI5_IRQHandler
  def_irq_handler  EXTI6_IRQHandler
  def_irq_handler  EXTI7_IRQHandler
  def_irq_handler  EXTI8_IRQHandler
  def_irq_handler  EXTI9_IRQHandler
  def_irq_handler  EXTI10_IRQHandler
  def_irq_handler  EXTI11_IRQHandler
  def_irq_handler  EXTI12_IRQHandler
  def_irq_handler  EXTI13_IRQHandler
  def_irq_handler  EXTI14_IRQHandler
  def_irq_handler  EXTI15_IRQHandler
  def_irq_handler  IWDG_IRQHandler
  def_irq_handler  GPDMA1_Channel0_IRQHandler
  def_irq_handler  GPDMA1_Channel1_IRQHandler
  def_irq_handler  GPDMA1_Channel2_IRQHandler
  def_irq_handler  GPDMA1_Channel3_IRQHandler
  def_irq_handler  GPDMA1_Channel4_IRQHandler
  def_irq_handler  GPDMA1_Channel5_IRQHandler
  def_irq_handler  GPDMA1_Channel6_IRQHandler
  def_irq_handler  GPDMA1_Channel7_IRQHandler
  def_irq_handler  ADC1_IRQHandler
  def_irq_handler  DAC1_IRQHandler
  def_irq_handler  FDCAN1_IT0_IRQHandler
  def_irq_handler  FDCAN1_IT1_IRQHandler
  def_irq_handler  TIM1_BRK_IRQHandler
  def_irq_handler  TIM1_UP_IRQHandler
  def_irq_handler  TIM1_TRG_COM_IRQHandler
  def_irq_handler  TIM1_CC_IRQHandler
  def_irq_handler  TIM2_IRQHandler
  def_irq_handler  TIM3_IRQHandler
  def_irq_handler  TIM4_IRQHandler
  def_irq_handler  TIM5_IRQHandler
  def_irq_handler  TIM6_IRQHandler
  def_irq_handler  TIM7_IRQHandler
  def_irq_handler  TIM8_BRK_IRQHandler
  def_irq_handler  TIM8_UP_IRQHandler
  def_irq_handler  TIM8_TRG_COM_IRQHandler
  def_irq_handler  TIM8_CC_IRQHandler
  def_irq_handler  I2C1_EV_IRQHandler
  def_irq_handler  I2C1_ER_IRQHandler
  def_irq_handler  I2C2_EV_IRQHandler
  def_irq_handler  I2C2_ER_IRQHandler
  def_irq_handler  SPI1_IRQHandler
  def_irq_handler  SPI2_IRQHandler
  def_irq_handler  USART1_IRQHandler
  def_irq_handler  USART2_IRQHandler
  def_irq_handler  USART3_IRQHandler
  def_irq_handler  UART4_IRQHandler
  def_irq_handler  UART5_IRQHandler
  def_irq_handler  LPUART1_IRQHandler
  def_irq_handler  LPTIM1_IRQHandler
  def_irq_handler  LPTIM2_IRQHandler
  def_irq_handler  TIM15_IRQHandler
  def_irq_handler  TIM16_IRQHandler
  def_irq_handler  TIM17_IRQHandler
  def_irq_handler  COMP_IRQHandler
  def_irq_handler  OTG_FS_IRQHandler
  def_irq_handler  CRS_IRQHandler
  def_irq_handler  FMC_IRQHandler
  def_irq_handler  OCTOSPI1_IRQHandler
  def_irq_handler  PWR_S3WU_IRQHandler
  def_irq_handler  SDMMC1_IRQHandler
  def_irq_handler  SDMMC2_IRQHandler
  def_irq_handler  GPDMA1_Channel8_IRQHandler
  def_irq_handler  GPDMA1_Channel9_IRQHandler
  def_irq_handler  GPDMA1_Channel10_IRQHandler
  def_irq_handler  GPDMA1_Channel11_IRQHandler
  def_irq_handler  GPDMA1_Channel12_IRQHandler
  def_irq_handler  GPDMA1_Channel13_IRQHandler
  def_irq_handler  GPDMA1_Channel14_IRQHandler
  def_irq_handler  GPDMA1_Channel15_IRQHandler
  def_irq_handler  I2C3_EV_IRQHandler
  def_irq_handler  I2C3_ER_IRQHandler
  def_irq_handler  SAI1_IRQHandler
  def_irq_handler  SAI2_IRQHandler
  def_irq_handler  TSC_IRQHandler
  def_irq_handler  RNG_IRQHandler
  def_irq_handler  FPU_IRQHandler
  def_irq_handler  HASH_IRQHandler
  def_irq_handler  LPTIM3_IRQHandler
  def_irq_handler  SPI3_IRQHandler
  def_irq_handler  I2C4_ER_IRQHandler
  def_irq_handler  I2C4_EV_IRQHandler
  def_irq_handler  MDF1_FLT0_IRQHandler
  def_irq_handler  MDF1_FLT1_IRQHandler
  def_irq_handler  MDF1_FLT2_IRQHandler
  def_irq_handler  MDF1_FLT3_IRQHandler
  def_irq_handler  UCPD1_IRQHandler
  def_irq_handler  ICACHE_IRQHandler
  def_irq_handler  LPTIM4_IRQHandler
  def_irq_handler  DCACHE1_IRQHandler
  def_irq_handler  ADF1_IRQHandler
  def_irq_handler  ADC4_IRQHandler
  def_irq_handler  LPDMA1_Channel0_IRQHandler
  def_irq_handler  LPDMA1_Channel1_IRQHandler
  def_irq_handler  LPDMA1_Channel2_IRQHandler
  def_irq_handler  LPDMA1_Channel3_IRQHandler
  def_irq_handler  DMA2D_IRQHandler
  def_irq_handler  DCMI_PSSI_IRQHandler
  def_irq_handler  OCTOSPI2_IRQHandler
  def_irq_handler  MDF1_FLT4_IRQHandler
  def_irq_handler  MDF1_FLT5_IRQHandler
  def_irq_handler  CORDIC_IRQHandler
  def_irq_handler  FMAC_IRQHandler

.end