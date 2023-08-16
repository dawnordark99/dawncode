//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################

/*
1���������ã�������ˢֱ�������������ת����
2��Ӳ�����ӣ�1)�Ӻõ����������������2������24V��Դ��
3�����Է�����ͨ��RUN,STOP,UP,DOWN�ĸ������ı�����ͣ��ת��ռ�ձ�
4��Ԥ�ڽ���������ĸ������󣬵��״̬���Ը��ݰ����ı�״̬��
5��ע�����
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
void Delay(u32 nCount)	 //�򵥵���ʱ����
{
	u16 t=10000;
	for(; nCount != 0; nCount--)//������ʱ�����Ǻ�0�Ƚ�
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
	LED_Init();		        //��ʼ��LED�˿�
	uart_init(9600);	//���ڳ�ʼ��������Ϊ115200
	Delay(10000);	
	OLED_Init();
	OLED_ShowString(0,0,"D1  D2  D3  D4",16);
	OLED_ShowString(0,2,"OFF",16);
	OLED_ShowString(32,2,"OFF",16);
	OLED_ShowString(64,2,"OFF",16);
	OLED_ShowString(96,2,"ON",16);
/*********************��2���ֽڵ�������1���ֽڱ���************************************/
    for (int i = 0; i < 500; i++) {
        // Split the uint16_t value into high and low bytes
        uint8_t high_byte = (uint8_t)(dataStorage[i] >> 8); // Get high byte
        uint8_t low_byte = (uint8_t)(dataStorage[i] & 0xFF); // Get low byte

        // Save the high and low bytes in the uint8_t array
        uint8_array[i * 2] = high_byte;
        uint8_array[i * 2 + 1] = low_byte;
    }
	
    Modbus_Poll_Init(0x01);  //����ӻ���ַ
	

	while(1)
	{

  
	}
}

