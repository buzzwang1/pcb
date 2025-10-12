
#ifndef __SYS_PKG_BOARD_H__
#define __SYS_PKG_BOARD_H__

#include "TypeDef.h"
#include "tGPPin.h"
#include "LED.h"
#include "cI2c.h"
#include "cPidFloat.h"
#include "cPidFloat.h"
#include "cStrT.h"

#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dac.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

#include "Eep.h"
#include "ina3221.h"
#include "MP8859.h"

#include "RomConst.h"

class cStatusLedMode
{
  public:
  typedef enum
  {
    nStToggle100ms,
    nStToggle200ms,
    nStToggle500ms,
    nStToggle1000ms,
    nStPuls500ms,
    nStPuls1000ms,
    nStPuls5000ms,
    nStSinglePuls1000ms,
    nStSinglePuls2000ms,
    nStSinglePuls3000ms,
    nStOn,
    nStOff,
  }tenMode;
};

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class cStatusLed : public LED<mpstPort, mui16Pin>, public cStatusLedMode
{
  public:
  tenMode mSm;
  u8      mCounter;

  cStatusLed(bool lbInverse);
  void vSetMode(cStatusLedMode::tenMode lSm);
  void vTick100ms();
};


class cDigiPower
{
  public:

  enum
  {
    nPin1   = 10,
    nPinLed = 6
  };

  //Digi Power
  cGpPin lcDigi1;
  cStatusLed<GPIOA_BASE, nPinLed> lcDigiLed;

  cDigiPower(u8 lu8InitState = 0);

  void vSetLedMode(cStatusLedMode::tenMode lSm);
  void vLedOn3000ms();
  void vLedOff();
  void vPowerOn();
  void vPowerOff();
  void vTick_100ms();
};

class cPmSwitch
{
public:
  //Slot0:
  // PA7 Enable

  //Slot1:
  // PB08 Enable

  cPmSwitch(u8 lu8Slot)
  {
    if (lu8Slot == 0)
    {
      cGpPin mEnable(GPIOA_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
    }
    else
    {
      cGpPin mEnable(GPIOB_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
    }
  }  
};



class cSysPkgBoard
{
  public:

  /*Test*/
  cGpPin mcWakeupPin;

  /*PA8 im Testmode für Interrupt Timing */
  cStatusLed<GPIOB_BASE, 9> mcStatusLed;

  cDigiPower           mcDigiPower; // DigiPower (3V3) vor I2C init, sonst error in I2C_Init.CheckPins

  // I2C2:
  cGpPin mcI2c2_SCL_Board;
  cGpPin mcI2c2_SDA_Board;

  cI2cMaster           mcI2C2_Board;
  cI2cEep              mcEep;

  cPmSwitch            mcSwitch;

  cSysPkgBoard();

  void vInit1(void);
  void vInit2();

  bool isError(cStr& lszErrorInfo);
  bool isReadyForSleep(cStr& lcStatus);

  bool isWakeUp();

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};



#ifdef __cplusplus
  extern "C" {
#endif

extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif // __BSP___SYS_PKG_BASE_MBv10b2_BOARD_H__
