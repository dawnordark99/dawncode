//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#ifndef _TIMER_H
#define _TIMER_H 

#include "stm32f4xx.h"

void SysTickConfig(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void LED_Init(void);
#endif
