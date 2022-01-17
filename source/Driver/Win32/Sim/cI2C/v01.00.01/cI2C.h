#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"

#include "cI2cTimer.h"

#include "tGPPin.h"
#include "ComDat.h"

//#include "ClockInfo.h"

#define I2C_DIRECTION_TX 0
#define I2C_DIRECTION_RX 1

/*typedef struct
{
  uint16_t CR1;
  uint16_t  RESERVED0;
  uint16_t CR2;
  uint16_t  RESERVED1;
  uint16_t OAR1;
  uint16_t  RESERVED2;
  uint16_t OAR2;
  uint16_t  RESERVED3;
  uint16_t DR;
  uint16_t  RESERVED4;
  uint16_t SR1;
  uint16_t  RESERVED5;
  uint16_t SR2;
  uint16_t  RESERVED6;
  uint16_t CCR;
  uint16_t  RESERVED7;
  uint16_t TRISE;
  uint16_t  RESERVED8;
} I2C_TypeDef;*/


typedef struct
{
  u32 CR1;         /*!< I2C Control register 1,            Address offset: 0x00 */
  u32 CR2;         /*!< I2C Control register 2,            Address offset: 0x04 */
  u32 OAR1;        /*!< I2C Own address 1 register,        Address offset: 0x08 */
  u32 OAR2;        /*!< I2C Own address 2 register,        Address offset: 0x0C */
  u32 TIMINGR;     /*!< I2C Timing register,               Address offset: 0x10 */
  u32 TIMEOUTR;    /*!< I2C Timeout register,              Address offset: 0x14 */
  u32 ISR;         /*!< I2C Interrupt and status register, Address offset: 0x18 */
  u32 ICR;         /*!< I2C Interrupt clear register,      Address offset: 0x1C */
  u32 PECR;        /*!< I2C PEC register,                  Address offset: 0x20 */
  u32 RXDR;        /*!< I2C Receive data register,         Address offset: 0x24 */
  u32 TXDR;        /*!< I2C Transmit data register,        Address offset: 0x28 */
} I2C_TypeDef;

extern I2C_TypeDef I2C1_BASE;
extern I2C_TypeDef I2C2_BASE;
extern I2C_TypeDef I2C3_BASE;

#define I2C1               ((I2C_TypeDef *) &I2C1_BASE)
#define I2C2               ((I2C_TypeDef *) &I2C2_BASE)
#define I2C3               ((I2C_TypeDef *) &I2C3_BASE)

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
    enStError,
    enStWait
  }tencI2c_States;

  typedef enum
  {
    enEvStart  = 0,
    enEvIrq,
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
  void  vClearClockLine();
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


  cI2cMaster_ComNodeList mcSlaves;
  cComNode              *mpcActiveSlave;
  cComDatMsg            *mpcActiveMsg;

  cComNode::tenComNodeState  menComState;
  uint8           *mpui8ComBuf;
  uint32           mui32ComByteCnt;

  cBotNet_DownLinkI2c_Timer mTimer;

  cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint32 lui32Slaves);
  ~cI2cMaster();

  inline uint32 SizeOf()
  {
    return mcSlaves.SizeOf() + sizeof(cI2cMaster);
  }

  void TIM_EV_IRQHandler();
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
  bool bStartNext();
};



class cI2cSlave : public cI2c
{
  public:
  cComDatMsg                *mpcMsg;

  cComNode                  *mpcI2cNode;
  cComNode::tenComNodeError  menErrorActive;

  cI2cSlave(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint8 lui8Adr1, uint8 lui8Adr2);
  ~cI2cSlave();

  void SetSlaveNode(cComNode *lpcI2cNode);

  void I2C_EV_IRQHandler();
  void I2C_ER_IRQHandler();

  void  vError();

  void vSetMsg(cComDatMsg* lpcMsg) { mpcMsg = lpcMsg; };
  void vSm(tencI2c_Events lenEvent);
};


#endif // __cI2C_H__
