#ifndef __UI_ELEMENT_BOX3DF_OBJECTS__H__
#define __UI_ELEMENT_BOX3DF_OBJECTS__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "uielement_box3df_face.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cSimple3Df_ObjTemplate
{
public:
  u16 mu16PointCnt;
  u16 mu16FaceCnt;
  u16 mu16TexIdxOffset;

  cSimple3Df_Point*         macPoints;
  cSimple3Df_Face*          macFaces;

  void vSet(int liPointCount, int liFaceCount, cSimple3Df_Point* lacPoints, cSimple3Df_Face* lacFaces, u16 lu16TexIdxOffset)
  {
    mu16PointCnt     = liPointCount;
    mu16FaceCnt      = liFaceCount;
    mu16TexIdxOffset = lu16TexIdxOffset;

    macPoints    = lacPoints;
    macFaces     = lacFaces;
  }
};

class Simple3Df_ObjTemplate_Cube : public cSimple3Df_ObjTemplate
{
public:
  enum
  {
    NODECOUNT = 8,
    TRICOUNT  = 12
  };

  static const cSimple3Df_Point macPointsDat[NODECOUNT];
  static const cSimple3Df_Face  macFacesDat[TRICOUNT];

  Simple3Df_ObjTemplate_Cube(u16 lu16TexIdxOffset)
  {
    vSet(NODECOUNT, TRICOUNT, (cSimple3Df_Point*)macPointsDat, (cSimple3Df_Face*)macFacesDat, lu16TexIdxOffset);
  }
};

class Simple3Df_ObjTemplate_Torus : public cSimple3Df_ObjTemplate
{
public:

  enum
  {
    NODECOUNT = 36,
    TRICOUNT  = 72
  };

  static const cSimple3Df_Point macPointsDat[NODECOUNT];
  static const cSimple3Df_Face  macFacesDat[TRICOUNT];

  Simple3Df_ObjTemplate_Torus(u16 lu16TexIdxOffset)
  {
    vSet(NODECOUNT, TRICOUNT, (cSimple3Df_Point*)macPointsDat, (cSimple3Df_Face*)macFacesDat, lu16TexIdxOffset);
  }
};

class Simple3Df_ObjTemplate_Sphere : public cSimple3Df_ObjTemplate
{
public:

  enum
  {
    NODECOUNT = 32,
    TRICOUNT  = 60
  };

  static const cSimple3Df_Point macPointsDat[NODECOUNT];
  static const cSimple3Df_Face  macFacesDat[TRICOUNT];

  Simple3Df_ObjTemplate_Sphere(u16 lu16TexIdxOffset)
  {
    vSet(NODECOUNT, TRICOUNT, (cSimple3Df_Point*)macPointsDat, (cSimple3Df_Face*)macFacesDat, lu16TexIdxOffset);
  }
};

class Simple3Df_ObjTemplate_Monkey : public cSimple3Df_ObjTemplate
{
public:

  enum
  {
    NODECOUNT = 122,
    TRICOUNT  = 223
  };

  static const cSimple3Df_Point macPointsDat[NODECOUNT];
  static const cSimple3Df_Face  macFacesDat[TRICOUNT];

  Simple3Df_ObjTemplate_Monkey(u16 lu16TexIdxOffset)
  {
    vSet(NODECOUNT, TRICOUNT, (cSimple3Df_Point*)macPointsDat, (cSimple3Df_Face*)macFacesDat, lu16TexIdxOffset);
  }
};



class cSimple3Df_Tex_Light_ObjTemplate: public cSimple3Df_ObjTemplate
{
public:
  cSimple3Df_Face_Data_Tex* mcFaces_Data;

  void vSet(int liPointCount, int liFaceCount, cSimple3Df_Point* lcPoints, cSimple3Df_Face* lcFaces, cSimple3Df_Face_Data_Tex* lcFaces_Data, u16 lu16TexIdxOffset)
  {
    cSimple3Df_ObjTemplate::vSet(liPointCount, liFaceCount, lcPoints, lcFaces, lu16TexIdxOffset);
    mcFaces_Data     = lcFaces_Data;
  }
};


class Simple3Df_Tex_Light_ObjTemplate_Cube : public cSimple3Df_Tex_Light_ObjTemplate 
{
public:
  Simple3Df_ObjTemplate_Cube mcRootObj;

  static const cSimple3Df_Face_Data_Tex  faces_data[Simple3Df_ObjTemplate_Cube::TRICOUNT];

