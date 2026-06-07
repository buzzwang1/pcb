#pragma once

#include "Typedef.h"
#include "cbArrayT.h"

class cDmaHelper
{
  public:
  static cBitField32 mReq;
  static void vSet(u8 lu8Ch)
  {
    mReq.vSet(lu8Ch);
  }
};

cBitField32 cDmaHelper::mReq;