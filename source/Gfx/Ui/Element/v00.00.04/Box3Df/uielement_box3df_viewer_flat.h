#ifndef __UI_ELEMENT_BOX3DF_VIEWER_FLAT__H__
#define __UI_ELEMENT_BOX3DF_VIEWER_FLAT__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "GfxDefs.h"
#include "cStringTools.h"
#include "rfont.h"
#include "painttribrflat.h"
#include "uielement_box3df_world.h"
#include "uielement_box3df_viewer_base.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cWorld_3Df_Viewer_Flat : public cWorld_3Df_ViewerWorld_Base
{
  public:
  cPaintTriBrigthFlat  mcPaintTriBrigthFlat;

  char mszPos[8*3*3];

  cWorld_3Df_Viewer_Flat()
    : cWorld_3Df_ViewerWorld_Base(), mcPaintTriBrigthFlat()
  {}

  cWorld_3Df_Viewer_Flat(float lfx, float lfy, float lfz, float lfrx, float lfry, float lfrz, 
                               cWorld_3Df_Viewer *lcViewer,
                               cWorld_3Df *lpcRefWorld, 
                               cRFont *lpcRFont)
    : cWorld_3Df_ViewerWorld_Base(lfx, lfy, lfz, lfrx, lfry, lfrz, lcViewer, lpcRefWorld, lpcRFont), mcPaintTriBrigthFlat()
  {
  }

  void World2Viewer()
  {
    cSimple3Df_Face                lcNewFace1;
    cSimple3Df_Face_Data_Tex_Light lcNewFace1_Data;
    cSimple3Df_Face                lcNewFace2;
    cSimple3Df_Face_Data_Tex_Light lcNewFace2_Data;

    
    cVector3f lcNormal;
    cVector3f lcCam;
    cVector3f lcCamRay;
    cVector3f lcLight;
    float lfDotProduct;

    /* ClipEbene */
    cVector3f lcClip_Plane_p = { 0.0f, 0.0f, -1.0f };
    cVector3f lcClip_Plane_n = { 0.0f, 0.0f, -1.0f };

    /* Set Light source relative 0 to the Viewer Position */
    /* Else the Light source position must inverse calculated like all other points */
    lcLight = { 0.0f, 0.0f, 0.0f };


    mu16PointsCnt = mpcRefWorld->mu32PointsCnt;

    // Make sure plane normal is indeed normal
    lcClip_Plane_n.vNormalise();

    // project nodes with world matrix
    for (u32 lu32i = 0; lu32i < mpcRefWorld->mu32PointsCnt; lu32i++)
    {
      cVector3f lcVecZwerg(mpcRefWorld->macPoints[lu32i].x, mpcRefWorld->macPoints[lu32i].y, mpcRefWorld->macPoints[lu32i].z);
      macPoints[lu32i] = cMatrix_4x4f::Math::Multiply(mcViewer->mcMatWorld, lcVecZwerg);

      //Distanz Punkt zur ClipEbene
      macPoints[lu32i].mfDistClip = lcClip_Plane_n.x * macPoints[lu32i].x +
                                    lcClip_Plane_n.y * macPoints[lu32i].y +
                                    lcClip_Plane_n.z * macPoints[lu32i].z -
                                    lcClip_Plane_n.fDotProduct(lcClip_Plane_p);

      macPoints[lu32i].mfDistLight = macPoints[lu32i].cSub(lcLight).fLength();
    }

    uint16 lau16IdxInsidePointCnt[3];  int32 lu32insidePointCnt = 0;
    uint16 lau16IdxOutsidePointCnt[3]; int32 li32OutsidePointCnt = 0;

    cSimple3Df_Face                lcFace;
    cSimple3Df_Face_Data_Tex_Light lcFace_Data;

    mu16FacesCnt = 0;

    for (u32 lu32i = 0; lu32i < mpcRefWorld->mu32FacesCnt; lu32i++)
    {
      lu32insidePointCnt = 0;
      li32OutsidePointCnt = 0;

      lcFace = mpcRefWorld->macFaces[lu32i];
      cSimple3Df_Tex_Light_ObjTemplate* lcObj = (cSimple3Df_Tex_Light_ObjTemplate*)mpcRefWorld->macFaces[lu32i].mpcObjRef->cRefTemplateObj;
      lcFace_Data = (cSimple3Df_Face_Data_Tex_Light*)&lcObj->mcFaces_Data[lu32i - mpcRefWorld->macFaces[lu32i].mpcObjRef->FaceIdx];
      lcFace_Data.mu16TexIdx += lcObj->mu16TexIdxOffset;

      lcCamRay = macPoints[lcFace.mau16PntIdx[0]].cNormalise();
      lcNormal = lcFace.cCrossProduct(macPoints);
      lfDotProduct = lcFace.fDotProduct(lcNormal, lcCamRay);

      // draw only triangles facing us
      if (!lcFace.is_hidden(lfDotProduct))
      {
        if (macPoints[lcFace.mau16PntIdx[0]].mfDistClip > 0) { lau16IdxInsidePointCnt[lu32insidePointCnt++] = lcFace.mau16PntIdx[0];  }
                                                        else { lau16IdxOutsidePointCnt[li32OutsidePointCnt++] = lcFace.mau16PntIdx[0];}
        if (macPoints[lcFace.mau16PntIdx[1]].mfDistClip > 0) { lau16IdxInsidePointCnt[lu32insidePointCnt++] = lcFace.mau16PntIdx[1];  }
                                                        else { lau16IdxOutsidePointCnt[li32OutsidePointCnt++] = lcFace.mau16PntIdx[1];}
        if (macPoints[lcFace.mau16PntIdx[2]].mfDistClip > 0) { lau16IdxInsidePointCnt[lu32insidePointCnt++] = lcFace.mau16PntIdx[2];  }
                                                        else { lau16IdxOutsidePointCnt[li32OutsidePointCnt++] = lcFace.mau16PntIdx[2];}

        if (lu32insidePointCnt == 3)
        {
          // All points lie on the inside of plane, so do nothing
          // and allow the triangle to simply pass through
          lcFace_Data.CalcCol(macPoints, lcFace, lcNormal, lcLight);

          macFaces_Data[mu16FacesCnt] = lcFace_Data;
          macFaces[mu16FacesCnt++] = lcFace;
        }
        else if ((lu32insidePointCnt == 1) && (li32OutsidePointCnt == 2))
        {
          // Triangle should be clipped. As two points lie outside
          // the plane, the triangle simply becomes a smaller triangle

          // Copy appearance info to new triangle
          lcNewFace1_Data = lcFace_Data;

          // The inside point is valid, so keep that...
          lcNewFace1.mau16PntIdx[0] = lau16IdxInsidePointCnt[0];

          // but the two new points are at the locations where the
          // original sides of the triangle (lines) intersect with the plane

          // Newly formed single triangle
          float lft;
          lcNewFace1.mau16PntIdx[1] = mu16PointsCnt;
          macPoints[mu16PointsCnt] = lcClip_Plane_p.cIntersectPlane(lcClip_Plane_p, lcClip_Plane_n, macPoints[lau16IdxInsidePointCnt[0]], macPoints[lau16IdxOutsidePointCnt[0]], lft);
          macPoints[mu16PointsCnt++].mfDistLight = macPoints[lu32i].cSub(lcLight).fLength();

          lcNewFace1.mau16PntIdx[2] = mu16PointsCnt;
          macPoints[mu16PointsCnt] = lcClip_Plane_p.cIntersectPlane(lcClip_Plane_p, lcClip_Plane_n, macPoints[lau16IdxInsidePointCnt[0]], macPoints[lau16IdxOutsidePointCnt[1]], lft);
          macPoints[mu16PointsCnt++].mfDistLight = macPoints[lu32i].cSub(lcLight).fLength();

          lcNewFace1_Data.CalcCol(macPoints, lcNewFace1, lcNormal, lcLight);
          AddFace(lcNewFace1, lcNewFace1_Data);
        }
        else if ((lu32insidePointCnt == 2) && (li32OutsidePointCnt == 1))
        {
          // Triangle should be clipped. As two points lie inside the plane,
          // the clipped triangle becomes a "quad". Fortunately, we can
          // represent a quad with two new triangles

          // Copy appearance info to new triangles
          lcNewFace1_Data = lcFace_Data;
          lcNewFace2_Data = lcFace_Data;

          // The first triangle consists of the two inside points and a new
          // point determined by the location where one side of the triangle
          // intersects with the plane
          float lft;
          lcNewFace1.mau16PntIdx[0] = lau16IdxInsidePointCnt[0];
          lcNewFace1.mau16PntIdx[1] = lau16IdxInsidePointCnt[1];

          lcNewFace1.mau16PntIdx[2] = mu16PointsCnt;
          macPoints[mu16PointsCnt] = lcClip_Plane_p.cIntersectPlane(lcClip_Plane_p, lcClip_Plane_n, macPoints[lau16IdxInsidePointCnt[0]], macPoints[lau16IdxOutsidePointCnt[0]], lft);
          macPoints[mu16PointsCnt++].mfDistLight = macPoints[lu32i].cSub(lcLight).fLength();
          lcNewFace1_Data.CalcCol(macPoints, lcNewFace1, lcNormal, lcLight);
          AddFace(lcNewFace1, lcNewFace1_Data);

          // The second triangle is composed of one of he inside points, a
          // new point determined by the intersection of the other side of the
          // triangle and the plane, and the newly created point above
          lcNewFace2.mau16PntIdx[0] = lau16IdxInsidePointCnt[1];
          lcNewFace2.mau16PntIdx[1] = lcNewFace1.mau16PntIdx[2];

          lcNewFace2.mau16PntIdx[2] = mu16PointsCnt;
          macPoints[mu16PointsCnt] = lcClip_Plane_p.cIntersectPlane(lcClip_Plane_p, lcClip_Plane_n, macPoints[lau16IdxInsidePointCnt[1]], macPoints[lau16IdxOutsidePointCnt[0]], lft);
          macPoints[mu16PointsCnt++].mfDistLight = macPoints[lu32i].cSub(lcLight).fLength();

          lcNewFace2_Data.CalcCol(macPoints, lcNewFace2, lcNormal, lcLight);
          AddFace(lcNewFace2, lcNewFace2_Data);
        }
      }
    }

    //Quicksort_Faces(0, mu16FacesCnt - 1);
    mcQSort.vQSort(0, mu16FacesCnt - 1);
  }


  void Show(GfxInt liGfxWidth, GfxInt liGfxHeight, cScreen* lpcScreen, bool bWireFrame)
  {
    u32 lu32i, lu32t;

    mcPaintTriBrigthFlat.vSetScreen(lpcScreen);

    // project nodes with world matrix
    for (lu32i = 0; lu32i < mu16PointsCnt; lu32i++)
    {
      // store projected node
      macPoints2D[lu32i].x = ((liGfxWidth  / 2) + (50.0f * macPoints[lu32i].x) / (-(macPoints[lu32i].z - 0.0f)));
      macPoints2D[lu32i].y = ((liGfxHeight / 2) - (50.0f * macPoints[lu32i].y) / (-(macPoints[lu32i].z - 0.0f)));
      
      //macPoints2D[lu32i].iz = 1.0f / macPoints[lu32i].z; //Only needed for texture mapping
    }

    cSimple3Df_Face lcFace;
    cSimple3Df_Face_Data_Tex_Light lcFaceData;
    cPaintTriBrigthFlat::tstTri lcTxMapTri;

    for (lu32i = 0; lu32i < mu16FacesCnt; lu32i++)
    {
      lcFace = macFaces[lu32i];
      lcFaceData = macFaces_Data[lu32i];

      for (lu32t = 0; lu32t < 3; lu32t++)
      {
        lcTxMapTri.unCol.u32Col  = lcFaceData.mu32BaseCol;
        lcTxMapTri.u8B           = lcFaceData.lau8B[lu32t];
        lcTxMapTri.Dat[lu32t].fx = macPoints2D[lcFace.mau16PntIdx[lu32t]].x;
        lcTxMapTri.Dat[lu32t].fy = macPoints2D[lcFace.mau16PntIdx[lu32t]].y;
      }

      mcPaintTriBrigthFlat.vTriangle(&lcTxMapTri);
    }

    if (mpcRFont != null)
    {
      mpcRFont->mui32Col = lpcScreen->u32GetCol(0xFFFFFF);
      mpcRFont->i8PutStringXY(4, 10, mcViewer->toString(), lpcScreen);
    }


    if (bWireFrame)
    {
      for (lu32i = 0; lu32i < mu16FacesCnt; lu32i++)
      {
        lcFace = macFaces[lu32i];

        lpcScreen->vLine((GfxInt)macPoints2D[lcFace.mau16PntIdx[0]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[0]].y, (GfxInt)macPoints2D[lcFace.mau16PntIdx[1]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[1]].y, 0x00FF00);
        lpcScreen->vLine((GfxInt)macPoints2D[lcFace.mau16PntIdx[1]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[1]].y, (GfxInt)macPoints2D[lcFace.mau16PntIdx[2]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[2]].y, 0x00FF00);
        lpcScreen->vLine((GfxInt)macPoints2D[lcFace.mau16PntIdx[2]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[2]].y, (GfxInt)macPoints2D[lcFace.mau16PntIdx[0]].x, (GfxInt)macPoints2D[lcFace.mau16PntIdx[0]].y, 0x00FF00);
      }
    }
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_VIEWER_FLAT__H__
