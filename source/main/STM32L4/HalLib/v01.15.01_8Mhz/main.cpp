
#include "main.h"





int main(void)
{
  while (1)
  {
    __asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}



// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
}

