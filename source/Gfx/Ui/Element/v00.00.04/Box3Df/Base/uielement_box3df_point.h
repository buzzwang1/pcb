#ifndef __UI_ELEMENT_BOX3DF__POINT_H__
#define __UI_ELEMENT_BOX3DF__POINT_H__

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "GfxDefs.h"
#include "math.h"
#include "Vector3f.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef cBaseVector3f cSimple3Df_BasePoint;
typedef cVector3f     cSimple3Df_Point;

class cSimple3Df_Point_Light:public cVector3f
{
  public:
    float mfDistClip;
    float mfDistLight;

    cSimple3Df_Point_Light()
      :cVector3f()
    {}

    cSimple3Df_Point_Light(float lfx, float lfy, float lfz)
      :cVector3f(lfx, lfy, lfz)
    {}

   
    void operator=(cVector3f lfv)
    {
      x = lfv.x; 
      y = lfv.y;
      z = lfv.z;
    }

    inline void Set(cVector3f lfv)
    {
      x = lfv.x;
      y = lfv.y; 
      z = lfv.z;
    }
};

// ----------------------------------------------


class cSimple3Df_2DPoint
{
  public:
    float mfx, mfy;

  cSimple3Df_2DPoint()
  {
    mfx = mfy = 0.0f;
  }

  cSimple3Df_2DPoint(float lfx, float lfy)
  {
    mfx = lfx;
    mfy = lfy;
  }
};

class cSimple3Df_2DzPoint
{
  public:
    float x, y;
    float iz;

  cSimple3Df_2DzPoint()
  {
    x = y = 0.0f;
    iz = 0.0f;
  }

  cSimple3Df_2DzPoint(float lfx, float lfy)
  {
    x = lfx;
    y = lfy;
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__UI_ELEMENT_BOX3DF__POINT_H__