  Simple3Df_Tex_Light_ObjTemplate_Cube(u16 lu16TexIdxOffset)
  : mcRootObj(lu16TexIdxOffset)
  {
    cSimple3Df_Tex_Light_ObjTemplate::vSet(Simple3Df_ObjTemplate_Cube::NODECOUNT, Simple3Df_ObjTemplate_Cube::TRICOUNT, (cSimple3Df_Point*)mcRootObj.macPointsDat, (cSimple3Df_Face*)mcRootObj.macFacesDat, (cSimple3Df_Face_Data_Tex*)faces_data, lu16TexIdxOffset);
  }
};

class Simple3Df_Tex_Light_ObjTemplate_Torus : public cSimple3Df_Tex_Light_ObjTemplate 
{
public:
  Simple3Df_ObjTemplate_Torus mcRootObj;

  static const cSimple3Df_Face_Data_Tex  faces_data[Simple3Df_ObjTemplate_Torus::TRICOUNT];

  Simple3Df_Tex_Light_ObjTemplate_Torus(u16 lu16TexIdxOffset)
  : mcRootObj(lu16TexIdxOffset)
  {
    cSimple3Df_Tex_Light_ObjTemplate::vSet(Simple3Df_ObjTemplate_Torus::NODECOUNT, Simple3Df_ObjTemplate_Torus::TRICOUNT, (cSimple3Df_Point*)mcRootObj.macPointsDat, (cSimple3Df_Face*)mcRootObj.macFacesDat, (cSimple3Df_Face_Data_Tex*)faces_data, lu16TexIdxOffset);
  }
};

class Simple3Df_Tex_Light_ObjTemplate_Sphere : public cSimple3Df_Tex_Light_ObjTemplate 
{
public:
  Simple3Df_ObjTemplate_Sphere mcRootObj;

  static const cSimple3Df_Face_Data_Tex  faces_data[Simple3Df_ObjTemplate_Sphere::TRICOUNT];

  Simple3Df_Tex_Light_ObjTemplate_Sphere(u16 lu16TexIdxOffset)
  : mcRootObj(lu16TexIdxOffset)
  {
    cSimple3Df_Tex_Light_ObjTemplate::vSet(Simple3Df_ObjTemplate_Sphere::NODECOUNT, Simple3Df_ObjTemplate_Sphere::TRICOUNT, (cSimple3Df_Point*)mcRootObj.macPointsDat, (cSimple3Df_Face*)mcRootObj.macFacesDat, (cSimple3Df_Face_Data_Tex*)faces_data, lu16TexIdxOffset);
  }
};

class Simple3Df_Tex_Light_ObjTemplate_Monkey : public cSimple3Df_Tex_Light_ObjTemplate 
{
public:
  Simple3Df_ObjTemplate_Monkey mcRootObj;

  static const cSimple3Df_Face_Data_Tex  faces_data[Simple3Df_ObjTemplate_Monkey::TRICOUNT];

  Simple3Df_Tex_Light_ObjTemplate_Monkey(u16 lu16TexIdxOffset)
  : mcRootObj(lu16TexIdxOffset)
  {
    cSimple3Df_Tex_Light_ObjTemplate::vSet(Simple3Df_ObjTemplate_Monkey::NODECOUNT, Simple3Df_ObjTemplate_Monkey::TRICOUNT, (cSimple3Df_Point*)mcRootObj.macPointsDat, (cSimple3Df_Face*)mcRootObj.macFacesDat, (cSimple3Df_Face_Data_Tex*)faces_data, lu16TexIdxOffset);
  }
};


class cSimple3Df_Obj
{
  public:
    float x,  y,  z;
    float rx, ry, rz;

    uint16 PointIdx;
    uint16 FaceIdx;

    cSimple3Df_ObjTemplate* cRefTemplateObj;

  cSimple3Df_Obj()
  {
    x  = y  = z  = 0;
    rx = ry = rz = 0;
    PointIdx = 0;
    FaceIdx = 0;
    cRefTemplateObj = null;
  }

  cSimple3Df_Obj(float lx, float ly, float lz, float lrx, float lry, float lrz, cSimple3Df_ObjTemplate* lcRefTemplateObj)
  {
    x  = lx;    y  = ly;    z  = lz;
    rx = lrx;   ry = lry;   rz = lrz;
    PointIdx = 0;
    FaceIdx = 0;
    cRefTemplateObj = lcRefTemplateObj;
  }
};



#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_OBJECTS__H__
