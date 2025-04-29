
#include "main.h"



// SAMD14D
// ARM®-based Cortex®-M0 32b MCU, (48 MHz max)
// Rom 16KB
// Ram  4KB



LED<(u32)&PORTA, 9> lcLedRed;


void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}



static volatile u32 mu32SysTick_Delay;
SercomUsart* mstCom = &(SERCOM0->USART);
char8 TestSource1[] = "abcd";
char8 TestDest1[8];

char8 TestSource2[] = "efgh";
char8 TestDest2[8];


void vSysTickInit(void)
{
  //SysTick_Config(system_gclk_gen_get_hz(GCLK_GENERATOR_0));
}

void vDelay_1ms(u32 count)
{
  mu32SysTick_Delay = count;
  while (0 != mu32SysTick_Delay);
}

void vDelay_Decrement(void)
{
  if (0 != mu32SysTick_Delay)
  {
    mu32SysTick_Delay--;
  }
}


void SysTick_Handler(void)
{
  vDelay_Decrement();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

     /* Infinite loop */
  while (1)
  {
  }
}
#endif


class cDma
{
public:
  __attribute__((__aligned__(16)))
  DmacDescriptor mDesc[6];  // The value must be 128 - bit aligned.
  DmacDescriptor mDescWriteBack[6]; // The value must be 128 - bit aligned.

  cDma()
  {
    // Configure synchronous bus clock
    PM->APBBSEL.bit.APBBDIV = 0;              // no prescaler
    PM->APBBMASK.bit.DMAC_ = 1;               // enable TC2 interface



    // Before the DMAC is enabled, it must be configured, as outlined by the following steps :
    //   - The SRAM address of where the descriptor memory section is located must be written to the Description Base
    //     Address(BASEADDR) register
    //   - The SRAM address of where the write - back section should be located must be written to the Write - Back
    //     Memory Base Address(WRBADDR) register
    //   - Priority level x of the arbiter can be enabled by writing a one to the Priority Level x Enable bit in the Control
    //     register(CTRL.LVLENx)
    DMAC->BASEADDR.reg = (u32)mDesc;
    DMAC->WRBADDR.reg  = (u32)mDescWriteBack;
    DMAC->CTRL.bit.LVLEN0 = 1;
    DMAC->CTRL.bit.LVLEN1 = 1;
    DMAC->CTRL.bit.LVLEN2 = 1;
    DMAC->CTRL.bit.LVLEN3 = 1;

    NVIC_EnableIRQ(DMAC_IRQn);
    DMAC->CTRL.bit.DMAENABLE = 1;

  }

  void vSetChannel0(u8* lpu8Src, u8* lpu8Dest, u16 lu16Count)
  {
    mDesc[0].SRCADDR.reg = (u32)lpu8Src;
    mDesc[0].DSTADDR.reg = (u32)lpu8Dest + lu16Count;
    mDesc[0].BTCNT.reg   = lu16Count;
    mDesc[0].DESCADDR.reg = 0; // The value must be 128 - bit aligned.

    mDesc[1].BTCTRL.bit.BLOCKACT = 0; // 0x0 = NOACT; 0x1 = INT; 0x2 = SUSPEND; 0x3 = BOTH
    mDesc[0].BTCTRL.bit.BEATSIZE = 0; // 1Byte
    mDesc[0].BTCTRL.bit.DSTINC   = 1; // Destination Increment
    mDesc[0].BTCTRL.bit.SRCINC   = 0; // Source      Increment
    mDesc[0].BTCTRL.bit.VALID    = 1;


    DMAC->CHID.reg = 0;
    DMAC->CHCTRLB.bit.TRIGACT  = 2; // 0x0 BLOCK; 0x2 BEAT; 0x3 TRANSACTION
    DMAC->CHCTRLB.bit.TRIGSRC  = 1; // SERCOM0 RX
    DMAC->CHINTENSET.bit.TCMPL = 1;
    DMAC->CHCTRLA.bit.ENABLE   = 1;
  }

