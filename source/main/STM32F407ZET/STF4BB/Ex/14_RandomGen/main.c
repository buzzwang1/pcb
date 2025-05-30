#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "rng.h"
#include "key.h"

 
//随机数发生器(RNG) 实验  -库函数版本
//STM32F4工程-库函数版本
    
int main(void)
{ 
	u32 random;
	u8 t=0,key;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	LED_Init();					//初始化LED
	KEY_Init();					//按键初始化
 	LCD_Init();         //初始化液晶接口
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RNG TEST");	
	LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
	LCD_ShowString(30,110,200,16,16,"2015/08/09");
	while(RNG_Init())
	{
		LCD_ShowString(30,130,200,16,16,"  RNG Error! ");
		delay_ms(200);
		LCD_ShowString(30,130,200,16,16,"RNG Trying...");
	}                                 
	LCD_ShowString(30,130,200,16,16,"RNG Ready!   ");
	LCD_ShowString(30,150,200,16,16,"KEY0:Get Random Num");
	LCD_ShowString(30,180,200,16,16,"Random Num:");	 
	LCD_ShowString(30,210,200,16,16,"Random Num[0-9]:");
  
	POINT_COLOR=BLUE;
	while(1) 
	{		
		delay_ms(10);
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			random=RNG_Get_RandomNum();
			LCD_ShowNum(30+8*11,180,random,10,16);

		} 
		if((t%20)==0)
		{ 
			LED0=!LED0;	//200ms,LED0
			random=RNG_Get_RandomRange(0,9);//[0,9]
			LCD_ShowNum(30+8*16,210,random,1,16);
		 }
		delay_ms(10);
		t++;
	}	
}

