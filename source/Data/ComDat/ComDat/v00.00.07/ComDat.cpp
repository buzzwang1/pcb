#include "ComDat.h"


cComNodeList::cComNodeList(uint32 lui32NodesCnt)
{
  mu8Size =   lui32NodesCnt;
  mu8Used =   0;
  mu8Pos  =  -1;

  mpacNodes = new cComNode*[mu8Size];


  u8 lu8SlavesIdx;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    mpacNodes[lu8SlavesIdx] = NULL;
  }
}

cComNodeList::~cComNodeList()
{
  delete []mpacNodes;
}


uint32 cComNodeList::SizeOf()
{
  return 0;
}

void cComNodeList::vStartAll()
{
  u8 lu8SlavesIdx;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    if (mpacNodes[lu8SlavesIdx] != NULL)
    {
      mpacNodes[lu8SlavesIdx]->mStatus.IsStartRequested = 1;
    }
  }
}

void cComNodeList::vSetErrorAll(cComNode::tenError lenError, cComNode::tenState lenState)
{
  u8 lu8SlavesIdx;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    if (mpacNodes[lu8SlavesIdx] != NULL)
    {
      mpacNodes[lu8SlavesIdx]->vComError(lenError, lenState);
    }
  }
}

bool cComNodeList::isEmpty()
{
  u8 lu8SlavesIdx;
  u8 lu8Pos = mu8Pos;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    lu8Pos++;
    if (lu8Pos >= mu8Size)
    {
      lu8Pos = 0;
    }
    if (mpacNodes[lu8Pos] != NULL)
    {
      if (mpacNodes[lu8Pos]->mStatus.IsStartRequested)
      {
        return false;
      }
    }
  }

  return true;
}

u8 cComNodeList::ui32GetFreeSlot()
{
  u8 lu8SlavesIdx;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    if (mpacNodes[lu8SlavesIdx] == NULL)
    {
      return lu8SlavesIdx;
    }
  }
  return -1;
}

u8 cComNodeList::ui32GetSlot(cComNode *lpcSlave)
{
  u8 lu8SlavesIdx;
  for (lu8SlavesIdx = 0; lu8SlavesIdx < mu8Size; lu8SlavesIdx++)
  {
    if (mpacNodes[lu8SlavesIdx] == lpcSlave)
    {
      return lu8SlavesIdx;
    }
  }
  return -1;
}



bool cComNodeList::isSlotFree()
{
  return (mu8Used < mu8Size);
}


void cComNodeList::vAdd(cComNode *lpcSlave)
{
  if (isSlotFree())
  {
    uint32 lui32FreeSlot;
    lui32FreeSlot = ui32GetFreeSlot();
    mpacNodes[lui32FreeSlot] = lpcSlave;
    mu8Used++;
  }
}

void cComNodeList::vRemove(cComNode *lpcSlave)
{
  uint32 lui32Slot;

  lui32Slot = ui32GetSlot(lpcSlave);
  mpacNodes[lui32Slot] = NULL;

  mu8Used--;
}

cComNode* cComNodeList::cGetNext()
{
  cComNode *lpcSlave;

  u8 lu8Cnt;

  lpcSlave = NULL;

  if (!isEmpty())
  {
    for (lu8Cnt = 0; lu8Cnt < mu8Size; lu8Cnt++)
    {
      mu8Pos++;
      if (mu8Pos >= mu8Size)
      {
        mu8Pos = 0;
      }
      if (mpacNodes[mu8Pos] != NULL)
      {
        if (mpacNodes[mu8Pos]->mStatus.IsStartRequested)
        {
          return mpacNodes[mu8Pos];
        }
      }
    }
  }
  return lpcSlave;
}
