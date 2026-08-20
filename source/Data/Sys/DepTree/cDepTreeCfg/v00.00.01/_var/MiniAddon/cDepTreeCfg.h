#ifndef _DEPTREECFG_H
#define _DEPTREECFG_H

#include "typedef.h"
#include "cBitField.h"

class cDepTreeCfg
{
public:

  enum cComp: u8
  {
    nBase = 0,
    nBoard3V3,
      nCom,
        nComOutCntrl,
          nComOut,
        nComIn,


      nQSpi1,
        nExtFlash,
      nLed,
      nUsart,


      nBoardI2C2,
        nBoardMonitor,

    // -------------------

    nPomoOut1Tmp,
    nPomoOut1,

    // --------------------

    nAddOn,
      nCharger,
        nBalIna,
          nBalPio,
            nBal,
      n5V0,
        nCanFd,
        n5V0Out,
      nUSB,
      nBatOut,
    nGfxSpi2,
      nGfx,

    nLast
  };
};

struct cDepTreeRequester
{
  enum cRequester : u8
  {
    nMcp = 0,
    nDummy,
    nLast
  };
};

struct cDepTreeLog
{
  enum cEvent : u8
  {
    nDummy = 0,
    nRequestOn,
    nReleaseOn,

    nStartInit,
    nStartDeInit,

    nFinishInit,
    nFinishDeInit,

    nLast
  };

  struct tstEntry
  {
    cDepTreeLog::cEvent enEvent;
    cDepTreeCfg::cComp  enComp;
  };

  static tstEntry mastLog[1024];
  static u16 mu16Pos;

  static void vAdd(cDepTreeLog::cEvent lenEvent, cDepTreeCfg::cComp lenComp)
  {
    mastLog[mu16Pos] = tstEntry{ lenEvent , lenComp };
    mu16Pos++;
    mu16Pos &= 1023;
  }
};


#endif /* _DEPTREECFG_H */
