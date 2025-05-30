#include "main.h"

// GD32F330G8
// ARM�-based Cortex�-M4 32b MCU, (80 MHz max)
// Rom 64KB
// Ram  8KB

//   - FBL  16k: 0x0800 0000 - 0x0800 3FFF
//   - APP  19k: 0x0800 4000 - 0x0800 FBFF
//   - DATA  1k: 0x0800 FC00 - 0x0800 FFFF
// Ram  8KB
// 
// TIM0, TIM1(32Bit), TIM2, TIM13, TIM14, TIM15, TIM16

// TIM0: -
// TIM1: Motor PWM(Ch0) und ADC (TIMER1_CH0)
// TIM2: Motor PWM(Ch0) und ADC (TIMER2_TRGO)
// TIM13: SysTick
// TIM14: -
// TIM15: optional (TimeStampArray)
// TIM16: Uart Timeout


// Servo:
//
// 
// PA00: LED             Tim0_Ch0(AF2)
//
// PA09: I2C0_SCL/U0_Tx: I2C0_SCL(AF4)/USART0_TX(AF1)
// PA10: I2C0_SDA:       I2C0_SCL(AF4)
//
// PA04: Ntc:            ADC_IN4
// 
// PB01: Poti:           ADC_IN9
// PA02: Mot Strom:      ADC_IN2
// 
// PA01: Mot Sleep:      1 = enable H-Bridge, 0 = disable H-Bridge
// PA05: Mot IN1:        TIMER1_CH0(AF2), ADC_Trigger(TIMER1_CH0)
// PA06: Mot IN2:        TIMER2_CH0(AF1), ADC_Trigger(TIMER2_TRGO)



// 128, 2000 0660       : UpLink_ComBuf.Rx
// 128, 2000 06E8       : UpLink_ComBuf.Tx
//   2, 2000 0770:      : UpLink Sync.Tx
//   3, 2000 0778:   +8 : UpLink Sync.Rx
//  65, 2000 0788:  +16 : UpLink Data.Tx
//  65, 2000 07d0:  +72 : UpLink Data.Rx
//   1, 2000 0820:  +72 : UpLink Ack.Tx
//   1, 2000 0828:   +8 : UpLink Ack.Rx
// 128, 2000 0838       : StreamSys_ComBuf.Rx
// 128, 2000 08C0       : StreamSys_ComBuf.Tx
//  64, 2000 0948       : StreamSys_Msg.Rx
//  64, 2000 0990       : StreamSys_Msg.Tx
//                      : cCliHistory
//                      : cCliHistoryData
//                      : cCliCmdList
//  64, 2000 0A48:      : cBotnetMsgRx
//  64, 2000 0A90:      : cBotnetMsgTx
//  --  2000 0AD8:      : -----------
//
// 1144 Byte
//
//
// Sram:  2048
// Heap:  1536
// Stack:  512
//
//
//
// 2000 0000
// 2000 0658 Heapbase
// 2000 0D00 Stackbase
// 2000 0FFF


LED<GPIOA, 0>       mcLedRed;


cServo1_Applikation mcMotor;


