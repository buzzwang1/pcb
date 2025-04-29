#ifndef __UI_ELEMENT_BOX3DF_WORLD__H__
#define __UI_ELEMENT_BOX3DF_WORLD__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "GfxDefs.h"
#include "uielement_box3df_obj.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cSimple3Df_World_Face: public cSimple3Df_Face
{
  public:
    cSimple3Df_Obj* mpcObjRef;

    cSimple3Df_World_Face()
      :cSimple3Df_Face()
    {
      mpcObjRef = null;
    }

    cSimple3Df_World_Face(uint16 lu16Idx1, uint16 lu16Idx2, uint16 lu16Idx3,
                          cSimple3Df_Obj* lpcObjRef)
      :cSimple3Df_Face(lu16Idx1, lu16Idx2, lu16Idx3)
    {
      mpcObjRef = lpcObjRef;
    }
};


class cWorld_3Df
{
  public:

  cSimple3Df_Point*        macPoints;
  cSimple3Df_World_Face*   macFaces;
  cSimple3Df_Obj**         macObj;

  uint16  mu16PointsSize;
  uint16  mu16FacesSize;
  uint16  mu16ObjsSize;

  uint16  mu16PointsCnt;
  uint16  mu16FacesCnt;
  uint16  mu16ObjsCnt;

  cWorld_3Df(uint16 lu16PointsCnt, uint16 lu16FacesCnt, uint16 lu16ObjsCnt)
  {
    mu16PointsCnt = 0;
    mu16FacesCnt  = 0;
    mu16ObjsCnt   = 0;

    mu16PointsSize = lu16PointsCnt;
    mu16FacesSize  = lu16FacesCnt;
    mu16ObjsSize   = lu16ObjsCnt;

    macPoints = (cSimple3Df_Point*)      new cSimple3Df_Point[lu16PointsCnt];
    macFaces  = (cSimple3Df_World_Face*) new cSimple3Df_World_Face[lu16PointsCnt];
    macObj    =                          new cSimple3Df_Obj*[lu16PointsCnt];
  }

  ~cWorld_3Df()
  {
    delete []macPoints;
    delete []macFaces;
    delete []macObj;
  }

  void AddObj(cSimple3Df_Obj *lpcObj)
  {
    uint16 lui16t;
    uint16 lui16i;

    lpcObj->PointIdx = mu16PointsCnt;
    lpcObj->FaceIdx  = mu16FacesCnt;

    //-------------------------------------------------

    cMatrix_4x4f        lcMatObj(false);
    cMatrix_4x4f_RotXYZ lcMatRotXYZ;
    cMatrix_4x4f_Trans  lcMatTrans;

    // rotation
    lcMatRotXYZ.Set(lpcObj->rx, lpcObj->ry, lpcObj->rz);
    lcMatTrans.Set(lpcObj->x, lpcObj->y, lpcObj->z);

    lcMatObj = cMatrix_4x4f::Math::Multiply(lcMatTrans, lcMatRotXYZ);

    lui16i = mu16PointsCnt;
    for (lui16t = 0; lui16t < lpcObj->cRefTemplateObj->mu16PointCnt; lui16t++)
    {
      macPoints[lui16i++] = cMatrix_4x4f::Math::Multiply(lcMatObj, lpcObj->cRefTemplateObj->macPoints[lui16t]);
    }

    lui16i = mu16FacesCnt;
    for (lui16t = 0; lui16t < lpcObj->cRefTemplateObj->mu16FaceCnt; lui16t++)
    {
      macFaces[lui16i].mau16PntIdx[0] = mu16PointsCnt + lpcObj->cRefTemplateObj->macFaces[lui16t].mau16PntIdx[0];
      macFaces[lui16i].mau16PntIdx[1] = mu16PointsCnt + lpcObj->cRefTemplateObj->macFaces[lui16t].mau16PntIdx[1];
      macFaces[lui16i].mau16PntIdx[2] = mu16PointsCnt + lpcObj->cRefTemplateObj->macFaces[lui16t].mau16PntIdx[2];

      macFaces[lui16i].mpcObjRef = lpcObj;
      lui16i++;
    }

    macObj[mu16ObjsCnt] = lpcObj;

    mu16PointsCnt += lpcObj->cRefTemplateObj->mu16PointCnt;
    mu16FacesCnt  += lpcObj->cRefTemplateObj->mu16FaceCnt;
    mu16ObjsCnt++;
  }

  void UpdateObj(cSimple3Df_Obj *lpcObj)
  {
    uint16 lui16t;

    cMatrix_4x4f        lcMatObj(false);
    cMatrix_4x4f_RotXYZ lcMatRotXYZ;
    cMatrix_4x4f_Trans  lcMatTrans;

    // rotation
    lcMatRotXYZ.Set(lpcObj->rx, lpcObj->ry, lpcObj->rz);
    lcMatTrans.Set(lpcObj->x, lpcObj->y, lpcObj->z);
    lcMatObj = cMatrix_4x4f::Math::Multiply(lcMatTrans, lcMatRotXYZ);

    for (lui16t = 0; lui16t < lpcObj->cRefTemplateObj->mu16PointCnt; lui16t++)
    {
      macPoints[lui16t + lpcObj->PointIdx] = cMatrix_4x4f::Math::Multiply(lcMatObj, lpcObj->cRefTemplateObj->macPoints[lui16t]);
    }
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_WORLD__H__
