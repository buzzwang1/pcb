
#include "main.h"

// CH641
// RiscV 32b MCU, (48 MHz max)
// Rom 16KB, Ram 2kb

//
// TIM1(AT), TIM2(GP)


// Servo:
//
//
// PB1: LED/U1_Tx
//
// PB3H: CH1P: TIM1_CH2
// PB4H: CH2P: TIM1_CH3
// PA2:  CH1N: TIM1_CH1N
// PA4:  CH2N: TIM1_CH3N
// 
//  PB4 PB3 PA4 PA2
//   0   0   0   0   |  Obere beiden durchgeschaltet
//   0   0   0   1   |  Kurzschluss
//   0   0   1   0   |  Kurzschluss
//   0   0   1   1   |  Kurzschluss
//   0   1   0   0   |  Freilauf
//   0   1   0   1   |  CCW
//   0   1   1   0   |  Kurzschluss
//   0   1   1   1   |  Kurzschluss
//   1   0   0   0   |  Freilauf
//   1   0   0   1   |  Kurzschluss
//   1   0   1   0   |  CW
//   1   0   1   1   |  Kurzschluss
//   1   1   0   0   |  Freilauf
//   1   1   0   1   |  Freilauf
//   1   1   1   0   |  Freilauf
//   1   1   1   1   |  Untere beiden durchgeschaltet
//
// Shunt: 10mOhm PGA = 75 => 3300mV / 75 = 44mV; 44mV/10mOhm = 4.4A
//  50mA: 0,5mV * 75 =  35,5mV = 11 digit
// 100mA: 1,0mV * 75 =  75,0mV = 23 digit
// 200mA: 2,0mV * 75 = 150,0mV = 46 digit
// ISP: A8 => ADC_IN8
// ISN: A7
// 
// PA6: Poti:           ADC_IN0
// 
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
// DMA
// DAM1_CH1              ADC



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



void vLedInit()
{
  // PB1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // PB1 ist im Low register
  GPIOB->CFGLR &= ~(0xF << (1 * 4));
  // 1 = 0001 => Output PP 10Mhz
  GPIOB->CFGLR |= ((1) << (1 * 4));
}

void vLedToggle()
{
  GPIOB->OUTDR ^= (1<<1);
}



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




void vMotCntrPort()
{  
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void vMotCntrTimer()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Lower channels always SW Controlled
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
  //GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void vMotInit()
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM1, ENABLE);

  vMotCntrPort();

  GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_4);
  GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4);

  TIM1->CTLR1 = 0x80;  // Autorelaod enable and deactivate Timer
  TIM1->BDTR = 0x0000; // Main Outputs disable

  TIM1->CTLR2   = 0x00;
  TIM1->SMCFGR  = 0x00;
  TIM1->SWEVGR  = 0x00;
  TIM1->CHCTLR1 = 0x6060; //CH2 Mode = PWM1 // CH1 Mode PWM1 für Adc Tigger
  TIM1->CHCTLR2 = 0x0060; //CH3 Mode = PWM1

  TIM1->CCER    = 0x0331; // Enable output CH2, Enable output CH3, Inverse, because high side pmos transistors are low active
                          // CH1 fo Adc Trigger

  // 8MHz: 20khz => 100% PWM => 2000khz => 8MHz/2000kHz => PSC = 4;
  TIM1->PSC = 4 - 1;
  TIM1->ATRLR = 100;

  TIM1->CH1CVR = 1; // Ch1 trigger ADC
  TIM1->CH2CVR = 0;
  TIM1->CH3CVR = 0;

  TIM1->BDTR = 0x8000; // Main Outputs enable
  TIM1->CTLR1 |= 0x01; // Enable Timer
}


u8 mu8PwmControl = 0;


void vMotStopTimer()
{
  if (mu8PwmControl)
  {
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    //Wait 1us @ 8Mhz
    __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
    __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
    vMotCntrPort();
  }
  mu8PwmControl = 0;
}





