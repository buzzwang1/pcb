#ifndef __MATH_MATRIX_4X4_H__
#define __MATH_MATRIX_4X4_H__


#ifdef USE_ARM_MATH
  #include "arm_math.h"
  static  arm_matrix_instance_f32 mstMat1 = {4,4, 0};
  static  arm_matrix_instance_f32 mstMat2 = {4,4, 0};
  static  arm_matrix_instance_f32 mstVec  = {4,1, 0};
  static  arm_matrix_instance_f32 mstRetM = {4,4, 0};
  static  arm_matrix_instance_f32 mstRetV = {4,1, 0};
#endif


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "Vector3f.h"


#define MAT_4x4f_XY(x, y) ((y)*4+(x))

// ----------------------------------------------

class cMatrix_4x4f
{
  public:

  float m[16];

  cMatrix_4x4f(bool lbInit)
  {
    if (lbInit) Set_Default();
  }

  void Set_Default()
  {
    float DefaultMat[16] =
         {1,   0,   0,   0,
          0,   1,   0,   0,
          0,   0,   1,   0,
          0,   0,   0,   1};

    Set(DefaultMat);
  }

  void Set(float lMatrix[16])
  {
    u8 c;

    for (c = 0; c < 4*4; c++)
    {
        m[c] = lMatrix[c];
    }
  }

  void Set(cMatrix_4x4f lcMatrix)
  {
    u8 c;

    for (c = 0; c < 4*4; c++)
    {
      m[c] = lcMatrix.m[c];
    }
  }

  class Math
  {
    public: 
    static inline cVector3f Multiply(cMatrix_4x4f &m, cVector3f &i)
    {
      cVector3f v;
     
      #ifdef USE_ARM_MATH
        mstVec.pData  = (float*)&i;
        mstMat1.pData = (float*)&m;
        mstRetV.pData = (float*)&v;
        arm_mat_mult_f32(&mstMat1, &mstVec, &mstRetV);
      #else
        v.x = i.x * m.m[MAT_4x4f_XY(0,0)] + i.y * m.m[MAT_4x4f_XY(1,0)] + i.z * m.m[MAT_4x4f_XY(2,0)] + m.m[MAT_4x4f_XY(3,0)];
        v.y = i.x * m.m[MAT_4x4f_XY(0,1)] + i.y * m.m[MAT_4x4f_XY(1,1)] + i.z * m.m[MAT_4x4f_XY(2,1)] + m.m[MAT_4x4f_XY(3,1)];
        v.z = i.x * m.m[MAT_4x4f_XY(0,2)] + i.y * m.m[MAT_4x4f_XY(1,2)] + i.z * m.m[MAT_4x4f_XY(2,2)] + m.m[MAT_4x4f_XY(3,2)];
      #endif
     
     return v;
    }

    static cMatrix_4x4f Multiply(cMatrix_4x4f &mat1, cMatrix_4x4f &mat2)
    {
      cMatrix_4x4f lcRet(false);

      #ifdef USE_ARM_MATH
        mstMat1.pData = (float*)&mat1;
        mstMat2.pData = (float*)&mat2;
        mstRetM.pData = (float*)&lcRet;
        arm_mat_mult_f32(&mstMat1, &mstMat2, &mstRetM);
      #else
        u8 x, y;
        for (x = 0; x < 4; x++)
        {
          for (y = 0; y < 4; y++)
          {
            lcRet.m[MAT_4x4f_XY(x,y)] = mat1.m[MAT_4x4f_XY(0,y)] * mat2.m[MAT_4x4f_XY(x,0)] +
                                        mat1.m[MAT_4x4f_XY(1,y)] * mat2.m[MAT_4x4f_XY(x,1)] +
                                        mat1.m[MAT_4x4f_XY(2,y)] * mat2.m[MAT_4x4f_XY(x,2)] +
                                        mat1.m[MAT_4x4f_XY(3,y)] * mat2.m[MAT_4x4f_XY(x,3)];
          }
        }
      #endif

      return lcRet;
    }

