#ifndef __UI_ELEMENT_BOX3D_OBJECT_H__
#define __UI_ELEMENT_BOX3D_OBJECT_H__


  /* Includes ------------------------------------------------------------------*/
#include "typedef.h"


#define ELEMENT_BOX3D_PRES             16384
#define ELEMENT_BOX3D_PSHIFT           14
#define ELEMENT_BOX3D_PROUNDBIT        (1 << (ELEMENT_BOX3D_PSHIFT-1))

#define ELEMENT_BOX3D_Coord(x)         ((long)(x * ELEMENT_BOX3D_PRES))

#define ELEMENT_BOX3D_NODE(a, b)  ((int32 (*)[3])mObj->miNodes)[a][b]
//#define ELEMENT_BOX3D_EDGE(a, b)  ((uint8 (*)[3])mObj->miFaces)[a][b]
#define ELEMENT_BOX3D_EDGE(a, b)  ((uint8 (*)[3])miaFacesSorted)[a][b]


#ifdef __GNUC__
  class __attribute__((packed)) cUiElement_Box3D_Obj_Node
#else
  class cUiElement_Box3D_Obj_Node
#endif
  {
  public:
    int32 x, y, z;

    cUiElement_Box3D_Obj_Node()
    {
    }


    cUiElement_Box3D_Obj_Node(i32 li32x, i32 li32y, i32 li32z)
    {
      x = li32x;
      y = li32y;
      z = li32z;
    }
  };


#ifdef __GNUC__
  class __attribute__((packed)) cUiElement_Box3D_Obj_Face
#else
  class cUiElement_Box3D_Obj_Face
#endif
  {
  public:
    u8 P1, P2, P3;

    cUiElement_Box3D_Obj_Face()
    {
    }

    cUiElement_Box3D_Obj_Face(u8 lu8P1, u8 lu8P2, u8 lu8P3)
    {
      P1 = lu8P1;
      P2 = lu8P2;
      P3 = lu8P3;
    }
  };

#ifdef __GNUC__
  class __attribute__((packed)) cUiElement_Box3D_Obj
#else
  class cUiElement_Box3D_Obj
#endif
  {
  public:
    int miNodeCount;
    int miFaceCount;

    const cUiElement_Box3D_Obj_Node* mNodes;
    const cUiElement_Box3D_Obj_Face* mFaces;

    void vSet(int liNodeCount, int liFaceCount, const cUiElement_Box3D_Obj_Node* lcNodes, const cUiElement_Box3D_Obj_Face* lcFaces)
    {
      miNodeCount = liNodeCount;
      miFaceCount = liFaceCount;

      mNodes = lcNodes;
      mFaces = lcFaces;
    }
  };


  //https://github.com/Bodmer/TFT_S6D02A1/blob/master/examples/TFT_3d_engine/TFT_3d_engine.ino

  class cUiElement_Box3D_Cube : public cUiElement_Box3D_Obj
  {
  public:
    enum
    {
      NODECOUNT = 8,
      TRICOUNT = 12
    };

    static const i32 mNodes[NODECOUNT][3];
    static const u8  mFaces[TRICOUNT][3];

    cUiElement_Box3D_Cube()
    {
      vSet(NODECOUNT, TRICOUNT, (const cUiElement_Box3D_Obj_Node*) mNodes, (const cUiElement_Box3D_Obj_Face*) mFaces);
    }
  };


    class cUiElement_Box3D_Cone : public cUiElement_Box3D_Obj
    {
    public:
      enum
      {
        NODECOUNT = 15,
        TRICOUNT = 26
      };

      static const i32 mNodes[NODECOUNT][3];
      static const u8  mFaces[TRICOUNT][3];

      cUiElement_Box3D_Cone()
      {
        vSet(NODECOUNT, TRICOUNT, (const cUiElement_Box3D_Obj_Node*)mNodes, (const cUiElement_Box3D_Obj_Face*)mFaces);
      }
    };


    class cUiElement_Box3D_Torus : public cUiElement_Box3D_Obj
    {
    public:
      cUiElement_Box3D_Torus()
      {
        vSet(NODECOUNT, TRICOUNT, (const cUiElement_Box3D_Obj_Node*)mNodes, (const cUiElement_Box3D_Obj_Face*)mFaces);
      }

      enum
      {
        NODECOUNT = 36,
        TRICOUNT = 72
      };

      static const i32 mNodes[NODECOUNT][3];
      static const u8  mFaces[TRICOUNT][3];
    };



    class cUiElement_Box3D_Sphere : public cUiElement_Box3D_Obj
    {
    public:
      cUiElement_Box3D_Sphere()
      {
        vSet(NODECOUNT, TRICOUNT, (const cUiElement_Box3D_Obj_Node*)mNodes, (const cUiElement_Box3D_Obj_Face*)mFaces);
      }

      enum
      {
        NODECOUNT = 32,
        TRICOUNT = 60
      };

      static const i32 mNodes[NODECOUNT][3];
      static const u8  mFaces[TRICOUNT][3];
    };


    class cUiElement_Box3D_Monkey : public cUiElement_Box3D_Obj
    {
    public:
      cUiElement_Box3D_Monkey()
      {
        vSet(NODECOUNT, TRICOUNT, (const cUiElement_Box3D_Obj_Node*)mNodes, (const cUiElement_Box3D_Obj_Face*)mFaces);
      }

      enum
      {
        NODECOUNT = 122,
        TRICOUNT = 223
      };

      static const i32 mNodes[NODECOUNT][3];
      static const u8  mFaces[TRICOUNT][3];
    };


#endif  //__UI_ELEMENT_BOX3D_OBJECT_H__
