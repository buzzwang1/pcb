#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "Ili9341_RegMap.h"
#include "cSpi.h"


#include "ComDat.h"
#include "ComRegMap.h"
#include "cComComp.h"
#include "cComSeq.h"


/*=========================================================================*/

class cIli9341 : public cComSeqHdlSpi<False, cIli9341_RegisterMap>
{
public:

  cSpiMasterMulti* mpcSpi;

  cIli9341(cSpiMasterMulti* lpcSpi)
         : cComSeqHdlSpi(lpcSpi, 0)
  {
    //vSetGRAM(lpau8GRAM);
    mpcSpi = lpcSpi;
  }

  void vSequenceStart(u8 lu8SeqIdx) override
  {
    switch (lu8SeqIdx)
    {
      case cIli9341_RegisterMap::tenSequences::nSEQ08WriteGRAMData:
        // 160Mhz / 4 = 40Mhz
        // 320x240*2 = 153600 Bytes = 1.228.800 Bits
        // 30720 ms
        mpcSpi->vSetPrescaler(4);
        mpcSpi->vSetDataWidth(32);
        mpcSpi->vSetHalfWordExchange();
        break;
    }
  }

  void vSequenceDone(u8 lu8SeqIdx) override
  {
    switch (lu8SeqIdx)
    {
      case cIli9341_RegisterMap::tenSequences::nSEQ08WriteGRAMData:
        mpcSpi->vRestoreCfg();
        break;
    }
  }

};

