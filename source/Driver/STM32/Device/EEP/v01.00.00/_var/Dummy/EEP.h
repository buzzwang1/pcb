#ifndef __I2C_EEP_H__
#define __I2C_EEP_H__

#include "Typedef.h"
#include "ComDat.h"

class cI2cEep : public cComNode
{
  public:

  u8         mu8RamMirrow[8];

  cI2cEep(void* lpcI2C, u8 lui8Adr, u16 lu16AdrEnd = 1023)
  {
    UNUSED(lui8Adr);
    UNUSED(lpcI2C);
    UNUSED(lu16AdrEnd);	
  }

  u32 u32GetSize() {return sizeof(mu8RamMirrow);}

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState)
  {
    UNUSED(lenError);
    UNUSED(lenState);
  }

  void vComStart(cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEvent);
  }

  void vComDone()
  {
  }

  void vTick10ms(void)
  {
  }


  void vCmdSetup()
  {
  }

  void vCmdReadByte(u16 lu16Adress)
  {
    UNUSED(lu16Adress);
  }

  void vCmdReadBlock(u16 lu16AdrStart, u16 lu16AdrEnd)
  {
    UNUSED(lu16AdrStart);
    UNUSED(lu16AdrEnd);

  }

  void vCmdWriteByte(u16 lu16Adress)
  {
    UNUSED(lu16Adress);
  }


  void vCmdWriteByte(u16 lu16Adress, u8 lu8Data)
  {
    UNUSED(lu16Adress);
    UNUSED(lu8Data);
  }

  void vCmdWriteAll()
  {
  }

  bool isIdle()
  {
    return (True);
  }

  bool isBusy()
  {
    return (False);
  }
};



#endif // __I2C_EEP_H__
