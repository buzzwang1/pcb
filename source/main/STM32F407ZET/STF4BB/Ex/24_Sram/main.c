#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "sram.h"

//�ⲿSRAM ʵ��
//STM32F4����-�⺯���汾


u32* testsram;


//�ⲿ�ڴ����(���֧��1M�ֽ��ڴ����)
void fsmc_sram_test(u16 x,u16 y)
{
  u32 i=0;
  u8 temp=0;
  u8 sval=0;  //�ڵ�ַ0����������
    LCD_ShowString(x,y,239,y+16,16,"Ex Memory Test:   0KB");
  //ÿ��4K�ֽ�,д��һ������,�ܹ�д��256������,�պ���1M�ֽ�
  for(i=0;i<1024*1024;i+=4096)
  {
    FSMC_SRAM_WriteBuffer(&temp,i,1);
    temp++;
  }
  //���ζ���֮ǰд�������,����У��
   for(i=0;i<1024*1024;i+=4096)
  {
      FSMC_SRAM_ReadBuffer(&temp,i,1);
    if(i==0)sval=temp;
     else if(temp<=sval)break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.
    LCD_ShowxNum(x+15*8,y,(u16)(temp-sval+1)*4,4,16,0);//��ʾ�ڴ�����
   }
}

int main(void)
{
  u8 key;

  testsram = (u32*)0x68000000;
   u8 i=0;
  u32 ts=0;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
  delay_init(168);  //��ʼ����ʱ����
  uart_init(115200);    //��ʼ�����ڲ�����Ϊ115200

  LED_Init();          //��ʼ��LED
   LCD_Init();         //LCD��ʼ��
   KEY_Init();          //������ʼ��
   FSMC_SRAM_Init();      //��ʼ���ⲿSRAM
   POINT_COLOR=RED;//��������Ϊ��ɫ
  LCD_ShowString(30,50,200,16,16,"STM32F4");
  LCD_ShowString(30,70,200,16,16,"SRAM TEST");
  LCD_ShowString(30,90,200,16,16,"www.csun.co.jp");
  LCD_ShowString(30,110,200,16,16,"2015/08/14");
  LCD_ShowString(30,130,200,16,16,"KEY0:Test Sram");
  LCD_ShowString(30,150,200,16,16,"KEY1:Test Data");
   POINT_COLOR=BLUE;//��������Ϊ��ɫ

  for(ts=0;ts<250000;ts++)
  {
    testsram[ts] = ts;
  }

  while(1)
  {
    key=KEY_Scan(0);//��֧������
    if(key==KEY0_PRES)fsmc_sram_test(60,170);//����SRAM����
    else if(key==KEY1_PRES)//��ӡԤ���������
    {
      for(ts=0;ts<250000;ts++)LCD_ShowxNum(60,190,testsram[ts],6,16,0);//��ʾ��������
    }else delay_ms(10);
    i++;
    if(i==20)//DS0��˸.
    {
      i=0;
      LED0=!LED0;
     }
  }
}
