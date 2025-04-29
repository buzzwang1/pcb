#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "dma.h"


 
//DMA ÊµÑé-¿âº¯Êý°æ±¾  
//STM32F4¹¤³Ì-¿âº¯Êý°æ±¾


#define SEND_BUF_SIZE 8200	//·¢ËÍÊý¾Ý³¤¶È,×îºÃµÈÓÚsizeof(TEXT_TO_SEND)+2µÄÕûÊý±¶.

u8 SendBuff[SEND_BUF_SIZE];	//·¢ËÍÊý¾Ý»º³åÇø
const u8 TEXT_TO_SEND[]={"STM32F4 DMA serial test"};	 

  
int main(void)
{ 
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;//½ø¶È
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÏµÍ³ÖÐ¶ÏÓÅÏÈ¼¶·Ö×é2
	delay_init(168);     //³õÊ¼»¯ÑÓÊ±º¯Êý
	uart_init(115200);	//³õÊ¼»¯´®¿Ú²¨ÌØÂÊÎª115200
	LED_Init();					//³õÊ¼»¯LED 
 	LCD_Init();					//LCD³õÊ¼»¯ 
	KEY_Init(); 				//°´¼ü³õÊ¼»¯ 
 	MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH4,ÍâÉèÎª´®¿Ú1,´æ´¢Æ÷ÎªSendBuff,³¤¶ÈÎª:SEND_BUF_SIZE.
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"STM32F4");	
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
	LCD_ShowString(30,110,200,16,16,"2014/5/6");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//ÉèÖÃ×ÖÌåÎªÀ¶É«      	 
 //ÏÔÊ¾ÌáÊ¾ÐÅÏ¢	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//Ìî³äASCII×Ö·û¼¯Êý¾Ý
    {
		if(t>=j)//¼ÓÈë»»ÐÐ·û
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
		}else//¸´ÖÆTEXT_TO_SENDÓï¾ä
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//ÉèÖÃ×ÖÌåÎªÀ¶É
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)  //KEY0°´ÏÂ
		{
			printf("\r\nDMA DATA:\r\n");
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");
      USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //DMA
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);    //DMA
		    while(1)
		    {
				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//µÈ´ýDMA2_Steam7´«ÊäÍê³É
				{ 
					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//Çå³ýDMA2_Steam7´«ÊäÍê³É±êÖ¾
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
			LED0=!LED0;//ÌáÊ¾ÏµÍ³ÕýÔÚÔËÐÐ	
			i=0;
		}		   
	}		    
}

