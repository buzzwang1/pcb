#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h"  


 
//SPIͨ�Žӿ�ʵ��-�⺯���汾
//STM32F4����-�⺯���汾
 
 
 
//Ҫд�뵽W25Q16���ַ�������
const u8 TEXT_Buffer[]={"STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
	
int main(void)
{ 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);     //��ʼ����ʱ����
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED
 	LCD_Init();					//LCD���
	KEY_Init(); 				//�������
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
		LED0=!LED0;		//DS0��˸
	}
	LCD_ShowString(30,150,200,16,16,"W25Q16 Ready!"); 
	FLASH_SIZE=2*1024*1024;	//FLASH ��СΪ2M�ֽ�
  	POINT_COLOR=BLUE;			//��������Ϊ���
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����,д��W25Q16   Ҫ�Ȳ��������ٴ�д�룬������Ϊʧ��
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
			LED0=!LED0;//��ʾϵͳ��������	
			i=0;
		}
	}
}

