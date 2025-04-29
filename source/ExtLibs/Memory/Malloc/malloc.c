#include "malloc.h"

u8  mem1base[MEM1_MAX_SIZE]            __attribute__((section(".RamMemBase"), used)) __attribute__ ((aligned (MEM1_BLOCK_SIZE)));
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE] __attribute__((section(".RamMemMapBase"), used));

u8  mem2base[MEM2_MAX_SIZE]            __attribute__((section(".ExtMemBase"), used)) __attribute__ ((aligned (MEM2_BLOCK_SIZE)));
u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((section(".ExtMemMapBase"), used));

u8  mem3base[MEM3_MAX_SIZE]            __attribute__((section(".CCRMemBase"), used)) __attribute__ ((aligned (MEM3_BLOCK_SIZE)));
u16 mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((section(".CCRMemMapBase"), used));


const u32 memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE, MEM2_ALLOC_TABLE_SIZE, MEM3_ALLOC_TABLE_SIZE};
const u32 memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE      , MEM2_BLOCK_SIZE,       MEM3_BLOCK_SIZE};
const u32 memsize[SRAMBANK]    = {MEM1_MAX_SIZE        , MEM2_MAX_SIZE,         MEM3_MAX_SIZE};


struct _m_mallco_dev mallco_dev=
{
  my_mem_init,
  my_mem_perused,
  {mem1base,mem2base,mem3base},
  {mem1mapbase,mem2mapbase,mem3mapbase},
  {0,0,0,}
};

struct _m_mallco_dev mallco_dev;

void mymemcpy(void *des,void *src,u32 n)
{
  u8 *xdes=des;
  u8 *xsrc=src; 
  while(n--)*xdes++=*xsrc++;
}

void mymemset(void *s,u8 c,u32 count)
{
  u8 *xs = s;
  while(count--)*xs++=c;
}

//memx
void my_mem_init(u8 memx)  
{  
  mymemset(mallco_dev.memmap[memx], 0,memtblsize[memx]*2);
  mymemset(mallco_dev.membase[memx], 0,memsize[memx]);
  mallco_dev.memrdy[memx]=1;
}  

u8 my_mem_perused(u8 memx)
{  
  u32 used=0;
  u32 i;

  for(i=0;i<memtblsize[memx];i++)
  {
   if(mallco_dev.memmap[memx][i])used++;
  }
  return (used*100)/(memtblsize[memx]);
}

u32 my_mem_malloc(u8 memx,u32 size)
{  
  signed long offset=0;

  u32 nmemb;
  u32 cmemb=0;

  u32 i;
  if(!mallco_dev.memrdy[memx])mallco_dev.init(memx);
  if(size==0)return 0XFFFFFFFF;
  nmemb=size/memblksize[memx];
  if(size%memblksize[memx])nmemb++;
  for(offset=memtblsize[memx]-1;offset>=0;offset--)
  {
    if(!mallco_dev.memmap[memx][offset])cmemb++;
    else cmemb=0;
    if(cmemb==nmemb)
    {
      for(i=0;i<nmemb;i++)
      {
          mallco_dev.memmap[memx][offset+i]=nmemb;  
      }
      return (offset*memblksize[memx]);
    }
  }
  return 0XFFFFFFFF;
}

//�ͷ��ڴ�(�ڲ�����) 
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
u8 my_mem_free(u8 memx,u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy[memx])//δ��ʼ��,��ִ�г�ʼ��
  {
    mallco_dev.init(memx);    
        return 1;//δ��ʼ��  
    }  
    if(offset<memsize[memx])//ƫ�����ڴ����. 
    {  
        int index=offset/memblksize[memx];      //ƫ�������ڴ�����  
        int nmemb=mallco_dev.memmap[memx][index];  //�ڴ������
        for(i=0;i<nmemb;i++)              //�ڴ������
        {  
            mallco_dev.memmap[memx][index+i]=0;  
        }  
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  
//�ͷ��ڴ�(�ⲿ����) 
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ 
void myfree(u8 memx,void *ptr)  
{  
  u32 offset;   
  if(ptr==NULL)return;//��ַΪ0.  
   offset=(u32)ptr-(u32)mallco_dev.membase[memx];     
    my_mem_free(memx,offset);  //�ͷ��ڴ�      
}  
//�����ڴ�(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(u8 memx,u32 size)  
{  
    u32 offset;   
  offset=my_mem_malloc(memx,size);               
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)mallco_dev.membase[memx]+offset);  
}  
//���·����ڴ�(�ⲿ����)
//memx:�����ڴ��
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(u8 memx,void *ptr,u32 size)  
{  
    u32 offset;    
    offset=my_mem_malloc(memx,size);     
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {                       
      mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);  //�������ڴ����ݵ����ڴ�   
        myfree(memx,ptr);                              //�ͷž��ڴ�
        return (void*)((u32)mallco_dev.membase[memx]+offset);          //�������ڴ��׵�ַ
    }  
}












