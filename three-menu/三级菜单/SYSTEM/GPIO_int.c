//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#include "GPIO_int.h"
#include "Control stage parameters.h"



void Hall_Gpio(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALL_A_GPIO_PIN|HALL_B_GPIO_PIN|HALL_C_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HALL_A_GPIO_PORT, &GPIO_InitStructure);
}

void Hallless_Gpio(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure; 	 
  
	GPIO_InitStructure.GPIO_Pin = HALLLESS_A_GPIO_PIN|HALLLESS_B_GPIO_PIN|HALLLESS_C_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(HALLLESS_A_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Debug_HALLLESS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Debug_HALLLESS_PORT, &GPIO_InitStructure);
}
