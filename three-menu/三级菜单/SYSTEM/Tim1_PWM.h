//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#ifndef _Tim1_PWM_H
#define _Tim1_PWM_H 
#include "stm32f4xx.h"

void Tim1_PWM_Init(void);
void TIM1_Gpio(void);
void Start_Motor(void);
void Stop_Motor(void);
void MOS_Q41PWM(void) ;    // 切换Q41MOS驱动
void MOS_Q16PWM(void) ;    // 切换Q16MOS驱动
void MOS_Q63PWM(void) ;    // 切换Q63MOS驱动
void MOS_Q32PWM(void) ;    // 切换Q32MOS驱动
void MOS_Q25PWM(void) ;    // 切换Q25MOS驱动
void MOS_Q54PWM(void) ;    // 切换Q54MOS驱动
void DRV8313_GPIO(void);
#endif
