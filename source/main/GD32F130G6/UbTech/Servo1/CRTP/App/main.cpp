#include "main.h"

// GD32F130G6
// ARM®-based Cortex®-M3 32b MCU, 8 Mhz (72 MHz max)
// Rom 32KB
//   - FBL  12k: 0x0800 0000 - 0x0800 2FFF
//   - APP  19k: 0x0800 3000 - 0x0800 7BFF
//   - DATA  1k: 0x0800 7C00 - 0x0800 7FFF
// Ram  4KB
// TIM1(32Bit)
// TIM0, TIM2, TIM13, TIM15, TIM16

// TIM0: Motor PWM(Ch0) und ADC (CH1),
// TIM2: System Tick
// TIM13:
// TIM15: optional (TimeStampArray)
// TIM16: Uart Timeout


// Servo:
//
// PB1: LED:          Tim2_Ch3(AF1); Tim13_Ch0(AF0); Tim0_Ch2_On(AF2)
//
// PA9: Usart Tx:     USART0_TX(AF1)
//
// PA8: Mot Enable:   Tim0_Ch0(AF2)
// PA7: Poti:         ADC_IN7
// PA6: Mot Fault:    -
// PA5: Mot Break:    Tim1_Ch0(AF2)
// PA4: Mot Sleep:    -
// PA3: Mot Phase:    Tim1_Ch3(AF2), Tim14_Ch1(AF0)
// PA2: Mot Strom:    ADC_IN2
// PA1: 7V:           ADC_IN1
// PA0: Mot Spannung: ADC_IN0


//#define PCB_PROJECTCFG_Test

#ifdef PCB_PROJECTCFG_Test
  LED<GPIOA, 10>    mcPA10;
#endif


cServo1_Applikation mcMotor;


cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);


cUartMpHdSlave gcUartMpHdU0;
//cBotNet_UpLinkUsartMpHd mcUpLink(&mcUartMpHdU0);

cBotNet_UpLinkUsartMpHdNoCheck gcUpLink(&gcUartMpHdU0);

cBotNet gcMainBn(&mcMyBotNetCfg);

//cBotNetMsgPortBtr   gcBtr(&gcMainBn);
cBotNetMsgPortSpop  gcSpop(&gcMainBn);
cBotNetMsgPortRRpt  gcRRpt(&gcMainBn);