  void vSetChannel1(u8* lpu8Src, u8* lpu8Dest, u16 lu16Count)
  {

    //   - Transfer Descriptor
    //     - The size of each access of the data transfer bus must be selected by writing the Beat Size bit group in
    //       the Block Transfer Control register (BTCTRL.BEATSIZE)
    //     - The transfer descriptor must be made valid by writing a one to the Valid bit in the Block Transfer Control
    //       register (BTCTRL.VALID)
    //     - Number of beats in the block transfer must be selected by writing the Block Transfer Count(BTCNT)
    //       register
    //     - Source address for the block transfer must be selected by writing the Block Transfer Source Address
    //       (SRCADDR) register
    //     - Destination address for the block transfer must be selected by writing the Block Transfer Destination
    //       Address(DSTADDR) register
    mDesc[1].SRCADDR.reg = (u32)lpu8Src + lu16Count;
    mDesc[1].DSTADDR.reg = (u32)lpu8Dest + lu16Count;
    mDesc[1].BTCNT.reg = lu16Count;
    mDesc[1].DESCADDR.reg = (u32)&mDesc[2]; // The value must be 128 - bit aligned.

    mDesc[1].BTCTRL.bit.BLOCKACT = 0; // 0x0 = NOACT; 0x1 = INT; 0x2 = SUSPEND; 0x3 = BOTH
    mDesc[1].BTCTRL.bit.BEATSIZE = 0; // 1Byte
    mDesc[1].BTCTRL.bit.DSTINC = 1; // Destination Increment
    mDesc[1].BTCTRL.bit.SRCINC = 1; // Source      Increment
    mDesc[1].BTCTRL.bit.VALID = 1;

    // Before a DMA channel is enabled, the DMA channel and the corresponding first transfer descriptor must be
    // configured, as outlined by the following steps :
    //   - DMA channel configurations
    //     - The channel number of the DMA channel to configure must be written to the Channel ID(CHID) register
    //     - Trigger action must be selected by writing the Trigger Action bit group in the Channel Control B register
    //       (CHCTRLB.TRIGACT)
    //     - Trigger source must be selected by writing the Trigger Source bit group in the Channel Control B register
    //       (CHCTRLB.TRIGSRC)
    DMAC->CHID.reg = 1;
    DMAC->CHCTRLB.bit.TRIGACT = 3; // 0x0 BLOCK; 0x2 BEAT; 0x3 TRANSACTION
    DMAC->CHCTRLB.bit.TRIGSRC = 0; // SWTRIGGER
    DMAC->CHINTENSET.bit.TCMPL = 1;
    DMAC->CHCTRLA.bit.ENABLE = 1;
  }

  void vTrigger1()
  {
    DMAC->SWTRIGCTRL.reg = (1 << 1);
  }

  void vSetChannel2(u8* lpu8Src, u8* lpu8Dest, u16 lu16Count)
  {

    //   - Transfer Descriptor
    //     - The size of each access of the data transfer bus must be selected by writing the Beat Size bit group in
    //       the Block Transfer Control register (BTCTRL.BEATSIZE)
    //     - The transfer descriptor must be made valid by writing a one to the Valid bit in the Block Transfer Control
    //       register (BTCTRL.VALID)
    //     - Number of beats in the block transfer must be selected by writing the Block Transfer Count(BTCNT)
    //       register
    //     - Source address for the block transfer must be selected by writing the Block Transfer Source Address
    //       (SRCADDR) register
    //     - Destination address for the block transfer must be selected by writing the Block Transfer Destination
    //       Address(DSTADDR) register
    mDesc[2].SRCADDR.reg = (u32)lpu8Src + lu16Count;
    mDesc[2].DSTADDR.reg = (u32)lpu8Dest + lu16Count;
    mDesc[2].BTCNT.reg = lu16Count;
    mDesc[2].DESCADDR.reg = 0; // The value must be 128 - bit aligned.

    mDesc[2].BTCTRL.bit.BLOCKACT = 1; // 0x0 = NOACT; 0x1 = INT; 0x2 = SUSPEND; 0x3 = BOTH
    mDesc[2].BTCTRL.bit.BEATSIZE = 0; // 1Byte
    mDesc[2].BTCTRL.bit.DSTINC = 1; // Destination Increment
    mDesc[2].BTCTRL.bit.SRCINC = 1; // Source      Increment
    mDesc[2].BTCTRL.bit.VALID = 1;

    // Before a DMA channel is enabled, the DMA channel and the corresponding first transfer descriptor must be
    // configured, as outlined by the following steps :
    //   - DMA channel configurations
    //     - The channel number of the DMA channel to configure must be written to the Channel ID(CHID) register
    //     - Trigger action must be selected by writing the Trigger Action bit group in the Channel Control B register
    //       (CHCTRLB.TRIGACT)
    //     - Trigger source must be selected by writing the Trigger Source bit group in the Channel Control B register
    //       (CHCTRLB.TRIGSRC)
    DMAC->CHID.reg = 2;
    DMAC->CHCTRLB.bit.TRIGACT = 0; // 0x0 BLOCK; 0x2 BEAT; 0x3 TRANSACTION
    DMAC->CHCTRLB.bit.TRIGSRC = 0; // SWTRIGGER
    DMAC->CHINTENSET.bit.TCMPL = 1;
    DMAC->CHCTRLA.bit.ENABLE = 1;
  }

