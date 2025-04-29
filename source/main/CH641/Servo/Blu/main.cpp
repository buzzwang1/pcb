
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

//#ifdef PCB_PROJECTCFG_Test
//  LED<GPIOC_BASE, 1>    mcPC1;
//  LED<GPIOC_BASE, 2>    mcPC2;
//  LED<GPIOC_BASE, 3>    mcPC3;
//  LED<GPIOC_BASE, 4>    mcPC4;
//#endif


//void vLedInit()
//{
//  // PB15
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  // PB15 ist im High register
//  GPIOB->CFGHR &= ~(0xF << ((15 & 7) * 4));
//  // 1 = 0001 => Output PP 10Mhz
//  GPIOB->CFGHR |= ((1) << ((15 & 7) * 4));
//}

//void vLedToggle()
//{
//  GPIOB->OUTDR ^= (1<<15);
//}



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


// USART1 TX DMA
//void DMA1_Channel5_IRQHandler(void)
//{
  // USART1 TX
  // Ender der Übertragung wird durch  U0.TC interrupt erledigt
  // Tx DMA Interrupt wird nicht benötigt
//}

// USART3 RX DMA
void DMA1_Channel4_IRQHandler(void)
{
  // USART1 RX
  if (DMA1->INTFR & DMA1_FLAG_TC4)
  {
    DMA1_Channel4->CFGR &= ~(1 << 0); // EN
    DMA1->INTFCR = DMA1_IT_TC4;
    //mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);
  }

}


