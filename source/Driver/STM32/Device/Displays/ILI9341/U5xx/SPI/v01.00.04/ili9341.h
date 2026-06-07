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

  cSpiMaster* mpcSpi;

  cIli9341(cSpiMaster* lpcSpi)
         : cComSeqHdlSpi(lpcSpi, 0)
  {
    mpcSpi = lpcSpi;
  }

  void vSequenceStart(u8 lu8SeqIdx) override
  {
    switch (lu8SeqIdx)
    {
      case cIli9341_RegisterMap::tenSequences::nSEQ08WriteGRAMData:
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