class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:
    cBn_MsgProcess()
      : cBotNet_MsgSysProcess(&gcMainBn)
    {
    }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    u8* lu8RxPayload = lcMsg.GetPayload().mpu8Data;
    switch (lcMsg.u16GetIdx())
    {
      // System
      case 8: // Request message
        switch (lu8RxPayload[0])
        {
          case 0: // System: Identification
            // TX 00 | 00 | 10 | RR RR DI DI HW HW RR RR BT : RR: Reserve = 0; DI Device Idx; HW: Hardware Version; BT Board Type
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] == 10))
            {
              u8 lu8Data[12];

              lu8Data[ 0] =  0; // R1
              lu8Data[ 1] =  0; // S1
              lu8Data[ 2] = 10; // S2

              // Reserve
              lu8Data[ 3] = 0;
              lu8Data[ 4] = 0;
              // HV: HW Version
              lu8Data[ 5] = u16GetRomConstBnDeviceID() >> 8;
              lu8Data[ 6] = u16GetRomConstBnDeviceID() & 0xFF;;

              // HV: HW Version
              lu8Data[ 7] = u16GetRomConstHwInfo() >> 8;
              lu8Data[ 8] = u16GetRomConstHwInfo() & 0xFF;;
              // SV: SW Version
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              // BT Board Type
              lu8Data[11] = 0; //u8GetRomConstBoardType();

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x09, lu8Data, sizeof(lu8Data));

              return True;
            }
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] == 16))
            {
              u8 lu8Data[5];

              lu8Data[ 0] =  0; // R1
              lu8Data[ 1] =  0; // S1
              lu8Data[ 2] = 16; // S2

              lu8Data[ 3] = cSystem::mAutomatik;
              lu8Data[ 4] = cSystem::mLed;

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x09, lu8Data, sizeof(lu8Data));
              return True;
            }
            break;
         }
         break;

      // MotCntl
      case 32: // Request message
        switch (lu8RxPayload[0])
        {
          case 1:
            // MotCntl: Status1
            // TX 01 | 02 | 01 | SS.PP.PP
            if ((lu8RxPayload[1] == 2) && (lu8RxPayload[2] == 1))
            {
              u8 lu8Data[3];

              lu8Data[ 0] = mcMotor.mStatus.u8Status;
              lu8Data[ 1] = (u8)(mcMotor.i16GetPosFiltered_Grad() >> 8);
              lu8Data[ 2] = (u8)(mcMotor.i16GetPosFiltered_Grad());

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 40, lu8Data, sizeof(lu8Data));
              return True;
            }
            // MotCntl: Status2
            // TX 01 | 02 | 02 | SS.DD.DD.VV.VV.PW
            if ((lu8RxPayload[1] == 2) && (lu8RxPayload[2] == 2))
            {
              u8 lu8Data[6];

              lu8Data[ 0] = mcMotor.mStatus.u8Status;
              // Distance
              lu8Data[ 1] = (u8)(mcMotor.mi16Dist >> 8);
              lu8Data[ 2] = (u8)(mcMotor.mi16Dist);
              // Speed
              lu8Data[ 3] = (u8)(0 >> 8);
              lu8Data[ 4] = (u8)(0);
              // PWM
              lu8Data[ 5] = (u8)mcMotor.u8GetPwm_Percent();

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 41, lu8Data, sizeof(lu8Data));
              return True;
            }
            // MotCntl: Status3
            // TX 01 | 02 | 03 | UU.UU.II.II.TT
            if ((lu8RxPayload[1] == 2) && (lu8RxPayload[2] == 3))
            {
              u8 lu8Data[5];

              // Distance
              lu8Data[ 0] = (u8)(mcMotor.i16GetVoltFiltered_mV() >> 8);
              lu8Data[ 1] = (u8)(mcMotor.i16GetVoltFiltered_mV());
              // Speed
              lu8Data[ 2] = (u8)(mcMotor.i16GetCurrentFiltered_mA() >> 8);
              lu8Data[ 3] = (u8)(mcMotor.i16GetCurrentFiltered_mA());
              // Temp
              i16 li16Temp = mcMotor.i168GetTempFilteredDegree();
              li16Temp += 64;
              if (li16Temp < 0) li16Temp = 0;
              if (li16Temp > 255) li16Temp = 255;
              lu8Data[ 4] = (u8)li16Temp;

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 42, lu8Data, sizeof(lu8Data));

              return True;
            }
            // MotCntl: Status4
            // TX 01 | 02 | 04 | II.II.PO.PO.TT
            if ((lu8RxPayload[1] == 2) && (lu8RxPayload[2] == 4))
            {
              u8 lu8Data[5];

              // Limit Current
              lu8Data[ 0] = (u8)(mcMotor.u16Get_SetLimCur_mA() >> 8);
              lu8Data[ 1] = (u8)(mcMotor.u16Get_SetLimCur_mA());
              // Limit Power
              lu8Data[ 2] = (u8)(mcMotor.u16Get_SetLimPow_mW() >> 8);
              lu8Data[ 3] = (u8)(mcMotor.u16Get_SetLimPow_mW());
              // Limit Temp
              i16 li16Temp = mcMotor.i16Get_SetLimTemp_Grad();
              li16Temp += 64;
              if (li16Temp < 0) li16Temp = 0;
              if (li16Temp > 255) li16Temp = 255;
              lu8Data[ 4] = (u8)li16Temp;

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 43, lu8Data, sizeof(lu8Data));

              return True;
            }
            // MotCntl: Cfg: Lp
            // TX 01 | 00 | xx | LP.LP.LP.LP                 // XX: LP Index; LP:
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] <= 7))
            {
              u8 lu8Data[7];

              lu8Data[ 0] =  1; // R1
              lu8Data[ 1] =  0; // S1
              lu8Data[ 2] = lu8RxPayload[2]; // S2

              cLowPassT<cFixPti2210>* lcLP = &mcMotor.mcLp[0];

              lcLP = &mcMotor.mcLp[lu8RxPayload[2]];
              lcLP->u8Encode(&lu8Data[3]);

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 33, lu8Data, sizeof(lu8Data));
              return True;
            }

            // MotCntl: Cfg: Pid
            // TX 01 | 01 | xx | PP.PP.PP.PP.II.II.II.II.DD.DD.DD.DD.IL.IL.IL.IL.
            if ((lu8RxPayload[1] == 1) && (lu8RxPayload[2] <= 3))
            {
              u8 lu8Data[19];

              lu8Data[ 0] =  1; // R1
              lu8Data[ 1] =  1; // S1
              lu8Data[ 2] = lu8RxPayload[2]; // S2

              cPidT<cFixPti2210>* lcPid = &mcMotor.mcPid[0];

              lcPid = &mcMotor.mcPid[lu8RxPayload[2]];

              lcPid->u8Encode(&lu8Data[3]);

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x21, lu8Data, sizeof(lu8Data));
              return True;
            }
            break;
         }
         break;

      // MotCntl
      case 34: // Set message
        switch (lu8RxPayload[0])
        {
          case 1:
            // MotCntl: Cfg: Lp
            // RX 01 | 00 | xx | LP.LP.LP.LP
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] <= 7))
            {
              u8* lu8Data = lcMsg.mcPayload.mpu8Data;

              cLowPassT<cFixPti2210>* lcLP = &mcMotor.mcLp[0];

              lcLP = &mcMotor.mcLp[lu8RxPayload[2]];
              lcLP->u8Decode(&lu8Data[3]);
              return True;
            }

            // MotCntl: Cfg: Pid
            // RX 01 | 01 | xx | PP.PP.PP.PP.II.II.II.II.DD.DD.DD.DD.IL.IL.IL.IL.
            if ((lu8RxPayload[1] == 1) && (lu8RxPayload[2] <= 3))
            {
              u8* lu8Data = lcMsg.GetPayload().mpu8Data;

              cPidT<cFixPti2210>* lcPid = &mcMotor.mcPid[0];

              lcPid = &mcMotor.mcPid[lu8RxPayload[2]];
              lcPid->u8Decode(&lu8Data[3]);
              return True;
            }
            break;
         }
         break;
      break;

      case 50: // MotCntl : Mode RX MM
        {
          mcMotor.vSetMotMode(lu8RxPayload[0]);
          return True;
        }
        break;

      case 51: // MotCntl: Pos RX PP.PP
        {
          i16 li16PosSoll = (i16)((lu8RxPayload[0] << 8) + lu8RxPayload[1]);
          mcMotor.vSetPos_Grad(li16PosSoll);
          return True;
        }
        break;

      case 52: // MotCntl: Speed RX DD.DD.VV.VV.PW
        {
          // Distance
          //i16 li16DistSoll = (i16)((lu8RxPayload[0] << 8) + lu8RxPayload[1]);
          //mcMotor.vSetDist_Grad(li16DistSoll);

          // Speed
          i16 li16SpeedSoll = (i16)((lu8RxPayload[2] << 8) + lu8RxPayload[3]);
          mcMotor.vSetSpeed_Grads(li16SpeedSoll);

          mcMotor.vSetPwm_Percent((i8)lu8RxPayload[4]);
          return True;
        }
        break;

      case 53: // MotCntl: Limits RX II.II.PO.PO.TT
        {
          // Current [mA]
          i16 li16LimCur = (i16)((lu8RxPayload[0] << 8) + lu8RxPayload[1]);
          mcMotor.vSetLimCur_mA(li16LimCur);

          // Power [W]
          i16 li16LimPow = (i16)((lu8RxPayload[2] << 8) + lu8RxPayload[3]);
          mcMotor.vSetLimPow_W(li16LimPow);


          i16 li16LimTemp = (i16)((i16)lu8RxPayload[4] - 64);
          mcMotor.vSetLimTemp_degree(li16LimTemp);
          return True;
        }
        break;

      default:
        break;
    }
    return False;
  }
};

