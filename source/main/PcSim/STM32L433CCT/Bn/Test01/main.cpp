#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"


// Test für Ringbuffer


void PrintBuf(cRingBufDynT<uint8, uint16> &lcBuf)
{
  printf("\nSize:  %4d\nCount: %4d\nHead:  %4d\nTail:  %4d\n", lcBuf.mtySize, lcBuf.mtyCnt, lcBuf.mtyHead, lcBuf.mtyTail);
  printf("Buf:   ");
  
  u16 lu16Cnt;
  u16 lu16IdxData;
  u16 lu16CntDataLine;

  lu16CntDataLine = 0;
  for (lu16Cnt = 0; lu16Cnt < lcBuf.mtySize; lu16Cnt++)
  {
    printf("%02X ", lcBuf.mtyBuf[lu16Cnt]);

    if (lu16CntDataLine < 32) lu16CntDataLine++;
                        else  {lu16CntDataLine = 0;printf("\n       ");}
  }

  printf("\n");
  printf("Data:  ");

  lu16IdxData     = lcBuf.mtyTail;
  lu16CntDataLine = 0;

  for (lu16Cnt = 0; lu16Cnt < lcBuf.mtyCnt; lu16Cnt++)
  {
    printf("%02X ", lcBuf.mtyBuf[lu16IdxData]);

    lu16IdxData++;
    if (lu16IdxData >= lcBuf.mtySize) lu16IdxData = 0;

    if (lu16CntDataLine < 32) lu16CntDataLine++;
                        else  {lu16CntDataLine = 0;printf("\n       ");}
  }

}

void main(void)
{
  u8 mu8Array1[] = {1,2,3};
  u8 mu8Array2[] = {1,2,3,4,5};
  u8 mu8Array3[] = {1,2,3,4,5,6,7,8,9,10};
  u8 mu8Array4[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
                    10,11,12,13,14,15,16,17,18,19,
                    20,21,22,23,24,25,26,27,28,29,
                    30,31,32,33,34,35,36,37,38,39,
                    40,41,42,43,44,45,46,47,48,49,
                    50,51};
  
  cRingBufDynT<uint8, uint16> mcRBuf1(10);
  cRingBufDynT<uint8, uint16> mcRBuf2(20);

  mcRBuf1.put(mu8Array1, 3);
  mcRBuf1.put(mu8Array4, sizeof(mu8Array4));

  mcRBuf1.move(&mcRBuf2, 100);

  PrintBuf(mcRBuf1);
  PrintBuf(mcRBuf2);

  for (u16 lut = 0; lut < 5; lut++)
  {
    mcRBuf1.put(mu8Array1, 3);
    mcRBuf1.move(&mcRBuf2, 100);
  }

  PrintBuf(mcRBuf1);
  PrintBuf(mcRBuf2);


  printf("\n\n\n");
}


