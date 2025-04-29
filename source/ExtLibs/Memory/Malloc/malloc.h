#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f4xx.h"


#ifndef NULL
  #define NULL 0
#endif


#define SRAMIN   0 // interner ram
#define SRAMEX   1 // externer ram
#define SRAMCCM  2 // core coupled ram


#define SRAMBANK   3


//mem1
#define MEM1_BLOCK_SIZE        32
#define MEM1_MAX_SIZE          40*1024  // 100*1024
#define MEM1_ALLOC_TABLE_SIZE  MEM1_MAX_SIZE/MEM1_BLOCK_SIZE

//mem2
#define MEM2_BLOCK_SIZE        32
#define MEM2_MAX_SIZE          960 *1024
#define MEM2_ALLOC_TABLE_SIZE  MEM2_MAX_SIZE/MEM2_BLOCK_SIZE

//mem3
#define MEM3_BLOCK_SIZE        32
#define MEM3_MAX_SIZE          60 * 1024
#define MEM3_ALLOC_TABLE_SIZE  MEM3_MAX_SIZE/MEM3_BLOCK_SIZE

/*
//mem1
#define MEM1_BLOCK_SIZE        _MEM1_BLOCK_SIZE
#define MEM1_MAX_SIZE          _MEM1_MAX_SIZE
#define MEM1_ALLOC_TABLE_SIZE  MEM1_MAX_SIZE/MEM1_BLOCK_SIZE

//mem2
#define MEM2_BLOCK_SIZE        _MEM2_BLOCK_SIZE
#define MEM2_MAX_SIZE          _MEM2_MAX_SIZE
#define MEM2_ALLOC_TABLE_SIZE  MEM2_MAX_SIZE/MEM2_BLOCK_SIZE

//mem3
#define MEM3_BLOCK_SIZE        _MEM3_BLOCK_SIZE
#define MEM3_MAX_SIZE          _MEM3_MAX_SIZE
#define MEM3_ALLOC_TABLE_SIZE  MEM3_MAX_SIZE/MEM3_BLOCK_SIZE*/


struct _m_mallco_dev
{
  void (*init)(u8);
  u8 (*perused)(u8);
  u8   *membase[SRAMBANK];
  u16 *memmap[SRAMBANK];
  u8  memrdy[SRAMBANK];
};
extern struct _m_mallco_dev mallco_dev;

void mymemset(void *s,u8 c,u32 count);
void mymemcpy(void *des,void *src,u32 n);
void my_mem_init(u8 memx);
u32 my_mem_malloc(u8 memx,u32 size);
u8 my_mem_free(u8 memx,u32 offset);
u8 my_mem_perused(u8 memx);

////////////////////////////////////////////////////////////////////////////////
void myfree(u8 memx,void *ptr);
void *mymalloc(u8 memx,u32 size);
void *myrealloc(u8 memx,void *ptr,u32 size);

#endif













