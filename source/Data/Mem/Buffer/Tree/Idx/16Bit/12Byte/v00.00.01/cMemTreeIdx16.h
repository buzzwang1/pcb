
#ifndef __MEMTREE_IDX16_H__
#define __MEMTREE_IDX16_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

class cMemTreeIdx16Node
{
  public:
  //! 12 Byte
  void*           pData;
  uint16          u16IdxBefore;
  uint16          u16IdxNext;
  uint16          u16IdxParent;
  uint16          u16IdxFirstChild;

  cMemTreeIdx16Node()
  {
    pData            = null;
    u16IdxBefore     = 0xFFFF;
    u16IdxNext       = 0xFFFF;
    u16IdxParent     = 0xFFFF;
    u16IdxFirstChild = 0xFFFF;
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
    mastTree[0].u16IdxBefore = 0xFFFF;
    mastTree[0].u16IdxNext   = 1;

    //Letzter Knoten
    mastTree[lui16ElementCount - 1].u16IdxBefore = lui16ElementCount - 2;
    mastTree[lui16ElementCount - 1].u16IdxNext = 0xFFFF;

    for (uint16 lui16NodeCnt = 1; lui16NodeCnt < (lui16ElementCount - 1); lui16NodeCnt++)
    {
      mastTree[lui16NodeCnt].u16IdxBefore = lui16NodeCnt - 1;
      mastTree[lui16NodeCnt].u16IdxNext   = lui16NodeCnt + 1;
    }

    //Root anlegen
    uint16 lui16Root;
    lui16Root = u16NodeAlloc();
    mastTree[lui16Root].pData            = null;
    mastTree[lui16Root].u16IdxBefore     = 0xFFFF;
    mastTree[lui16Root].u16IdxNext       = 0xFFFF;
    mastTree[lui16Root].u16IdxParent     = 0xFFFF;
    mastTree[lui16Root].u16IdxFirstChild = 0xFFFF;

  }

  ~cMemTreeIdx16()
  {
    delete []mastTree;
  }

private:
  //Schneidet den übergebenen Node aus dem Array heraus
  void vNodeUnlink(uint16 lui16Node)
  {
    uint16 lui16NodeParent;
    uint16 lui16NodeBefore;
    uint16 lui16NodeNext;

    lui16NodeParent = mastTree[lui16Node].u16IdxParent;
    lui16NodeBefore = mastTree[lui16Node].u16IdxBefore;
    lui16NodeNext   = mastTree[lui16Node].u16IdxNext;

    if (mastTree[lui16NodeParent].u16IdxFirstChild == lui16Node)
    {
      mastTree[lui16NodeParent].u16IdxFirstChild = lui16NodeNext;
    }

    if (lui16NodeBefore != 0xFFFF)
    {
      mastTree[lui16NodeBefore].u16IdxNext = lui16NodeNext;
    }
    
    if (lui16NodeNext != 0xFFFF)
    {
      mastTree[lui16NodeNext].u16IdxBefore = lui16NodeBefore;
    }
  }

  //Fügt den übergebenen Node in das Array hinein
  void vNodeInsertAfter(uint16 lui16Node, uint16 lui16NodeToInsert)
  {
    uint16 lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxNext;
    mastTree[lui16Node].u16IdxNext = lui16NodeToInsert;
    mastTree[lui16NodeToInsert].u16IdxBefore = lui16Node;
    mastTree[lui16NodeToInsert].u16IdxNext   = lui16NodeNext;

    if (lui16NodeNext != 0xFFFF)
    {
      mastTree[lui16NodeNext].u16IdxBefore = lui16NodeToInsert;
    }
  }


