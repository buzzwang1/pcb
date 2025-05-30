#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"  


 
//SPI通信接口实验-库函数版本
//STM32F4工程-库函数版本
 
 
 
//要写入到W25Q16的字符串数组
const u8 TEXT_Buffer[]={"STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
	
int main(void)
{ 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);     //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	LED_Init();					//初始化LED
 	LCD_Init();					//LCD初�
	KEY_Init(); 				//按键初�
	W25QXX_Init();			//W25QXX
 	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"STM32F4");
	LCD_ShowString(30,70,200,16,16,"SPI TEST");
	LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
	LCD_ShowString(30,110,200,16,16,"2015/08/06");
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");
	while(W25QXX_ReadID()!=W25Q16)								//W25Q128
	{
		LCD_ShowString(30,150,200,16,16,"W25Q16 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!      ");
		delay_ms(500);
		LED0=!LED0;		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q16 Ready!"); 
	FLASH_SIZE=2*1024*1024;	//FLASH 大小为2M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝�
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q16   要先擦除才能再次写入，否则结果为失败
		{ 
			LCD_Fill(0,170,239,319,WHITE);
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q16....");
			W25QXX_Write((u8*)TEXT_Buffer,0,SIZE);		// SIZE
			LCD_ShowString(30,170,200,16,16,"W25Q16 Write Finished!");
		}
		if(key==KEY0_PRES)//KEY0
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q16.... ");
			W25QXX_Read(datatemp,0,SIZE);
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");
			LCD_ShowString(30,190,200,16,16,datatemp);
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0=!LED0;//提示系统正在运行	
			i=0;
		}
	}
}