cBn_MsgProcess gcBnMsgProcess;



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

      lszStr.Setf((const char8*)"P: %d", mcMotor.i16GetPosFiltered_Grad());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"U: %d", mcMotor.i16GetVoltFiltered_mV());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"I: %d", mcMotor.i16GetCurrentFiltered_mA());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Ti: %d", mcMotor.i16GetIntTemp_Grad());
      lcCli->bPrintLn(lszStr);
      lszStr.Setf((const char8*)"Te: %d", mcMotor.i168GetTempFilteredDegree());
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
  cCliCmd_Status  mcCliCmd_Status;

  cBotNetMotCli()
  {
    gcMainBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
  }
};

cBotNetMotCli mcBnMotCli;

cCliCmd_Status    mcCliCmd_Status;


cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_Status},  1);


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
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
    vDtwStart();
  #endif
  // USART1 TX
  // Ender der Übertragung wird durch  U0.TC interrupt erledigt
  // Tx DMA Interrupt wird nicht benötigt
  // if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1))
  // {
  //  #ifdef TEST_BnLinkUsartMpHd
  //    cTimeStampArray::vSetTimeStamp(25);
  //    //gpio_bit_reset(GPIOB, GPIO_PIN_1);
  //  #endif
  //   dma_channel_disable(DMA_CH1);
  //   DMA_INTC = DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH1);
  //   mcUartMpHdU0.ComIrqHandler(cComNode::tenEvent::enEvTyIrq, cComNode::tenEvent::enEvDmaTxTc);
  // }

  // USART1 RX
  if (DMA_INTF & DMA_FLAG_ADD(DMA_INTF_FTFIF, DMA_CH2))
  {
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vReset();
      cTimeStampArray::vSetTimeStamp(0);
      //gpio_bit_set(GPIOB, GPIO_PIN_1);
    #endif
    gcUartMpHdU0.IrqHandler(cComNode::tenEvent::enEvDmaRxTc);
    #ifdef TEST_BnLinkUsartMpHd
      cTimeStampArray::vSetTimeStamp(1);
    #endif

  }
  #ifdef PCB_PROJECTCFG_Test
    __asm volatile ("nop");
    mcPA10.vSet0();
  #endif
}