void vMotCw(u8 lu8PWM)
{
  // Cw Tim1 Ch3
  //
  // PB3H: CH1P: TIM1_CH2  => Tim1 
  // PA2:  CH1N: TIM1_CH1N => close

  // PB4H: CH2P: TIM1_CH3  => Tim1
  // PA4:  CH2N: TIM1_CH3N => open


  if (mu8PwmControl == 1)
  {
    TIM1->CH3CVR = lu8PWM;
  }
  else
  {
    mu8PwmControl = 1;

    TIM1->BDTR = 0x0000; // Main Outputs disable
    TIM1->CTLR1 = 0x80; // Autorelaod enable and deactivate Timer

    // open the lower nmos transistors
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);

    //Give control over the upper pmos transistors Timer
    cGpCfg::vSetCnf(GPIOB, 3, cGpCfg::nAfPp);
    cGpCfg::vSetCnf(GPIOB, 4, cGpCfg::nAfPp);


    // PB3H: CH1P: TIM1_CH2
    // PA2:  CH1N: TIM1_CH1N

    // PB4H: CH2P: TIM1_CH3
    // PA4:  CH2N: TIM1_CH3N

    TIM1->CH2CVR = 0;   // Must be kept 0  in this case, else the H-bridge is shorted
    TIM1->CH3CVR = lu8PWM;

    TIM1->BDTR = 0x8000; // Main Outputs enable
    TIM1->CTLR1 |= 0x01; // Enable Timer

    GPIO_ResetBits(GPIOA, GPIO_Pin_4);    // open the lower left
    GPIO_SetBits(GPIOA, GPIO_Pin_2);      // close the lower right
  }
}


void vMotCcw(u8 lu8PWM)
{
  // Cw Tim1 Ch2
  //
  // PB3H: CH1P: TIM1_CH2  => Tim1 
  // PA2:  CH1N: TIM1_CH1N => open

  // PB4H: CH2P: TIM1_CH3  => Tim1
  // PA4:  CH2N: TIM1_CH3N => close

  if (mu8PwmControl == 2)
  {
    TIM1->CH2CVR = lu8PWM;
  }
  else
  {
    mu8PwmControl = 2;

    TIM1->BDTR = 0x0000; // Main Outputs disable
    TIM1->CTLR1 = 0x80; // Autorelaod enable and deactivate Timer

    // open the lower nmos transistors
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);

    //Give control over the upper pmos transistors Timer
    cGpCfg::vSetCnf(GPIOB, 3, cGpCfg::nAfPp);
    cGpCfg::vSetCnf(GPIOB, 4, cGpCfg::nAfPp);

    // PB3H: CH1P: TIM1_CH2
    // PA2:  CH1N: TIM1_CH1N

    // PB4H: CH2P: TIM1_CH3
    // PA4:  CH2N: TIM1_CH3N

    TIM1->CH2CVR = lu8PWM;
    TIM1->CH3CVR = 0;  // Must be kept 0  in this case, else the H-bridge is shorted

    TIM1->BDTR = 0x8000; // Main Outputs enable
    TIM1->CTLR1 |= 0x01; // Enable Timer

    GPIO_ResetBits(GPIOA, GPIO_Pin_2);    // open the lower right
    GPIO_SetBits(GPIOA, GPIO_Pin_4);      // close the lower left
  }
}

void vSetPwm(i16 li16Pwm)
{
  if (li16Pwm <-99) li16Pwm = -101;
  if (li16Pwm > 99) li16Pwm =  101;

  if (li16Pwm > 0) vMotCw(li16Pwm);
              else vMotCcw(-li16Pwm);
}



void vMotDecayGnd()
{
  vMotStopTimer();

  GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4);    // close the upper transistors
  //Wait 1us @ 8Mhz
  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
  GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_4);  // open the lower transistors
}


void vMotFreeRun()
{
  vMotStopTimer();

  GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4);    // close the upper transistors
  GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_4);  // close the lower transistors
}

typedef enum
{
  nV_Vhv   = ADC_Channel_15,
  nV_shunt = ADC_Channel_8,
  nV_poti  = ADC_Channel_0,

  nIdx_ADC1_shunt1 = 0,
  nIdx_ADC1_shunt2 = 1,
  nIdx_ADC1_shunt3 = 2,
  nIdx_ADC1_poti   = 3,
  nIdx_ADC1_Vhv    = 4,
  nChnCount = 5,
}tenAdcCfg;

i16 mi16ADC1_Calibrattion_Val;
u16 mAdcResult[nChnCount]; // Result ADC1

