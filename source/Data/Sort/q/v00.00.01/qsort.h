#ifndef __CLASS_QSORT_H__
#define __CLASS_QSORT_H__

#include "typedef.h"

class cQSortExt
{
  public: 
  // -1: Reference  < Target
  //  0: Reference == Target
  //  1: Reference  > Target
  virtual i32  i32Cmp(i32 li32Reference, i32 li32Target) = 0;
  virtual void vSwap(i32 li32i, i32 li32j) = 0;
};


#ifdef __GNUC__
class __attribute__((packed)) cQSort
#else
class cQSort
#endif
{
  public:
 
  cQSortExt* mcQSortExt;

  cQSort(cQSortExt* lcQSortExt)
  {
    mcQSortExt = lcQSortExt;
  }

  u16 u16Split(i32 li32Left, i32 li32Rigth)
  {
    i32 li32i;
    i32 li32j;
    i32 li32Pivot;

    li32i     = li32Left;
    li32j     = li32Rigth - 1;
    li32Pivot = li32Rigth;

    while(li32i < li32j)
    {
      while ((li32i < li32Rigth) && (mcQSortExt->i32Cmp(li32Pivot, li32i) > 0))
      {
        li32i++;
      }

      while ((li32j > li32Left) && (mcQSortExt->i32Cmp(li32Pivot, li32j) <= 0))
      {
        li32j--;
      }

      if (li32i < li32j)
      {
        mcQSortExt->vSwap(li32i, li32j);

        if (li32Pivot == li32i)
        {
          li32Pivot = li32j;
        }
        else if (li32Pivot == li32j)
        {
          li32Pivot = li32i;
        }
      }
    }
    
    if (mcQSortExt->i32Cmp(li32Pivot, li32i) < 0)
    {
      mcQSortExt->vSwap(li32i, li32Rigth);
    }

    return li32i;
  }

  void vQSort(i32 li32Left, i32 li32Rigth)
  {
    if (li32Left < li32Rigth)
    {
      i32 li32Idx;
      li32Idx = u16Split(li32Left, li32Rigth);
      vQSort(li32Left,    li32Idx - 1);
      vQSort(li32Idx + 1, li32Rigth);
    }
  }
};



#endif //__CLASS_QSORT_H__
