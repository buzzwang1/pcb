#ifndef __MATH_VECTOR_3F_H__
#define __MATH_VECTOR_3F_H__

#ifdef USE_ARM_MATH
  #include "arm_math.h"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "math.h"

class cBaseVector3f
{
  public:
  float x, y, z;
};


class cVector3f: public cBaseVector3f
{
public:
  #ifdef USE_ARM_MATH
    float w;
  #endif

  cVector3f()
  {
    //x = y = z = 0;
    #ifdef USE_ARM_MATH
      w = 1.0f;
    #endif
  }

  cVector3f(float lx, float ly, float lz)
  {
    x = lx;
    y = ly;
    z = lz;
    #ifdef USE_ARM_MATH
      w = 1.0f;
    #endif
  }

  inline void vAdd(cVector3f &v)
  {
    x += v.x; y += v.y; z += v.z;
  }

  inline cVector3f cAdd(cVector3f &v)
  {
    return { x + v.x, y + v.y, z + v.z };
  }

  inline cVector3f cAdd(cVector3f &v1, cVector3f &v2)
  {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
  }

  inline void vSub(cVector3f &v)
  {
    x -= v.x; y -= v.y; z -= v.z;
  }

  inline cVector3f cSub(cVector3f &v)
  {
    return { x - v.x, y - v.y, z - v.z };
  }

  inline cVector3f cSub(cVector3f &v1, cVector3f &v2)
  {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
  }

  inline void vMul(float k)
  {
    x *= k; y *= k; z *= k;
  }

  inline cVector3f cMul(float k)
  {
    return { x * k, y * k, z * k };
  }

  inline cVector3f cMul(cVector3f &v1, float k)
  {
    return { v1.x * k, v1.y * k, v1.z * k };
  }

  inline void vDiv(float k)
  {
    x /= k; y /= k; z /= k;
  }

  inline cVector3f cDiv(float k)
  {
    return { x / k, y / k, z / k };
  }

  inline cVector3f cDiv(cVector3f &v1, float k)
  {
    return { v1.x / k, v1.y / k, v1.z / k };
  }

  inline float fDotProduct(cVector3f &v)
  {
    #ifdef USE_ARM_MATH
      float lfRet;
      arm_dot_prod_f32((float*)(this), (float*)(&v), 3, &lfRet);
      return lfRet;
    #else
      return x*v.x + y*v.y + z*v.z;
    #endif
  }

  inline float fDotProduct(cVector3f &v1, cVector3f &v2)
  {
    #ifdef USE_ARM_MATH
      float lfRet;
      arm_dot_prod_f32((float*)(&v1), (float*)(&v2), 3, &lfRet);
      return lfRet;
    #else
      return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
    #endif
  }

  inline float fLength()
  {
    #ifdef USE_ARM_MATH
      float lfRet;
      arm_sqrt_f32(x*x + y*y + z*z, &lfRet);
      return lfRet;
    #else
      return sqrtf(x*x + y*y + z*z);
    #endif    
  }

  inline float fLength(cVector3f &v)
  {
    #ifdef USE_ARM_MATH
      float lfRet;
      arm_sqrt_f32((v.x*v.x + v.y * v.y + v.z * v.z), &lfRet);
      return lfRet;
    #else
      return sqrtf((v.x*v.x + v.y*v.y + v.z*v.z));
    #endif
  }

  inline void vNormalise()
  {
    vDiv(fLength());
  }

  inline cVector3f cNormalise()
  {
    float l = fLength();
    return { x / l, y / l, z / l };
  }

  inline cVector3f cNormalise(cVector3f &v)
  {
    float l = fLength(v);
    return { v.x / l, v.y / l, v.z / l };
  }

  cVector3f cCrossProduct(cVector3f &v)
  {
    cVector3f ret;
    ret.x = y * v.z - z * v.y;
    ret.y = z * v.x - x * v.z;
    ret.z = x * v.y - y * v.x;
    return ret;
  }

  cVector3f cCrossProduct(cVector3f &v1, cVector3f &v2)
  {
    cVector3f v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
  }

  cVector3f cIntersectPlane(cVector3f &plane_p, cVector3f &plane_n, cVector3f &lineStart, cVector3f &lineEnd, float &t)
  {
    plane_n.vNormalise();
    float plane_d = -plane_n.fDotProduct(plane_p);
    float ad = plane_n.fDotProduct(lineStart);
    float bd = plane_n.fDotProduct(lineEnd);
    t = (-plane_d - ad) / (bd - ad);
    cVector3f lineStartToEnd  = cSub(lineEnd, lineStart);
    cVector3f lineToIntersect = cMul(lineStartToEnd, t);
    return cAdd(lineStart, lineToIntersect);
  }
};

#ifdef __cplusplus
}
#endif

#endif  //__MATH_VECTOR_3F_H__
