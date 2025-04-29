#ifndef __UI_ELEMENT_BOX3D_H__
#define __UI_ELEMENT_BOX3D_H__


/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_box3dobj.h"
#include "uielement_Base.h"
#include "MemTools.h"
#include "PaintTriBasic.h"
#include "qsort.h"

class cUiElement_Box3D : public cUiElement, public cQSortExt
{
public:


  typedef struct
  {
    long x, y, z;
  } Vector3;

  typedef struct
  {
    int x, y, z;
  } Vector3i;

  // fixed point identity matrix
  typedef struct
  {
    int32 m[4][4];
  } Matrix4;

  Matrix4 DefaultMat =
  { ELEMENT_BOX3D_PRES,   0,    0,    0,
      0,  ELEMENT_BOX3D_PRES,   0,    0,
      0,    0,  ELEMENT_BOX3D_PRES,   0,
      0,    0,    0,  ELEMENT_BOX3D_PRES };

  cPaintTriBasic         mcPaintTriBasic;
  uint8                  mui8Play;
  bool                   mboWireFrame;
  cUiElement_Box3D_Obj  *mObj;
    
  cUiElement_Box3D_Obj_Node* mcaProjNodes;
  cUiElement_Box3D_Obj_Face* mcaFacesSorted;
  cQSort mcQSort;

  cUiElement_Box3D(cMsgBox *lcMsgBox, cScreen *lpcScreen, cUiElement_Box3D_Obj *lObj)
    : cUiElement(lcMsgBox, lpcScreen),
      mcQSort(this)
  {
    u16 lu16i;

    mui8Play = 1;
    mboWireFrame = True;
    mMatInit(&m_world);

    menType = cUiElement::tenType::nBox3D;

    mObj = lObj;

    mcaProjNodes   = new cUiElement_Box3D_Obj_Node[mObj->miNodeCount];
    mcaFacesSorted = new cUiElement_Box3D_Obj_Face[mObj->miFaceCount];

    for (lu16i = 0; lu16i < mObj->miFaceCount; lu16i++)
    {
      mcaFacesSorted[lu16i] = mObj->mFaces[lu16i];
    }

    mcPaintTriBasic.vSetScreen(lpcScreen);
  }

  ~cUiElement_Box3D()
  {
    delete []mcaProjNodes;
    delete []mcaFacesSorted;
  }

  // QSort Callback
  i32 i32Cmp(i32 li32Reference, i32 li32Target)
  {
    //sort by first point z-Coordinates
    return mcaProjNodes[mcaFacesSorted[li32Reference].P1].z - mcaProjNodes[mcaFacesSorted[li32Target].P1].z;
  }

  void vSwap(i32 li32i, i32 li32j)
  {
    cUiElement_Box3D_Obj_Face lcTemp;

    lcTemp = mcaFacesSorted[li32i];
    mcaFacesSorted[li32i] = mcaFacesSorted[li32j];
    mcaFacesSorted[li32j] = lcTemp;
  }

  // ----------------------------------------------
  // functions
  // ----------------------------------------------
  // fixed point multiplication
  inline int32 pMultiply(int32 x, int32 y)
  {
    return ((x * y) + ELEMENT_BOX3D_PROUNDBIT) >> ELEMENT_BOX3D_PSHIFT;
  }

  static const uint16 lut[];

  #define LUT(a) ((int32)(lut[a]))

  Matrix4  m_world;
  Vector3i mesh_rotation = { 0, 0, 0 };
  Vector3i mesh_position = { 0, 0, 0 };

  // ----------------------------------------------
  // SIN/COS from 90 degrees LUT
  // ----------------------------------------------
  int32 SIN(uint16 angle) {
    angle += 90;
    if (angle > 450) return LUT(0);
    if (angle > 360 && angle < 451) return -LUT(angle - 360);
    if (angle > 270 && angle < 361) return -LUT(360 - angle);
    if (angle > 180 && angle < 271) return  LUT(angle - 180);
    return LUT(180 - angle);
  }

  int32 COS(uint16 angle) {
    if (angle > 360) return LUT(0);
    if (angle > 270 && angle < 361) return  LUT(360 - angle);
    if (angle > 180 && angle < 271) return -LUT(angle - 180);
    if (angle > 90 && angle < 181) return -LUT(180 - angle);
    return LUT(angle);
  }

  // ----------------------------------------------
  // Matrix operation
  // ----------------------------------------------

  void mMatInit(Matrix4* Mat)
  {
    cMemTools::vMemCpy((uint8*)Mat, (uint8*)&DefaultMat, sizeof(Matrix4));
  }

