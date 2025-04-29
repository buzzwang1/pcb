#ifndef __TEST_H__
#define __TEST_H__

#include "typedef.h"

class cTest1
{
public:
  enum
  {
    NODECOUNT = 8,
  };

  static const float macPointsDat[NODECOUNT][3];
};


/* -------------------------------------   */

class cBaseVector3f
{
public:
  float x, y, z;
};


class cVector3f: public cBaseVector3f
{
  public:
  cVector3f(float lx, float ly, float lz)
  {
    x = lx;
    y = ly;
    z = lz;
  }
};


class cTest2
{
public:
  enum
  {
    NODECOUNT = 8,
  };

  static const cBaseVector3f macPointsDat[NODECOUNT];
};

class cTest3_Root
{
public:
  u16 mu16PointCnt;

  cBaseVector3f* macPoints;

  void vSet(int liPointCount, cBaseVector3f* lacPoints)
  {
    mu16PointCnt = liPointCount;
    macPoints    = lacPoints;
  }
};

class cTest3 : public cTest3_Root
{
public:
  enum
  {
    NODECOUNT = 8,
  };

  static const cBaseVector3f macPointsDat[NODECOUNT];

  cTest3()
  {
    vSet(NODECOUNT, (cBaseVector3f*)macPointsDat);
  }
};



class cTest4
{
public:
  enum
  {
    NODECOUNT = 8,
  };

  static const cVector3f macPointsDat[NODECOUNT];
};



#endif  //Test
