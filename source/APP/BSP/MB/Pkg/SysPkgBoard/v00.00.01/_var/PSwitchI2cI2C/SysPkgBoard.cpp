
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
  : lcDigi1(GPIOA_BASE, 1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, lu8InitState)
{
}


void cDigiPower::vPowerOn()
{
  lcDigi1.vSet1();
}

void cDigiPower::vPowerOff()
{
  lcDigi1.vSet0();
}




cSysPkgBoard::cSysPkgBoard()
  : mcDigiPower(1),
    mcStatusLed(False),
    mcI2c3_SCL_Board(GPIOA_BASE, 7, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c3_SDA_Board(GPIOB_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C3_Board(I2C3, &mcI2c3_SCL_Board, &mcI2c3_SDA_Board, DMA1, DMA1_Channel3, DMA1_Channel2, 8),
    mcEep(&mcI2C3_Board, 0xA0),
    mcBoardCntrl(&mcI2C3_Board)
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

  if (mcI2C3_Board.mSm == cComNode::enStError)
  {
    lszErrorInfo += (rsz)" ErrI2cBoard";
    lbRet = True;
  }
  else
  {
    if (mcBoardCntrl.mu8InaErr & 1)
    {
      lszErrorInfo += (rsz)" ErrI2cBoardIna1";
      lbRet = True;
    }

    if (mcBoardCntrl.mu8InaErr & 2)
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
    lbRet = mcBoardCntrl.bPowerDown(lcStatus);
  }

  return lbRet;
}


void cSysPkgBoard::vTick1msHp(void)
{
}

void cSysPkgBoard::vTick1msLp(void)
{
  mcI2C3_Board.bStartNext();
}

void cSysPkgBoard::vTick10msLp(void)
{
  mcBoardCntrl.vUpdateAll(mcSys.mcSMan.mcOpMode.IsAuto());

  mcI2C3_Board.vTick10ms();
  mcEep.vTick10ms();
}

void cSysPkgBoard::vTick100msLp(void)
{
  cStatusLedMode::tenMode lenModeLed1; // LED1 ist die Led auf dem uC Board PB09

  lenModeLed1 = cStatusLedMode::nStPuls1000ms;
  
  if (!mcSys.mcSMan.mcOpMode.IsAuto())
  {
    lenModeLed1 = cStatusLedMode::nStPuls500ms;
  }
    
  mcStatusLed.vSetMode(lenModeLed1);
  mcStatusLed.vTick100ms();
}

void cSysPkgBoard::vTick1000msLp(void)
{
}


// --------------------- I2C2 ------------------------------------------

void I2C3_EV_IRQHandler(void)
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTI2C2IRQ
  ////    u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
  ////    u32 lu32TimEnd; mu8IntLvl++;
  ////    mcPA05.vSet1();
  ////  #endif
  ////#endif

  mcSys.mcBoard.mcI2C3_Board.I2C_EV_IRQHandler();

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

void I2C3_ER_IRQHandler(void)
{
 //// #ifdef PCB_PROJECTCFG_Test
 ////   #ifdef TESTI2C2IRQ
 ////     u32 lu32TimStart = cDiffTimerHw::u32GetTimer();
 ////     u32 lu32TimEnd; mu8IntLvl++;
 ////     mcPA05.vSet1();
 ////   #endif
 //// #endif

  mcSys.mcBoard.mcI2C3_Board.I2C_ER_IRQHandler();

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