    static cMatrix_4x4f Matrix_PointAt(cVector3f &pos, cVector3f &target, cVector3f &up)
    {
      // Calculate new forward direction
      cVector3f newForward = target.cSub(pos);
      newForward = newForward.cNormalise();

      // Calculate new Up direction
      cVector3f a = newForward.cMul(up.fDotProduct(newForward));
      cVector3f newUp = up.cSub(a);
      newUp = newUp.cNormalise();

      // New Right direction is easy, its just cross product
      cVector3f newRight = newUp.cCrossProduct(newForward);

      // Construct Dimensioning and Translation Matrix	
      cMatrix_4x4f matrix(false);
      matrix.m[MAT_4x4f_XY(0,0)] = newRight.x;   matrix.m[MAT_4x4f_XY(0,1)] = newRight.y;   matrix.m[MAT_4x4f_XY(0,2)] = newRight.z;   matrix.m[MAT_4x4f_XY(0,3)] = 0.0f;
      matrix.m[MAT_4x4f_XY(1,0)] = newUp.x;      matrix.m[MAT_4x4f_XY(1,1)] = newUp.y;      matrix.m[MAT_4x4f_XY(1,2)] = newUp.z;      matrix.m[MAT_4x4f_XY(1,3)] = 0.0f;
      matrix.m[MAT_4x4f_XY(2,0)] = newForward.x; matrix.m[MAT_4x4f_XY(2,1)] = newForward.y; matrix.m[MAT_4x4f_XY(2,2)] = newForward.z; matrix.m[MAT_4x4f_XY(2,3)] = 0.0f;
      matrix.m[MAT_4x4f_XY(3,0)] = pos.x;        matrix.m[MAT_4x4f_XY(3,1)] = pos.y;        matrix.m[MAT_4x4f_XY(3,2)] = pos.z;        matrix.m[MAT_4x4f_XY(3,3)] = 1.0f;
      return matrix;
    }