  void vNodeInsertTopOfParent(uint16 lui16NodeParent, uint16 lui16NodeToInsert)
  {
    uint16 lui16NodeFirstChild;

    lui16NodeFirstChild = u16NodeGetFirstChild(lui16NodeParent);

    mastTree[lui16NodeParent].u16IdxFirstChild = lui16NodeToInsert;

    mastTree[lui16NodeToInsert].u16IdxNext   = lui16NodeFirstChild;
    mastTree[lui16NodeToInsert].u16IdxBefore = 0xFFFF;
    mastTree[lui16NodeToInsert].u16IdxParent = lui16NodeParent;

    if (lui16NodeFirstChild != 0xFFFF)
    {
      mastTree[lui16NodeFirstChild].u16IdxBefore = lui16NodeToInsert;
    }
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
      lui16NodeNext = mastTree[lui16Node].u16IdxNext;
      mui16FirstFreeNode = lui16NodeNext;
      if (lui16NodeNext != 0xFFFF)
      {
        mastTree[lui16NodeNext].u16IdxBefore = 0xFFFF;
      }
      mastTree[lui16NodeNext].u16IdxFirstChild = 0xFFFF;
      mastTree[lui16NodeNext].u16IdxParent     = 0xFFFF;

    }
    return lui16Node;
  }

  // Fügt den den übergebenen Knoten der Free-Liste hinzu
  // Er darf keine Children enthalten ?
  void vNodeFree(uint16 lui16Node)
  {
    vNodeUnlink(lui16Node);

    mui16ElementCount--;
    if (mui16FirstFreeNode != 0xFFFF)
    {
      mastTree[mui16FirstFreeNode].u16IdxBefore = lui16Node;
      mastTree[lui16Node].u16IdxBefore = 0xFFFF;
      mastTree[lui16Node].u16IdxNext = mui16FirstFreeNode;
      mui16FirstFreeNode = lui16Node;
    }
    else
    {
      mui16FirstFreeNode = lui16Node;
      mastTree[lui16Node].u16IdxBefore = 0xFFFF;
      mastTree[lui16Node].u16IdxNext   = 0xFFFF;
    }
  }


