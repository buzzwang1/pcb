#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
 
//SPI 驱动代码	   
//STM32F4工程-库函数版本
////////////////////////////////////////////////////////////////////////////////// 	


 
						  	    													  
void SPI1_Init(void);			 //初始化SPI1口
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节
		 
#endif

