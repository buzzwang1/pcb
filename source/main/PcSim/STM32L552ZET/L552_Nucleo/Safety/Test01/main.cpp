#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"


void PrintHexData(const u8* lpu8Data, u32 lu32Count)
{
  while (lu32Count)
  {
    printf("%02X", *lpu8Data++);
    lu32Count--;
  }
}

void main(void)
{
  int8 li8i;

  const u8 TestInput1[] = {5, 0x00, 0x01, 0x02, 0x03, 0x04};
  const u8 TestInput2[] = {6, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04};
  const u8 TestInput3[] = {5, 0x00, 0x01, 0x03, 0x02, 0x04};
  const u8 TestInput4[] = {5, 0x00, 0x01, 0x02, 0x03, 0x05};

  const u8* TestInput[4] = {TestInput1, TestInput2, TestInput3, TestInput4};

  printf("Test\n");

  {
    printf("\nChecksumme:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      printf("  0x%02X\n", cSafety::u8CheckSum8(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0], 1));
    }
  }

  {
    printf("\nCrc8:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      printf("  0x%02X\n", cSafety::u8Crc8(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0], 7, 0));
    }
  }

  {
    printf("\nCrc16:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      printf("  0x%04X\n", cSafety::u16Crc16(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0], 0x1021, 0));
    }
  }

  {
    printf("\nCrc32:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      printf("  0x%08X\n", cSafety::u32Crc32(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0])); // , 0x04C11DB7, 0xFFFFFFFF));
    }
  }

  {
    u8 lpu8Result[16];
    printf("\nMD5:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      cSafety::vMd5(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0], lpu8Result);
      printf("  ");
      PrintHexData(lpu8Result, 16);
      printf("\n");
    }
  }

  {
    u8 lpu8Result[20];
    printf("\nSHA1:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      cSha1 lcSha1;
      lcSha1.Update(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0]);
      lcSha1.Final(lpu8Result);

      printf("  ");
      PrintHexData(lpu8Result, 20);
      printf("\n");
    }
  }

  {
    u8 lpu8Result[32];
    printf("\nSHA256:\n");
    for (li8i = 0; li8i < 4; li8i++)
    {
      cSha256 lcSha256;
      lcSha256.Update(((u8*)&(TestInput[li8i][1])), TestInput[li8i][0]);
      lcSha256.Final(lpu8Result);

      printf("  ");
      PrintHexData(lpu8Result, 32);
      printf("\n");
    }
  }
}


