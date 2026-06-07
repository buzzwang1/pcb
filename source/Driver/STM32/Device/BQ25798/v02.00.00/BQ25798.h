#ifndef __BQ25798_H__
#define __BQ25798_H__

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "BQ25798_RegMap.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define BQ25798_I2C_ADDRESS_CONF_0       (0x6B)
  #define BQ25798_I2C_ADDRESS_DEFAULT      BQ25798_I2C_ADDRESS_CONF_0
/*=========================================================================*/



class cBQ25798: public cComSeqHdlI2c<cBQ25798_RegisterMap>
{
  public:

  cBQ25798(cI2cMaster* lpcI2C, u8 lu8Adr)
    : cComSeqHdlI2c(lpcI2C, lu8Adr << 1)
  {
  }
};

#endif // __BQ25798_H__