void USART0_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (USART_STAT(USART0) & USART_STAT_TC)
  {
    gcUartMpHdU0.IrqHandler(cComNode::tenEvent::enEvUsartTc);
  }
  else
  {
    USART_INTC(USART0) = 0xFFFF;
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}

void TIMER16_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (timer_interrupt_flag_get(TIMER16, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER16, TIMER_INT_FLAG_UP);
    timer_disable(TIMER16);
    gcUartMpHdU0.TIM_EV_IRQHandler();
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}


u16 MAIN_u16Counter_250ms;
u16 MAIN_u16Counter_250ms_old;

void TIMER2_IRQHandler(void)
{
  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet1();
  #endif

  if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
  {
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);

    MAIN_u16Counter_250ms++;
  }

  #ifdef PCB_PROJECTCFG_Test
    mcPA10.vSet0();
  #endif
}


//static void system_clock_16m_irc8m(void)
//{
//  /* AHB = SYSCLK */
//  //RCU_CFG0 |= RCU_AHB_CKSYS_DIV1;
//  /* APB2 = AHB/2 */
//  //RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
//  /* APB1 = AHB/2 */
//  //RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;
//  /* PLL = (IRC8M/2) *  4 = 16 MHz */
//  RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF | RCU_CFG0_PLLMF4 | RCU_CFG0_PLLPREDV);
//  RCU_CFG1 &= ~(RCU_CFG1_PLLPRESEL | RCU_CFG1_PLLMF5 | RCU_CFG1_PREDV);
//  RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2 | (RCU_PLL_MUL4 & (~RCU_CFG1_PLLMF5)));
//  RCU_CFG1 |= (RCU_PLL_MUL18 & RCU_CFG1_PLLMF5);
//  /* enable PLL */
//  RCU_CTL0 |= RCU_CTL0_PLLEN;
//
//  /* wait until PLL is stable */
//  while (0U == (RCU_CTL0 & RCU_CTL0_PLLSTB)) {}
//
//  /* select PLL as system clock */
//  RCU_CFG0 &= ~RCU_CFG0_SCS;
//  RCU_CFG0 |= RCU_CKSYSSRC_PLL;
//
//  /* wait until PLL is selected as system clock */
//  while (0U == (RCU_CFG0 & RCU_SCSS_PLL)) {}
//}


