
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
// PA05: Ntc:            ADC1: ADC_IN5/DAC2_OUT
// Pc02: Poti:           ADC1: ADC_IN12                 0.5V => 0.5V =>  620d / 0x026c
// PA04: Mot Strom:      ADC2: ADC_IN4  4x Verstärkung  0.5V => 2.0V => 2482d / 0x09b2
// DMA1.Chl1 für ADC getriggert von TIM1_CH1
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
  LED<GPIOA, 10>    mcPA10;
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

void vLedOff()
{
  GPIOB->BCR = (1 << 15);
}

cServo1_Applikation mcMotor;


class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:

  void vSendStatus(cBotNet* lpcBn, u16 lu16DestAdress)
  {
    cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgStatusMsg, 24, lpcBn->mcAdr.Get(), lu16DestAdress, 0x90);

    u8* lpu8Data;
    lpu8Data = (u8*)lcMsgStatusMsg.mcPayload.mpu8Data; 
    
    // Idx
    MemWriteu16(lpu8Data, (i16)mcMotor.i16GetPosDegree());     lpu8Data += 2; // 0, 1
    MemWriteu16(lpu8Data, (i16)mcMotor.u16GetSupplyVoltage()); lpu8Data += 2; // 2, 3
    MemWriteu16(lpu8Data, (i16)mcMotor.i16GetCurrent());       lpu8Data += 2; // 4, 5
    MemWriteu16(lpu8Data, (i16)mcMotor.i16GetExtTemp());       lpu8Data += 2; // 6, 7
    MemWriteu8(lpu8Data,   (u8)mcMotor.mStatus.u8Status);                     // 8
    lcMsgStatusMsg.mcPayload.muiLen = 9;


    lcMsgStatusMsg.vEncode();
    lpcBn->bSendMsg(&lcMsgStatusMsg);
  }

  bool bMsg(cBotNetMsg_MsgProt &lcMsg, cBotNet* lpcBn)
  {
    switch (lcMsg.mu16Idx)
    {
      case 80: // i16 Sollpos
        {
          i16 li16PosSoll;
          //i16* lpi16Strom;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            li16PosSoll = (i16)MemReadu16(lcMsg.mcPayload.mpu8Data);
            mcMotor.vSetMotMode_ServoPos();
            mcMotor.vSetPosDegree(li16PosSoll);

            if (li16PosSoll <= 0)  li16PosSoll-=li16PosSoll;
            if (li16PosSoll > 360)
            {
              mcMotor.vSetMotDisable();
            }
            else
            {
              mcMotor.vSetMotEnable();
            }
            vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
          }
        }
        break;
      case 81: // i16 Sollpos + i16Strombegrenzung
      {
        i16  li16PosSoll;
        i16  li16PosCur;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          mcMotor.vSetMotMode_ServoPosCur();
          li16PosSoll = (i16)MemReadu16(lcMsg.mcPayload.mpu8Data);
          li16PosCur  = (i16)MemReadu16((u8*)(lcMsg.mcPayload.mpu8Data)+2);
          mcMotor.vSetPosDegree(li16PosSoll);
          mcMotor.vSetCur_mA(li16PosCur);

          if (li16PosSoll <= 0)  li16PosSoll -= li16PosSoll;
          if ((li16PosSoll > 360) && (li16PosCur > 4))
          {
            mcMotor.vSetMotDisable();
          }
          else
          {
            mcMotor.vSetMotEnable();
          }
          vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
        }
      }
      break;
      case 84: // i16 Sollpwm
        {
          i16 li16PwmSoll;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            li16PwmSoll = (i16)MemReadu16(lcMsg.mcPayload.mpu8Data);
            mcMotor.vSetMotMode_MotorPwm();
            mcMotor.vSetSpeed(li16PwmSoll);
            if (li16PwmSoll != 0)
            {
              mcMotor.vSetMotEnable();
            }
            else
            {
              mcMotor.vSetMotDisable();
            }
            vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
          }
        }
        break;
      case 85: // Motor mit i16Strombegrenzung
      {
        i16 li16CurSoll;
        //i16* lpi16Strom;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          li16CurSoll = (i16)MemReadu16(lcMsg.mcPayload.mpu8Data);
          mcMotor.vSetMotMode_MotorCur();
          mcMotor.vSetCur_mA(li16CurSoll);
          if (li16CurSoll != 0)
          {
            mcMotor.vSetMotEnable();
          }
          else
          {
            mcMotor.vSetMotDisable();
          }
          vSendStatus(lpcBn, lcMsg.cGetSAdr().Get());
        }
      }
      break;
      case 90: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
          i32* lpi32Para;
          if (lcMsg.mcPayload.muiLen >= 3*4)
          {
            lpi32Para = (i32*)lcMsg.mcPayload.mpu8Data;

            mcMotor.mcMyPidPos.mFpKp.vSetFrac(MemReadu32(lpi32Para)); lpi32Para++;
            mcMotor.mcMyPidPos.mFpKi.vSetFrac(MemReadu32(lpi32Para)); lpi32Para++;
            mcMotor.mcMyPidPos.mFpKd.vSetFrac(MemReadu32(lpi32Para));
          }
        }
        break;
      case 95: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
          i32* lpi32Para;
          if (lcMsg.mcPayload.muiLen >= 3*4)
          {
            lpi32Para = (i32*)lcMsg.mcPayload.mpu8Data;

            mcMotor.mcMyPidCur.mFpKp.vSetFrac(MemReadu32(lpi32Para)); lpi32Para++;
            mcMotor.mcMyPidCur.mFpKi.vSetFrac(MemReadu32(lpi32Para)); lpi32Para++;
            mcMotor.mcMyPidCur.mFpKd.vSetFrac(MemReadu32(lpi32Para));
          }
        }
      break;
      default:
      break;
    }
    return True;
  }

};


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);

