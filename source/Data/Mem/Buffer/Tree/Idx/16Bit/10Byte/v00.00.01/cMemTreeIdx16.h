
#ifndef __MEMTREE_IDX16_H__
#define __MEMTREE_IDX16_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

class cMemTreeIdx16Node
{
  public:
  //! 10 Byte
  void*           pData;
  int16           i16Depth;
  uint16          u16IdxNodeBefore;
  uint16          u16IdxNodeNext;

  cMemTreeIdx16Node()
  {
    pData            = null;
    i16Depth         = 0;
    u16IdxNodeBefore = 0xFFFF;
    u16IdxNodeNext   = 0xFFFF;
  }
};

class cMemTreeIdx16
{
  public:
  cMemTreeIdx16Node* mastTree;
  uint16             mui16TreeSize;
  uint16             mui16ElementCount;

  uint16             mui16FirstFreeNode;

  cMemTreeIdx16(uint16 lui16ElementCount)
  {
    if (lui16ElementCount  < 4)      lui16ElementCount = 4;
    if (lui16ElementCount == 0xFFFF) lui16ElementCount--;


    mastTree = new cMemTreeIdx16Node[lui16ElementCount];
    mui16ElementCount = lui16ElementCount;
    mui16TreeSize = 0;

    mui16FirstFreeNode = 0;

    //Alle freien Knoten miteinander verbinden
    //Erster Knoten
    mastTree[0].u16IdxNodeBefore = 0xFFFF;
    mastTree[0].u16IdxNodeNext   = 1;

    //Letzter Knoten
    mastTree[lui16ElementCount - 1].u16IdxNodeBefore = lui16ElementCount - 2;
    mastTree[lui16ElementCount - 1].u16IdxNodeNext = 0xFFFF;

    for (uint16 lui16NodeCnt = 1; lui16NodeCnt < (lui16ElementCount - 1); lui16NodeCnt++)
    {
      mastTree[lui16NodeCnt].u16IdxNodeBefore = lui16NodeCnt - 1;
      mastTree[lui16NodeCnt].u16IdxNodeNext   = lui16NodeCnt + 1;
    }

    //Root anlegen
    uint16 lui16Root;
    lui16Root = u16NodeAlloc();
    mastTree[lui16Root].pData      = null;
    mastTree[lui16Root].i16Depth   = 0;
    mastTree[lui16Root].u16IdxNodeBefore = 0xFFFF;
    mastTree[lui16Root].u16IdxNodeNext = 0xFFFF;
  }

  ~cMemTreeIdx16()
  {
    delete []mastTree;
  }

private:

  // ------------------- Helfer -----------------------

  //Schneidet den übergebenen Node-Bereich aus dem Array heraus
  void vNodeUnlink(uint16 lui16NodeBegin, uint16 lui16NodeEnd)
  {
    uint16 lui16NodeBefore;
    uint16 lui16NodeNext;

    lui16NodeBefore = mastTree[lui16NodeBegin].u16IdxNodeBefore;
    lui16NodeNext = mastTree[lui16NodeEnd].u16IdxNodeNext;

    mastTree[lui16NodeBefore].u16IdxNodeNext = lui16NodeNext;

    if (lui16NodeNext != 0xFFFF)
    {
      mastTree[lui16NodeNext].u16IdxNodeBefore = lui16NodeBefore;
    }
  }

  //Schneidet den übergebenen Node aus dem Array heraus
  void vNodeUnlink(uint16 lui16Node)
  {
    vNodeUnlink(lui16Node, lui16Node);
  }

  //Fügt den übergebenen Node-Bereich in das Array hinein
  void vNodeInsertAfter(uint16 lui16Node, uint16 lui16NodeToInsertStart, uint16 lui16NodeToInsertStop)
  {
    uint16 lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxNodeNext;
    mastTree[lui16Node].u16IdxNodeNext = lui16NodeToInsertStart;
    mastTree[lui16NodeToInsertStart].u16IdxNodeBefore = lui16Node;
    mastTree[lui16NodeToInsertStop].u16IdxNodeNext = lui16NodeNext;

    if (lui16NodeNext != 0xFFFF)
    {
      mastTree[lui16NodeNext].u16IdxNodeBefore = lui16NodeToInsertStop;
    }
  }

  //Fügt den übergebenen Node in das Array hinein
  void vNodeInsertAfter(uint16 lui16Node, uint16 lui16NodeToInsert)
  {
    vNodeInsertAfter(lui16Node, lui16NodeToInsert, lui16NodeToInsert);
  }

  // Findet den nächsten freien Knoten, nimmt den Speicher 
  // und gibt den Index darauf zurück
  uint16 u16NodeAlloc()
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;

