
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
u8           mstComTxStatus = 0;


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


class cRingBuf
{
public:
  u8* mtyBuf;
  u16  mtySize;
  u16  mtyHead;
  u16  mtyCnt;
  u16  mtyTail;

  inline void inc(u16& lVal)
  {
    lVal++;
    if (lVal >= mtySize) lVal = 0;
  }

  inline void dec(u16& lVal)
  {
    if (lVal == 0) lVal = mtySize;
    lVal--;
  }

  cRingBuf(const u8* lptyBuf, const u16 lu16Size)
  {
    reset();
    mtySize = lu16Size;
    mtyBuf = (u8*)lptyBuf;
  }

  inline void reset()
  {
    mtyHead = mtyTail = mtyCnt = 0;
  }

  inline bool isEmpty()
  {
    return mtyCnt == 0;
  }

  inline u16 cnt()
  {
    return mtyCnt;
  }

  inline u16 space_left()
  {
    return (mtySize - mtyCnt);
  }

  inline bool isFull()
  {
    return mtyCnt == mtySize;
  }

  inline void put(u8 lu8Item)
  {
    _dai();
    if (mtyCnt < mtySize)
    {
      mtyBuf[mtyHead] = lu8Item;
      inc(mtyHead);
      mtyCnt++;
    }
    _eai();
  }

  inline u8 get(void)
  {
    u8 ltyZwerg;
    _dai();

    if (mtyCnt > 0)
    {
      ltyZwerg = mtyBuf[mtyTail];
      inc(mtyTail);
      mtyCnt--;
    }
    else
    {
      ltyZwerg = 0;
    }
    _eai();
    return ltyZwerg;
  }
};


u8 mau8Com0BufferTx[64];
u8 mau8Com0BufferRx[64];
cRingBuf mxCom0BufTx(mau8Com0BufferTx, 64);
cRingBuf mxCom0BufRx(mau8Com0BufferRx, 64);

u8 mu8_1000ms = 1;

void MAIN_vTick1000msLp(void)
{
  lcLedRed.Toggle();
  mu8_1000ms = 1;
}


void SERCOM0_Handler(void)
{
  if (mstCom->INTFLAG.bit.RXC)
  {
    volatile u8 lu8RxData = mstCom->DATA.bit.DATA;
    mxCom0BufRx.put(lu8RxData);
    lcLedRed.Toggle();
  }
  else
    if (mstCom->INTFLAG.bit.TXC)
    {
      if (mxCom0BufTx.cnt())
      {
        volatile u8 lu8TxData = mxCom0BufTx.get();
        mstCom->DATA.bit.DATA = lu8TxData;
      }
      else
      {
        REG_SERCOM0_USART_INTENCLR = 2; // Clear TXC
        mstComTxStatus = 0;
        mstCom->INTFLAG.bit.TXC = 1; //Writing a one to this bit will clear the flag.
      }
    }
}

inline void vSERCOM0_Send()
{
  if (mstComTxStatus == 0)
  {
    mstCom->INTENSET.bit.TXC = 1;
    mstComTxStatus = 1;
    mstCom->DATA.bit.DATA = mxCom0BufTx.get();
  }
}

inline void vSERCOM0_Send(char8 lchData)
{
  mxCom0BufTx.put(lchData);
  vSERCOM0_Send();
}

inline void vSERCOM0_Send(const char8* lpchStr)
{
  while (*lpchStr != 0)
  {
    mxCom0BufTx.put(*lpchStr++);
  }
  vSERCOM0_Send();
}

inline void vSERCOM0_Send(const u8* lpu8Data, u16 lu16Count)
{
  while (lu16Count)
  {
    mxCom0BufTx.put(*lpu8Data++);
    lu16Count--;
  }
  vSERCOM0_Send();
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
  if (lstCom == &(SERCOM0->USART))
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
  }

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

    if (mxCom0BufRx.cnt())
    {
      volatile u8 lu8TxData = mxCom0BufRx.get();
      string2[4] = lu8TxData;
      vSERCOM0_Send((const char8*)string2);
    }

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

