
#include "main.h"

// CH32V305
// RiscV 32b MCU, (144 MHz max)
// Rom 128KB, Ram 32kb

//
// TIM1(AT), TIM2(GP), TIM3(GP), TIM4(GP), TIM5(GP), TIM6(BT), TIM7(BT), TIM8(AT), TIM9(AT), TIM10(AT)


// Servo:
//
//
// PB15: LED
//
// PB10: I2C0_SCL/U0_Tx: I2C2_SCL(AF6)/USART3_TX(AF3)
// PB11: I2C0_SDA:       I2C2_SDA(AF6)
//
// PA05: Ntc:            ADC_IN5/DAC2_OUT
// Pc02: Poti:           ADC_IN12
// PA04: Mot Strom:      ADC_IN4
//
// PB6: Poti 3v3
// PB7: Poti GND
// 
// PB14: Mot Sleep:      1 = enable H-Bridge, 0 = disable H-Bridge
// PA06: Mot IN1:        TIM3_CH1(AF2)
// PA07: Mot IN2:        TIM3_CH2(AF2)
// PA08: TrigTest:       TIM1_CH1(AF1): Test Trigger ADC

//  SysTick (1ms/1Khz): Cyclic Caller
//  TIM10   (1us/1Mhz): => Usart3
//  TIM3  (500ns/2Mhz): => /100 => 20Khz 0..100step PWM Motor Ch1/Ch2
//  TIM1  (500ns/2Mhz): => /100 => 20Khz Trigger ADC1
//
//  USART3 => BotNet
//  DMA1.Chl2 = U3_Tx
//  DMA1.Chl3 = U3_Rx

//
// Interrupts:
// Systick:              Prio 7
// DMA1_CHl3 (USART RX): Prio 2
// USART3 TX:            Prio 2
// TIM10:                Prio 2
//

// BKP-Register
// Ersten 2x 32Bit werden von SPOP-APP für BLU gesetzt.
// Die uC-Backupregister haben aber nur 16 Bit, stehen aber an 32Bit Adressen
// Daher werden zwei 16Bit Backupregister zu einen 32Bit zusammenfügt
// [0]: SPOP String ersten 16Bit => 0x4F50 'SP'
// [1]: SPOP String ersten 16Bit => 0x5350 'OP'
// [2]: BN-DST Adr: z.B. EE12
// [3]: BN-SRC Adr: z.B. 1EEE
//
//


//#define PCB_PROJECTCFG_Test

#ifdef PCB_PROJECTCFG_Test
  LED<GPIOC_BASE, 1>    mcPC1;
  LED<GPIOC_BASE, 2>    mcPC2;
  LED<GPIOC_BASE, 3>    mcPC3;
  LED<GPIOC_BASE, 4>    mcPC4;
#endif


void vLedInit()
{
  // PB15
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // PB15 ist im High register
  GPIOB->CFGHR &= ~(0xF << ((15 & 7) * 4));
  // 1 = 0001 => Output PP 10Mhz
  GPIOB->CFGHR |= ((1) << ((15 & 7) * 4));
}

void vLedToggle()
{
  GPIOB->OUTDR ^= (1<<15);
}


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cUartMpHdSlave mcUartMpHdU3;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU3);
cBotNet_UpLinkUsartMpHd_RMsg mcUpLink(&mcUartMpHdU3);
cBotNet mcBn(&mcMyBotNetCfg);

#ifdef PCB_PROJECTCFG_Test
  ////static void vDtwStart()
  ////{
  ////  DWT->CTRL &= 0xFF000000UL;
  ////  DWT->CYCCNT = 0;
  ////  DWT->CPICNT = 0;
  ////  DWT->EXCCNT = 0;
  ////  DWT->CTRL |= 0x07F0001UL;
  ////}
#endif