public:


  uint16 u16NodeParseUnsafe(uint16 lui16Node)
  {
    return mastTree[lui16Node].u16IdxNext;
  }

  uint16 u16NodeParse(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return mastTree[lui16Node].u16IdxNext;
  }

  uint16 u16NodeParseBeforeUnsafe(uint16 lui16Node)
  {
    return mastTree[lui16Node].u16IdxBefore;
  }

  uint16 u16NodeParseBefore(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return mastTree[lui16Node].u16IdxBefore;
  }

  uint16 u16GetNodeRoot()
  {
    return 0;
  }


  uint16 u16NodeGetFirstChild(uint16 lu16ParentNode)
  {
    return mastTree[lu16ParentNode].u16IdxFirstChild;
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
        lui16Node = lui16NodeNext;
        lui16NodeNext = mastTree[lui16Node].u16IdxNext;
      } while (lui16NodeNext != 0xFFFF);
    }
    return lui16Node;
  }


  uint16 u16TreeParseUnsafe(uint16 lui16Node)
  {
    uint16 lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxFirstChild;
    if (lui16NodeNext != 0xFFFF) return lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxNext;
    if (lui16NodeNext != 0xFFFF) return lui16NodeNext;

    while (1)
    {
      lui16Node = mastTree[lui16Node].u16IdxParent;
      if (lui16Node == 0xFFFF) return lui16Node;
      lui16NodeNext = mastTree[lui16Node].u16IdxNext;
      if (lui16NodeNext != 0xFFFF) return lui16NodeNext;
    }

    return 0xFFFF;
  }

  uint16 u16TreeParse(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16TreeParseUnsafe(lui16Node);
  }

  uint16 u16TreeParseChild(uint16 lui16Node, uint16 lui16Parent)
  {
    uint16 lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxFirstChild;
    if (lui16NodeNext != 0xFFFF) return lui16NodeNext;

    lui16NodeNext = mastTree[lui16Node].u16IdxNext;
    if (lui16NodeNext != 0xFFFF) return lui16NodeNext;

    while (1)
    {
      lui16Node = mastTree[lui16Node].u16IdxParent;
      if (lui16Node == lui16Parent) return 0xFFFF;
      if (lui16Node == 0xFFFF) return lui16Node;
      
      lui16NodeNext = mastTree[lui16Node].u16IdxNext;
      if (lui16NodeNext != 0xFFFF) return lui16NodeNext;
    }

    return 0xFFFF;
  }

  uint16 u16TreeParseBeforeUnsafe(uint16 lui16Node)
  {
    uint16 lui16NodeBefore;

    while (1)
    {
      lui16NodeBefore = mastTree[lui16Node].u16IdxBefore;

      if (lui16NodeBefore == 0xFFFF)
      {
        return mastTree[lui16Node].u16IdxParent;
      }
      else
      {
        lui16Node = lui16NodeBefore;
        if (mastTree[lui16Node].u16IdxFirstChild == 0xFFFF)
        {
          return lui16Node;
        }
        else
        {
          uint16 lui16NodeLastChild;
          while (1)
          {
            lui16NodeLastChild = u16NodeGetLastChild(lui16Node);
            if (lui16NodeLastChild == 0xFFFF) return lui16Node;
            lui16Node = lui16NodeLastChild;
          }
        }
      }
    }

    return 0xFFFF;
  }

  uint16 u16TreeParseBefore(uint16 lui16Node)
  {
    if (lui16Node == 0xFFFF) return lui16Node;
    return u16TreeParseBeforeUnsafe(lui16Node);
  }


  uint16 u16TreeGetParent(uint16 lu16Child)
  {
    return mastTree[lu16Child].u16IdxParent;
  }

  void vTreeSetChildToTop(uint16 lu16Child)
  {
    uint16 lui16NodeParent;
    uint16 lui16NodeFirstChild;

    lui16NodeParent     = u16TreeGetParent(lu16Child);
    lui16NodeFirstChild = u16NodeGetFirstChild(lui16NodeParent);
    
    if (lui16NodeFirstChild != lu16Child)
    {
      vNodeUnlink(lu16Child);      
      vNodeInsertTopOfParent(lui16NodeParent, lu16Child);
    }
  }

  void vTreeSetChildToBottom(uint16 lu16Child)
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;

    lui16NodeNext = lu16Child;
    do
    {
      lui16Node = lui16NodeNext;
      lui16NodeNext = u16NodeParseUnsafe(lui16Node);
    } while (lui16NodeNext != 0xFFFF);

    if (lui16Node != lu16Child)
    {
      vNodeUnlink(lu16Child);
      vNodeInsertAfter(lui16Node, lu16Child);
    }
  }


  uint16 u16NodeAddChildTop(uint16 lui16ParentNode, void* lpData)
  {
    uint16 lui16NodeNew;

    lui16NodeNew = u16NodeAlloc();

    if (lui16NodeNew != 0xFFFF)
    {
      mastTree[lui16NodeNew].pData = lpData;
      vNodeInsertTopOfParent(lui16ParentNode, lui16NodeNew);
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

      lui16Node = u16NodeGetLastChild(lu16ParentNode);
      if (lui16Node != 0xFFFF)
      {
        vNodeInsertAfter(lui16Node, lui16NodeNew);
        mastTree[lui16NodeNew].u16IdxParent = lu16ParentNode;
      }
      else
      {
        vNodeInsertTopOfParent(lu16ParentNode, lui16NodeNew);
      }
    }

    return lui16NodeNew;
  }

  void vTreeKillChilds(uint16 lu16ParentNode)
  {
    uint16 lui16Node;
    uint16 lui16NodeNext;

    lui16Node = lu16ParentNode;

    while (1)
    {
      lui16NodeNext = u16NodeGetLastChild(lui16Node);
      if (lui16NodeNext == 0xFFFF) break;
      lui16Node = lui16NodeNext;
    }

    while (1)
    {
      if (lui16Node == lu16ParentNode) return;
      lui16NodeNext = u16TreeParseBeforeUnsafe(lui16Node);
      vNodeFree(lui16Node);      
      lui16Node = lui16NodeNext;
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
    //uint16 lui16Node1;
    uint16 lui16Node2;
    //uint16 lui16Node3;

    uint16 lui16Node7;
    uint16 lui16Node8;
    uint16 lui16Node9;

    lui16Root = 0;

    //lui16Node1 = u16NodeAddChildTop(lui16Root,    (void*)(1));
    lui16Node2 = u16NodeAddChildBottom(lui16Root, (void*)(2));
    //lui16Node3 = u16NodeAddChildBottom(lui16Root, (void*)(3));

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


    vTreeSetChildToTop(lui16Node2);
    /* ------------------------- */
    /*
      0 -+- 2 -+- 7 -+- 8 --- 9 -+- 10
         |     |     |           \- 11
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
