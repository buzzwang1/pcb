#ifndef __UI_ELEMENT_BOX3DF_FACE__H__
#define __UI_ELEMENT_BOX3DF_FACE__H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "GfxDefs.h"
#include "math.h"
#include "uielement_box3df_point.h"
#include "Matrix4x4.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct
{
  GfxInt iGfxX;
  GfxInt iGfxY;
}tstSimple3Df_Face_Tex;

class cSimple3Df_Face
{
public:
  uint16 mau16PntIdx[3];

  cSimple3Df_Face()
  {
    mau16PntIdx[0] = mau16PntIdx[1] = mau16PntIdx[2] = 0;
  }

  cSimple3Df_Face(uint16 lu16Idx1, uint16 lu16Idx2, uint16 lu16Idx3)
  {
    mau16PntIdx[0] = lu16Idx1;
    mau16PntIdx[1] = lu16Idx2;
    mau16PntIdx[2] = lu16Idx3;
  }

  inline void Swap(cSimple3Df_Face &lcFace)
  {
    cSimple3Df_Face lcTemp;

    lcTemp = lcFace;
    lcFace = *this;
    *this = lcTemp;
  }

  cVector3f cMid(cVector3f* lcPoints)
  {
    cVector3f lcRet;

    lcRet.x = (lcPoints[mau16PntIdx[0]].x + lcPoints[mau16PntIdx[1]].x + lcPoints[mau16PntIdx[2]].x) / 3;
    lcRet.y = (lcPoints[mau16PntIdx[0]].y + lcPoints[mau16PntIdx[1]].y + lcPoints[mau16PntIdx[2]].y) / 3;
    lcRet.z = (lcPoints[mau16PntIdx[0]].z + lcPoints[mau16PntIdx[1]].z + lcPoints[mau16PntIdx[2]].z) / 3;

    return lcRet;
  }

  cVector3f cMid(cSimple3Df_Point_Light* lcPoints)
  {
    cVector3f lcRet;

    lcRet.x = (lcPoints[mau16PntIdx[0]].x + lcPoints[mau16PntIdx[1]].x + lcPoints[mau16PntIdx[2]].x) / 3;
    lcRet.y = (lcPoints[mau16PntIdx[0]].y + lcPoints[mau16PntIdx[1]].y + lcPoints[mau16PntIdx[2]].y) / 3;
    lcRet.z = (lcPoints[mau16PntIdx[0]].z + lcPoints[mau16PntIdx[1]].z + lcPoints[mau16PntIdx[2]].z) / 3;

    return lcRet;
  }

  cVector3f cCrossProduct(cVector3f* lacPoints)
  {
    cVector3f lcAxB;
    cVector3f lcVecA;
    cVector3f lcVecB;

    lcVecA.x = lacPoints[mau16PntIdx[0]].x - lacPoints[mau16PntIdx[1]].x;
    lcVecA.y = lacPoints[mau16PntIdx[0]].y - lacPoints[mau16PntIdx[1]].y;
    lcVecA.z = lacPoints[mau16PntIdx[0]].z - lacPoints[mau16PntIdx[1]].z;

    lcVecB.x = lacPoints[mau16PntIdx[2]].x - lacPoints[mau16PntIdx[1]].x;
    lcVecB.y = lacPoints[mau16PntIdx[2]].y - lacPoints[mau16PntIdx[1]].y;
    lcVecB.z = lacPoints[mau16PntIdx[2]].z - lacPoints[mau16PntIdx[1]].z;

    lcAxB.x = lcVecA.y * lcVecB.z - lcVecA.z * lcVecB.y;
    lcAxB.y = lcVecA.z * lcVecB.x - lcVecA.x * lcVecB.z;
    lcAxB.z = lcVecA.x * lcVecB.y - lcVecA.y * lcVecB.x;

    lcAxB.vNormalise();

    return lcAxB;
  }