void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    volatile u32 lu32MCause;
    CSRR_READ(lu32MCause, 0x342);
    if ((lu32MCause & 0x80000000) == 0) // Is it an Exception ?
    {
      switch (lu32MCause & 0xFF)
      {
        case 0: // Synchronous Instruction address misalignment
          asm volatile ("NOP");
          break;
        case 1 : // Synchronous Fetch command access error
          asm volatile ("NOP");
          break;
        case 2 : // Synchronous Illegal instructions
          asm volatile ("NOP");
          break;
        case 3 : // Synchronous Breakpoints
          asm volatile ("NOP");
          break;
        case 4 : // Synchronous Load instruction access address misalignment
          asm volatile ("NOP");
          break;
        case 5 : // Non - precision asynchronous Load command access error
          asm volatile ("NOP");
          break;
        case 6 : // Synchronous Store / AMO instruction access address misalignment
          asm volatile ("NOP");
          break;
        case 7 : // Non - precision asynchronous Store / AMO command access error
          asm volatile ("NOP");
          break;
        case 8 : // Synchronous Environment call in User mode
          asm volatile ("NOP");
          break;
        case 11 : // Synchronous Environment call in Machine mode
          asm volatile ("NOP");
          break;
      }
    }
  #endif

  while (1)
  {
  }
}


// USART3 TX DMA
void DMA1_Channel2_IRQHandler(void)
{
  // USART1 TX
  // Ender der Übertragung wird durch  U0.TC interrupt erledigt
  // Tx DMA Interrupt wird nicht benötigt
  // if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, 2
  // DMA_CH1))
  // {
  //   dma_channel_disable(DMA_CH1);
  //   DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
  //   mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  // }
}

// USART3 RX DMA
void DMA1_Channel3_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPC1.vSet1();
    ////vDtwStart();
  #endif

  // USART1 RX
  if (DMA1->INTFR & DMA1_FLAG_TC3)
  {
    DMA1_Channel3->CFGR &= ~(1 << 0); // EN
    DMA1->INTFCR = DMA1_IT_TC3;
    mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
  }

  #ifdef PCB_PROJECTCFG_Test
    __asm volatile ("nop");
    mcPC1.vSet0();
  #endif
}


void USART3_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPC2.vSet1();
  #endif

  if (USART3->CTLR1 & (1 << 13)) // Enable ?
  {
    if (USART3->STATR & USART_FLAG_TC)
    {
      USART3->STATR &= ~USART_FLAG_TC;
      mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
    }

    if (USART3->STATR & 0xF)
    {
      USART3->STATR &= ~USART_FLAG_ORE;
      mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    mcUartMpHdU3.vInitHw();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPC2.vSet0();
  #endif
}


void TIM10_UP_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPC3.vSet1();
  #endif

  TIM_ClearFlag(TIM10, TIM_FLAG_Update);
  TIM_Cmd(TIM10, DISABLE);
  mcUartMpHdU3.TIM_EV_IRQHandler();

  #ifdef PCB_PROJECTCFG_Test
    mcPC3.vSet0();
  #endif
}


u32 MAIN_u32Counter_100ms;

// Wird alle 1ms aufgerufen
void SysTick_Handler(void)
{
  if (SysTick->SR == 1)
  {
    SysTick->SR = 0; //clear State flag
    #ifdef PCB_PROJECTCFG_Test
      mcPC4.vSet1();
    #endif

    MAIN_u32Counter_100ms++;
    mcBn.vTickHp1ms();

    #ifdef PCB_PROJECTCFG_Test
      mcPC4.vSet0();
    #endif
  }
}


void vInitSysTick()
{
  SysTick->SR &= ~(1 << 0);//clear State flag
  SysTick->CMP = cClockInfo::u32GetClockSys() / 8000000 * 1000; //  8Mhz => 1000
                                                                // 16Mhz => 2000
  SysTick->CTLR = (1 << 5) | (1 << 3) | (1 << 1) | (1 << 0); // Init(Counter == 0), Upcounting, Enable, Irq Enable, Autoreload

  // QingKeV4_Processor_Manual.PDF
  // Note: The smaller the priority value, the higher the priority.If the same preemption priority interrupt
  // hangs at the same time, the interrupt with the higher priority will be executed first.
  NVIC_SetPriority(SysTicK_IRQn, (7 << 5) | (0x01 << 4));
  NVIC_EnableIRQ(SysTicK_IRQn);
}


void MAIN_vInitSystem(void)
{
  vLedInit();

  // Add Uplink
  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);

  vInitSysTick();
}


void MAIN_vGotoApp(void *p)
{
  register uint32_t app_base = (u32)p;
  asm volatile ("jalr ra, %0" : : "r" (app_base));
}


