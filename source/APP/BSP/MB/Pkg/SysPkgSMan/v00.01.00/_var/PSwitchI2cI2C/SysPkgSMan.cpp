
#include "SysPkgSMan.h"

#include "System.h"


cWorkMode::cWorkMode()
{
  vSet(0);
}

void cWorkMode::vSet(u16 lCounter_s)
{
  mCounter_s = lCounter_s;
}

bool cWorkMode::IsAuto()
{
  return (mCounter_s == 0);
}

bool cWorkMode::IsManual()
{
  return (!IsAuto());
}

void cWorkMode::vTick1000ms()
{
  if (mCounter_s)
  {
    mCounter_s--;
  }
}

const char* cWorkMode::ToString()
{
  if (IsAuto())
  {
    return (const char*)"auto";
  }
  else
  {
    return (const char*)"manuel";
  }
}




cSysPkgSMan::cSysPkgSMan()
  : mcOpMode(), 
    mcWufHandler()
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