  void vTrigger2()
  {
    DMAC->SWTRIGCTRL.reg = (1 << 2);
  }

};

cDma mcDma;


u8 mau8Com0BufferTx[64];
u8 mau8Com0BufferRx[64];

u8 mu8_1000ms = 1;

void MAIN_vTick1000msLp(void)
{
  lcLedRed.Toggle();
  mu8_1000ms = 1;
}


void DMAC_Handler(void)
{
  if (DMAC->INTSTATUS.reg == (1 << 0))
  {
    DMAC->CHID.reg = 0;
    DMAC->CHINTFLAG.reg = (1 << 1); //Clear TCMPL
    DMAC->CHID.reg = 0;
    DMAC->CHCTRLA.bit.ENABLE   = 1;
  }
  else
  if (DMAC->INTSTATUS.reg == (1 << 1))
  {
    DMAC->CHID.reg = 1;
    DMAC->CHINTFLAG.reg = (1 << 1); //Clear TCMPL
  }
  else
  if (DMAC->INTSTATUS.reg == (1 << 2))
  {
    DMAC->CHID.reg = 2;
    DMAC->CHINTFLAG.reg = (1 << 1); //Clear TCMPL
  }

}

inline void vSERCOM0_Send()
{
}

inline void vSERCOM0_Send(char8 lchData)
{
}

inline void vSERCOM0_Send(const char8* lpchStr)
{
}

inline void vSERCOM0_Send(const u8* lpu8Data, u16 lu16Count)
{
}

void TC2_Handler(void)
{
  static u16 test = 0;
  REG_TC2_INTFLAG = TC_INTFLAG_OVF;

  test++;
  if (test == 1000)
  {
    test = 0;
    MAIN_vTick1000msLp();
  }
}


void Tim2_vInit()
{
  // Enable TC2,TC3 clock from GCLK1
  //REG_GCLK_PCHCTRL31 = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;

  // Configure asynchronous clock source
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC1_TC2;  // select TC2 peripheral channel
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;   // select source GCLK_GEN[0]
  GCLK->CLKCTRL.bit.CLKEN = 1;                   // enable TC2 generic clock

  // Configure synchronous bus clock
  PM->APBCSEL.bit.APBCDIV = 0;              // no prescaler
  PM->APBCMASK.bit.TC2_ = 1;                // enable TC2 interface

  // 8Mhz / (125 * 64) = 1ms
  REG_TC2_CTRLA = TC_CTRLA_MODE_COUNT8 | TC_CTRLA_PRESCALER_DIV64;
  REG_TC2_COUNT8_PER = 125;

  TC2->COUNT8.INTENSET.bit.OVF = 1; // set overflow interrupt
  NVIC_EnableIRQ(TC2_IRQn); // enable interrupts for TC2

  TC2->COUNT8.CTRLA.bit.ENABLE = 1; // enable TC2

  while (TC2->COUNT8.STATUS.bit.SYNCBUSY == 1); // wait for TC2 to be enabled
}


