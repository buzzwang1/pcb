#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "NRF905_RegMap.h"
#include "cSpi.h"


#include "ComDat.h"
#include "ComRegMap.h"
#include "cComComp.h"
#include "cComSeq.h"


/*=========================================================================*/

class cNRF905 : public cComSeqHdlSpi<True, cNRF905_RegisterMap>
{
public:

  cNRF905(cSpiMasterSingle* lpcSpi)
    : cComSeqHdlSpi(lpcSpi, 0)
  {
  }
};