    lui16Node = mui16FirstFreeNode;
    if (lui16Node != 0xFFFF)
    {
      //Aus der Free-Liste entfernen
      mui16ElementCount++;
      lui16NodeNext = mastTree[lui16Node].u16IdxNodeNext;
      mui16FirstFreeNode = lui16NodeNext;
      if (lui16NodeNext != 0xFFFF)
      {
        mastTree[lui16NodeNext].u16IdxNodeBefore = 0xFFFF;
      }
    }
    return lui16Node;
  }

  // Fügt den den übergebenen Knoten der Free-Liste hinzu
  void vNodeFree(uint16 lui16Node)
  {
    vNodeUnlink(lui16Node);

    mui16ElementCount--;
    if (mui16FirstFreeNode != 0xFFFF)
    {
      mastTree[mui16FirstFreeNode].u16IdxNodeBefore = lui16Node;
      mastTree[lui16Node].u16IdxNodeBefore = 0xFFFF;
      mastTree[lui16Node].u16IdxNodeNext = mui16FirstFreeNode;
      mui16FirstFreeNode = lui16Node;
    }
    else
    {
      mui16FirstFreeNode = lui16Node;
      mastTree[lui16Node].u16IdxNodeBefore = 0xFFFF;
      mastTree[lui16Node].u16IdxNodeNext   = 0xFFFF;
    }
  }


