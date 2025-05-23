#include "usmart.h"
#include "usmart_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
#include "delay.h"	 
#include "wm8978.h" 

//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(u32 nus)",
		

 	(void*)WM8978_Write_Reg,"void WM8978_Write_Reg(u8 reg,u8 val)", 
 	(void*)WM8978_Read_Reg,"u8 WM8978_Read_Reg(u8 reg)", 
 	(void*)WM8978_HPvol_Set,"void WM8978_HPvol_Set(u8 voll,u8 volr)", 
 	(void*)WM8978_SPKvol_Set,"void WM8978_SPKvol_Set(u8 volx)", 
 	(void*)WM8978_3D_Set,"void WM8978_3D_Set(u8 depth)",  
 	(void*)WM8978_EQ_3D_Dir,"void WM8978_EQ_3D_Dir(u8 dir)", 
 	(void*)WM8978_EQ1_Set,"void WM8978_EQ1_Set(u8 cfreq,u8 gain)", 
 	(void*)WM8978_EQ2_Set,"void WM8978_EQ2_Set(u8 cfreq,u8 gain)", 
 	(void*)WM8978_EQ3_Set,"void WM8978_EQ3_Set(u8 cfreq,u8 gain)", 
 	(void*)WM8978_EQ4_Set,"void WM8978_EQ4_Set(u8 cfreq,u8 gain)", 
 	(void*)WM8978_EQ5_Set,"void WM8978_EQ5_Set(u8 cfreq,u8 gain)", 	
};

///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),
	0,
	0,		//ID
	1,		//,0,10;1,16
	0,		//bitx:,0,;1,
	0,		//MAX_PARM
	0,		//PARM_LEN
};   



















