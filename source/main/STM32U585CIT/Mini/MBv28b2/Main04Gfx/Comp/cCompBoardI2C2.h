#pragma once

#include "cSysDPool.h"

class cCompBoardI2C2 : public cComponent
{
  public:

  cGpPin     mcI2c2_SCL_Board;
  cGpPin     mcI2c2_SDA_Board;
  cI2cMaster mcI2C2_Board;

  cCompBoardI2C2()
    : cComponent(cDepTreeCfg::cComp::nBoardI2C2, cDepTreeCfg::cComp::nBoard3V3),
      mcI2c2_SCL_Board(GPIOB_BASE, 13),
      mcI2c2_SDA_Board(GPIOB_BASE, 14),
      mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 1, 400000, 0) // InitDelay == 0, weil in bInit manuell initialisiert wird
  {
    cSysDPool::mBoard.mcI2c = &mcI2C2_Board;
  }

  bool bInit() override
  {
    mcI2c2_SCL_Board.vInit(GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mcI2c2_SDA_Board.vInit(GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mcI2C2_Board.vInit();

    if (mcI2C2_Board.mError != cComNode::tenError::enNoError)
    {
      this->mFlags.stFlags.Error = True;
    }

    cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    mcI2C2_Board.vTick1ms();
    return cComponent::bRun();
  };

  bool isError(cStr& lszErrorInfo)
  {
    bool lbRet = False;

    if (mcI2C2_Board.mSm == cComNode::enStError)
    {
      lszErrorInfo += (rsz)" ErrI2cBoard";
      lbRet = True;
    }
    return lbRet;
  }
};


#ifdef __cplusplus
extern "C" {
#endif

extern void I2C2_EV_IRQHandler(void);
extern void I2C2_ER_IRQHandler(void);

void I2C2_EV_IRQHandler(void)
{
  cSysDPool::mBoard.mcI2c->I2C_EV_IRQHandler();
}

void I2C2_ER_IRQHandler(void)
{
  cSysDPool::mBoard.mcI2c->I2C_ER_IRQHandler();
}

#ifdef __cplusplus
}
#endif
