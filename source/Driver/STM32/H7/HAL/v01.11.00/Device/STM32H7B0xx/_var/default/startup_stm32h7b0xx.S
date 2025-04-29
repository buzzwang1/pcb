/**
  ******************************************************************************
  * @file      startup_stm32h7b0xx.s
  * @author    MCD Application Team
  * @brief     STM32H7B0xx Devices vector table for GCC based toolchain. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
    
  .syntax unified
  .cpu cortex-m7
  .fpu softvfp
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
  .long WWDG_IRQHandler                   /* Window WatchDog              */
  .long PVD_PVM_IRQHandler                /* PVD/PVM through EXTI Line detection */
  .long RTC_TAMP_STAMP_CSS_LSE_IRQHandler /* Tamper and TimeStamps through the EXTI line */
  .long RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
  .long FLASH_IRQHandler                  /* FLASH                        */
  .long RCC_IRQHandler                    /* RCC                          */
  .long EXTI0_IRQHandler                  /* EXTI Line0                   */
  .long EXTI1_IRQHandler                  /* EXTI Line1                   */
  .long EXTI2_IRQHandler                  /* EXTI Line2                   */
  .long EXTI3_IRQHandler                  /* EXTI Line3                   */
  .long EXTI4_IRQHandler                  /* EXTI Line4                   */
  .long DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
  .long DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
  .long DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
  .long DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
  .long DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
  .long DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
  .long DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
  .long ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
  .long FDCAN1_IT0_IRQHandler             /* FDCAN1 interrupt line 0      */
  .long FDCAN2_IT0_IRQHandler             /* FDCAN2 interrupt line 0      */
  .long FDCAN1_IT1_IRQHandler             /* FDCAN1 interrupt line 1      */
  .long FDCAN2_IT1_IRQHandler             /* FDCAN2 interrupt line 1      */
  .long EXTI9_5_IRQHandler                /* External Line[9:5]s          */
  .long TIM1_BRK_IRQHandler               /* TIM1 Break interrupt         */
  .long TIM1_UP_IRQHandler                /* TIM1 Update interrupt        */
  .long TIM1_TRG_COM_IRQHandler           /* TIM1 Trigger and Commutation interrupt */
  .long TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
  .long TIM2_IRQHandler                   /* TIM2                         */
  .long TIM3_IRQHandler                   /* TIM3                         */
  .long TIM4_IRQHandler                   /* TIM4                         */
  .long I2C1_EV_IRQHandler                /* I2C1 Event                   */
  .long I2C1_ER_IRQHandler                /* I2C1 Error                   */
  .long I2C2_EV_IRQHandler                /* I2C2 Event                   */
  .long I2C2_ER_IRQHandler                /* I2C2 Error                   */
  .long SPI1_IRQHandler                   /* SPI1                         */
  .long SPI2_IRQHandler                   /* SPI2                         */
  .long USART1_IRQHandler                 /* USART1                       */
  .long USART2_IRQHandler                 /* USART2                       */
  .long USART3_IRQHandler                 /* USART3                       */
  .long EXTI15_10_IRQHandler              /* External Line[15:10]s        */
  .long RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
  .long DFSDM2_IRQHandler                 /* DFSDM2 Interrupt             */
  .long TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
  .long TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
  .long TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
  .long TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
  .long DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
  .long FMC_IRQHandler                    /* FMC                          */
  .long SDMMC1_IRQHandler                 /* SDMMC1                       */
  .long TIM5_IRQHandler                   /* TIM5                         */
  .long SPI3_IRQHandler                   /* SPI3                         */
  .long UART4_IRQHandler                  /* UART4                        */
  .long UART5_IRQHandler                  /* UART5                        */
  .long TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
  .long TIM7_IRQHandler                   /* TIM7                         */
  .long DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
  .long DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
  .long DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
  .long DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
  .long DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long FDCAN_CAL_IRQHandler              /* FDCAN calibration unit interrupt*/
  .long DFSDM1_FLT4_IRQHandler            /* DFSDM Filter4 Interrupt      */
  .long DFSDM1_FLT5_IRQHandler            /* DFSDM Filter5 Interrupt      */
  .long DFSDM1_FLT6_IRQHandler            /* DFSDM Filter6 Interrupt      */
  .long DFSDM1_FLT7_IRQHandler            /* DFSDM Filter7 Interrupt      */
  .long DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
  .long DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
  .long DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
  .long USART6_IRQHandler                 /* USART6                       */
  .long I2C3_EV_IRQHandler                /* I2C3 event                   */
  .long I2C3_ER_IRQHandler                /* I2C3 error                   */
  .long OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
  .long OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
  .long OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
  .long OTG_HS_IRQHandler                 /* USB OTG HS                   */
  .long DCMI_PSSI_IRQHandler              /* DCMI, PSSI                   */
  .long CRYP_IRQHandler                   /* CRYP crypto global interrupt */
  .long HASH_RNG_IRQHandler               /* RNG, HASH                    */
  .long FPU_IRQHandler                    /* FPU                          */
  .long UART7_IRQHandler                  /* UART7                        */
  .long UART8_IRQHandler                  /* UART8                        */
  .long SPI4_IRQHandler                   /* SPI4                         */
  .long SPI5_IRQHandler                   /* SPI5                         */
  .long SPI6_IRQHandler                   /* SPI6                         */
  .long SAI1_IRQHandler                   /* SAI1                         */
  .long LTDC_IRQHandler                   /* LTDC                         */
  .long LTDC_ER_IRQHandler                /* LTDC error                   */
  .long DMA2D_IRQHandler                  /* DMA2D                        */
  .long SAI2_IRQHandler                   /* SAI2                         */
  .long OCTOSPI1_IRQHandler               /* OCTOSPI1                     */
  .long LPTIM1_IRQHandler                 /* LPTIM1                       */
  .long CEC_IRQHandler                    /* HDMI_CEC                     */
  .long I2C4_EV_IRQHandler                /* I2C4 Event                   */
  .long I2C4_ER_IRQHandler                /* I2C4 Error                   */
  .long SPDIF_RX_IRQHandler               /* SPDIF_RX                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long DMAMUX1_OVR_IRQHandler            /* DMAMUX1 Overrun interrupt    */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long 0                                 /* Reserved                     */
  .long DFSDM1_FLT0_IRQHandler            /* DFSDM Filter0 Interrupt      */
  .long DFSDM1_FLT1_IRQHandler            /* DFSDM Filter1 Interrupt      */
  .long DFSDM1_FLT2_IRQHandler            /* DFSDM Filter2 Interrupt      */
  .long DFSDM1_FLT3_IRQHandler            /* DFSDM Filter3 Interrupt      */
  .long 0                                 /* Reserved                     */
  .long SWPMI1_IRQHandler                 /* Serial Wire Interface 1 global interrupt */
  .long TIM15_IRQHandler                  /* TIM15 global Interrupt      */ 
  .long TIM16_IRQHandler                  /* TIM16 global Interrupt      */
  .long TIM17_IRQHandler                  /* TIM17 global Interrupt      */
  .long MDIOS_WKUP_IRQHandler             /* MDIOS Wakeup  Interrupt     */
  .long MDIOS_IRQHandler                  /* MDIOS global Interrupt      */
  .long JPEG_IRQHandler                   /* JPEG global Interrupt       */
  .long MDMA_IRQHandler                   /* MDMA global Interrupt       */
  .long 0                                 /* Reserved                    */
  .long SDMMC2_IRQHandler                 /* SDMMC2 global Interrupt     */
  .long HSEM1_IRQHandler                  /* HSEM1 global Interrupt      */
  .long 0                                 /* Reserved                    */
  .long DAC2_IRQHandler                   /* DAC2 global Interrupt       */
  .long DMAMUX2_OVR_IRQHandler            /* DMAMUX Overrun interrupt    */
  .long BDMA2_Channel0_IRQHandler         /* BDMA2 Channel 0 global Interrupt */
  .long BDMA2_Channel1_IRQHandler         /* BDMA2 Channel 1 global Interrupt */
  .long BDMA2_Channel2_IRQHandler         /* BDMA2 Channel 2 global Interrupt */
  .long BDMA2_Channel3_IRQHandler         /* BDMA2 Channel 3 global Interrupt */
  .long BDMA2_Channel4_IRQHandler         /* BDMA2 Channel 4 global Interrupt */
  .long BDMA2_Channel5_IRQHandler         /* BDMA2 Channel 5 global Interrupt */
  .long BDMA2_Channel6_IRQHandler         /* BDMA2 Channel 6 global Interrupt */
  .long BDMA2_Channel7_IRQHandler         /* BDMA2 Channel 7 global Interrupt */
  .long COMP_IRQHandler                   /* COMP global Interrupt      */   
  .long LPTIM2_IRQHandler                 /* LP TIM2 global interrupt     */
  .long LPTIM3_IRQHandler                 /* LP TIM3 global interrupt     */
  .long UART9_IRQHandler                  /* UART9 global interrupt       */
  .long USART10_IRQHandler                /* USART10 global interrupt     */
  .long LPUART1_IRQHandler                /* LP UART1 interrupt           */
  .long 0                                 /* Reserved                     */
  .long CRS_IRQHandler                    /* Clock Recovery Global Interrupt */
  .long ECC_IRQHandler                    /* ECC diagnostic Global Interrupt */
  .long 0                                 /* Reserved                     */
  .long DTS_IRQHandler                    /* DTS                          */
  .long 0                                 /* Reserved                     */
  .long WAKEUP_PIN_IRQHandler             /* Interrupt for all 6 wake-up pins */
  .long OCTOSPI2_IRQHandler               /* OCTOSPI2                     */
  .long OTFDEC1_IRQHandler                /* OTFDEC1                      */
  .long OTFDEC2_IRQHandler                /* OTFDEC2                      */
  .long GFXMMU_IRQHandler                 /* GFXMMU                       */
  .long BDMA1_IRQHandler                  /* BDMA1                        */

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

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler. 
* As they are weak aliases, any function with the same name will override 
* this definition.
* 
*******************************************************************************/
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
  def_irq_handler  RTC_TAMP_STAMP_CSS_LSE_IRQHandler
  def_irq_handler  RTC_WKUP_IRQHandler
  def_irq_handler  FLASH_IRQHandler
  def_irq_handler  RCC_IRQHandler
  def_irq_handler  EXTI0_IRQHandler
  def_irq_handler  EXTI1_IRQHandler
  def_irq_handler  EXTI2_IRQHandler 
  def_irq_handler  EXTI3_IRQHandler
  def_irq_handler  EXTI4_IRQHandler
  def_irq_handler  DMA1_Stream0_IRQHandler
  def_irq_handler  DMA1_Stream1_IRQHandler
  def_irq_handler  DMA1_Stream2_IRQHandler
  def_irq_handler  DMA1_Stream3_IRQHandler
  def_irq_handler  DMA1_Stream4_IRQHandler
  def_irq_handler  DMA1_Stream5_IRQHandler
  def_irq_handler  DMA1_Stream6_IRQHandler
  def_irq_handler  ADC_IRQHandler
  def_irq_handler  FDCAN1_IT0_IRQHandler
  def_irq_handler  FDCAN2_IT0_IRQHandler
  def_irq_handler  FDCAN1_IT1_IRQHandler
  def_irq_handler  FDCAN2_IT1_IRQHandler
  def_irq_handler  EXTI9_5_IRQHandler
  def_irq_handler  TIM1_BRK_IRQHandler
  def_irq_handler  TIM1_UP_IRQHandler
  def_irq_handler  TIM1_TRG_COM_IRQHandler
  def_irq_handler  TIM1_CC_IRQHandler
  def_irq_handler  TIM2_IRQHandler
  def_irq_handler  TIM3_IRQHandler
  def_irq_handler  TIM4_IRQHandler
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
  def_irq_handler  DFSDM2_IRQHandler
  def_irq_handler  TIM8_BRK_TIM12_IRQHandler
  def_irq_handler  TIM8_UP_TIM13_IRQHandler
  def_irq_handler  TIM8_TRG_COM_TIM14_IRQHandler
  def_irq_handler  TIM8_CC_IRQHandler
  def_irq_handler  DMA1_Stream7_IRQHandler
  def_irq_handler  FMC_IRQHandler
  def_irq_handler  SDMMC1_IRQHandler
  def_irq_handler  TIM5_IRQHandler
  def_irq_handler  SPI3_IRQHandler
  def_irq_handler  UART4_IRQHandler
  def_irq_handler  UART5_IRQHandler
  def_irq_handler  TIM6_DAC_IRQHandler
  def_irq_handler  TIM7_IRQHandler
  def_irq_handler  DMA2_Stream0_IRQHandler
  def_irq_handler  DMA2_Stream1_IRQHandler
  def_irq_handler  DMA2_Stream2_IRQHandler
  def_irq_handler  DMA2_Stream3_IRQHandler
  def_irq_handler  DMA2_Stream4_IRQHandler
  def_irq_handler  FDCAN_CAL_IRQHandler
  def_irq_handler  DFSDM1_FLT4_IRQHandler
  def_irq_handler  DFSDM1_FLT5_IRQHandler 
  def_irq_handler  DFSDM1_FLT6_IRQHandler 
  def_irq_handler  DFSDM1_FLT7_IRQHandler
  def_irq_handler  DMA2_Stream5_IRQHandler
  def_irq_handler  DMA2_Stream6_IRQHandler
  def_irq_handler  DMA2_Stream7_IRQHandler
  def_irq_handler  USART6_IRQHandler
  def_irq_handler  I2C3_EV_IRQHandler
  def_irq_handler  I2C3_ER_IRQHandler
  def_irq_handler  OTG_HS_EP1_OUT_IRQHandler
  def_irq_handler  OTG_HS_EP1_IN_IRQHandler
  def_irq_handler  OTG_HS_WKUP_IRQHandler
  def_irq_handler  OTG_HS_IRQHandler
  def_irq_handler  DCMI_PSSI_IRQHandler
  def_irq_handler  CRYP_IRQHandler   
  def_irq_handler  HASH_RNG_IRQHandler
  def_irq_handler  FPU_IRQHandler
  def_irq_handler  UART7_IRQHandler
  def_irq_handler  UART8_IRQHandler
  def_irq_handler  SPI4_IRQHandler
  def_irq_handler  SPI5_IRQHandler
  def_irq_handler  SPI6_IRQHandler
  def_irq_handler  SAI1_IRQHandler
  def_irq_handler  LTDC_IRQHandler
  def_irq_handler  LTDC_ER_IRQHandler
  def_irq_handler  DMA2D_IRQHandler
  def_irq_handler  SAI2_IRQHandler
  def_irq_handler  OCTOSPI1_IRQHandler
  def_irq_handler  LPTIM1_IRQHandler
  def_irq_handler  CEC_IRQHandler
  def_irq_handler  I2C4_EV_IRQHandler 
  def_irq_handler  I2C4_ER_IRQHandler
  def_irq_handler  SPDIF_RX_IRQHandler
  def_irq_handler  DMAMUX1_OVR_IRQHandler
  def_irq_handler  DFSDM1_FLT0_IRQHandler
  def_irq_handler  DFSDM1_FLT1_IRQHandler
  def_irq_handler  DFSDM1_FLT2_IRQHandler
  def_irq_handler  DFSDM1_FLT3_IRQHandler
  def_irq_handler  SWPMI1_IRQHandler
  def_irq_handler  TIM15_IRQHandler
  def_irq_handler  TIM16_IRQHandler
  def_irq_handler  TIM17_IRQHandler
  def_irq_handler  MDIOS_WKUP_IRQHandler
  def_irq_handler  MDIOS_IRQHandler
  def_irq_handler  JPEG_IRQHandler
  def_irq_handler  MDMA_IRQHandler
  def_irq_handler  SDMMC2_IRQHandler
  def_irq_handler  HSEM1_IRQHandler
  def_irq_handler  DAC2_IRQHandler
  def_irq_handler  DMAMUX2_OVR_IRQHandler
  def_irq_handler  BDMA2_Channel0_IRQHandler
  def_irq_handler  BDMA2_Channel1_IRQHandler
  def_irq_handler  BDMA2_Channel2_IRQHandler
  def_irq_handler  BDMA2_Channel3_IRQHandler
  def_irq_handler  BDMA2_Channel4_IRQHandler
  def_irq_handler  BDMA2_Channel5_IRQHandler
  def_irq_handler  BDMA2_Channel6_IRQHandler
  def_irq_handler  BDMA2_Channel7_IRQHandler
  def_irq_handler  COMP_IRQHandler
  def_irq_handler  LPTIM2_IRQHandler
  def_irq_handler  LPTIM3_IRQHandler
  def_irq_handler  LPTIM4_IRQHandler
  def_irq_handler  LPTIM5_IRQHandler
  def_irq_handler  UART9_IRQHandler
  def_irq_handler  USART10_IRQHandler
  def_irq_handler  LPUART1_IRQHandler
  def_irq_handler  CRS_IRQHandler
  def_irq_handler  ECC_IRQHandler
  def_irq_handler  DTS_IRQHandler
  def_irq_handler  WAKEUP_PIN_IRQHandler
  def_irq_handler  OCTOSPI2_IRQHandler
  def_irq_handler  OTFDEC1_IRQHandler
  def_irq_handler  OTFDEC2_IRQHandler
  def_irq_handler  GFXMMU_IRQHandler
  def_irq_handler  BDMA1_IRQHandler
   
.end