public:

  uint16 u16TreeParseChild(uint16 lui16Node, uint16 lui16Parent)
  {
    lui16Node = mastTree[lui16Node].u16IdxNodeNext;

    if (lui16Node != 0xFFFF)
    {
      if (mastTree[lui16Parent].i16Depth < mastTree[lui16Node].i16Depth)
        return lui16Node;
    }

    return 0xFFFF;
  }

  uint16 u16TreeParseUnsafe(uint16 lui16Node, int16 li16Depth)
  {
    lui16Node = mastTree[lui16Node].u16IdxNodeNext;

    if (lui16Node != 0xFFFF)
    {
      if (li16Depth < mastTree[lui16Node].i16Depth)
        return lui16Node;
    }

    return 0xFFFF;
  }


  uint16 u16TreeParse(uint16 lui16Node, int16 li16Depth)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16TreeParseUnsafe(lui16Node, li16Depth);
  }

  uint16 u16TreeParse(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return mastTree[lui16Node].u16IdxNodeNext;
  }

  uint16 u16TreeParseBeforeUnsafe(uint16 lui16Node, int16 li16Depth)
  {
    lui16Node = mastTree[lui16Node].u16IdxNodeBefore;

    if (lui16Node != 0xFFFF)
    {
      if (li16Depth < mastTree[lui16Node].i16Depth)
        return lui16Node;
    }
    return 0xFFFF;
  }

  uint16 u16TreeParseBefore(uint16 lui16Node, int16 li16Depth)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16TreeParseBeforeUnsafe(lui16Node, li16Depth);
  }

   
  uint16 u16NodeParseUnsafe(uint16 lui16Node)
  {
    int16 li16Depth;

    li16Depth = mastTree[lui16Node].i16Depth;
    lui16Node = mastTree[lui16Node].u16IdxNodeNext;

    while ((lui16Node != 0xFFFF) && (mastTree[lui16Node].i16Depth >= li16Depth))
    {
      if (mastTree[lui16Node].i16Depth == li16Depth) return lui16Node;
      lui16Node = mastTree[lui16Node].u16IdxNodeNext;
    }
    return 0xFFFF;
  }

  uint16 u16NodeParse(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16NodeParseUnsafe(lui16Node);
  }

  uint16 u16NodeParseBeforeUnsafe(uint16 lui16Node)
  {
    int16 li16Depth;
    uint16 lui16NodeBefore;

    li16Depth = mastTree[lui16Node].i16Depth;

    lui16NodeBefore = u16TreeParseBefore(lui16Node, li16Depth);
    while (lui16NodeBefore != 0xFFFF)
    {
      lui16Node = lui16NodeBefore;
      lui16NodeBefore = u16TreeParseBefore(lui16NodeBefore, li16Depth);
    }

    lui16Node = u16TreeParseBefore(lui16Node, 0);
    if (lui16Node != 0xFFFF)
      if (mastTree[lui16Node].i16Depth == li16Depth) return lui16Node;

    return 0xFFFF;
  }

  uint16 u16NodeParseBefore(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16NodeParseBeforeUnsafe(lui16Node);
  }

  uint16 u16GetNodeRoot()
  {
    return 0;
  }

  uint16 u16NodeGetFirstChild(uint16 lu16ParentNode)
  {
    int16  li16Depth;
    li16Depth = mastTree[lu16ParentNode].i16Depth;

    return u16TreeParseUnsafe(lu16ParentNode, li16Depth);
  }


  uint16 u16TreeGetLastChild(uint16 lu16ParentNode)
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;
    int16  li16Depth;

    li16Depth = mastTree[lu16ParentNode].i16Depth;

    lui16Node = u16NodeGetFirstChild(lu16ParentNode);
    if (lui16Node != 0xFFFF)
    {
      lui16NodeNext = lui16Node;
      do
      {
        lui16Node = lui16NodeNext;
        lui16NodeNext = u16TreeParseUnsafe(lui16Node, li16Depth);
      } while (lui16NodeNext != 0xFFFF);
    }
    return lui16Node;
  }


  uint16 u16NodeGetLastChild(uint16 lu16ParentNode)
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;

    lui16Node = u16NodeGetFirstChild(lu16ParentNode);
    if (lui16Node != 0xFFFF)
    {
      lui16NodeNext = lui16Node;
      do
      {
        lui16Node     = lui16NodeNext;
        lui16NodeNext = u16NodeParseUnsafe(lui16Node);
      } while (lui16NodeNext != 0xFFFF);
    }
    return lui16Node;
  }


  uint16 u16TreeGetParent(uint16 lu16Child)
  {
    uint16 lui16Node;
    uint16 lui16NodeBefore;

    lui16Node       = lu16Child;
    lui16NodeBefore = u16NodeParseBefore(lu16Child);
    if (lui16NodeBefore != 0xFFFF)
    {
      do
      {
        lui16Node       = lui16NodeBefore;
        lui16NodeBefore = u16NodeParseBeforeUnsafe(lui16Node);
      } while (lui16NodeBefore != 0xFFFF);
    }
    return mastTree[lui16Node].u16IdxNodeBefore;
  }

  void vTreeSetChildToTop(uint16 lu16Child)
  {
    uint16 lui16NodeParent;
    uint16 lui16NodeFirstChild;

    lui16NodeParent     = u16TreeGetParent(lu16Child);
    lui16NodeFirstChild = u16NodeGetFirstChild(lui16NodeParent);
    
    if (lui16NodeFirstChild != lu16Child)
    {
      uint16 lui16NodeToInsertStart;
      uint16 lui16NodeToInsertStop;

      lui16NodeToInsertStart = lu16Child;
      lui16NodeToInsertStop  = u16TreeGetLastChild(lu16Child);
      // Falls er keine Kinder hat
      if (lui16NodeToInsertStop == 0xFFFF)
      {
        lui16NodeToInsertStop = lui16NodeToInsertStart;
      }

      vNodeUnlink(lui16NodeToInsertStart, lui16NodeToInsertStop);

      vNodeInsertAfter(lui16NodeParent, lui16NodeToInsertStart, lui16NodeToInsertStop);
    }
  }

  void vTreeSetChildToBottom(uint16 lu16Child)
  {
    uint16 lui16Node;
    uint16 lui16NodeTarget;
    uint16 lui16NodeNext;

    lui16NodeNext = lu16Child;
    do
    {
      lui16Node = lui16NodeNext;
      lui16NodeNext = u16NodeParseUnsafe(lui16Node);
    } while (lui16NodeNext != 0xFFFF);

    if (lui16Node != lu16Child)
    {
      uint16 lui16NodeToInsertStart;
      uint16 lui16NodeToInsertStop;

      lui16NodeToInsertStart = lu16Child;
      lui16NodeToInsertStop = u16TreeGetLastChild(lu16Child);
      // Falls er keine Kinder hat
      if (lui16NodeToInsertStop == 0xFFFF)
      {
        lui16NodeToInsertStop = lui16NodeToInsertStart;
      }

      vNodeUnlink(lui16NodeToInsertStart, lui16NodeToInsertStop);

      lui16NodeTarget = u16TreeGetLastChild(lui16Node);

      // Falls keine Kinder
      if (lui16NodeTarget == 0xFFFF)
      {
        lui16NodeTarget = lui16Node;
      }

      vNodeInsertAfter(lui16NodeTarget, lui16NodeToInsertStart, lui16NodeToInsertStop);
    }
  }


  uint16 u16NodeAddChildTop(uint16 lu16ParentNode, void* lpData)
  {
    uint16 lui16NodeNew;

    lui16NodeNew = u16NodeAlloc();

    if (lui16NodeNew != 0xFFFF)
    {
      mastTree[lui16NodeNew].pData = lpData;

      vNodeInsertAfter(lu16ParentNode, lui16NodeNew);

      mastTree[lui16NodeNew].i16Depth = mastTree[lu16ParentNode].i16Depth + 1;
    }

    return lui16NodeNew;
  }

  uint16 u16NodeAddChildBottom(uint16 lu16ParentNode, void* lpData)
  {
    uint16 lui16Node;
    uint16 lui16NodeNew;

    lui16NodeNew = u16NodeAlloc();

    if (lui16NodeNew != 0xFFFF)
    {
      mastTree[lui16NodeNew].pData = lpData;

      lui16Node = u16TreeGetLastChild(lu16ParentNode);
      if (lui16Node != 0xFFFF)
      {
        vNodeInsertAfter(lui16Node, lui16NodeNew);
      }
      else
      {
        vNodeInsertAfter(lu16ParentNode, lui16NodeNew);
      }

      mastTree[lui16NodeNew].i16Depth = mastTree[lu16ParentNode].i16Depth + 1;
    }

    return lui16NodeNew;
  }

  void vTreeKillChilds(uint16 lu16ParentNode)
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;

    // Ist der Folge Knoten ein Children ?
    // wenn Child gefunden, dann löschen
    lui16Node = u16NodeGetFirstChild(lu16ParentNode);
    while (1)
    {
      lui16NodeNext = mastTree[lui16Node].u16IdxNodeNext;
      vNodeFree(lui16Node);
      
      if (lui16NodeNext == 0xFFFF) break;      
      lui16Node = lui16NodeNext;
      if (mastTree[lui16Node].i16Depth <= mastTree[lu16ParentNode].i16Depth) break;
    }
  }

  void vTreeKillNode(uint16 lu16Node)
  {
    vTreeKillChilds(lu16Node);
    vNodeFree(lu16Node);
  }
  
};