void USART1_IRQHandler(void)
{

  if (USART1->CTLR1 & (1 << 13)) // Enable ?
  {
    if (USART1->STATR & USART_FLAG_TC)
    {
      USART1->STATR &= ~USART_FLAG_TC;
      //mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
    }

    if (USART1->STATR & 0xF)
    {
      USART1->STATR &= ~USART_FLAG_ORE;
      //mcUartMpHdU3.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    //mcUartMpHdU3.vInitHw();
  }
}


void TIM2_IRQHandler(void)
{
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_Cmd(TIM2, DISABLE);
//  mcUartMpHdU3.TIM_EV_IRQHandler();
}


u32 MAIN_u32Counter_100ms;

// Wird alle 1ms aufgerufen
void SysTick_Handler(void)
{
  if (SysTick->SR == 1)
  {
    SysTick->SR = 0; //clear State flag
    MAIN_u32Counter_100ms++;
  }
}


void vInitSysTick()
{
  SysTick->SR &= ~(1 << 0);//clear State flag
  SysTick->CMP = cClockInfo::u32GetClockSys() / 8000000 * 1000; //  8Mhz => 1000
                                                                // 16Mhz => 2000
  
  //               Autoreload,     HCLK/8,        Irq Enable,      Enable
  SysTick->CTLR =   (1 << 3) |    (0 << 2)     |   (1 << 1)   |   (1 << 0);

  // QingKeV2_Processor_Manual.PDF
  // Note: The smaller the priority value, the higher the priority.If the same preemption priority interrupt
  // hangs at the same time, the interrupt with the higher priority will be executed first.
  NVIC_SetPriority(SysTicK_IRQn, (7 << 5) | (0x01 << 4));
  NVIC_EnableIRQ(SysTicK_IRQn);
}


void MAIN_vInitSystem(void)
{
  //vLedInit();

  vInitSysTick();
}


void MAIN_vGotoApp(void *p)
{
  register uint32_t app_base = (u32)p;
  asm volatile ("jalr ra, %0" : : "r" (app_base));
}


int main(void)
{
  ////u8           lu8LedBlink;
  ////volatile u32 mu32ResetReason = RCC->RSTSCKR;
  // 31 LPWRRSTF:   Low-power reset occurred. Cleared by writing to the RMVF bit.
  // 30 WWDGRSTF:   Window watchdog reset occurred. Cleared by writing to the RMVF
  // 29 IWDGRSTF:   Independent watchdog reset occurred. Cleared by writing to the RMVF bit
  // 28 SFTRSTF:    Software reset occurred. Cleared by writing to the RMVF bit
  // 27 PORRSTF:    Power - on / power - down reset occurred. Cleared by writing to the RMVF bit
  // 26 PINRSTF     NRST pin reset occurred. Cleared by writing to the RMVF bit

  ////RCC_ClearFlag();  // All reset flag clear
  ////
  ////if (!RomConst_IsValid())
  ////{
  ////  // RomConst Error
  ////  mcBn.mcAdr.Set(0x1EEE);
  ////  mu32ResetReason |= RCC_IWDGRSTF;
  ////  lu8LedBlink = 0x33;
  ////}
  ////else
  ////{
  ////  lu8LedBlink = 0xFF;
  ////}
  ////
  ////cBnSpop_vResetWdog();
  ////
  ////#ifndef PCB_PROJECTCFG_Test
  ////  if (mu32ResetReason & RCC_IWDGRSTF)
  ////  {
  ////    //Bei WatchDog im Bootloader bleiben
  ////  }
  ////  else
  ////  if (mu32ResetReason & RCC_SFTRSTF)
  ////  {
  ////    // Bei SW-Reset nur im Bootloader bleiben,
  ////    // wenn der SW-Reset nicht vom Applikation kam,
  ////    // sonst in die App springen
  ////    u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
  ////    if (cBnSpop_u32ReadBackupData(0) != lu32Zwerg)
  ////    {
  ////      MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
  ////    }
  ////  }
  ////  else
  ////  {
  ////    // Bei Power on reset direkt in die App
  ////    MAIN_vGotoApp(ROMCONST_PARTITION_START_ADRESS_APP);
  ////  }
  ////#else
  ////  UNUSED(mu32ResetReason);
  ////#endif
  ////
  ////MAIN_vInitSystem();
  ////
  ////u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
  ////if (cBnSpop_u32ReadBackupData(0) == lu32Zwerg)
  ////{
  ////  u32 lcAdress = cBnSpop_u32ReadBackupData(1);
  ////
  ////  cBnSpop_vWriteBackupData(0, 0);
  ////  cBnSpop_vWriteBackupData(1, 0);
  ////
  ////  mcBn.mcUpLink->mStatus.IsOnline = 1;
  ////  mcBn.mcSpop.vEnter(lcAdress >> 16, (u16)lcAdress, &mcBn);
  ////}
  ////
  ////while (1)
  ////{
  ////  cBnSpop_vResetWdog();
  ////
  ////  if ((MAIN_u32Counter_100ms % 10) == 0)
  ////  {
  ////    mcBn.vTickLp10ms();
  ////  }
  ////  if (MAIN_u32Counter_100ms > 100)
  ////  {
  ////    if (lu8LedBlink & 1)
  ////    {
  ////      // Byte rotieren
  ////      lu8LedBlink >>= 1;
  ////      lu8LedBlink |= 0x80;
  ////      vLedToggle();
  ////    }
  ////    else
  ////    {
  ////      lu8LedBlink >>= 1;
  ////    }
  ////
  ////    MAIN_u32Counter_100ms -= 100;
  ////  }
  ////  mcBn.vProcess10ms();
  ////
  ////  cBnSpop_vResetWdog();
  ////
  ////  // WFI scheint im Moment nicht richtig zu gehen
  ////  // Grund noch unbekannt, DMA U3 RX TC interrupt ging dann nicht mehr ?
  ////  // Oder U3->DMA und WFI vertragen sich nicht.
  ////  //__WFI();
  ////}
}



static void SetSysClockTo_8MHz_HSI(void)
{
  //Flash 0 wait state
  FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
  FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

  //HCLK = SYSCLK = APB1
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV3;

  SystemCoreClock = 8000000;
}


/*static void SetSysClockTo_24MHZ_HSI(void)
{
  //Flash 0 wait state
  FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
  FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

  // HCLK = SYSCLK = APB1
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;

  SystemCoreClock = 24000000;
}*/


void MainSystemRccInit(void)
{
  RCC->CTLR |= (uint32_t)0x00000001;
  RCC->CFGR0 &= (uint32_t)0xF8FE0000;
  RCC->CTLR &= (uint32_t)0xFEFFFFFF;
  RCC->INTR = 0x00140000;

  RCC_AdjustHSICalibrationValue(0x10);

  SetSysClockTo_8MHz_HSI();
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  //#ifndef PCB_PROJECTCFG_Test
  //  cBnSpop_vStartWdog();
  //#endif

  MainSystemRccInit();
}



