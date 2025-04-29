#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "24cxx.h" 
//////////////////////////////////////////////////////////////////////////////////	 
 
//´¥ÃşÆÁÇı¶¯£¨Ö§³ÖADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001AµÈ£© ´úÂë	   
//STM32F4¹¤³Ì-¿âº¯Êı°æ±¾
//////////////////////////////////////////////////////////////////////////////////

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,
	0,
	0,
};
//Ä¬ÈÏÎªtouchtype=0µÄÊı¾İ.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

//SPIĞ´Êı¾İ
//Ïò´¥ÃşÆÁICĞ´Èë1byteÊı¾İ
//num:ÒªĞ´ÈëµÄÊı¾İ
void TP_Write_Byte(u8 num)
{
	u8 count=0;
	for(count=0;count<8;count++)
	{ 	  
		if(num&0x80)TDIN=1;
		else TDIN=0;
		num<<=1;
		TCLK=0;
		delay_us(1);
		TCLK=1;		//ÉÏÉıÑØÓĞĞ§
	}
}
//SPI¶ÁÊı¾İ 
//´Ó´¥ÃşÆÁIC¶ÁÈ¡adcÖµ
//CMD:Ö¸Áî
//·µ»ØÖµ:¶Áµ½µÄÊı¾İ
u16 TP_Read_AD(u8 CMD)
{ 	 
	u8 count=0;
	u16 Num=0;
	TCLK=0;		//ÏÈÀ­µÍÊ±ÖÓ
	TDIN=0; 	//À­µÍÊı¾İÏß
	TCS=0; 		//Ñ¡ÖĞ´¥ÃşÆÁIC
	TP_Write_Byte(CMD);//·¢ËÍÃüÁî×Ö
	delay_us(6);//ADS7846µÄ×ª»»Ê±¼ä×î³¤Îª6us
	TCLK=0;
	delay_us(1);
	TCLK=1;		//¸ø1¸öÊ±ÖÓ£¬Çå³ıBUSY
	delay_us(1);
	TCLK=0;
	for(count=0;count<16;count++)//¶Á³ö16Î»Êı¾İ,Ö»ÓĞ¸ß12Î»ÓĞĞ§ 
	{
		Num<<=1;
		TCLK=0;	//ÏÂ½µÑØÓĞĞ§
		delay_us(1);
 		TCLK=1;
 		if(DOUT)Num++;
	}  	
	Num>>=4;   	//Ö»ÓĞ¸ß12Î»ÓĞĞ§.
	TCS=1;		//ÊÍ·ÅÆ¬Ñ¡
	return(Num);   
}
//¶ÁÈ¡Ò»¸ö×ø±êÖµ(x»òÕßy)
//Á¬Ğø¶ÁÈ¡READ_TIMES´ÎÊı¾İ,¶ÔÕâĞ©Êı¾İÉıĞòÅÅÁĞ,
//È»ºóÈ¥µô×îµÍºÍ×î¸ßLOST_VAL¸öÊı,È¡Æ½¾ùÖµ 
//xy:Ö¸Áî£¨CMD_RDX/CMD_RDY£©
//·µ»ØÖµ:¶Áµ½µÄÊı¾İ
#define READ_TIMES 5 	//¶ÁÈ¡´ÎÊı
#define LOST_VAL 1	  	//¶ªÆúÖµ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//ÅÅĞò
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//ÉıĞòÅÅÁĞ
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 
//¶ÁÈ¡x,y×ø±ê
//×îĞ¡Öµ²»ÄÜÉÙÓÚ100.
//x,y:¶ÁÈ¡µ½µÄ×ø±êÖµ
//·µ»ØÖµ:0,Ê§°Ü;1,³É¹¦¡£
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);
	//if(xtemp<100||ytemp<100)return 0;//¶ÁÊıÊ§°Ü
	*x=xtemp;
	*y=ytemp;
	return 1;//¶ÁÊı³É¹¦
}
//Á¬Ğø2´Î¶ÁÈ¡´¥ÃşÆÁIC,ÇÒÕâÁ½´ÎµÄÆ«²î²»ÄÜ³¬¹ı
//ERR_RANGE,Âú×ãÌõ¼ş,ÔòÈÏÎª¶ÁÊıÕıÈ·,·ñÔò¶ÁÊı´íÎó.
//¸Ãº¯ÊıÄÜ´ó´óÌá¸ß×¼È·¶È
//x,y:¶ÁÈ¡µ½µÄ×ø±êÖµ
//·µ»ØÖµ:0,Ê§°Ü;1,³É¹¦¡£
#define ERR_RANGE 50 //Îó²î·¶Î§ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//Ç°ºóÁ½´Î²ÉÑùÔÚ+-50ÄÚ
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
}  
//////////////////////////////////////////////////////////////////////////////////
//ÓëLCD²¿·ÖÓĞ¹ØµÄº¯Êı  
//»­Ò»¸ö´¥Ãşµã
//ÓÃÀ´Ğ£×¼ÓÃµÄ
//x,y:×ø±ê
//color:ÑÕÉ«
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//ºáÏß
	LCD_DrawLine(x,y-12,x,y+13);//ÊúÏß
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	LCD_Draw_Circle(x,y,6);//»­ÖĞĞÄÈ¦
}	  
//»­Ò»¸ö´óµã(2*2µÄµã)		   
//x,y:×ø±ê
//color:ÑÕÉ«
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//ÖĞĞÄµã 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}						  
//////////////////////////////////////////////////////////////////////////////////
//´¥Ãş°´¼üÉ¨Ãè
//tp:0,ÆÁÄ»×ø±ê;1,ÎïÀí×ø±ê(Ğ£×¼µÈÌØÊâ³¡ºÏÓÃ)
//·µ»ØÖµ:µ±Ç°´¥ÆÁ×´Ì¬.
//0,´¥ÆÁÎŞ´¥Ãş;1,´¥ÆÁÓĞ´¥Ãş
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//ÓĞ°´¼ü°´ÏÂ
	{
		if(tp)TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//¶ÁÈ¡ÎïÀí×ø±ê
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//¶ÁÈ¡ÆÁÄ»×ø±ê
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//½«½á¹û×ª»»ÎªÆÁÄ»×ø±ê
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//Ö®Ç°Ã»ÓĞ±»°´ÏÂ
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//°´¼ü°´ÏÂ  
			tp_dev.x[4]=tp_dev.x[0];//¼ÇÂ¼µÚÒ»´Î°´ÏÂÊ±µÄ×ø±ê
			tp_dev.y[4]=tp_dev.y[0];
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//Ö®Ç°ÊÇ±»°´ÏÂµÄ
		{
			tp_dev.sta&=~(1<<7);//±ê¼Ç°´¼üËÉ¿ª	
		}else//Ö®Ç°¾ÍÃ»ÓĞ±»°´ÏÂ
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//·µ»Øµ±Ç°µÄ´¥ÆÁ×´Ì¬
}	  
//////////////////////////////////////////////////////////////////////////	 
//±£´æÔÚEEPROMÀïÃæµÄµØÖ·Çø¼ä»ùÖ·,Õ¼ÓÃ13¸ö×Ö½Ú(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
//±£´æĞ£×¼²ÎÊı
void TP_Save_Adjdata(void)
{
	s32 temp;
	//±£´æĞ£Õı½á¹û!
	temp=tp_dev.xfac*100000000;//±£´æxĞ£ÕıÒòËØ
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);
	temp=tp_dev.yfac*100000000;//±£´æyĞ£ÕıÒòËØ
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp,4);
	//±£´æxÆ«ÒÆÁ¿
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);
	//±£´æyÆ«ÒÆÁ¿
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);
	//±£´æ´¥ÆÁÀàĞÍ
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);
	temp=0X0A;//±ê¼ÇĞ£×¼¹ıÁË
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
}
//µÃµ½±£´æÔÚEEPROMÀïÃæµÄĞ£×¼Öµ
//·µ»ØÖµ£º1£¬³É¹¦»ñÈ¡Êı¾İ
//        0£¬»ñÈ¡Ê§°Ü£¬ÒªÖØĞÂĞ£×¼
u8 TP_Get_Adjdata(void)
{					  
	s32 tempfac;
	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);//¶ÁÈ¡±ê¼Ç×Ö,¿´ÊÇ·ñĞ£×¼¹ı£¡
	if(tempfac==0X0A)//´¥ÃşÆÁÒÑ¾­Ğ£×¼¹ıÁË
	{
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);		   
		tp_dev.xfac=(float)tempfac/100000000;//µÃµ½xĞ£×¼²ÎÊı
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);
		tp_dev.yfac=(float)tempfac/100000000;//µÃµ½yĞ£×¼²ÎÊı
	    //µÃµ½xÆ«ÒÆÁ¿
		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);
 	    //µÃµ½yÆ«ÒÆÁ¿
		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);
 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//¶ÁÈ¡´¥ÆÁÀàĞÍ±ê¼Ç
		if(tp_dev.touchtype)//X,Y·½ÏòÓëÆÁÄ»Ïà·´
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y·½ÏòÓëÆÁÄ»ÏàÍ¬
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}	 
//ÌáÊ¾×Ö·û´®
u8* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

