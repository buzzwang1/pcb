#include "samd10.h"

/**
 * \brief We assume that the main() returns, most embedded systems
 * don't do that. If your main() is not returning then you can define MAIN_DONT_RETURN.
 * By defining MAIN_DONT_RETURN, you will save some stack and code space.
 */

/* Header for end of stack, declared in linker file */
/* Sections declared in linker files: contain BSS, Zero, Stack */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;


/**
 * \brief This is the code that gets called on processor reset.
 * A naked attribute is used to skip the prologue and epilogue,
 * this is the start of the chain.
 */
void _start(void) __attribute__ ((weak, alias("_alt_start")));
extern int PreMain(void);
void __attribute__ ((naked)) Reset_Handler( void )
{
	register uint32_t *pSrc, *pDest;

	// Initialize the relocate segment
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest)
	{
		for (; pDest < &_erelocate;)
		{
			*pDest++ = *pSrc++;
		}
	}

	// Set the vector table base address
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

  PreMain();

	// Branch to the start function in the library. If it is not
	// there, we use the _alt_start function in this module.
	//
    _start();
}


/**
 * \brief This is the alternative start code that gets called if the
 * default library start files are not used. The BSS is cleared and
 * the main function is called.
 */
extern int main(void);

void
#ifdef MAIN_DONT_RETURN
 __attribute__ ((naked))
#endif
_alt_start(void)
{
    register uint32_t *pDest;

 	// Clear the zero segment
	for (pDest = &_szero; pDest < &_ezero;)
    {
		*pDest++ = 0;
	}

    // Branch to our user application
    main();

#ifndef MAIN_DONT_RETURN
	// Infinite loop
	while (1);
#endif
}


/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Dummy_Handler( void )
{
	while ( 1 )
	{
	}
}

/* Cortex-M0+ core handlers */
void NMI_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler       ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SVC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PendSV_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SysTick_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Peripherals handlers */
void PM_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SYSCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void WDT_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void RTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EIC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DMAC_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void USB_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void EVSYS_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM0_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM1_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void SERCOM2_Handler         ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TCC0_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC1_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void TC2_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void ADC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void AC_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void DAC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void PTC_Handler             ( void ) __attribute__ ((weak, alias("Dummy_Handler")));

/* Exception Table */
__attribute__ ((section(".vectors")))
const DeviceVectors exception_table =
{
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    .pvStack              = (void*) (&_estack),

    .pfnReset_Handler     = (void*) Reset_Handler,
    .pfnNMI_Handler       = (void*) NMI_Handler,
    .pfnHardFault_Handler = (void*) HardFault_Handler,
    .pfnReservedM12       = (void*) (0UL), /* Reserved */
    .pfnReservedM11       = (void*) (0UL), /* Reserved */
    .pfnReservedM10       = (void*) (0UL), /* Reserved */
    .pfnReservedM9        = (void*) (0UL), /* Reserved */
    .pfnReservedM8        = (void*) (0UL), /* Reserved */
    .pfnReservedM7        = (void*) (0UL), /* Reserved */
    .pfnReservedM6        = (void*) (0UL), /* Reserved */
    .pfnSVC_Handler       = (void*) SVC_Handler,
    .pfnReservedM4        = (void*) (0UL), /* Reserved */
    .pfnReservedM3        = (void*) (0UL), /* Reserved */
    .pfnPendSV_Handler    = (void*) PendSV_Handler,
    .pfnSysTick_Handler   = (void*) SysTick_Handler,

    /* Configurable interrupts */
    .pfnPM_Handler        = (void*) PM_Handler,             /*  0 Power Manager */
    .pfnSYSCTRL_Handler   = (void*) SYSCTRL_Handler,        /*  1 System Control */
    .pfnWDT_Handler       = (void*) WDT_Handler,            /*  2 Watchdog Timer */
    .pfnRTC_Handler       = (void*) RTC_Handler,            /*  3 Real-Time Counter */
    .pfnEIC_Handler       = (void*) EIC_Handler,            /*  4 External Interrupt Controller */
    .pfnNVMCTRL_Handler   = (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
    .pfnDMAC_Handler      = (void*) DMAC_Handler,           /*  6 Direct Memory Access Controller */
    .pfnReserved7         = (void*) USB_Handler,            /*  7 Universal Serial Bus */
    .pfnEVSYS_Handler     = (void*) EVSYS_Handler,          /*  8 Event System Interface */
    .pfnSERCOM0_Handler   = (void*) SERCOM0_Handler,        /*  9 Serial Communication Interface 0 */
    .pfnSERCOM1_Handler   = (void*) SERCOM1_Handler,        /* 10 Serial Communication Interface 1 */
    .pfnSERCOM2_Handler   = (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
    .pfnTCC0_Handler      = (void*) TCC0_Handler,           /* 12 Basic Timer Counter 0 */
    .pfnTC1_Handler       = (void*) TC1_Handler,            /* 13 Basic Timer Counter 1 */
    .pfnTC2_Handler       = (void*) TC2_Handler,            /* 14 Basic Timer Counter 2 */
    .pfnADC_Handler       = (void*) ADC_Handler,            /* 15 Analog Digital Converter */
    .pfnAC_Handler        = (void*) AC_Handler,             /* 16 Analog Comparators */
    .pfnDAC_Handler       = (void*) DAC_Handler,            /* 17 Digital Analog Converter */
    .pfnPTC_Handler       = (void*) PTC_Handler             /* 18 Peripheral Touch Controller */
};
