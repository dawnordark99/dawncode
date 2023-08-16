//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "stm32f4xx.h"
 
typedef struct {
	    uint8_t    Control_Mode;   // 控制模式  
	    uint8_t    TripFlagDMC;    // 过流 保护标志
	    uint8_t    drive_car;      // 开始驱动电机状态
	    uint8_t    olddrive_car;   // 历史开始驱动电机状态
	    uint8_t    clear_PWMtripz; // 清除保护标志
	    uint8_t    Run_mode;       // 正反转运行状态
	    uint16_t   Qiehuan_count;  // 切换状态的计数值
	    uint8_t    Start_order;    // 启动PWM，启动电机
	    uint16_t   Duty;           // 切换状态的计数值
	    uint16_t   Speed_Count;    // 速度环计时
	    uint16_t   Current_Count;  // 电流环计时	
			uint16_t   Aim_Speed;
			uint32_t   Aim_Duty;
	   	 }State; 

#define  State_DEFAULTS  {0,0,0,0,0,0,0,1,0,0,0,0,0} // 初始化参数

uint8_t Key_Scanning(void);

void OLED_Display(void);
#endif