  cVector3f cCrossProduct(cSimple3Df_Point_Light* lacPoints)
  {
    cVector3f lcAxB;
    cVector3f lcVecA;
    cVector3f lcVecB;

    lcVecA.x = lacPoints[mau16PntIdx[0]].x - lacPoints[mau16PntIdx[1]].x;
    lcVecA.y = lacPoints[mau16PntIdx[0]].y - lacPoints[mau16PntIdx[1]].y;
    lcVecA.z = lacPoints[mau16PntIdx[0]].z - lacPoints[mau16PntIdx[1]].z;

    lcVecB.x = lacPoints[mau16PntIdx[2]].x - lacPoints[mau16PntIdx[1]].x;
    lcVecB.y = lacPoints[mau16PntIdx[2]].y - lacPoints[mau16PntIdx[1]].y;
    lcVecB.z = lacPoints[mau16PntIdx[2]].z - lacPoints[mau16PntIdx[1]].z;

    lcAxB.x = lcVecA.y * lcVecB.z - lcVecA.z * lcVecB.y;
    lcAxB.y = lcVecA.z * lcVecB.x - lcVecA.x * lcVecB.z;
    lcAxB.z = lcVecA.x * lcVecB.y - lcVecA.y * lcVecB.x;

    lcAxB.vNormalise();

    return lcAxB;
  }

  float fDist(cVector3f lcPoint1, cVector3f lcPoint2)
  {
    return sqrtf((lcPoint1.x - lcPoint2.x) * (lcPoint1.x - lcPoint2.x) +
                 (lcPoint1.y - lcPoint2.y) * (lcPoint1.y - lcPoint2.y) +
                 (lcPoint1.z - lcPoint2.z) * (lcPoint1.z - lcPoint2.z));
  }

  float fDotProduct(cVector3f lcVec1, cVector3f lcVec2)
  {
    return (lcVec1.x * lcVec2.x + 
            lcVec1.y * lcVec2.y +
            lcVec1.z * lcVec2.z);
  }

  uint8 is_hidden(float lfDotProd)
  {
    return lfDotProd < 0 ? false : true;
  }
};


class cSimple3Df_Face_Data_Col
{
  public:
    uint32 mu32BaseCol;

    cSimple3Df_Face_Data_Col()
    {
      mu32BaseCol = 0;
    }

    cSimple3Df_Face_Data_Col(uint32 lcol)
    {
      mu32BaseCol = lcol;
    }

    inline void Swap(cSimple3Df_Face_Data_Col &lcFace_Data)
    {
      cSimple3Df_Face_Data_Col lcTemp;

      lcTemp = lcFace_Data;
      lcFace_Data = *this;
      *this = lcTemp;
    }
};

class cSimple3Df_Face_Data_Tex
{
  public:
    uint32 mu32BaseCol;

    tstSimple3Df_Face_Tex mastTex[3];

    uint16 mu16TexIdx;

    cSimple3Df_Face_Data_Tex()
    {
      mu32BaseCol = 0;
      mastTex[0].iGfxX = mastTex[1].iGfxX = mastTex[2].iGfxX = 0;
      mastTex[0].iGfxY = mastTex[1].iGfxY = mastTex[2].iGfxY = 0;
      mu16TexIdx   = 0;
    }

    cSimple3Df_Face_Data_Tex(uint32 lu32col,
                             GfxInt liGfxTexX1, GfxInt liGfxTexY1, GfxInt liGfxTexX2,
                             GfxInt liGfxTexY2, GfxInt liGfxTexX3, GfxInt liGfxTexY3,
                             uint16 lu16TexIdx)
    {
      mu32BaseCol = lu32col;

      mastTex[0].iGfxX = liGfxTexX1;
      mastTex[1].iGfxX = liGfxTexX2;
      mastTex[2].iGfxX = liGfxTexX3;
      mastTex[0].iGfxY = liGfxTexY1;
      mastTex[1].iGfxY = liGfxTexY2;
      mastTex[2].iGfxY = liGfxTexY3;

      mu16TexIdx   = lu16TexIdx;
    }

