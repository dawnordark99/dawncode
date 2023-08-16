//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#include "stm32f4xx.h"


#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    
				   
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_9)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_9)


#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_8)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_8)


#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_5)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_4)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOB,GPIO_Pin_4)


#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_3)//CS
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 mod);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 mod);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowChar1(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowChar2(u8 x,u8 y,u8 chr);
