//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################

/*
1、程序作用：测试无刷直流电机霍尔开环转动；
2、硬件连接：1)接好电机、霍尔传感器；2）接上24V电源；
3、测试方法：通过RUN,STOP,UP,DOWN四个按键改变电机启停、转动占空比
4、预期结果：操作四个按键后，电机状态可以根据按键改变状态；
5、注意事项：
*/

#include "stm32f4xx.h"
#include "Timer.h"
#include "dma.h"
#include "OLED.h"
#include "usart.h"	
#include "DataScope_DP.h"
#include "modrol_key.h"
#include "modrol_menu.h"
#include "Variables.h"
#include "find_data.h"
#include "slave_rtu.h"

extern uint16_t dataStorage[];


uint8_t uint8_array[500]={0}; // Each uint16_t splits into 2 uint8_t elements
void Delay(u32 nCount)	 //简单的延时函数
{
	u16 t=10000;
	for(; nCount != 0; nCount--)//此种延时函数是和0比较
	for(;t!=0;t--);
} 

int main(void)
{	

	
	Delay(10000);	
	SysTickConfig();

	Delay(10000);	
	TIM3_Int_Init(7200-1,10-1);
    Key_Gpio();	
	keyInit();
	LED_Init();		        //初始化LED端口
	uart_init(9600);	//串口初始化波特率为115200
	Delay(10000);	
	OLED_Init();
	OLED_ShowString(0,0,"D1  D2  D3  D4",16);
	OLED_ShowString(0,2,"OFF",16);
	OLED_ShowString(32,2,"OFF",16);
	OLED_ShowString(64,2,"OFF",16);
	OLED_ShowString(96,2,"ON",16);
/*********************将2个字节的数组拆成1个字节保存************************************/
    for (int i = 0; i < 500; i++) {
        // Split the uint16_t value into high and low bytes
        uint8_t high_byte = (uint8_t)(dataStorage[i] >> 8); // Get high byte
        uint8_t low_byte = (uint8_t)(dataStorage[i] & 0xFF); // Get low byte

        // Save the high and low bytes in the uint8_t array
        uint8_array[i * 2] = high_byte;
        uint8_array[i * 2 + 1] = low_byte;
    }
	
    Modbus_Poll_Init(0x01);  //定义从机地址
	

	while(1)
	{

  
	}
}