i16 Get_CalibrationValue(ADC_TypeDef* ADCx)
{
  __IO u8  i, j;
  u16      buf[10];
  __IO u16 t;

  // 10 times calibration
  for (i = 0; i < 10; i++)
  {
    ADC_ResetCalibration(ADCx);
    while (ADC_GetResetCalibrationStatus(ADCx));
    ADC_StartCalibration(ADCx);
    while (ADC_GetCalibrationStatus(ADCx));
    buf[i] = ADCx->RDATAR;
  }

  // Bubble sort: Biggest to the front
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 9; j++)
    {
      if (buf[j] > buf[j + 1])
      {
        t = buf[j];
        buf[j] = buf[j + 1];
        buf[j + 1] = t;
      }
    }
  }

  // Average the values from the middle
  // discard the 2 largest and the 2 smallest values
  t = 0;
  for (i = 2; i < 8; i++)
  {
    t += buf[i];
  }

  t = (t / 6) + ((t % 6) / 3);

  return (i16)(512 - (i16)t);
}

void vAdcInit(void)
{
  // dma_ch0 config used for ADC
  // enable the DMA clock

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /*DMA_InitTypeDef DMA_InitStructure;
  
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(mAdcResult);
  DMA_InitStructure.DMA_DIR            = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize     = nChnCount;
  DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc      = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode           = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority       = DMA_Priority_Low;
  DMA_InitStructure.DMA_M2M            = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);*/

  DMA1_Channel1->CFGR  = 0x000005A1;
  DMA1_Channel1->CNTR  = nChnCount;
  DMA1_Channel1->PADDR = (u32)&ADC1->RDATAR;
  DMA1_Channel1->MADDR = (u32) & (mAdcResult);


  // -----------------------------------------------

  EXTEN->CTLR2 = 0x00000100; // ISP_AE ISP_OP enable;

  // -----------------------------------------------

  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div2); // 4MHz

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  
  // -----------------------------------------------

  ADC_DeInit(ADC1);

  /*ADC_InitTypeDef  ADC_InitStructure;
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel       = nChnCount;
  ADC_Init(ADC1, &ADC_InitStructure);*/


  // For the 10-bit resolution, the total conversion time is sampling time + 11” ADC_CLK cycles
  //
  //  3cycles: Total conversion time:   3 + 11 = 14cycles =  3,5us
  //  9Cycles: Total conversion time:   9 + 11 = 20cycles =  5,0us
  // 15cycles: Total conversion time:  15 + 11 = 26cycles =  6,5us
  // 30cycles: Total conversion time:  30 + 11 = 41cycles = 10,25us
  // 43cycles: Total conversion time:  43 + 11 = 54cycles = 13,5us
  // 57cycles: Total conversion time:  57 + 11 = 68cycles = 17,0us
  // 73cycles: Total conversion time:  73 + 11 = 84cycles = 21,0us
  //241cycles: Total conversion time: 241 + 11 =252cycles = 63,0us
  //ADC_RegularChannelConfig(ADC1, nV_shunt, nIdx_ADC1_shunt1 + 1, ADC_SampleTime_15Cycles); // 6,5us
  //ADC_RegularChannelConfig(ADC1, nV_shunt, nIdx_ADC1_shunt2 + 1, ADC_SampleTime_15Cycles); // 6,5us
  //ADC_RegularChannelConfig(ADC1, nV_shunt, nIdx_ADC1_shunt3 + 1, ADC_SampleTime_15Cycles); // 6,5us
  //ADC_RegularChannelConfig(ADC1, nV_poti,  nIdx_ADC1_poti   + 1, ADC_SampleTime_15Cycles); // 6,5us
  //ADC_RegularChannelConfig(ADC1, nV_Vhv,   nIdx_ADC1_Vhv    + 1, ADC_SampleTime_15Cycles); // 6,5us
  //                                                                                         // -----
  //                                                                                         // 32,5us
  //                                                                                         //
  //

  ADC_Cmd(ADC1, ENABLE);

  //Wait 1us @ 8Mhz
  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");

  mi16ADC1_Calibrattion_Val = Get_CalibrationValue(ADC1);

  ADC1->SAMPTR1 = 0x00010000;
  ADC1->SAMPTR2 = 0x02000002;
  ADC1->RSQR1 = 0x00400000; // 4 for (nChnCount - 1)
  ADC1->RSQR3 = 0x00F02108;
  ADC1->CTLR1 = 0x00000100;
  ADC1->CTLR2 = 0x00120101;

  //ADC_DMACmd(ADC1, ENABLE);
  //ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}



