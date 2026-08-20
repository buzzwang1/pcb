#pragma once

#include "Typedef.h"
#include "ComSeqHdl.h"
#include "XPT2046_RegMap.h"
#include "cSpi.h"


#include "ComDat.h"
#include "ComRegMap.h"
#include "cComComp.h"
#include "cComSeq.h"


/*=========================================================================*/

class cXPT2046 : public cComSeqHdlSpi<False, cXPT2046_RegisterMap>
{
public:

  cXPT2046(cSpiMasterMulti* lpcSpi)
         : cComSeqHdlSpi(lpcSpi, 0)
  {
  }
};

