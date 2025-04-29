#include "ComDat.h"


cComNodeList::cComNodeList(uint32 lui32NodesCnt)
{
  ui32Size =   lui32NodesCnt;
  ui32Used =   0;
  ui32Pos  =  -1;

  mpacNodes = new cComNode*[ui32Size];


  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    mpacNodes[lui32SlavesIdx] = NULL;
  }
}

#ifdef _WIN32
  cComNodeList::~cComNodeList()
  {
    delete []mpacNodes;
  }
#endif // _WIN32

uint32 cComNodeList::SizeOf()
{
  uint32 lui32Size;
  uint32 lui32SlavesIdx;

  lui32Size = sizeof(cComNodeList);

  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] != NULL)
    {
      lui32SlavesIdx += mpacNodes[lui32SlavesIdx]->SizeOf();
    }
  }
  return lui32Size;
}

void cComNodeList::vResetPos()
{
  ui32Pos = -1;
}


void cComNodeList::vStartAll()
{
  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] != NULL)
    {
      mpacNodes[lui32SlavesIdx]->mStatus.IsStartRequested = 1;
    }
  }
}

void cComNodeList::vSetErrorAll(cComNode::tenError lenError, cComNode::tenState lenState)
{
  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] != NULL)
    {
      mpacNodes[lui32SlavesIdx]->vComError(lenError, lenState);
    }
  }
}

bool cComNodeList::isEmpty()
{
  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] != NULL)
    {
      if (mpacNodes[lui32SlavesIdx]->mStatus.IsStartRequested)
      {
        return false;
      }
    }
  }

  return true;
}

uint32 cComNodeList::ui32GetFreeSlot()
{
  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] == NULL)
    {
      return lui32SlavesIdx;
    }
  }
  return -1;
}

uint32 cComNodeList::ui32GetSlot(cComNode *lpcSlave)
{
  uint32 lui32SlavesIdx;
  for (lui32SlavesIdx = 0; lui32SlavesIdx < ui32Size; lui32SlavesIdx++)
  {
    if (mpacNodes[lui32SlavesIdx] == lpcSlave)
    {
      return lui32SlavesIdx;
    }
  }
  return -1;
}



bool cComNodeList::isSlotFree()
{
  return (ui32Used < ui32Size);
}


void cComNodeList::vAdd(cComNode *lpcSlave)
{
  if (isSlotFree())
  {
    uint32 lui32FreeSlot;
    lui32FreeSlot = ui32GetFreeSlot();
    mpacNodes[lui32FreeSlot] = lpcSlave;
    ui32Used++;
  }
}

void cComNodeList::vRemove(cComNode *lpcSlave)
{
  uint32 lui32Slot;

  lui32Slot = ui32GetSlot(lpcSlave);
  mpacNodes[lui32Slot] = NULL;

  ui32Used--;
}

cComNode* cComNodeList::cGetNext()
{
  cComNode *lpcSlave;

  uint32 lui32Cnt;

  lpcSlave = NULL;

  if (!isEmpty())
  {
    for (lui32Cnt = 0; lui32Cnt < ui32Size; lui32Cnt++)
    {
      ui32Pos++;
      if (ui32Pos >= ui32Size)
      {
        ui32Pos = 0;
      }
      if (mpacNodes[ui32Pos] != NULL)
      {
        if (mpacNodes[ui32Pos]->mStatus.IsStartRequested)
        {
          return mpacNodes[ui32Pos];
        }
      }
    }
  }
  return lpcSlave;
}
