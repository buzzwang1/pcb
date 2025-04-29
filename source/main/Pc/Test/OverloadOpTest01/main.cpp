#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"




void main(void)
{
  u8 mcMem[40];
  cbArrayExtT<u16> mcMemTest1(mcMem, sizeof(mcMem));


  for (u8 lu8t = 0; lu8t < 40; lu8t++)
  {
    mcMemTest1[lu8t] = lu8t;
  }



  mcMemTest1[10] = 0;
  mcMemTest1[10]++;

  if (mcMemTest1[10] == 1)
  {
    u8 lu8Temp;

    lu8Temp = mcMemTest1[10];
  }



}