  Matrix4 mMultiply(const Matrix4 mat1, const Matrix4 mat2)
  {
    Matrix4 mat;
    unsigned char r, c;

    for (c = 0; c < 4; c++)
    {
      for (r = 0; r < 4; r++)
      {
        mat.m[c][r] = pMultiply(mat1.m[0][r], mat2.m[c][0]) +
          pMultiply(mat1.m[1][r], mat2.m[c][1]) +
          pMultiply(mat1.m[2][r], mat2.m[c][2]) +
          pMultiply(mat1.m[3][r], mat2.m[c][3]);
      }
    }
    return mat;
  }

  Matrix4 mRotateX(const unsigned int angle)
  {
    Matrix4 mat;
    mMatInit(&mat);
    mat.m[1][1] = COS(angle);
    mat.m[1][2] = SIN(angle);
    mat.m[2][1] = -SIN(angle);
    mat.m[2][2] = COS(angle);
    return mat;
  }

  Matrix4 mRotateY(const unsigned int angle)
  {
    Matrix4 mat;
    mMatInit(&mat);
    mat.m[0][0] = COS(angle);
    mat.m[0][2] = -SIN(angle);
    mat.m[2][0] = SIN(angle);
    mat.m[2][2] = COS(angle);
    return mat;
  }

  Matrix4 mRotateZ(const unsigned int angle)
  {
    Matrix4 mat;
    mMatInit(&mat);
    mat.m[0][0] = COS(angle);
    mat.m[0][1] = SIN(angle);
    mat.m[1][0] = -SIN(angle);
    mat.m[1][1] = COS(angle);
    return mat;
  }

  Matrix4 mTranslate(const int32 x, const int32 y, const int32 z)
  {
    Matrix4 mat;
    mMatInit(&mat);
    mat.m[3][0] = x << ELEMENT_BOX3D_PSHIFT;
    mat.m[3][1] = y << ELEMENT_BOX3D_PSHIFT;
    mat.m[3][2] = z << ELEMENT_BOX3D_PSHIFT;
    return mat;
  }

  /*Matrix4 mScale(const float ratio)
  {
    Matrix4 mat;
    mMatInit(&mat);
    mat.m[0][0] *= ratio;
    mat.m[1][1] *= ratio;
    mat.m[2][2] *= ratio;
    return mat;
  }*/

  // ----------------------------------------------
  // Shoelace algorithm to get the surface
  // ----------------------------------------------
  int shoelace(const cUiElement_Box3D_Obj_Node* lcaNodes, const cUiElement_Box3D_Obj_Face& lcFace)
  {
    int surface = 0;

    surface += (lcaNodes[lcFace.P1].x * lcaNodes[lcFace.P2].y) -
               (lcaNodes[lcFace.P2].x * lcaNodes[lcFace.P1].y);
    surface += (lcaNodes[lcFace.P2].x * lcaNodes[lcFace.P3].y) -
               (lcaNodes[lcFace.P3].x * lcaNodes[lcFace.P2].y);
    surface += (lcaNodes[lcFace.P3].x * lcaNodes[lcFace.P1].y) -
               (lcaNodes[lcFace.P1].x * lcaNodes[lcFace.P3].y);

    //return surface * 0.5;
    return (surface + 1) / 2;
  }

  // ----------------------------------------------
  // Shoelace algorithm for triangle visibility
  // ----------------------------------------------
  uint8 is_hidden(const cUiElement_Box3D_Obj_Node* lcaNodes, const cUiElement_Box3D_Obj_Face &lcFace)
  {
    // (x1y2 - y1x2) + (x2y3 - y2x3) ...
    return ( ( (lcaNodes[lcFace.P1].x * lcaNodes[lcFace.P2].y) -
               (lcaNodes[lcFace.P1].x * lcaNodes[lcFace.P1].y)   ) +
             ( (lcaNodes[lcFace.P2].x * lcaNodes[lcFace.P3].y) -
               (lcaNodes[lcFace.P3].x * lcaNodes[lcFace.P2].y)   ) +
             ( (lcaNodes[lcFace.P3].x * lcaNodes[lcFace.P1].y) -
               (lcaNodes[lcFace.P1].x * lcaNodes[lcFace.P3].y)   ) ) < 0 ? false : true;

  }