cBn_MsgProcess mcBn_MsgProcess;


cUartMpHdSlave mcUartMpHdU3;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);
cBotNet_UpLinkUsartMpHd_RMsg mcUpLink(&mcUartMpHdU3);

cBotNet mcBn(&mcMyBotNetCfg, &mcBn_MsgProcess);


class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (lcParam.Len() != 0)
      {
        i16 li16Val = lcParam.Atoi();
        mcMotor.vSetMotMode_MotorPwm();
        mcMotor.vSetSpeed(li16Val);
        if (li16Val != 0) mcMotor.vSetMotEnable();
                    else  mcMotor.vSetMotDisable();
      }
      else
      {
        lszStr.Setf("A: %d", mcMotor.mSpeed_Soll);
        lcCli->bPrintLn(lszStr);
      }
      return True;
    }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      char8 lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      if (lcParam.Len() != 0)
      {
        i16 li16Val = lcParam.Atoi();
        mcMotor.vSetMotMode_ServoPos();
        mcMotor.vSetPosDegree(li16Val);
        mcMotor.vSetMotEnable();
      }
      else
      {
        lszStr.Setf("A: %d", mcMotor.i16GetPosDegree());
        lcCli->bPrintLn(lszStr);
      }
      return True;
    }
};

class cCliCmd_Status: public cCliCmd
{
  public:
    cCliCmd_Status():cCliCmd((const char*)"?", (const char*)"status")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lszStr.Setf((const char8*)"P: %d", mcMotor.i16GetPosFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"U: %d", mcMotor.i16GetVoltFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I: %d", mcMotor.i16GetCurrentFilteredDegree());
      lcCli->bPrintLn(lszStr);
      //lszStr.Setf((const char8*)"Ti: %d", mcMotor.i16GetIntTemp());
      //lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Te: %d", mcMotor.i16GetTempFilteredDegree());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"PF: %d\r\n", mcMotor.mPowerFailCounter);
      lcCli->bPrintLn(lszStr);

      for (u8 lu8t=0; lu8t < mcMotor.nChnCount; lu8t++)
      {
        lszStr.Setf((const char8*)"%d: %d", lu8t + 1, mcMotor.mAdcResult[lu8t]);
        lcCli->bPrintLn(lszStr);
      }

      return True;
    }
};


class cBotNetMotCli
{
  public:
  cCliCmd_SetPwm  mcCliCmd_SetPwm;
  cCliCmd_SetPos  mcCliCmd_SetPos;
  cCliCmd_Status  mcCliCmd_Status;

  cBotNetMotCli()
  {
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPwm);
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPos);
    mcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
  }
};

cBotNetMotCli mcBnMotCli;


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


u32 MAIN_u32Counter_1ms = 1;
u32 MAIN_u32Counter_1ms_old = 1;

// Wird alle 1ms aufgerufen
void SysTick_Handler(void)
{
  if (SysTick->SR == 1)
  {
    SysTick->SR = 0; //clear State flag
    #ifdef PCB_PROJECTCFG_Test
      mcPC4.vSet1();
    #endif

    MAIN_u32Counter_1ms++;
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

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(0, 0xE000, 0);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  mcBn.vStreamPortConnect(1, 0xE000, 1);

  /*cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg, 16, 0x1100, 0xE000, 0x50);

  u8 t;

  for (t=0;t<16;t++)
  {
    lcMsgTestMsg.mcPayload[t] = t;
  }
  lcMsgTestMsg.vEncode();
  mcBn.mcUpLink.mStatus.IsOnline = 1;
  mcBn.bSendMsg(&lcMsgTestMsg);*/

  vInitSysTick();
}


int main(void)
{
  u8 lu8LedBlink;

  cBnSpop_vResetWdog();

  if (!RomConst_IsValid())
  {
    // RomConst Error
    mcBn.mcAdr.Set(0x1EEE);
    lu8LedBlink = 0x0F;
  }
  else
  {
    lu8LedBlink = 0x01;
  }

  MAIN_vInitSystem();

  #ifndef TEST_BnLinkUsartMpHd
    u8 lu8LedEnable = mcMotor.u8GetLedEnable();
  #endif

  while (1)
  {
    if (MAIN_u32Counter_1ms_old != MAIN_u32Counter_1ms)
    {
      MAIN_u32Counter_1ms_old = MAIN_u32Counter_1ms;
      mcMotor.vTick1ms();

      if ((MAIN_u32Counter_1ms % 10) == 0)
      {
        mcBn.vTickLp10ms();
      }

      #ifndef TEST_BnLinkUsartMpHd
        // Im Testmodues wird die Led als Testpin benutzt
        if ((MAIN_u32Counter_1ms % 125) == 0)
        {
          //static i16 li16Pos = 30;
          //mcMotor.vSetPosDegree(li16Pos);
          //li16Pos = -li16Pos;

          if (lu8LedEnable)
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
          }
          else
          {
            vLedOff();
          }
        }
      #endif
    }

    mcBn.vProcess10ms();

    cBnSpop_vResetWdog();

    //__asm("wfi");
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