class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:

  void vSendStatus(cBotNet* lpcBn, u16 lu16DestAdress)
  {
    cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgStatusMsg, 24, lpcBn->mcAdr.Get(), lu16DestAdress, 0x90);

    i16* lpi16Data;
    lpi16Data = (i16*)lcMsgStatusMsg.mcPayload.mpu8Data;

                                                       // Idx
    *lpi16Data++ = (i16)mcMotor.i16GetPosDegree();     // 0, 1
    *lpi16Data++ = (i16)mcMotor.u16GetSupplyVoltage(); // 2, 3
    *lpi16Data++ = (i16)mcMotor.i16GetCurrent();       // 4, 5
    *lpi16Data++ = (i16)mcMotor.i16GetExtTemp();       // 6, 7
    *(u8*)lpi16Data = (u8)mcMotor.mStatus.u8Status;       // 8
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
          i16* lpi16PosSoll;
          //i16* lpi16Strom;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            lpi16PosSoll = (i16*)lcMsg.mcPayload.mpu8Data;
            mcMotor.vSetMotMode_ServoPos();
            mcMotor.vSetPosDegree(*lpi16PosSoll);
            
            if (*lpi16PosSoll <= 0)  *lpi16PosSoll-=*lpi16PosSoll;
            if (*lpi16PosSoll > 360)
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
        i16* lpi16Payload;
        i16  lpi16PosSoll;
        i16  lpi16PosCur;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          mcMotor.vSetMotMode_ServoPosCur();
          lpi16Payload = (i16*)lcMsg.mcPayload.mpu8Data;
          lpi16PosSoll = *lpi16Payload++;
          lpi16PosCur  = *lpi16Payload;
          mcMotor.vSetPosDegree(lpi16PosSoll);
          mcMotor.vSetCur_mA(lpi16PosCur);
          
          if (lpi16PosSoll <= 0)  lpi16PosSoll -= lpi16PosSoll;
          if ((lpi16PosSoll > 360) && (lpi16PosCur > 4))
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
          i16* lpi16PwmSoll;
          //i16* lpi16Strom;
          if (lcMsg.mcPayload.muiLen >= 2)
          {
            lpi16PwmSoll = (i16*)lcMsg.mcPayload.mpu8Data;
            mcMotor.vSetMotMode_MotorPwm();
            mcMotor.vSetSpeed(*lpi16PwmSoll);
            if (*lpi16PwmSoll != 0)
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
        i16* lpi16CurSoll;
        //i16* lpi16Strom;
        if (lcMsg.mcPayload.muiLen >= 2)
        {
          lpi16CurSoll = (i16*)lcMsg.mcPayload.mpu8Data;
          mcMotor.vSetMotMode_MotorCur();
          mcMotor.vSetCur_mA(*lpi16CurSoll);
          if (*lpi16CurSoll != 0)
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
          
            mcMotor.mcMyPidPos.mFpKp.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidPos.mFpKi.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidPos.mFpKd.vSetFrac(*lpi32Para++);
          }
        }
        break;
      case 95: // Fp1814 Kp, Fp1814 Ki, Fp1814 Kd
        {
          i32* lpi32Para;
          if (lcMsg.mcPayload.muiLen >= 3*4)
          {
            lpi32Para = (i32*)lcMsg.mcPayload.mpu8Data;
          
            mcMotor.mcMyPidCur.mFpKp.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidCur.mFpKi.vSetFrac(*lpi32Para++);
            mcMotor.mcMyPidCur.mFpKd.vSetFrac(*lpi32Para++);
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


cUartMpHdSlave mcUartMpHdU0;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);
cBotNet_UpLinkUsartMpHd_RMsg mcUpLink(&mcUartMpHdU0);

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

      cStr_Create(lszStr, 32);

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

      cStr_Create(lszStr, 32);

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
      cStr_Create(lszStr, 32);

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


/*static volatile u32 mu32SysTick_Delay;

void vSysTickInit(void)
{
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    while (1);
  }
  NVIC_SetPriority(SysTick_IRQn, 0x00);
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
}*/


void SysTick_Handler(void)
{
  //vDelay_Decrement();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


void DMA_Channel1_2_IRQHandler(void)
{
  // USART1 TX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(25);
      //gpio_bit_reset(GPIOB, GPIO_PIN_1);
    #endif
    dma_channel_disable(DMA_CH1);
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
    mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vReset();
      cTimeStampArray::vSetTimeStamp(0);
      //gpio_bit_set(GPIOB, GPIO_PIN_1);
    #endif

    DMA_CHCTL(DMA_CH2) &= ~DMA_CHXCTL_CHEN;
    DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2);
    mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvDmaRxTc);

    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(1);
    #endif
  }
}

void USART0_IRQHandler(void)
{
  if (USART_CTL0(USART0) & USART_CTL0_UEN)
  {
    if (USART_STAT(USART0) & USART_STAT_TC)
    {
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(26);
      #endif

      USART_INTC(USART0) = USART_STAT_TC;
      mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvUsartTc);
      #ifdef TEST_BnLinkUsartMpHd
        cTimeStampArray::vSetTimeStamp(27);
      #endif
    }

    if (USART_STAT(USART0) & 0xF)
    {
      USART_INTC(USART0) = USART_STAT_ORERR;
      mcUartMpHdU0.ComIrqHandler(cComNode::tenEventType::enEvTyError, cComNode::tenEvent::enEvUsartErOre);
    }
  }
  else
  {
    //usart_deinit(USART0);
    mcUartMpHdU0.vInitHw();
  }
}

