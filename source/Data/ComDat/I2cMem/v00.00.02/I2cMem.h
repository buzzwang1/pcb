#ifndef __I2CMEM_H__
#define __I2CMEM_H__

#include "Typedef.h"
#include "cI2C.h"
#include "cComBuf.h"


#define I2CMEM_DEFAULT_ADDRESS       (0x2 << 1)

class cI2cMem:cI2cMaster_Slave
{
  public:

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdSync,
    enCmdData,
    enCmdAck,
    enCmdEnd
  }tenI2cMem_Cmd;

  typedef enum
  {
    enCnstMaxData  = 65, // 64 (Max Message Lenght) + 1 (Protokoll Byte)
  }tenI2cMem_Consts;

  cI2cMaster*      mI2C;


  uint16     mui16DataToTx;
  uint16     mui16DataToRx;

  cComDatMsg mpcMsgSync;
  cComDatMsg mpcMsgData;
  cComDatMsg mpcMsgAck;

  bool mbSync;

  tenI2cMem_Cmd      menCmd;
  uint8              mIdx;

  uint8 mAdrSync;
  uint8 mAdrData;
  uint8 mAdrAck;

  bool mbOnline;

  cComBuf  *mcRxComBuf;
  cComBuf  *mcTxComBuf;

  cI2cMem(cI2cMaster* lpcI2C, uint8 lui8Adr);
  ~cI2cMem();

  void  vComError(cComNode::tenComNodeError lenError, cComNode::tenComNodeState lenState);
  void  vComStart();
  void  vComDone();

  void  vSync();
};

#endif // __I2CMEM_H__
