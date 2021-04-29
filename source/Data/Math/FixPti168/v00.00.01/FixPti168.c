#include "FixPti168.h"



void FPi168_vDiv_Save(FPi168_tunFPi168 unFPi168Src1, FPi168_tunFPi168 unFPi168Src2, FPi168_tunFPi168* unFPi168Dest)
{
  FPi168_tunFPi168 lunTemp;

  if (unFPi168Src2.u32FPi168 == 0)
  {
    FPi168_vSetMax(lunTemp);
    unFPi168Dest->u32FPi168 = lunTemp.u32FPi168;
  }

  FPi168_vDiv(unFPi168Src1, unFPi168Src2, lunTemp);
  unFPi168Dest->u32FPi168 = lunTemp.u32FPi168;
}





