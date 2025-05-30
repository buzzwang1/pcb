/* Cortex-M4 stm32f413xx.s
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

  /* External Interrupts */
  .long  WWDG_IRQHandler                   /* Window WatchDog                             */
  .long  PVD_IRQHandler                    /* PVD through EXTI Line detection             */
  .long  TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
  .long  RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line            */
  .long  FLASH_IRQHandler                  /* FLASH                                       */
  .long  RCC_IRQHandler                    /* RCC                                         */
  .long  EXTI0_IRQHandler                  /* EXTI Line0                                  */
  .long  EXTI1_IRQHandler                  /* EXTI Line1                                  */
  .long  EXTI2_IRQHandler                  /* EXTI Line2                                  */
  .long  EXTI3_IRQHandler                  /* EXTI Line3                                  */
  .long  EXTI4_IRQHandler                  /* EXTI Line4                                  */
  .long  DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                               */
  .long  DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                               */
  .long  DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                               */
  .long  DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                               */
  .long  DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                               */
  .long  DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                               */
  .long  DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                               */
  .long  ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s                        */
  .long  CAN1_TX_IRQHandler                /* CAN1 TX                                     */
  .long  CAN1_RX0_IRQHandler               /* CAN1 RX0                                    */
  .long  CAN1_RX1_IRQHandler               /* CAN1 RX1                                    */
  .long  CAN1_SCE_IRQHandler               /* CAN1 SCE                                    */
  .long  EXTI9_5_IRQHandler                /* External Line[9:5]s                         */
  .long  TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9                         */
  .long  TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10                       */
  .long  TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11      */
  .long  TIM1_CC_IRQHandler                /* TIM1 Capture Compare                        */
  .long  TIM2_IRQHandler                   /* TIM2                                        */
  .long  TIM3_IRQHandler                   /* TIM3                                        */
  .long  TIM4_IRQHandler                   /* TIM4                                        */
  .long  I2C1_EV_IRQHandler                /* I2C1 Event                                  */
  .long  I2C1_ER_IRQHandler                /* I2C1 Error                                  */
  .long  I2C2_EV_IRQHandler                /* I2C2 Event                                  */
  .long  I2C2_ER_IRQHandler                /* I2C2 Error                                  */
  .long  SPI1_IRQHandler                   /* SPI1                                        */
  .long  SPI2_IRQHandler                   /* SPI2                                        */
  .long  USART1_IRQHandler                 /* USART1                                      */
  .long  USART2_IRQHandler                 /* USART2                                      */
  .long  USART3_IRQHandler                 /* USART3                                      */
  .long  EXTI15_10_IRQHandler              /* External Line[15:10]s                       */
  .long  RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line       */
  .long  OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line         */
  .long  TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12                        */
  .long  TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13                       */
  .long  TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14      */
  .long  TIM8_CC_IRQHandler                /* TIM8 Capture Compare                        */
  .long  DMA1_Stream7_IRQHandler           /* DMA1 Stream7                                */
  .long  FSMC_IRQHandler                   /* FSMC                                        */
  .long  SDIO_IRQHandler                   /* SDIO                                        */
  .long  TIM5_IRQHandler                   /* TIM5                                        */
  .long  SPI3_IRQHandler                   /* SPI3                                        */
  .long  UART4_IRQHandler                  /* UART4                                       */
  .long  UART5_IRQHandler                  /* UART5                                       */
  .long  TIM6_DAC_IRQHandler               /* TIM6, DAC1 and DAC2                         */
  .long  TIM7_IRQHandler                   /* TIM7                                        */
  .long  DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                               */
  .long  DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                               */
  .long  DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                               */
  .long  DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                               */
  .long  DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                               */
  .long  DFSDM1_FLT0_IRQHandler            /* DFSDM1 Filter0                              */
  .long  DFSDM1_FLT1_IRQHandler            /* DFSDM1 Filter1                              */
  .long  CAN2_TX_IRQHandler                /* CAN2 TX                                     */
  .long  CAN2_RX0_IRQHandler               /* CAN2 RX0                                    */
  .long  CAN2_RX1_IRQHandler               /* CAN2 RX1                                    */
  .long  CAN2_SCE_IRQHandler               /* CAN2 SCE                                    */
  .long  OTG_FS_IRQHandler                 /* USB OTG FS                                  */
  .long  DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                               */
  .long  DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                               */
  .long  DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                               */
  .long  USART6_IRQHandler                 /* USART6                                      */
  .long  I2C3_EV_IRQHandler                /* I2C3 event                                  */
  .long  I2C3_ER_IRQHandler                /* I2C3 error                                  */
  .long  CAN3_TX_IRQHandler                /* CAN3 TX                                     */
  .long  CAN3_RX0_IRQHandler               /* CAN3 RX0                                    */
  .long  CAN3_RX1_IRQHandler               /* CAN3 RX1                                    */
  .long  CAN3_SCE_IRQHandler               /* CAN3 SCE                                    */
  .long  0                                 /* Reserved                                    */
  .long  0                                 /* Reserved                                    */
  .long  RNG_IRQHandler                    /* RNG                                         */
  .long  FPU_IRQHandler                    /* FPU                                         */
  .long  UART7_IRQHandler                  /* UART7                                       */
  .long  UART8_IRQHandler                  /* UART8                                       */
  .long  SPI4_IRQHandler                   /* SPI4                                        */
  .long  SPI5_IRQHandler                   /* SPI5                                        */
  .long  0                                 /* Reserved                                    */
  .long  SAI1_IRQHandler                   /* SAI1                                        */
  .long  UART9_IRQHandler                  /* UART9                                       */
  .long  UART10_IRQHandler                 /* UART10                                      */
  .long  0                                 /* Reserved                                    */
  .long  0                                 /* Reserved                                    */
  .long  QUADSPI_IRQHandler                /* QuadSPI                                     */
  .long  0                                 /* Reserved                                    */
  .long  0                                 /* Reserved                                    */
  .long  FMPI2C1_EV_IRQHandler             /* FMPI2C1 Event                               */
  .long  FMPI2C1_ER_IRQHandler             /* FMPI2C1 Error                               */
  .long  LPTIM1_IRQHandler                 /* LPTIM1                                      */
  .long  DFSDM2_FLT0_IRQHandler            /* DFSDM2 Filter0                              */
  .long  DFSDM2_FLT1_IRQHandler            /* DFSDM2 Filter1                              */
  .long  DFSDM2_FLT2_IRQHandler            /* DFSDM2 Filter2                              */
  .long  DFSDM2_FLT3_IRQHandler            /* DFSDM2 Filter3                              */
  
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
  def_irq_handler  Default_Handler

  /* External Interrupts */
  def_irq_handler  WWDG_IRQHandler                   /* Window WatchDog                             */
  def_irq_handler  PVD_IRQHandler                    /* PVD through EXTI Line detection             */
  def_irq_handler  TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
  def_irq_handler  RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line            */
  def_irq_handler  FLASH_IRQHandler                  /* FLASH                                       */
  def_irq_handler  RCC_IRQHandler                    /* RCC                                         */
  def_irq_handler  EXTI0_IRQHandler                  /* EXTI Line0                                  */
  def_irq_handler  EXTI1_IRQHandler                  /* EXTI Line1                                  */
  def_irq_handler  EXTI2_IRQHandler                  /* EXTI Line2                                  */
  def_irq_handler  EXTI3_IRQHandler                  /* EXTI Line3                                  */
  def_irq_handler  EXTI4_IRQHandler                  /* EXTI Line4                                  */
  def_irq_handler  DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                               */
  def_irq_handler  DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                               */
  def_irq_handler  DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                               */
  def_irq_handler  DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                               */
  def_irq_handler  DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                               */
  def_irq_handler  DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                               */
  def_irq_handler  DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                               */
  def_irq_handler  ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s                        */
  def_irq_handler  CAN1_TX_IRQHandler                /* CAN1 TX                                     */
  def_irq_handler  CAN1_RX0_IRQHandler               /* CAN1 RX0                                    */
  def_irq_handler  CAN1_RX1_IRQHandler               /* CAN1 RX1                                    */
  def_irq_handler  CAN1_SCE_IRQHandler               /* CAN1 SCE                                    */
  def_irq_handler  EXTI9_5_IRQHandler                /* External Line[9:5]s                         */
  def_irq_handler  TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9                         */
  def_irq_handler  TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10                       */
  def_irq_handler  TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11      */
  def_irq_handler  TIM1_CC_IRQHandler                /* TIM1 Capture Compare                        */
  def_irq_handler  TIM2_IRQHandler                   /* TIM2                                        */
  def_irq_handler  TIM3_IRQHandler                   /* TIM3                                        */
  def_irq_handler  TIM4_IRQHandler                   /* TIM4                                        */
  def_irq_handler  I2C1_EV_IRQHandler                /* I2C1 Event                                  */
  def_irq_handler  I2C1_ER_IRQHandler                /* I2C1 Error                                  */
  def_irq_handler  I2C2_EV_IRQHandler                /* I2C2 Event                                  */
  def_irq_handler  I2C2_ER_IRQHandler                /* I2C2 Error                                  */
  def_irq_handler  SPI1_IRQHandler                   /* SPI1                                        */
  def_irq_handler  SPI2_IRQHandler                   /* SPI2                                        */
  def_irq_handler  USART1_IRQHandler                 /* USART1                                      */
  def_irq_handler  USART2_IRQHandler                 /* USART2                                      */
  def_irq_handler  USART3_IRQHandler                 /* USART3                                      */
  def_irq_handler  EXTI15_10_IRQHandler              /* External Line[15:10]s                       */
  def_irq_handler  RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line       */
  def_irq_handler  OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line         */
  def_irq_handler  TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12                        */
  def_irq_handler  TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13                       */
  def_irq_handler  TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14      */
  def_irq_handler  TIM8_CC_IRQHandler                /* TIM8 Capture Compare                        */
  def_irq_handler  DMA1_Stream7_IRQHandler           /* DMA1 Stream7                                */
  def_irq_handler  FSMC_IRQHandler                   /* FSMC                                        */
  def_irq_handler  SDIO_IRQHandler                   /* SDIO                                        */
  def_irq_handler  TIM5_IRQHandler                   /* TIM5                                        */
  def_irq_handler  SPI3_IRQHandler                   /* SPI3                                        */
  def_irq_handler  UART4_IRQHandler                  /* UART4                                       */
  def_irq_handler  UART5_IRQHandler                  /* UART5                                       */
  def_irq_handler  TIM6_DAC_IRQHandler               /* TIM6, DAC1 and DAC2                         */
  def_irq_handler  TIM7_IRQHandler                   /* TIM7                                        */
  def_irq_handler  DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                               */
  def_irq_handler  DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                               */
  def_irq_handler  DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                               */
  def_irq_handler  DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                               */
  def_irq_handler  DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                               */
  def_irq_handler  DFSDM1_FLT0_IRQHandler            /* DFSDM1 Filter0                              */
  def_irq_handler  DFSDM1_FLT1_IRQHandler            /* DFSDM1 Filter1                              */
  def_irq_handler  CAN2_TX_IRQHandler                /* CAN2 TX                                     */
  def_irq_handler  CAN2_RX0_IRQHandler               /* CAN2 RX0                                    */
  def_irq_handler  CAN2_RX1_IRQHandler               /* CAN2 RX1                                    */
  def_irq_handler  CAN2_SCE_IRQHandler               /* CAN2 SCE                                    */
  def_irq_handler  OTG_FS_IRQHandler                 /* USB OTG FS                                  */
  def_irq_handler  DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                               */
  def_irq_handler  DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                               */
  def_irq_handler  DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                               */
  def_irq_handler  USART6_IRQHandler                 /* USART6                                      */
  def_irq_handler  I2C3_EV_IRQHandler                /* I2C3 event                                  */
  def_irq_handler  I2C3_ER_IRQHandler                /* I2C3 error                                  */
  def_irq_handler  CAN3_TX_IRQHandler                /* CAN3 TX                                     */
  def_irq_handler  CAN3_RX0_IRQHandler               /* CAN3 RX0                                    */
  def_irq_handler  CAN3_RX1_IRQHandler               /* CAN3 RX1                                    */
  def_irq_handler  CAN3_SCE_IRQHandler               /* CAN3 SCE                                    */
  def_irq_handler  RNG_IRQHandler                    /* RNG                                         */
  def_irq_handler  FPU_IRQHandler                    /* FPU                                         */
  def_irq_handler  UART7_IRQHandler                  /* UART7                                       */
  def_irq_handler  UART8_IRQHandler                  /* UART8                                       */
  def_irq_handler  SPI4_IRQHandler                   /* SPI4                                        */
  def_irq_handler  SPI5_IRQHandler                   /* SPI5                                        */
  def_irq_handler  SAI1_IRQHandler                   /* SAI1                                        */
  def_irq_handler  UART9_IRQHandler                  /* UART9                                       */
  def_irq_handler  UART10_IRQHandler                 /* UART10                                      */
  def_irq_handler  QUADSPI_IRQHandler                /* QuadSPI                                     */
  def_irq_handler  FMPI2C1_EV_IRQHandler             /* FMPI2C1 Event                               */
  def_irq_handler  FMPI2C1_ER_IRQHandler             /* FMPI2C1 Error                               */
  def_irq_handler  LPTIM1_IRQHandler                 /* LPTIM1                                      */
  def_irq_handler  DFSDM2_FLT0_IRQHandler            /* DFSDM2 Filter0                              */
  def_irq_handler  DFSDM2_FLT1_IRQHandler            /* DFSDM2 Filter1                              */
  def_irq_handler  DFSDM2_FLT2_IRQHandler            /* DFSDM2 Filter2                              */
  def_irq_handler  DFSDM2_FLT3_IRQHandler            /* DFSDM2 Filter3                              */

.end
