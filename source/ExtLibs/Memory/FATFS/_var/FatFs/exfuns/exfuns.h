#ifndef __EXFUNS_H
#define __EXFUNS_H 					   
#include <stm32f4xx.h>
#include "ff.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
//FATFS 扩展代码	   
//STM32F4工程-库函数版本
////////////////////////////////////////////////////////////////////////////////// 	

extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern u8 *fatbuf;//SD卡数据缓存区




//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN		0X00	//bin文件
#define T_LRC		0X10	//lrc文件
#define T_NES		0X20	//nes文件

#define T_TEXT		0X30	//.txt文件
#define T_C			0X31	//.c文件
#define T_H			0X32    //.h文件

#define T_WAV		0X40	//WAV文件
#define T_MP3		0X41	//MP3文件 
#define T_APE		0X42	//APE文件
#define T_FLAC		0X43	//FLAC文件

#define T_BMP		0X50	//bmp文件
#define T_JPG		0X51	//jpg文件
#define T_JPEG		0X52	//jpeg文件		 
#define T_GIF		0X53	//gif文件   

#define T_AVI		0X60	//avi文件  

 
u8 exfuns_init(void);
u8 f_typetell(u8 *fname);
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);
u32 exf_fdsize(u8 *fdname);	 																		   
#endif