int main(void)
{
  u8           lu8LedBlink;
  volatile u32 mu32ResetReason = RCC->RSTSCKR;
  // 31 LPWRRSTF:   Low-power reset occurred. Cleared by writing to the RMVF bit.
  // 30 WWDGRSTF:   Window watchdog reset occurred. Cleared by writing to the RMVF
  // 29 IWDGRSTF:   Independent watchdog reset occurred. Cleared by writing to the RMVF bit
  // 28 SFTRSTF:    Software reset occurred. Cleared by writing to the RMVF bit
  // 27 PORRSTF:    Power - on / power - down reset occurred. Cleared by writing to the RMVF bit
  // 26 PINRSTF     NRST pin reset occurred. Cleared by writing to the RMVF bit

  RCC_ClearFlag();  // All reset flag clear

  if (!RomConst_IsValid())
  {
    // RomConst Error
    mcBn.mcAdr.Set(0x1EEE);
    mu32ResetReason |= RCC_IWDGRSTF;
    lu8LedBlink = 0x33;
  }
  else
  {
    lu8LedBlink = 0xFF;
  }

  cBnSpop_vResetWdog();

  #ifndef PCB_PROJECTCFG_Test
    if (mu32ResetReason & RCC_IWDGRSTF)
    {
      //Bei WatchDog im Bootloader bleiben
    }
    else
    if (mu32ResetReason & RCC_SFTRSTF)
    {
      // Bei SW-Reset nur im Bootloader bleiben,
      // wenn der SW-Reset nicht vom Applikation kam,
      // sonst in die App springen
      u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
      if (cBnSpop_u32ReadBackupData(0) != lu32Zwerg)
      {
        MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
      }
    }
    else
    {
      // Bei Power on reset direkt in die App
      MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
    }
  #else
    UNUSED(mu32ResetReason);
  #endif
  
  MAIN_vInitSystem();
  
  u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
  if (cBnSpop_u32ReadBackupData(0) == lu32Zwerg)
  {
    u32 lcAdress = cBnSpop_u32ReadBackupData(1);
  
    cBnSpop_vWriteBackupData(0, 0);
    cBnSpop_vWriteBackupData(1, 0);
  
    mcBn.mcUpLink->mStatus.IsOnline = 1;
    mcBn.mcSpop.vEnter(lcAdress >> 16, (u16)lcAdress, &mcBn);
  }

  while (1)
  {
    cBnSpop_vResetWdog();

    if ((MAIN_u32Counter_100ms % 10) == 0)
    {
      mcBn.vTickLp10ms();
    }
    if (MAIN_u32Counter_100ms > 100)
    {
      if (lu8LedBlink & 1)
      {
        // Byte rotieren
        lu8LedBlink >>= 1;
        lu8LedBlink |= 0x80;
        vLedToggle();
      }
      else
      {
        lu8LedBlink >>= 1;
      }

      MAIN_u32Counter_100ms -= 100;
    }
    mcBn.vProcess10ms();

    cBnSpop_vResetWdog();

    // WFI scheint im Moment nicht richtig zu gehen
    // Grund noch unbekannt, DMA U3 RX TC interrupt ging dann nicht mehr ?
    // Oder U3->DMA und WFI vertragen sich nicht.
    //__WFI();
  }
}



void MainSystemRccInit(void)
{
  #ifdef CH32V30x_D8C
    RCC->CFGR0 &= (uint32_t)0xF8FF0000;
  #else
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
  #endif 

  RCC->CTLR &= (uint32_t)0xFEF6FFFF;
  RCC->CTLR &= (uint32_t)0xFFFBFFFF;
  RCC->CFGR0 &= (uint32_t)0xFF80FFFF;

  #ifdef CH32V30x_D8C
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;
  #else
    RCC->INTR = 0x009F0000;
  #endif
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  #ifndef PCB_PROJECTCFG_Test
    cBnSpop_vStartWdog();
  #endif


  MainSystemRccInit();

  // 1. This routine uses synchronous triggering to demonstrate the use of interrupt nesting.
  //    The maximum nesting level is 8, and the hardware stack only Save the lower three levels,
  //    high priority level 5 interrupts need to use software push stack, low priority level 3 can
  //    Use hardware push or software push.
  //
  // 2. If you only use hardware push, you can configure the nesting level to be 4 and close
  //    the global when the hardware push overflows Interrupt (configure CSR (0x804) in the startup
  //    file, the configuration value is changed from 0x1f to 0x0b)
  //
  // 3. If you do not use hardware push stack, configure bit0 of CSR (0x804) to clear to 0,
  //    and remove the interrupt function declaration "WCH-Interrupt-fast" keyword
}