    inline void Swap(cSimple3Df_Face_Data_Tex &lcFace_Data)
    {
      cSimple3Df_Face_Data_Tex lcTemp;

      lcTemp = lcFace_Data;
      lcFace_Data = *this;
      *this = lcTemp;
    }
};

class cSimple3Df_Face_Data_Tex_Light: public cSimple3Df_Face_Data_Tex
{
  public:
    uint8  lau8B[3];

    cSimple3Df_Face_Data_Tex_Light()
      :cSimple3Df_Face_Data_Tex()
    {
      lau8B[0] = lau8B[1] = lau8B[2] = 255;
    }

    cSimple3Df_Face_Data_Tex_Light(uint32 lu32Col,
                                   GfxInt liGfxTexX1, GfxInt liGfxTexY1, GfxInt liGfxTexX2, GfxInt liGfxTexY2, GfxInt liGfxTexX3, GfxInt liGfxTexY3, uint16 lu16TexIdx)
                                   :cSimple3Df_Face_Data_Tex(lu32Col, liGfxTexX1, liGfxTexY1, liGfxTexX2, liGfxTexY2, liGfxTexX3, liGfxTexY3, lu16TexIdx)
    {
      lau8B[0] = lau8B[1] = lau8B[2] = 255;
    }

    void operator=(cSimple3Df_Face_Data_Tex &lcFace)
    {
      mastTex[0]  = lcFace.mastTex[0];
      mastTex[1]  = lcFace.mastTex[1];
      mastTex[2]  = lcFace.mastTex[2];
      mu32BaseCol = lcFace.mu32BaseCol;
      mu16TexIdx  = lcFace.mu16TexIdx;
    }

    inline void Swap(cSimple3Df_Face_Data_Tex_Light &lcFace_Data)
    {
      cSimple3Df_Face_Data_Tex_Light lcTemp;

      lcTemp = lcFace_Data;
      lcFace_Data = *this;
      *this = lcTemp;
    }


    float fMidLight(cSimple3Df_Point_Light* lacPoints, cSimple3Df_Face &lcFace)
    {
      return (lacPoints[lcFace.mau16PntIdx[0]].mfDistLight + lacPoints[lcFace.mau16PntIdx[0]].mfDistLight + lacPoints[lcFace.mau16PntIdx[0]].mfDistLight) / 3;
    }

    void CalcCol(cSimple3Df_Point_Light* lacPoints, cSimple3Df_Face &lcFace, cVector3f lcNormal, cVector3f lcLightPos)
    {
      uint8  lui8tPntCnt;
      uint16 lu8Brightness;
      float lfDotProduct;

      cVector3f lcLightRay;

      for (lui8tPntCnt = 0; lui8tPntCnt < 3; lui8tPntCnt++)
      {
        lcLightRay = lacPoints[lcFace.mau16PntIdx[lui8tPntCnt]].cSub(lcLightPos).cNormalise();
        lfDotProduct = lcFace.fDotProduct(lcNormal, lcLightRay);

        float dp = -lfDotProduct / (lacPoints[lcFace.mau16PntIdx[lui8tPntCnt]].mfDistLight / 25.0f);

        if (dp > 0)
        {
          lu8Brightness = 255;
          lu8Brightness = (uint16)((float)lu8Brightness * dp);
          if (lu8Brightness > 255) lu8Brightness = 255;

          lau8B[lui8tPntCnt] = (u8)lu8Brightness;
        }
        else
        {
          lau8B[lui8tPntCnt] = 0;
        }
      }
    }

    u8 u8GetCol()
    {
      uint16 lu16Brightness;
      lu16Brightness = lau8B[0] + lau8B[1] + lau8B[2];
      return (u8)(lu16Brightness / 3);
    }
};



#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF_FACE__H__
