
#include "SysPkgBoard.h"

#include "System.h"


template <const uintptr_t mpstPort, const uint16 mui16Pin>
cStatusLed<mpstPort, mui16Pin>::cStatusLed(bool lbInverse)
  : LED<mpstPort, mui16Pin>()
{
  mSm = nStToggle500ms;
  mCounter = 0;
  this->mInverse = lbInverse;
};

template <const uintptr_t mpstPort, const uint16 mui16Pin>
void cStatusLed<mpstPort, mui16Pin>::vSetMode(cStatusLedMode::tenMode lSm)
{
  if (mSm != lSm) mCounter = 0;
  mSm = lSm;
}

template <const uintptr_t mpstPort, const uint16 mui16Pin>
void cStatusLed<mpstPort, mui16Pin>::vTick100ms()
{
  switch (mSm)
  {
    case nStToggle100ms:
      this->Toggle();
      break;
    case nStToggle200ms:
      if (mCounter & 1)
      {
        this->Toggle();
      }
      break;
    case nStToggle500ms:
      if ((mCounter % 5) == 0)
      {
        this->Toggle();
      }
      break;
    case nStToggle1000ms:
      if ((mCounter % 10) == 0)
      {
        this->Toggle();
      }
      break;
    case nStPuls500ms:
      if ((mCounter % 5) == 0)
      {
        this->On();
      }
      else
      {
        this->Off();
      }
      break;
    case nStPuls1000ms:
      if ((mCounter % 10) == 0)
      {
        this->On();
      }
      else
      {
        this->Off();
      }
      break;
    case nStPuls5000ms:
      if (mCounter == 0)
      {
        this->On();
      }
      else
      {
        this->Off();
      }
      break;
    case nStOn:
      this->On();
      break;
    case nStOff:
      this->Off();
      break;

    case nStSinglePuls1000ms:
      if (mCounter < 10)
      {
        this->On();
      }
      else
      {
        vSetMode(nStOff);
      }
      return;
      break;
    case nStSinglePuls2000ms:
      if (mCounter < 20)
      {
        this->On();
      }
      else
      {
        vSetMode(nStOff);
      }
      return;
      break;
    case nStSinglePuls3000ms:
      if (mCounter < 30)
      {
        this->On();
      }
      else
      {
        vSetMode(nStOff);
      }
      return;
      break;
  }

  if (mCounter < 49)
  {
    mCounter++;
  }
  else
  {
    mCounter = 0;
  }
}


cDigiPower::cDigiPower(u8 lu8InitState)
  : lcDigi1(GPIOA_BASE, nPin1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, lu8InitState),
    lcDigiLed(False)
{
}


void cDigiPower::vSetLedMode(cStatusLedMode::tenMode lSm)
{
  lcDigiLed.vSetMode(lSm);
}

void cDigiPower::vLedOn3000ms()
{
  vSetLedMode(cStatusLedMode::tenMode::nStSinglePuls3000ms);
}

void cDigiPower::vLedOff()
{
  vSetLedMode(cStatusLedMode::tenMode::nStOff);
}

void cDigiPower::vPowerOn()
{
  GPIOA->BSRR = (1 << nPin1);
  vLedOn3000ms();
}

void cDigiPower::vPowerOff()
{
  vLedOff();
  GPIOA->BRR = (1 << nPin1);
}

void cDigiPower::vTick_100ms()
{
  lcDigiLed.vTick100ms();
}



