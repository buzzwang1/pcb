#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "dma.h"


 
//DMA 实验-库函数版本  
//STM32F4工程-库函数版本


#define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
const u8 TEXT_TO_SEND[]={"STM32F4 DMA serial test"};	 

  
int main(void)
{ 
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;//进度
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);     //初始化延时函数
	uart_init(115200);	//初始化串口波特率为115200
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化 
	KEY_Init(); 				//按键初始化 
 	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"STM32F4");	
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
 //显示提示信息	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//填充ASCII字符集数据
    {
		if(t>=j)//加入换行符
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}else//复制TEXT_TO_SEND语句
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//设置字体为蓝�
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)  //KEY0按下
		{
			printf("\r\nDMA DATA:\r\n");
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");
      USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //DMA
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);    //DMA
		    while(1)
		    {
				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
				{ 
					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//清除DMA2_Steam7传输完成标志
					break; 
		        }
				pro=DMA_GetCurrDataCounter(DMA2_Stream7);
				pro=1-pro/SEND_BUF_SIZE;
				pro*=100;
				LCD_ShowNum(30,170,pro,3,16);
		    }			    
			LCD_ShowNum(30,170,100,3,16);
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");
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