void vInitTim2()
{
  rcu_periph_clock_enable(RCU_TIMER2);

  MAIN_u16Counter_250ms     = 0;
  MAIN_u16Counter_250ms_old = 0;


  /*timer_parameter_struct timer_initpara;
  // TIMER configuration
  timer_initpara.prescaler = (cClockInfo::u32GetClockSys() / 1000000) - 1;;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.period = 1000 - 1; // 1ms
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);

  // auto-reload preload enable
  timer_auto_reload_shadow_enable(TIMER2);
  // auto-reload preload enable
  timer_enable(TIMER2);*/


  // Das Gleiche wie oben 1ms-Timer, Autoreload, @8Mhz
  // Braucht aber so 90Byte weniger
  TIMER_DMAINTEN(TIMER2) = 1;
  TIMER_PSC(TIMER2)      = 23;    //  23 für 24Mhz, 15 für 16Mhz und 7 für 8Mhz
  TIMER_CAR(TIMER2)      = 0x3E7;
  TIMER_CTL0(TIMER2)     = 0x081;

  /*NVIC config*/
  nvic_irq_enable(TIMER2_IRQn, 1, 1);
}


void MAIN_vInitSystem(void)
{
  //SystemInit();
  //vSysTickInit();

  cSystem::vInit();
  cBnMsgPool::vInit();

  // Add MsgSys
  //gcBtr.vAddMsgSys();
  gcSpop.vAddMsgSys();
  gcRRpt.vAddMsgSys();

  // Add Uplink
  gcMainBn.bAddLink((cBotNet_LinkBase*)&gcUpLink);

  gcBnMsgProcess.vAddMsgSys();

  gcMainBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);

  /*cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgTestMsg, 16, 0x1100, 0xE000, 0x50);

  u8 t;

  for (t=0;t<16;t++)
  {
    lcMsgTestMsg.mcPayload[t] = t;
  }
  lcMsgTestMsg.vEncode();
  mcMainBn.mcUpLink.mStatus.IsOnline = 1;
  mcMainBn.bSendMsg(&lcMsgTestMsg);*/

  vInitTim2();
}


int main(void)
{
  fwdgt_counter_reload();
  MAIN_vInitSystem();

  while (1)
  {
    if (MAIN_u16Counter_250ms_old != MAIN_u16Counter_250ms)
    {
      MAIN_u16Counter_250ms_old = MAIN_u16Counter_250ms;
      mcMotor.vTick1ms();
      gcMainBn.vProcess(1000);
      gcUartMpHdU0.vTick1ms();
    }

    // Im Testmodues wird die Led als Testpin benutzt
    if (MAIN_u16Counter_250ms >= 250)
    {
      #ifndef TEST_BnLinkUsartMpHd
        cSystem::vLedTick250ms();
      #endif
      MAIN_u16Counter_250ms = 0;
    }

    fwdgt_counter_reload();

    __asm("wfi");
  }
}



void MainSystemInit(void)
{
  /* FPU settings */
  #if (__FPU_PRESENT == 1U) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
  #endif

  /* configure system clock */
  // Externe 24Mhz
  rcu_osci_on(RCU_HXTAL);
  while (RESET == rcu_flag_get(RCU_FLAG_HXTALSTB));
  rcu_system_clock_source_config(RCU_CKSYSSRC_HXTAL);

  #ifdef  VECT_TAB_OFFSET
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, VECT_TAB_OFFSET);
  #endif
}