class cMemTreeIdx16_Test : public cMemTreeIdx16
{
public:

  cMemTreeIdx16_Test()
    : cMemTreeIdx16(30)
  {
  }

  void Test()
  {
    /*
      0 -+- 1
         |
         +- 2 -+- 4
         |     +- 5
         |     +- 6
         |     \- 7 -+- 8 --- 9 -+- 10
         |           |           \- 11
         |           \- 12
         \- 3
    */

    uint16 lui16Root;
    uint16 lui16Node1;
    uint16 lui16Node2;
    uint16 lui16Node3;

    uint16 lui16Node7;
    uint16 lui16Node8;
    uint16 lui16Node9;

    lui16Root = 0;

    lui16Node1 = u16NodeAddChildTop(lui16Root,    (void*)(1));
    lui16Node2 = u16NodeAddChildBottom(lui16Root, (void*)(2));
    lui16Node3 = u16NodeAddChildBottom(lui16Root, (void*)(3));

    u16NodeAddChildBottom(lui16Node2, (void*)(4));
    u16NodeAddChildBottom(lui16Node2, (void*)(5));
    u16NodeAddChildBottom(lui16Node2, (void*)(6));
    lui16Node7 = u16NodeAddChildBottom(lui16Node2, (void*)(7));

    lui16Node8 = u16NodeAddChildBottom(lui16Node7, (void*)(8));
    lui16Node9 = u16NodeAddChildBottom(lui16Node8, (void*)(9));
    u16NodeAddChildBottom(lui16Node9, (void*)(10));
    u16NodeAddChildBottom(lui16Node9, (void*)(11));

    u16NodeAddChildBottom(lui16Node7, (void*)(12));

    vTreeSetChildToBottom(lui16Node2);
    /* ------------------------- */
    /*
      0 -+- 1
         |
         +- 3
         |
         \- 2 -+- 4
               +- 5
               +- 6
               \- 7 -+- 8 --- 9 -+- 10
                     |           \- 11
                     \- 12
    */

    
    vTreeSetChildToTop(lui16Node7);
    /* ------------------------- */
    /*
      0 -+- 1
         |
         +- 3
         |
         \- 2 -+- 7 -+- 8 --- 9 -+- 10
               |     |           \- 11
               |     \- 12
               +- 4
               +- 5
               \- 6
    */


    vTreeKillChilds(lui16Node9);
    /* ------------------------- */
    /*
      0 -+- 2 -+- 7 -+- 8 --- 9
         |     |     |
         |     |     \- 12
         |     +- 4
         |     +- 5
         |     \- 6
         |
         +- 1
         |
         \- 3
    */

    vTreeKillChilds(lui16Node7);
    /* ------------------------- */
    /*
      0 -+- 2 -+- 7
         |     +- 4
         |     +- 5
         |     \- 6
         |
         +- 1
         |
         \- 3
    */

    vTreeKillNode(lui16Node2);
    /* ------------------------- */
    /*
      0 -+- 1
         |
         \- 3
    */

  }
};


#ifdef __cplusplus
}
#endif

#endif //__MEMTREE_IDX16_H__
