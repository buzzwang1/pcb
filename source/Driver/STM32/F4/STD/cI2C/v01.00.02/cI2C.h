#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "tGPPin.h"
#include "ComDat.h"
#include "ClockInfo.h"


#define I2C_DIRECTION_TX 0
#define I2C_DIRECTION_RX 1


class cComNodeI2c_Slave : public cComNode
{
public:
  cComDatMsg mpcMsg;

  cComNodeI2c_Slave(uint32 lui32TxDataSize, uint32 lui32RxDataSize)
  {
    mpcMsg.vMemAlloc(lui32TxDataSize, lui32RxDataSize);
  };

  //~cComNodeI2c_Slave() {};

  inline uint32 SizeOf()
  {
    return mpcMsg.SizeOf() -
           sizeof(mpcMsg) +
           sizeof(cComNodeI2c_Slave);
  }
};


class cI2cMaster_ComNodeList :public cComNodeList
{
  public:
    cI2cMaster_ComNodeList(uint32 lui32SlavesCnt);

  inline uint32 SizeOf()
  {
    return cComNodeList::SizeOf() + 
           sizeof(cI2cMaster_ComNodeList);
  }
};

class cI2c
{
  public:

  typedef enum
  {
    enStIdle  = 0,
    enStTx,
    enStRx,
    enStError
  }tencI2c_States;

  typedef enum
  {
    enEvStart  = 0,
    enEvIrq,
    enEvDma,
    enEvError
  }tencI2c_Events;

  typedef enum
  {
    enNop  = 0,
    enResetHwFull,
    enResetHwDma
  }tencI2c_Consts;

  tencI2c_States     mSm;
  tenComDeviceStatus munDeviceStatus;
  tenComBusStatus    munBusStatus;
  tenComStatus       munComStatus;

  cGpPin *mPinSCL;
  cGpPin *mPinSDA;

  I2C_TypeDef   *mI2C;

  uint8 mui8Adr1;
  uint8 mui8Adr2;

  cI2c(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda);

  void  vSetAdr2(u8 lui8Adr2);
  void  vInitHw(bool lbMaster);
  void  vReInitHw(tencI2c_Consts lenOption, bool lbMaster);

  void I2C_StopDMA();
  void I2C_StartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction);

  bool bCheckBus();
  bool bCheckPins();

  uint8 isErrorActive(bool lbMaster);
};


class cI2cMaster : public cI2c
{
  public:

  uint16             mu16ReInitTicks;
  uint16             mu16ComStuckTicks;
  uint16             mu16ComStuckTicksReload;

  uint32               mui32DataIdx;


  cI2cMaster_ComNodeList mcSlaves;
  cComNode              *mpcActiveSlave;
  cComDatMsg            *mpcActiveMsg;

  cComNode::tenComNodeState  menComState;
  uint8           *mpui8ComBuf;
  uint32           mui32ComByteCnt;

  cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint32 lui32Slaves);
  ~cI2cMaster();

  void I2C_EV_DMAHandler();

  inline uint32 SizeOf()
  {
    return mcSlaves.SizeOf() + sizeof(cI2cMaster);
  }

  void I2C_EV_IRQHandler();
  void I2C_ER_IRQHandler();

  void vSetReInitTicks(u16 lu16ReInitTicks);
  void vError();

  void Tick();

  void vAddSlave(cComNode *lpcSlave);
  void vRemoveSlave(cComNode *lpcSlave);
  void vReInitAllSlave();
  void vSm(tencI2c_Events lenEvent);

  void vStartMsg(cComDatMsg *lpcActiveMsg);
  bool vStartNext();
};



#endif // __cI2C_H__