  // ----------------------------------------------
  // draw flat color (not flat shading)
  // ----------------------------------------------
  void vShow(const cUiElement_Box3D_Obj_Node* lcaNodes, const cUiElement_Box3D_Obj_Face* mcaFaces)
  {
    int i;
    int surface;
    uint16 col = 0x00FF;
    uint16 color;
    cPaintTriBasic::tstTri lstTri;
    cUiElement_Box3D_Obj_Face lcFace;

    i = mObj->miFaceCount - 1;
    do
    {
      lcFace = mcaFaces[i];
      // draw only triangles facing us
      //surface = shoelace(n, i);
      if ((surface = shoelace(lcaNodes, lcFace)) < 0)
      {
        // this is an ugly hack but it 'somehow' fakes shading
        // depending on the size of the surface of the triangle
        // change the color toward brighter/darker
        color = (uint16)((uint32)(col * (-surface)) / 100);
        if (color > 0x00FF) 
        {
          color = 0x00FF;
        }

        lstTri.Dat[0].fx = (float)lcaNodes[lcFace.P1].x;
        lstTri.Dat[0].fy = (float)lcaNodes[lcFace.P1].y;
        lstTri.Dat[1].fx = (float)lcaNodes[lcFace.P2].x;
        lstTri.Dat[1].fy = (float)lcaNodes[lcFace.P2].y;
        lstTri.Dat[2].fx = (float)lcaNodes[lcFace.P3].x;
        lstTri.Dat[2].fy = (float)lcaNodes[lcFace.P3].y;

        lstTri.unCol.u32Col = color;

        mcPaintTriBasic.vTriangle(&lstTri);

        if (mboWireFrame)
        {
          mcRefScreen->vLine(lcaNodes[lcFace.P1].x, lcaNodes[lcFace.P1].y, lcaNodes[lcFace.P3].x, lcaNodes[lcFace.P3].y, 0x404040);
          mcRefScreen->vLine(lcaNodes[lcFace.P2].x, lcaNodes[lcFace.P2].y, lcaNodes[lcFace.P1].x, lcaNodes[lcFace.P1].y, 0x404040);
          mcRefScreen->vLine(lcaNodes[lcFace.P3].x, lcaNodes[lcFace.P3].y, lcaNodes[lcFace.P2].x, lcaNodes[lcFace.P2].y, 0x404040);
        }
      }
    } while (i--);
  }

  virtual void OnLoaded() override
  {
    vDoScene(miGfxWidth, miGfxHeight);
  }
  
  void vDoScene(GfxInt liGfxWidth, GfxInt liGfxHeight)
  {
    Vector3i p;
    int i;

    // rotation
    m_world = mRotateX(mesh_rotation.x);
    m_world = mMultiply(mRotateY(mesh_rotation.y), m_world);
    m_world = mMultiply(mRotateZ(mesh_rotation.z), m_world);
    // scaling
    //m_world = mMultiply(mScale(1.5), m_world);

    // project nodes with world matrix
    for (i = 0; i < mObj->miNodeCount; i++)
    {
      p.x = (m_world.m[0][0] * (mObj->mNodes[i].x >> ELEMENT_BOX3D_PSHIFT)+
             m_world.m[1][0] * (mObj->mNodes[i].y >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[2][0] * (mObj->mNodes[i].z >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[3][0]) / ELEMENT_BOX3D_PRES;

      p.y = (m_world.m[0][1] * (mObj->mNodes[i].x >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[1][1] * (mObj->mNodes[i].y >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[2][1] * (mObj->mNodes[i].z >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[3][1]) / ELEMENT_BOX3D_PRES;

      p.z = (m_world.m[0][2] * (mObj->mNodes[i].x >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[1][2] * (mObj->mNodes[i].y >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[2][2] * (mObj->mNodes[i].z >> ELEMENT_BOX3D_PSHIFT) +
             m_world.m[3][2]) / ELEMENT_BOX3D_PRES;

      // store projected node
      mcaProjNodes[i].x = (64 * p.x) / (64 + p.z) + (liGfxWidth  / 2);
      mcaProjNodes[i].y = (64 * p.y) / (64 + p.z) + (liGfxHeight / 2);
      mcaProjNodes[i].z = p.z;
    }

    mcQSort.vQSort(0, mObj->miFaceCount - 1);

    if (mui8Play)
    {
      mesh_rotation.x += 3;
      mesh_rotation.y += 2;
      mesh_rotation.z++;

      if (mesh_rotation.x > 360) mesh_rotation.x -= 360;
      if (mesh_rotation.y > 360) mesh_rotation.y -= 360;
      if (mesh_rotation.z > 360) mesh_rotation.z -= 360;

      vRepaint();
    }
  }

  virtual void OnTimer(u32 lu32Timediff_ms) override
  {
    UNUSED(lu32Timediff_ms);
    vDoScene(miGfxWidth, miGfxHeight);
  }

  virtual void OnPaint() override
  {    
    vShow(mcaProjNodes, mcaFacesSorted);
  }


  virtual void vMsgHdl(cMsg* lpcMsg) override
  {
    cUiElement::vMsgHdl(lpcMsg);

    switch ((cMsg::tenMsgIDs)lpcMsg->mID)
    {
    case cMsg::tenMsgIDs::nCursor:
    {
      cCursor::tenMsgEvent lenCursorEvent;

      lenCursorEvent = (cCursor::tenMsgEvent)lpcMsg->mSubID;

      switch (lenCursorEvent)
      {
        case cCursor::tenMsgEvent::nPressEndShort:
        {
          if (mstStatus.CursorIn)
          {
            mui8Play = !mui8Play;
          }
        }
        break;

      default:
        break;
      }
    }
    break;

    default:
      break;
    }
  } 
};

#endif  //__UI_ELEMENT_BOX3D_H__