    static cMatrix_4x4f Matrix_QuickInverse(cMatrix_4x4f &m) // Only for Rotation/Translation Matrices
    {
      cMatrix_4x4f matrix(false);
      matrix.m[MAT_4x4f_XY(0,0)] = m.m[MAT_4x4f_XY(0,0)]; matrix.m[MAT_4x4f_XY(0,1)] = m.m[MAT_4x4f_XY(1,0)]; matrix.m[MAT_4x4f_XY(0,2)] = m.m[MAT_4x4f_XY(2,0)]; matrix.m[MAT_4x4f_XY(0,3)] = 0.0f;
      matrix.m[MAT_4x4f_XY(1,0)] = m.m[MAT_4x4f_XY(0,1)]; matrix.m[MAT_4x4f_XY(1,1)] = m.m[MAT_4x4f_XY(1,1)]; matrix.m[MAT_4x4f_XY(1,2)] = m.m[MAT_4x4f_XY(2,1)]; matrix.m[MAT_4x4f_XY(1,3)] = 0.0f;
      matrix.m[MAT_4x4f_XY(2,0)] = m.m[MAT_4x4f_XY(0,2)]; matrix.m[MAT_4x4f_XY(2,1)] = m.m[MAT_4x4f_XY(1,2)]; matrix.m[MAT_4x4f_XY(2,2)] = m.m[MAT_4x4f_XY(2,2)]; matrix.m[MAT_4x4f_XY(2,3)] = 0.0f;

      matrix.m[MAT_4x4f_XY(3,0)] = -(m.m[MAT_4x4f_XY(3,0)] * matrix.m[MAT_4x4f_XY(0,0)] + m.m[MAT_4x4f_XY(3,1)] * matrix.m[MAT_4x4f_XY(1,0)] + m.m[MAT_4x4f_XY(3,2)] * matrix.m[MAT_4x4f_XY(2,0)]);
      matrix.m[MAT_4x4f_XY(3,1)] = -(m.m[MAT_4x4f_XY(3,0)] * matrix.m[MAT_4x4f_XY(0,1)] + m.m[MAT_4x4f_XY(3,1)] * matrix.m[MAT_4x4f_XY(1,1)] + m.m[MAT_4x4f_XY(3,2)] * matrix.m[MAT_4x4f_XY(2,1)]);
      matrix.m[MAT_4x4f_XY(3,2)] = -(m.m[MAT_4x4f_XY(3,0)] * matrix.m[MAT_4x4f_XY(0,2)] + m.m[MAT_4x4f_XY(3,1)] * matrix.m[MAT_4x4f_XY(1,2)] + m.m[MAT_4x4f_XY(3,2)] * matrix.m[MAT_4x4f_XY(2,2)]);
      matrix.m[MAT_4x4f_XY(3,3)] = 1.0f;
      return matrix;

      /*
      //https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
      cMatrix_4x4f matrix(false);
      int i;

      float A2323 = m.m[2,2] * m.m[3,3] - m.m[2,3] * m.m[3,2];
      float A1323 = m.m[2,1] * m.m[3,3] - m.m[2,3] * m.m[3,1];
      float A1223 = m.m[2,1] * m.m[3,2] - m.m[2,2] * m.m[3,1];
      float A0323 = m.m[2,0] * m.m[3,3] - m.m[2,3] * m.m[3,0];
      float A0223 = m.m[2,0] * m.m[3,2] - m.m[2,2] * m.m[3,0];
      float A0123 = m.m[2,0] * m.m[3,1] - m.m[2,1] * m.m[3,0];
      float A2313 = m.m[1,2] * m.m[3,3] - m.m[1,3] * m.m[3,2];
      float A1313 = m.m[1,1] * m.m[3,3] - m.m[1,3] * m.m[3,1];
      float A1213 = m.m[1,1] * m.m[3,2] - m.m[1,2] * m.m[3,1];
      float A2312 = m.m[1,2] * m.m[2,3] - m.m[1,3] * m.m[2,2];
      float A1312 = m.m[1,1] * m.m[2,3] - m.m[1,3] * m.m[2,1];
      float A1212 = m.m[1,1] * m.m[2,2] - m.m[1,2] * m.m[2,1];
      float A0313 = m.m[1,0] * m.m[3,3] - m.m[1,3] * m.m[3,0];
      float A0213 = m.m[1,0] * m.m[3,2] - m.m[1,2] * m.m[3,0];
      float A0312 = m.m[1,0] * m.m[2,3] - m.m[1,3] * m.m[2,0];
      float A0212 = m.m[1,0] * m.m[2,2] - m.m[1,2] * m.m[2,0];
      float A0113 = m.m[1,0] * m.m[3,1] - m.m[1,1] * m.m[3,0];
      float A0112 = m.m[1,0] * m.m[2,1] - m.m[1,1] * m.m[2,0];

      float det = m.m[0,0] * ( m.m[1,1] * A2323 - m.m[1,2] * A1323 + m.m[1,3] * A1223 ) 
                - m.m[0,1] * ( m.m[1,0] * A2323 - m.m[1,2] * A0323 + m.m[1,3] * A0223 ) 
                + m.m[0,2] * ( m.m[1,0] * A1323 - m.m[1,1] * A0323 + m.m[1,3] * A0123 ) 
                - m.m[0,3] * ( m.m[1,0] * A1223 - m.m[1,1] * A0223 + m.m[1,2] * A0123 );
      det = 1 / det;

      matrix.m[0,0] = det *   ( m.m[1,1] * A2323 - m.m[1,2] * A1323 + m.m[1,3] * A1223 );
      matrix.m[0,1] = det * - ( m.m[0,1] * A2323 - m.m[0,2] * A1323 + m.m[0,3] * A1223 );
      matrix.m[0,2] = det *   ( m.m[0,1] * A2313 - m.m[0,2] * A1313 + m.m[0,3] * A1213 );
      matrix.m[0,3] = det * - ( m.m[0,1] * A2312 - m.m[0,2] * A1312 + m.m[0,3] * A1212 );
      matrix.m[1,0] = det * - ( m.m[1,0] * A2323 - m.m[1,2] * A0323 + m.m[1,3] * A0223 );
      matrix.m[1,1] = det *   ( m.m[0,0] * A2323 - m.m[0,2] * A0323 + m.m[0,3] * A0223 );
      matrix.m[1,2] = det * - ( m.m[0,0] * A2313 - m.m[0,2] * A0313 + m.m[0,3] * A0213 );
      matrix.m[1,3] = det *   ( m.m[0,0] * A2312 - m.m[0,2] * A0312 + m.m[0,3] * A0212 );
      matrix.m[2,0] = det *   ( m.m[1,0] * A1323 - m.m[1,1] * A0323 + m.m[1,3] * A0123 );
      matrix.m[2,1] = det * - ( m.m[0,0] * A1323 - m.m[0,1] * A0323 + m.m[0,3] * A0123 );
      matrix.m[2,2] = det *   ( m.m[0,0] * A1313 - m.m[0,1] * A0313 + m.m[0,3] * A0113 );
      matrix.m[2,3] = det * - ( m.m[0,0] * A1312 - m.m[0,1] * A0312 + m.m[0,3] * A0112 );
      matrix.m[3,0] = det * - ( m.m[1,0] * A1223 - m.m[1,1] * A0223 + m.m[1,2] * A0123 );
      matrix.m[3,1] = det *   ( m.m[0,0] * A1223 - m.m[0,1] * A0223 + m.m[0,2] * A0123 );
      matrix.m[3,2] = det * - ( m.m[0,0] * A1213 - m.m[0,1] * A0213 + m.m[0,2] * A0113 );
      matrix.m[3,3] = det *   ( m.m[0,0] * A1212 - m.m[0,1] * A0212 + m.m[0,2] * A0112 );

      return matrix;*/
    }