static void vSerComUsartInit(SercomUsart* lstCom)
{
  /* Get a pointer to the hardware module instance */
  SercomUsart* const lstSerCom = lstCom;

  // Configure asynchronous clock source
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_SERCOMX_SLOW;
  GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;        // select source GCLK_GEN[0]
  GCLK->CLKCTRL.bit.CLKEN = 1;                        // enable TC2 generic clock

  // Configure synchronous bus clock
  if (lstCom == &(SERCOM0->USART))
  {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_SERCOM0_CORE;
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;        // select source GCLK_GEN[0]
    GCLK->CLKCTRL.bit.CLKEN = 1;                        // enable TC2 generic clock

    PM->APBCMASK.bit.SERCOM0_ = 1;
  }

  if (lstCom == &(SERCOM1->USART))
  {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_SERCOM1_CORE;
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;        // select source GCLK_GEN[0]
    GCLK->CLKCTRL.bit.CLKEN = 1;                        // enable TC2 generic clock

    PM->APBCMASK.bit.SERCOM1_ = 1;
  }

  if (lstCom == &(SERCOM2->USART))
  {
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_SERCOM2_CORE;
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_GEN_GCLK0;        // select source GCLK_GEN[0]
    GCLK->CLKCTRL.bit.CLKEN = 1;                        // enable TC2 generic clock

    PM->APBCMASK.bit.SERCOM2_ = 1;
  }

  //PA6 => SerCom0/Pad[0] =>Tx => Output
  //PA7 => SerCom0/Pad[1] =>Rx => Input
  tcGpPin<(u32)&PORTA, 6> lcSercom0Tx(SYSTEM_PINMUX_PIN_DIR_OUTPUT, SYSTEM_PINMUX_PIN_PULL_NONE, SYSTEM_PINMUX_PIN_STRENGTH_NORMAL, 1);
  tcGpPin<(u32)&PORTA, 7> lcSercom0Rx(SYSTEM_PINMUX_PIN_DIR_INPUT, SYSTEM_PINMUX_PIN_PULL_NONE, SYSTEM_PINMUX_PIN_STRENGTH_NORMAL, 1);

  lcSercom0Tx.vSetAF(2); // 2 = C = Sercom
  lcSercom0Rx.vSetAF(2); // 2 = C = Sercom


  // Before the USART is enabled, it must be configured, as outlined in the following steps:
  // - USART mode with external or internal clock must be selected first by writing 0x0 or 0x1 to the Operating Mode
  //   bit group in the Control A register (CTRLA.MODE)
  lstSerCom->CTRLA.bit.MODE = SERCOM_USART_CTRLA_MODE_USART_INT_CLK_Val;

  // - Communication mode (asynchronous or synchronous) must be selected by writing to the Communication Mode
  //   bit in the Control A register (CTRLA.CMODE)
  lstSerCom->CTRLA.bit.CMODE = 0;

  // - SERCOM pad to use for the receiver must be selected by writing to the Receive Data Pinout bit group in the
  //   Control A register (CTRLA.RXPO)
  // - SERCOM pads to use for the transmitter and external clock must be selected by writing to the Transmit Data
  //   Pinout bit in the Control A register (CTRLA.TXPO)
  lstSerCom->CTRLA.bit.RXPO = 1; //PAD[1]
  lstSerCom->CTRLA.bit.TXPO = 0; //PAD[0]

  // - Character size must be selected by writing to the Character Size bit group in the Control B register
  //   (CTRLB.CHSIZE)
  lstSerCom->CTRLB.bit.CHSIZE = 0; //8 Bit

  // - MSB- or LSB-first data transmission must be selected by writing to the Data Order bit in the Control A register
  //   (CTRLA.DORD)
  lstSerCom->CTRLA.bit.DORD = 1;

  // - When parity mode is to be used, even or odd parity must be selected by writing to the Parity Mode bit in the
  //   Control B register (CTRLB.PMODE) and enabled by writing 0x1 to the Frame Format bit group in the Control A
  //   register (CTRLA.FORM)
  //lstSerCom->CTRLA.bit.FORM = 0;

  // - Number of stop bits must be selected by writing to the Stop Bit Mode bit in the Control B register
  //   (CTRLB.SBMODE)
  //lstSerCom->CTRLB.bit.SBMODE = 0;

  // - When using an internal clock, the Baud register (BAUD) must be written to generate the desired baud rate
  //lstSerCom->CTRLA.bit.SAMPR = 0; // 16x over-sampling using arithmetic baud rate generation.
  //lstSerCom->CTRLA.bit.SAMPR = 2; //  8x over-sampling using arithmetic baud rate generation.
  //lstSerCom->BAUD.bit.BAUD = 13; // 1Mhz / 8 / 13 = 9616 baud

  // 1Mhz  / (62.5 * 16) = 1ms = 1000baud
  //lstSerCom->CTRLA.bit.SAMPR = 1; // 16x over-sampling using fractional baud rate generation.
  // 1Mhz  / (62.5 * 8) = 1ms = 2000baud
  lstSerCom->CTRLA.bit.SAMPR = 3; //  8x over-sampling using fractional baud rate generation.
  //lstSerCom->BAUD.FRACFP.FP = 0; lstSerCom->BAUD.FRACFP.BAUD = 13; // 1Mhz / (13 * 8) = 9615 Baud
  lstSerCom->BAUD.FRACFP.FP = 1; lstSerCom->BAUD.FRACFP.BAUD = 104;  // 8Mhz / (104,125 * 8) = 9603 Baud



  // Configure synchronous bus clock
  /*if (lstCom == &(SERCOM0->USART))
  {
    NVIC_EnableIRQ(SERCOM0_IRQn);
  }

  if (lstCom == &(SERCOM1->USART))
  {
    NVIC_EnableIRQ(SERCOM1_IRQn);
  }

  if (lstCom == &(SERCOM2->USART))
  {
    NVIC_EnableIRQ(SERCOM2_IRQn);
  }*/

  lstSerCom->INTENSET.bit.RXC = 1;

  // - The transmitter and receiver can be enabled by writing ones to the Receiver Enable and Transmitter Enable
  //   bits in the Control B register (CTRLB.RXEN and CTRLB.TXEN
  lstSerCom->CTRLB.bit.RXEN = 1;
  lstSerCom->CTRLB.bit.TXEN = 1;
  lstSerCom->CTRLA.bit.ENABLE = 1;
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  vSysTickInit();

  vSerComUsartInit(mstCom);

  mcDma.vSetChannel0((u8*)(&SERCOM0->USART.DATA.reg), mau8Com0BufferRx, 10);
  mcDma.vSetChannel1((u8*)TestSource1, (u8*)TestDest1, 4);
  mcDma.vSetChannel2((u8*)TestSource2, (u8*)TestDest2, 4);

  mcDma.vTrigger1();

  Tim2_vInit();
}

