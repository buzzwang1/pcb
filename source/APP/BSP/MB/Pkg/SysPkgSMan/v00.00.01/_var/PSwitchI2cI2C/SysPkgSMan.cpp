
#include "SysPkgSMan.h"

#include "System.h"


cWorkMode::cWorkMode()
{
  mCounter = 0;
  vSet(cWorkMode::nStAuto);
}

void cWorkMode::vSet(cWorkMode::tenState lMode)
{
  mMode = lMode;
  if (mMode == cWorkMode::nStManuel)
  {
    mCounter = 60 * 10; // 10min
  }
}

bool cWorkMode::IsAuto()
{
  return (mMode == cWorkMode::nStAuto);
}

void cWorkMode::vTick1000ms()
{
  if (mCounter)
  {
    mCounter--;
    if (mCounter == 0)
    {
      vSet(cWorkMode::nStAuto);
    }
  }
}

const char* cWorkMode::ToString()
{
  if (mMode == cWorkMode::nStManuel)
  {
    return (const char*)"manuel";
  }
  else
  {
    return (const char*)"auto";
  }
}




cSysPkgSMan::cSysPkgSMan()
  : mcOpMode()
{
};


void cSysPkgSMan::vInit1(void)
{
}

void cSysPkgSMan::vInit2(void)
{
}


void cSysPkgSMan::vTick1msHp(void)
{
}

void cSysPkgSMan::vTick1msLp(void)
{
}

void cSysPkgSMan::vTick10msLp(void)
{
}

void cSysPkgSMan::vTick100msLp(void)
{
}

void cSysPkgSMan::vTick1000msLp(void)
{
  mcOpMode.vTick1000ms();
}