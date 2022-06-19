#ifndef __UI_ELEMENT_BOX3DF_VIEWER_BASE__H__
#define __UI_ELEMENT_BOX3DF_VIEWER_BASE__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "GfxDefs.h"
#include "cStringTools.h"
#include "rfont.h"
#include "uielement_box3df_world.h"
#include "qsort.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cWorld_3Df_Viewer
{
  public: 

  cVector3f mcPos;
  cVector3f mcLookDir;
  cVector3f mcLookSideDir;
  cVector3f mcDirA;

  cMatrix_4x4f        mcMatWorld;
  cMatrix_4x4f_RotZYX mcMatRotZYX;
  cMatrix_4x4f_RotY   mcMatRotY;
  cMatrix_4x4f_RotZ   mcMatRotZ;

  char mszPos[8*3*3];


  cWorld_3Df_Viewer()
  : mcMatWorld(false)
  {
    mcPos =  { 0.0f, 0.0f, 0.0f };
    mcDirA = { 0.0f, 0.0f, 0.0f };
  }

  char* toString()
  {
    char lszValue[10];
    mszPos[0] = '<';
    mszPos[1] = 0;
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcPos.x, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcPos.y, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcPos.z, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ">\n<");

    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcDirA.x, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcDirA.y, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcDirA.z, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ">\n<");

    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcLookDir.x, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcLookDir.y, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ",");
    cStrTools::szStrCat(mszPos, cStrTools::i8Ftoan(mcLookDir.z, lszValue, 8, 2));
    cStrTools::szStrCat(mszPos, ">");

    return mszPos;
  }

  void vSet(float lfx, float lfy, float lfz, float lfrx, float lfry, float lfrz)
  {
    if (lfrx > 360.0f) lfrx -= 360.0f;
    if (lfry > 360.0f) lfry -= 360.0f;
    if (lfrz > 360.0f) lfrz -= 360.0f;

    if (lfrx < 0.0f) lfrx += 360.0f;
    if (lfry < 0.0f) lfry += 360.0f;
    if (lfrz < 0.0f) lfrz += 360.0f;

    mcPos  = { lfx,  lfy,  lfz };
    mcDirA = { lfrx, lfry, lfrz };

    // rotation
    mcMatRotZYX.Set(mcDirA.x, mcDirA.y, mcDirA.z);
    
    mcMatRotZ.Set(mcDirA.z);
    mcMatRotY.Set(90.0f);

    // Create "Point At" Matrix for camera
    cVector3f vUp = { 0, 1, 0 };
    cVector3f vTarget = { 0, 0, 1 };

    mcLookDir = cMatrix_4x4f::Math::Multiply(mcMatRotZYX, vTarget);
    vUp       = cMatrix_4x4f::Math::Multiply(mcMatRotZ, vUp);
    mcLookSideDir = cMatrix_4x4f::Math::Multiply(mcMatRotY, mcLookDir);
    mcLookSideDir.y = 0.0f;

    vTarget = mcPos.cAdd(mcLookDir);
    cMatrix_4x4f matCamera = cMatrix_4x4f::Math::Matrix_PointAt(mcPos, vTarget, vUp);

    // Make view matrix from camera
    mcMatWorld = cMatrix_4x4f::Math::Matrix_QuickInverse(matCamera);
  }
};


class cWorld_3Df_ViewerWorld_Base: public cQSortExt
{
  public:

  cSimple3Df_Point_Light*         macPoints;
  cSimple3Df_2DzPoint*            macPoints2D;
  cSimple3Df_Face*                macFaces;
  cSimple3Df_Face_Data_Tex_Light* macFaces_Data;

  uint16  mu16PointsCnt;
  uint16  mu16FacesCnt;

  cWorld_3Df_Viewer *mcViewer;

  cWorld_3Df *mpcRefWorld;

  cRFont *mpcRFont;

  cQSort    mcQSort;

  cWorld_3Df_ViewerWorld_Base()
    : mcQSort(this)
  {
    mcViewer = null; 
  }

  cWorld_3Df_ViewerWorld_Base(float lfx, float lfy, float lfz, float lfrx, float lfry, float lfrz, 
                              cWorld_3Df_Viewer *lcViewer,
                              cWorld_3Df *lpcRefWorld,
                              cRFont *lpcRFont)
    : mcQSort(this)
  {
    mcViewer = lcViewer;

    mcViewer->vSet(lfx, lfy, lfz, lfrx, lfry, lfrz);

    mpcRefWorld  = lpcRefWorld;
    mpcRFont     = lpcRFont;

    macPoints     = (cSimple3Df_Point_Light*)         new cSimple3Df_Point_Light[lpcRefWorld->mu16PointsSize];
    macPoints2D   = (cSimple3Df_2DzPoint*)            new cSimple3Df_2DzPoint[lpcRefWorld->mu16PointsSize];
    macFaces      = (cSimple3Df_Face*)                new cSimple3Df_Face[lpcRefWorld->mu16FacesSize];
    macFaces_Data = (cSimple3Df_Face_Data_Tex_Light*) new cSimple3Df_Face_Data_Tex_Light[lpcRefWorld->mu16FacesSize];
  }

  ~cWorld_3Df_ViewerWorld_Base()
  {
    delete[]macPoints;
    delete[]macPoints2D;
    delete[]macFaces;
    delete[]macFaces_Data;
  }

  // QSort Callback
  i32 i32Cmp(i32 li32Reference, i32 li32Target)
  {
    //sort by first point z-Coordinates
    i32 liRef = (i32)(macPoints[macFaces[li32Reference].mau16PntIdx[0]].z + 
                      macPoints[macFaces[li32Reference].mau16PntIdx[1]].z + 
                      macPoints[macFaces[li32Reference].mau16PntIdx[2]].z);

    i32 liTgt = (i32)(macPoints[macFaces[li32Target].mau16PntIdx[0]].z + 
                      macPoints[macFaces[li32Target].mau16PntIdx[1]].z + 
                      macPoints[macFaces[li32Target].mau16PntIdx[2]].z);

    return (liRef - liTgt);
  }

  void vSwap(i32 li32i, i32 li32j)
  {
    macFaces[li32i].Swap(macFaces[li32j]);
    macFaces_Data[li32i].Swap(macFaces_Data[li32j]);
  }

  void AddFace(cSimple3Df_Face &lcNewFace, cSimple3Df_Face_Data_Tex_Light &lcNewFace_Data)
  {
    macFaces_Data[mu16FacesCnt] = lcNewFace_Data;
    macFaces[mu16FacesCnt] = lcNewFace;
    mu16FacesCnt++;
  }

  virtual void World2Viewer() = 0;
  virtual void Show(GfxInt liGfxWidth, GfxInt liGfxHeight, cScreen* lpcScreen, bool bWireFrame) = 0;
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_VIEWER_BASE__H__