    static cMatrix_4x4f mScale(const float ratio)
    {
      cMatrix_4x4f lcRet(true);
      lcRet.m[MAT_4x4f_XY(0,0)] *= ratio;
      lcRet.m[MAT_4x4f_XY(1,1)] *= ratio;
      lcRet.m[MAT_4x4f_XY(2,2)] *= ratio;
      return lcRet;
    }
  };


  inline float fSin(float angle) 
  {
    #ifdef USE_ARM_MATH
      return arm_sin_f32(angle * 3.141592653f / 180.0f);
    #else
      return sinf(angle * 3.141592653f / 180.0f);
    #endif

  }

  inline float fCos(float angle) 
  { 
    #ifdef USE_ARM_MATH
      return arm_cos_f32(angle * 3.141592653f / 180.0f);
    #else
      return cosf(angle * 3.141592653f / 180.0f);
    #endif
  }
};

class cMatrix_4x4f_RotX: public cMatrix_4x4f
{
  public:
  cMatrix_4x4f_RotX()
    :cMatrix_4x4f(false)
  {
    Set(0);
  }

  cMatrix_4x4f_RotX(const float angle)
    :cMatrix_4x4f(false)
  {
    Set(angle);
  }

  void Set(const float angle)
  {
    cMatrix_4x4f::Set_Default();
    m[MAT_4x4f_XY(1,1)] = fCos(angle);
    m[MAT_4x4f_XY(1,2)] = fSin(angle);
    m[MAT_4x4f_XY(2,1)] = -fSin(angle);
    m[MAT_4x4f_XY(2,2)] = fCos(angle);
  }
};

class cMatrix_4x4f_RotY: public cMatrix_4x4f
{
  public:
  cMatrix_4x4f_RotY()
    :cMatrix_4x4f(false)
  {
    Set(0);
  }

  cMatrix_4x4f_RotY(const float angle)
    :cMatrix_4x4f(false)
  {
    Set(angle);
  }

  void Set(const float angle)
  {
    cMatrix_4x4f::Set_Default();
    m[MAT_4x4f_XY(0,0)] = fCos(angle);
    m[MAT_4x4f_XY(0,2)] = -fSin(angle);
    m[MAT_4x4f_XY(2,0)] = fSin(angle);
    m[MAT_4x4f_XY(2,2)] = fCos(angle);
  }
};

class cMatrix_4x4f_RotZ: public cMatrix_4x4f
{
  public:
  cMatrix_4x4f_RotZ()
    :cMatrix_4x4f(false)
  {
    Set(0);
  }

