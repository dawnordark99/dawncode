//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#ifndef _Tim1_PWM_H
#define _Tim1_PWM_H 
#include "stm32f4xx.h"

void Tim1_PWM_Init(void);
void TIM1_Gpio(void);
void Start_Motor(void);
void Stop_Motor(void);
void MOS_Q41PWM(void) ;    // �л�Q41MOS����
void MOS_Q16PWM(void) ;    // �л�Q16MOS����
void MOS_Q63PWM(void) ;    // �л�Q63MOS����
void MOS_Q32PWM(void) ;    // �л�Q32MOS����
void MOS_Q25PWM(void) ;    // �л�Q25MOS����
void MOS_Q54PWM(void) ;    // �л�Q54MOS����
void DRV8313_GPIO(void);
#endif