cSysPkgBoard::cSysPkgBoard()
  : mcWakeup(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcStatusLed(False),
    mcDigiPower(1),
    mcI2c2_SCL_Board(GPIOB_BASE, 10, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c2_SDA_Board(GPIOB_BASE, 14, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, DMA1, DMA1_Channel5, DMA1_Channel4, 8),
    mcEep(&mcI2C2_Board, 0xA0, 0),  // Vorerst keine Daten einlesen, die Daten werden später im System.Init eingelese
                                    // Den vollen EEPROM (1024 Byte) würde sonst 30ms dauern
    mcLipoMon(&mcI2C2_Board)
{
}

void cSysPkgBoard::vInit1(void)
{
  mcDigiPower.vPowerOn();
}

void cSysPkgBoard::vInit2()
{
}


bool cSysPkgBoard::isError(cStr& lszErrorInfo)
{
  bool lbRet = False;

  if (mcI2C2_Board.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2cBoard";
    lbRet = True;
  }
  else
  {
    if (mcLipoMon.mu8InaErr & 1)
    {
      lszErrorInfo += (rsz)" ErrI2cBoardIna1";
      lbRet = True;
    }

    if (mcLipoMon.mu8InaErr & 2)
    {
      lszErrorInfo += (rsz)" ErrI2cBoardIna2";
      lbRet = True;
    }
  }

  return lbRet;
}


bool cSysPkgBoard::isReadyForSleep(cStr& lcStatus)
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  // Warten bis EEPROM fertig ist
  if (mcEep.IsInit())
  {
    if (mcEep.isBusy())
    {
      lbRet = False;
      lszStrBuf.Setf((rsz)"Eep");
      if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
      lcStatus += lszStrBuf;
    }
  }


  if (lbRet)
  {
    lbRet = mcLipoMon.bPowerDown(lcStatus);
  }

  return lbRet;
}


void cSysPkgBoard::vTick1msHp(void)
{
}

void cSysPkgBoard::vTick1msLp(void)
{
  mcI2C2_Board.bStartNext();
}

void cSysPkgBoard::vTick10msLp(void)
{
  mcLipoMon.vUpdateAll_10ms(mcSys.mcSMan.mcOpMode.IsAuto());

  if (mcSys.mcSMan.mcPowerManager.enSysState == cPowerManager::tenSysStates::nStRun)
  {
    mcI2C2_Board.vTick10ms();

    if (mcSys.mcCom.mcBn.mcSpop.mcEep != null)
    {
      mcEep.vTick10ms();
    }
  }
}

void cSysPkgBoard::vTick100msLp(void)
{
  cStatusLedMode::tenMode lenModeLed1; // LED1 ist die Led auf dem uC Board PB09
  cStatusLedMode::tenMode lenModeLed2; // LED2 sind die 4 Status Leds auf dem Mini-Board

  lenModeLed1 = cStatusLedMode::nStPuls5000ms;
  lenModeLed2 = cStatusLedMode::nStOff;

    
  if (mcLipoMon.isExternalSupplyAvailable())
  {
    lenModeLed1 = cStatusLedMode::nStToggle1000ms;
    lenModeLed2 = cStatusLedMode::nStOn;
  
    switch (mcLipoMon.lenSmLipo)
    {
      case cLipoMon::enSmStNotCharging:
      case cLipoMon::enSmStNotCharging_PowerDown:
        lenModeLed1 = cStatusLedMode::nStToggle1000ms;
        lenModeLed2 = cStatusLedMode::nStOn;
        break;
      case cLipoMon::enSmStNotCharging_Error_Ina:
        lenModeLed1 = cStatusLedMode::nStToggle200ms;
        lenModeLed2 = cStatusLedMode::nStToggle100ms;
        break;
      case cLipoMon::enSmStNotCharging_Error_LowInputVoltage:
        lenModeLed1 = cStatusLedMode::nStToggle200ms;
        lenModeLed2 = cStatusLedMode::nStToggle200ms;
        break;
      case cLipoMon::enSmStNotCharging_Error_CellsDefect:
        lenModeLed1 = cStatusLedMode::nStToggle100ms;
        lenModeLed2 = cStatusLedMode::nStToggle100ms;
        break;
      case cLipoMon::enSmStChargingSave:
        lenModeLed1 = cStatusLedMode::nStToggle1000ms;
        lenModeLed2 = cStatusLedMode::nStPuls1000ms;
        break;
      case cLipoMon::enSmStChargingNormal:
        lenModeLed1 = cStatusLedMode::nStToggle1000ms;
        lenModeLed2 = cStatusLedMode::nStPuls500ms;
        break;
      case cLipoMon::enSmStCharged:
        lenModeLed1 = cStatusLedMode::nStToggle1000ms;
        lenModeLed2 = cStatusLedMode::nStOn;
        break;
    }
  }
    
  if (!mcSys.mcSMan.mcOpMode.IsAuto())
  {
    lenModeLed1 = cStatusLedMode::nStToggle200ms;
  }
  
  if (mcLipoMon.mTestMode)
  {
    lenModeLed2 = cStatusLedMode::nStOn;
  }
    
  mcStatusLed.vSetMode(lenModeLed1);
  mcDigiPower.vSetLedMode(lenModeLed2);
    
  mcStatusLed.vTick100ms();
  mcDigiPower.vTick_100ms();
    
}

void cSysPkgBoard::vTick1000msLp(void)
{
}


// --------------------- I2C2 ------------------------------------------

void I2C2_EV_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C2IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcBoard.mcI2C2_Board.I2C_EV_IRQHandler();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C2IRQ
  ////    mcPA05.vSet0();
  ////    lu32TimEnd = cDiffTimerHw::u32GetTimer();
  ////    if (lu32TimEnd > lu32TimStart)
  ////    {
  ////      mcTestClassMaxCyc[2].vSetMaxTimer(lu32TimEnd - lu32TimStart);
  ////    }
  ////    mcTestClassMaxCyc[2].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
  ////  #endif
  ////#endif
}

void I2C2_ER_IRQHandler(void)
{
 //// #ifdef PCB_PROJECTCFG_Test
 ////   #ifdef TESTI2C2IRQ
 ////     u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
 ////     u32 lu32TimEnd; mu8IntLvl++;
 ////     mcPA05.vSet1();
 ////   #endif
 //// #endif

  mcSys.mcBoard.mcI2C2_Board.I2C_ER_IRQHandler();

 //// #ifdef PCB_PROJECTCFG_Test
 ////   #ifdef TESTI2C2IRQ
 ////     mcPA05.vSet0();
 ////     lu32TimEnd = cDiffTimerHw::u32GetTimer();
 ////     if (lu32TimEnd > lu32TimStart)
 ////     {
 ////       mcTestClassMaxCyc[3].vSetMaxTimer(lu32TimEnd - lu32TimStart);
 ////     }
 ////     mcTestClassMaxCyc[3].vSetMaxIntLvl(mu8IntLvl); mu8IntLvl--;
 ////   #endif
 //// #endif
}