volatile u32 MAIN_u32Counter_1ms;

// Wird alle 1ms aufgerufen
void SysTick_Handler(void)
{
  ////if (SysTick->SR == 1)
  {
    SysTick->SR = 0; //clear State flag
    MAIN_u32Counter_1ms++;
  }
}


void vInitSysTick()
{
  SysTick->SR &= ~(1 << 0);//clear State flag
  SysTick->CMP = cClockInfo::u32GetClockSys() / 8000000 * 1000; //  8Mhz => 1000
                                                                // 16Mhz => 2000
  
  SysTick->CNT = 0;
  //               Autoreload,     HCLK/8,        Irq Enable,      Enable
  SysTick->CTLR =   (1 << 3) |    (0 << 2)     |   (1 << 1)   |   (1 << 0);

  // QingKeV2_Processor_Manual.PDF
  // Note: The smaller the priority value, the higher the priority.If the same preemption priority interrupt
  // hangs at the same time, the interrupt with the higher priority will be executed first.
  NVIC_SetPriority(SysTicK_IRQn, 0xB0);
  NVIC_EnableIRQ(SysTicK_IRQn);
}


void MAIN_vInitSystem(void)
{
  vLedInit();

  vInitSysTick();

  _eai();
}


void MAIN_vGotoApp(void *p)
{
  register uint32_t app_base = (u32)p;
  asm volatile ("jalr ra, %0" : : "r" (app_base));
}

i16 mi16PwmSet = 0;
u8  mu8ShuntOffsetSet = 0;
i16 mi16ShuntOffset = 0;

int main(void)
{
  u8           lu8LedBlink;
  ////volatile u32 mu32ResetReason = RCC->RSTSCKR;
  // 31 LPWRRSTF:   Low-power reset occurred. Cleared by writing to the RMVF bit.
  // 30 WWDGRSTF:   Window watchdog reset occurred. Cleared by writing to the RMVF
  // 29 IWDGRSTF:   Independent watchdog reset occurred. Cleared by writing to the RMVF bit
  // 28 SFTRSTF:    Software reset occurred. Cleared by writing to the RMVF bit
  // 27 PORRSTF:    Power - on / power - down reset occurred. Cleared by writing to the RMVF bit
  // 26 PINRSTF     NRST pin reset occurred. Cleared by writing to the RMVF bit
  
  lu8LedBlink = 0xFF;


  vMotInit();
  vAdcInit();


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
  MAIN_vInitSystem();
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
  while (1)
  {
  ////  cBnSpop_vResetWdog();
  ////
  ////  if ((MAIN_u32Counter_100ms % 10) == 0)
  ////  {
  ////    mcBn.vTickLp10ms();
  ////  }
    if (MAIN_u32Counter_1ms > 1000)
    {
      if (lu8LedBlink & 1)
      {
        // Byte rotieren
        lu8LedBlink >>= 1;
        lu8LedBlink |= 0x80;
        vLedToggle();


        if (mu8ShuntOffsetSet == 0)
        {
          mu8ShuntOffsetSet = 1;
          mi16ShuntOffset   = (mAdcResult[nIdx_ADC1_shunt1] + mAdcResult[nIdx_ADC1_shunt2] + mAdcResult[nIdx_ADC1_shunt3]) / 3;
        }       

        vSetPwm(mi16PwmSet);
        if (mi16PwmSet < 100) mi16PwmSet += 10;
        //mi16PwmSet = -mi16PwmSet;
      }
      else
      {
        lu8LedBlink >>= 1;
      }
  
      MAIN_u32Counter_1ms -= 1000;
    }
  ////  mcBn.vProcess10ms();
  ////
  ////  cBnSpop_vResetWdog();
  ////
  ////  // WFI scheint im Moment nicht richtig zu gehen
  ////  // Grund noch unbekannt, DMA U3 RX TC interrupt ging dann nicht mehr ?
  ////  // Oder U3->DMA und WFI vertragen sich nicht.
  ////  //__WFI();
  }
}



static void SetSysClockTo_8MHz_HSI(void)
{
  //Flash 0 wait state
  FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
  FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

  //HCLK = SYSCLK = APB1
  // 24MHZ / 3 = 8Mhz
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