int main(void)
{
  MAIN_vInitSystem();

  const char8 string[] = "\r\n--Hello World!--";
  char8 string2[] = "\r\n-- --";

  while (1)
  {
    if (mu8_1000ms)
    {
      mu8_1000ms = 0;
      vSERCOM0_Send((const char8*)string);
    }

    /*if (mxCom0BufRx.cnt())
    {
      volatile u8 lu8TxData = mxCom0BufRx.get();
      string2[4] = lu8TxData;
      vSERCOM0_Send((const char8*)string2);
    }*/

    //CycCall_vIdle();
    __asm("wfi");
  }
}



void SystemInit_HSI_1Mhz(void)
{
  // Keep the default device state after reset
  // On any reset the synchronous clocks start to their initial state :
  // - OSC8M is enabled and divided by 8
  // - GCLK_MAIN uses OSC8M as source
  // - CPU and BUS clocks are undivided
  SystemCoreClock = 8000000 / 8;
}

void SystemInit_HSI_4Mhz(void)
{
  SYSCTRL->OSC8M.bit.PRESC = 1;
  SystemCoreClock = 8000000 / 4;
}


void SystemInit_HSI_8Mhz(void)
{
  SYSCTRL->OSC8M.bit.PRESC = 0;
  SystemCoreClock = 8000000 / 1;
}



/* This is done before the initialize the C/C++ library */
void PreMain(void)
{
  /* Change default QOS values to have the best performance and correct USB behaviour */
  SBMATRIX->SFR[SBMATRIX_SLAVE_HMCRAMC0].reg = 2;
  DMAC->QOSCTRL.bit.DQOS = 2;
  DMAC->QOSCTRL.bit.FQOS = 2;
  DMAC->QOSCTRL.bit.WRBQOS = 2;

  /* Overwriting the default value of the NVMCTRL.CTRLB.MANW bit (errata reference 13134) */
  NVMCTRL->CTRLB.bit.MANW = 1;

  SystemInit_HSI_8Mhz(); // SysClk = 1Mhz
}