//ÌáÊ¾Ğ£×¼½á¹û(¸÷¸ö²ÎÊı)
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");
	LCD_ShowNum(40+24,160,x0,4,16);		//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24+80,160,y0,4,16);	//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24,180,x1,4,16);		//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24+80,180,y1,4,16);	//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24,200,x2,4,16);		//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24+80,200,y2,4,16);	//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24,220,x3,4,16);		//ÏÔÊ¾ÊıÖµ
	LCD_ShowNum(40+24+80,220,y3,4,16);	//ÏÔÊ¾ÊıÖµ
 	LCD_ShowNum(40+56,240,fac,3,16); 	//ÏÔÊ¾ÊıÖµ,¸ÃÊıÖµ±ØĞëÔÚ95~105·¶Î§Ö®ÄÚ.

}
		 
//´¥ÃşÆÁĞ£×¼´úÂë
//µÃµ½ËÄ¸öĞ£×¼²ÎÊı
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//×ø±ê»º´æÖµ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//ÇåÆÁ   
	POINT_COLOR=RED;//ºìÉ« 
	LCD_Clear(WHITE);//ÇåÆÁ 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//ÏÔÊ¾ÌáÊ¾ĞÅÏ¢
	TP_Drow_Touch_Point(20,20,RED);//»­µã1 
	tp_dev.sta=0;//Ïû³ı´¥·¢ĞÅºÅ 
	tp_dev.xfac=0;//xfacÓÃÀ´±ê¼ÇÊÇ·ñĞ£×¼¹ı,ËùÒÔĞ£×¼Ö®Ç°±ØĞëÇåµô!ÒÔÃâ´íÎó	 
	while(1)//Èç¹ûÁ¬Ğø10ÃëÖÓÃ»ÓĞ°´ÏÂ,Ôò×Ô¶¯ÍË³ö
	{
		tp_dev.scan(1);//É¨ÃèÎïÀí×ø±ê
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//°´¼ü°´ÏÂÁËÒ»´Î(´ËÊ±°´¼üËÉ¿ªÁË.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//±ê¼Ç°´¼üÒÑ¾­±»´¦Àí¹ıÁË.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//Çå³ıµã1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//»­µã2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//Çå³ıµã2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//»­µã3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//Çå³ıµã3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//»­µã4
					break;
				case 4:	 //È«²¿ËÄ¸öµãÒÑ¾­µÃµ½
	    		    //¶Ô±ßÏàµÈ
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//µÃµ½1,2µÄ¾àÀë
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//µÃµ½3,4µÄ¾àÀë
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//²»ºÏ¸ñ
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Çå³ıµã4
   	 					TP_Drow_Touch_Point(20,20,RED);								//»­µã1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êı¾İ   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//µÃµ½1,3µÄ¾àÀë
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//µÃµ½2,4µÄ¾àÀë
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//²»ºÏ¸ñ
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Çå³ıµã4
   	 					TP_Drow_Touch_Point(20,20,RED);								//»­µã1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êı¾İ   
						continue;
					}//ÕıÈ·ÁË
								   
					//¶Ô½ÇÏßÏàµÈ
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//µÃµ½1,4µÄ¾àÀë
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//µÃµ½2,3µÄ¾àÀë
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//²»ºÏ¸ñ
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Çå³ıµã4
   	 					TP_Drow_Touch_Point(20,20,RED);								//»­µã1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//ÏÔÊ¾Êı¾İ   
						continue;
					}//ÕıÈ·ÁË
					//¼ÆËã½á¹û
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//µÃµ½xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//µÃµ½xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//µÃµ½yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//µÃµ½yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//´¥ÆÁºÍÔ¤ÉèµÄÏà·´ÁË.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//Çå³ıµã4
   	 					TP_Drow_Touch_Point(20,20,RED);								//»­µã1
						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//ĞŞ¸Ä´¥ÆÁÀàĞÍ.
						if(tp_dev.touchtype)//X,Y·½ÏòÓëÆÁÄ»Ïà·´
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y·½ÏòÓëÆÁÄ»ÏàÍ¬
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//ÇåÆÁ
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");
					delay_ms(1000);
					TP_Save_Adjdata();
 					LCD_Clear(WHITE);//ÇåÆÁ
					return;//Ğ£ÕıÍê³É
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}	 
//´¥ÃşÆÁ³õÊ¼»
//·µ»ØÖµ:0,Ã»ÓĞ½øĞĞĞ£×¼
//       1,½øĞĞ¹ıĞ£×¼
u8 TP_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;	
	
	if(lcddev.id==0X5510)		//µçÈİ´¥ÃşÆÁ
	{
		if(GT9147_Init()==0)	//ÊÇGT9147
		{ 
			tp_dev.scan=GT9147_Scan;	//É¨Ãèº¯ÊıÖ¸ÏòGT9147´¥ÃşÆÁÉ¨Ãè
		}else
		{
			OTT2001A_Init();
			tp_dev.scan=OTT2001A_Scan;	//É¨Ãèº¯ÊıÖ¸ÏòOTT2001A´¥ÃşÆÁÉ¨Ãè
		}
		tp_dev.touchtype|=0X80;	//µçÈİÆÁ 
		tp_dev.touchtype|=lcddev.dir&0X01;//ºáÆÁ»¹ÊÇÊúÆÁ 
		return 0;
	}else
	{
  
		
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOF, ENABLE);//Ê¹ÄÜGPIOB,C,FÊ±ÖÓ

    //GPIOB1,2³õÊ¼»¯ÉèÖÃ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//PB1/PB2 ÉèÖÃÎªÉÏÀ­ÊäÈë
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//ÊäÈëÄ£Ê½
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ÉÏÀ­
    GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PB0ÉèÖÃÎªÍÆÍìÊä³ö
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//Êä³öÄ£Ê½
	  GPIO_Init(GPIOB, &GPIO_InitStructure);//³õÊ¼»¯
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PC13ÉèÖÃÎªÍÆÍìÊä³ö
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//Êä³öÄ£Ê½
	  GPIO_Init(GPIOC, &GPIO_InitStructure);//³õÊ¼»¯	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PF11ÉèÖÃÍÆÍìÊä³ö
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//Êä³öÄ£Ê½
	  GPIO_Init(GPIOF, &GPIO_InitStructure);//³õÊ¼»¯			
		
   
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//µÚÒ»´Î¶ÁÈ¡³õÊ¼»¯	 
		AT24CXX_Init();		//³õÊ¼»¯24CXX
		if(TP_Get_Adjdata())return 0;//ÒÑ¾­Ğ£×¼
		else			   //Î´Ğ£×¼?
		{ 										    
			LCD_Clear(WHITE);//ÇåÆÁ
			TP_Adjust();  	//ÆÁÄ»Ğ£×¼ 
			TP_Save_Adjdata();	 
		}			
		TP_Get_Adjdata();	
	}
	return 1; 									 
}

