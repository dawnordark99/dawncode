//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#include "State_Machine.h"
#include "OLED.h"
#include "ADC_int.h"
#include "Tim1_PWM.h"
#include "Control stage parameters.h"
#include "modrol_key.h"
	
uint8_t Key_Scanning(void)
{
	static uint8_t val=0;
	static uint16_t Count = 0;
	
	if(RUN_STATUS == 0 && (Count == 0))
	{
		
		OLED_ShowCHinese(0,2,12,1);
		OLED_ShowCHinese(16,2,13,1); 
		OLED_ShowCHinese(32,2,14,1);
		OLED_ShowCHinese(48,2,15,1); 
     	val =1;
		Count = 200;
	}
	
	if(STOP_STATUS == 0 && (Count == 0))
	{
		
		OLED_ShowCHinese(0,4,16,1);   
		OLED_ShowCHinese(16,4,17,1); 
		OLED_ShowCHinese(32,4,14,1);
		OLED_ShowCHinese(48,4,15,1); 
        val =2;
		Count = 200;
	}
	
	if(UP_STATUS == 0 && (Count == 0))
	{
		
		OLED_ShowCHinese(8,6,74,1);   
		OLED_ShowCHinese(24,6,61,1); 
		OLED_ShowCHinese(40,6,62,1);
		OLED_ShowCHinese(56,6,63,1); 

	    val =3;	
		Count = 200;
	}
	
	if(DOWN_STATUS == 0 && (Count == 0))
	{
		
		OLED_ShowCHinese(8,6,74,1);   
		OLED_ShowCHinese(24,6,61,1); 
		OLED_ShowCHinese(40,6,62,1);
		OLED_ShowCHinese(56,6,63,1); 
		OLED_ShowCHinese(72,6,73,1);
		OLED_ShowCHinese(88,6,71,1);
		OLED_ShowCHinese(104,6,72,1);
	    val =4;	
		Count = 200;
	}
	

	
	if (Count > 0)
	{
		
		Count--;
	}
	
	return val;
}

void OLED_Display(void)
{
	//第一行
//	OLED_ShowCHinese(32,0,46,1);
//	OLED_ShowCHinese(48,0,47,1); 
//	OLED_ShowCHinese(64,0,48,1);
//	OLED_ShowCHinese(80,0,49,1); 
    
}



