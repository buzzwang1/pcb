#include "cSysStageS5.h"
#include "cSysStages.h"


cStage5::cStage5()
  : mcWakeupPin(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcStatusLed(False),
    mcI2c2_SCL_Board(GPIOB_BASE, 10, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2c2_SDA_Board(GPIOB_BASE, 14, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
    mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 1, 8),
    mcBoardCntr(&mcI2C2_Board)
{
  mbInitBoard2Finished = False;
}


void cStage5::vDoInitBoard2(cStages* lpcStages)
{
  UNUSED(lpcStages);
  mbInitBoard2Finished = True;
}

void cStage5::vDoRunBoard2(cStages* lpcStages)
{
  static u8 lu8Tick100ms = 0;

  UNUSED(lpcStages);
  lpcStages->mcS1.mcSysTick.vDoProcess();
  lpcStages->mcS5.mcBoardCntr.vUpdateAll_10ms(False);

  if (lu8Tick100ms == 100)
  {
    mcStatusLed.vTick100ms();
    lu8Tick100ms = 0;
  }
  
  lu8Tick100ms += 10;
  osDelay(10);
}


// --------------------- I2C2 ------------------------------------------

void I2C2_EV_IRQHandler(void)
{
  //mcSys.mcBoard.mcI2C2_Board.I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  //mcSys.mcBoard.mcI2C2_Board.I2C_ER_IRQHandler();
}




