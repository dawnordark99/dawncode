//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#ifndef _TIMER_H
#define _TIMER_H 

#include "stm32f4xx.h"

void SysTickConfig(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void LED_Init(void);
#endif