void TIMER16_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER16, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER16, TIMER_INT_FLAG_UP);
    timer_disable(TIMER16);
    mcUartMpHdU0.TIM_EV_IRQHandler();
  }
}


u32 MAIN_u32Counter_10ms;
u32 MAIN_u32Counter_1000ms;
u32 MAIN_u32Counter_1000ms_old;

void TIMER13_IRQHandler(void)
{
  if (timer_interrupt_flag_get(TIMER13, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);

    MAIN_u32Counter_10ms++;
    MAIN_u32Counter_1000ms++;
    mcBn.vTickHp1ms();
  }
}


void vInitTim13()
{
  rcu_periph_clock_enable(RCU_TIMER13);

  MAIN_u32Counter_10ms       = 0;
  MAIN_u32Counter_1000ms     = 0;
  MAIN_u32Counter_1000ms_old = 0;


  /*timer_parameter_struct timer_initpara;
  // TIMER configuration
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER13, &timer_initpara);
  timer_interrupt_enable(TIMER13, TIMER_INT_UP);

  // auto-reload preload enable
  timer_auto_reload_shadow_enable(TIMER13);
  // auto-reload preload enable
  timer_enable(TIMER13);*/


  // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
  // Braucht aber so 90Byte weniger
  TIMER_DMAINTEN(TIMER13) = 1;
  TIMER_PSC(TIMER13)      = 7;
  TIMER_CAR(TIMER13)      = 0x3E7;
  TIMER_CTL0(TIMER13)     = 0x081;

  /*NVIC config*/
  nvic_irq_enable(TIMER13_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

  // Add Uplink
  mcBn.bAddLink((cBotNet_LinkBase*)&mcUpLink);

  // Connect the CmdPort's output to external Port (to PC CmdPort 0xE000.0)
  mcBn.vStreamPortConnect(0, 0xE000, 0);

  // Add MemPort and connect the MemPort's output to external Port  (to PC CmdPort 0xE000.1)
  // Should be Index 1
  mcBn.vStreamPortConnect(1, 0xE000, 1);

  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg, 16, 0x1100, 0xE000, 0x50);

  /*u8 t;

  for (t = 0; t<16; t++)
  {
    lcMsgTestMsg.mcPayload[t] = t;
  }
  lcMsgTestMsg.vEncode();
  mcBn.mcUpLink.mStatus.IsOnline = 1;
  mcBn.bSendMsg(&lcMsgTestMsg);*/

  vInitTim13();
}


int main(void)
{
  fwdgt_counter_reload();
  MAIN_vInitSystem();

  #ifndef TEST_BnLinkUsartMpHd
    u8 lu8LedEnable = mcMotor.u8GetLedEnable();
  #endif

  while (1)
  {
    if (MAIN_u32Counter_1000ms_old != MAIN_u32Counter_1000ms)
    {
      MAIN_u32Counter_1000ms_old = MAIN_u32Counter_1000ms;
      mcMotor.vTick1ms();
    }

    if (MAIN_u32Counter_10ms >= 10)
    {
      mcBn.vTickLp10ms();
      MAIN_u32Counter_10ms = 0;
    }

    #ifndef TEST_BnLinkUsartMpHd
      // Im Testmodues wird die Led als Testpin benutzt
      if (MAIN_u32Counter_1000ms >= 1000)
      {
        //static i16 li16Pos = 30;
        //mcMotor.vSetPosDegree(li16Pos);
        //li16Pos = -li16Pos;

        if (lu8LedEnable) mcLedRed.Toggle();
                     else mcLedRed.Off();
        MAIN_u32Counter_1000ms = 0;
      }
    #endif

    fwdgt_counter_reload();

    __asm("wfi");
  }
}