  cMatrix_4x4f_RotZ(const float angle)
    :cMatrix_4x4f(false)
  {
    Set(angle);
  }

  void Set(const float angle)
  {
    cMatrix_4x4f::Set_Default();
    m[MAT_4x4f_XY(0,0)] = fCos(angle);
    m[MAT_4x4f_XY(0,1)] = fSin(angle);
    m[MAT_4x4f_XY(1,0)] = -fSin(angle);
    m[MAT_4x4f_XY(1,1)] = fCos(angle);
  }
};


class cMatrix_4x4f_RotXYZ : public cMatrix_4x4f
{
public:
  cMatrix_4x4f_RotXYZ()
    :cMatrix_4x4f(false)
  {
    Set(0, 0, 0);
  }

  cMatrix_4x4f_RotXYZ(const float ax, const float ay, const float az)
    :cMatrix_4x4f(false)
  {
    Set(ax, ay, az);
  }

  void Set(const float aa, const float ab, const float ac)
  {
    cMatrix_4x4f::Set_Default();

    float ca = fCos(aa);
    float sa = fSin(aa);

    float cb = fCos(ab);
    float sb = fSin(ab);

    float cc = fCos(ac);
    float sc = fSin(ac);

    m[MAT_4x4f_XY(0,0)] =  cb * cc;
    m[MAT_4x4f_XY(0,1)] =  sa * sb * cc - ca * sc;
    m[MAT_4x4f_XY(0,2)] =  ca * sb * cc + sa * sc;

    m[MAT_4x4f_XY(1,0)] =  cb * sc;
    m[MAT_4x4f_XY(1,1)] =  sa * sb * sc + ca * cc;
    m[MAT_4x4f_XY(1,2)] =  ca * sb * sc - sa * cc;

    m[MAT_4x4f_XY(2,0)] = -sb;
    m[MAT_4x4f_XY(2,1)] = sa * cb;
    m[MAT_4x4f_XY(2,2)] = ca * cb;
  }
};

class cMatrix_4x4f_RotZYX : public cMatrix_4x4f
{
public:
  cMatrix_4x4f_RotZYX()
    :cMatrix_4x4f(false)
  {
    Set(0, 0, 0);
  }

  cMatrix_4x4f_RotZYX(const float ax, const float ay, const float az)
    :cMatrix_4x4f(false)
  {
    Set(ax, ay, az);
  }

  void Set(const float aa, const float ab, const float ac)
  {
    cMatrix_4x4f::Set_Default();

    float ca = fCos(aa);
    float sa = fSin(aa);

    float cb = fCos(ab);
    float sb = fSin(ab);

    float cc = fCos(ac);
    float sc = fSin(ac);

    m[MAT_4x4f_XY(0,0)] = cb * cc;
    m[MAT_4x4f_XY(0,1)] = cb * sc;
    m[MAT_4x4f_XY(0,2)] = -sb;

    m[MAT_4x4f_XY(1,0)] =  sa * sb * cc - ca * sc;
    m[MAT_4x4f_XY(1,1)] =  sa * sb * sc + ca * cc;
    m[MAT_4x4f_XY(1,2)] =  sa * cb;

    m[MAT_4x4f_XY(2,0)] =  ca * sb * cc + sa * sc;
    m[MAT_4x4f_XY(2,1)] =  ca * sb * sc - sa * cc;
    m[MAT_4x4f_XY(2,2)] =  ca * cb;

  }
};

class cMatrix_4x4f_Trans: public cMatrix_4x4f
{
  public:
  cMatrix_4x4f_Trans()
    :cMatrix_4x4f(false)
  {
    Set(0, 0, 0);
  }

  cMatrix_4x4f_Trans(float x, float y, float z)
    :cMatrix_4x4f(false)
  {
    Set(x, y, z);
  }

  void Set(float x, float y, float z)
  {
    cMatrix_4x4f::Set_Default();
    m[MAT_4x4f_XY(3,0)] = x;
    m[MAT_4x4f_XY(3,1)] = y;
    m[MAT_4x4f_XY(3,2)] = z;
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__MATH_MATRIX_4X4_H__
