#ifndef __USMART_H
#define __USMART_H	  		  
#include "usmart_str.h"
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F4����-�⺯���汾
/////////////////////////////////////////////////////////////////////////////////////
//USMART��Դռ�����@MDK 3.80A@2.0�汾��
//FLASH:4K~K�ֽ�(ͨ��USMART_USE_HELP��USMART_USE_WRFUNS����)
//SRAM:72�ֽ�(���ٵ������)
//SRAM���㹫ʽ:   SRAM=PARM_LEN+72-4  ����PARM_LEN������ڵ���4.
//Ӧ�ñ�֤��ջ��С��100���ֽ�.
////////////////////////////////////////////�û����ò���////////////////////////////////////////////////////	  
#define MAX_FNAME_LEN 		30	//��������󳤶ȣ�Ӧ������Ϊ��С����������ĳ��ȡ�											   
#define MAX_PARM 			10	//���Ϊ10������ ,�޸Ĵ˲���,�����޸�usmart_exe��֮��Ӧ.
#define PARM_LEN 			200	//���в���֮�͵ĳ��Ȳ�����PARM_LEN���ֽ�,ע�⴮�ڽ��ղ���Ҫ��֮��Ӧ(��С��PARM_LEN)


#define USMART_ENTIMX_SCAN 	1	//ʹ��TIM�Ķ�ʱ�ж���ɨ��SCAN����,�������Ϊ0,��Ҫ�Լ�ʵ�ָ�һ��ʱ��ɨ��һ��scan����.
								//ע��:���Ҫ��runtimeͳ�ƹ���,��������USMART_ENTIMX_SCANΪ1!!!!
								
#define USMART_USE_HELP		1	//ʹ�ð�������ֵ��Ϊ0�����Խ�ʡ��700���ֽڣ����ǽ������޷���ʾ������Ϣ��
#define USMART_USE_WRFUNS	1	//ʹ�ö�д����,ʹ������,���Զ�ȡ�κε�ַ��ֵ,������д�Ĵ�����ֵ.
///////////////////////////////////////////////END///////////////////////////////////////////////////////////

#define USMART_OK 			0  //�޴���
#define USMART_FUNCERR 		1  //��������
#define USMART_PARMERR 		2  //��������
#define USMART_PARMOVER 	3  //�������
#define USMART_NOFUNCFIND 	4  //δ�ҵ�ƥ�亯��

#define SP_TYPE_DEC      	0  //10���Ʋ�����ʾ
#define SP_TYPE_HEX       	1  //16���Ʋ�����ʾ


 //�������б�	 
struct _m_usmart_nametab
{
	void* func;			//����ָ��
	const u8* name;		//������(���Ҵ�)	 
};
//usmart
struct _m_usmart_dev
{
	struct _m_usmart_nametab *funs;	//������ָ��

	void (*init)(u8);
	u8 (*cmd_rec)(u8*str);
	void (*exe)(void);
	void (*scan)(void);
	u8 fnum;
	u8 pnum;
	u8 id;							//id
	u8 sptype;						//:0,10;1,16;
	u16 parmtype;
	u8  plentbl[MAX_PARM];
	u8  parm[PARM_LEN];
	u8 runtimeflag;					//0,;1,,:USMART_ENTIMX_SCAN
	u32 runtime;					//:0.1ms,*0.1ms
};
extern struct _m_usmart_nametab usmart_nametab[];	//��usmart_config.c���涨��
extern struct _m_usmart_dev usmart_dev;				//��usmart_config.c���涨��


void usmart_init(u8 sysclk);
u8 usmart_cmd_rec(u8*str);
void usmart_exe(void);
void usmart_scan(void);
u32 read_addr(u32 addr);
void write_addr(u32 addr,u32 val);
u32 usmart_get_runtime(void);
void usmart_reset_runtime(void);

#endif





























