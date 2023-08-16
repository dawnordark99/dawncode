//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#ifndef __DRIVE_PARAMETERS_H
#define __DRIVE_PARAMETERS_H

#define TEST_MANUELL                    0

//霍尔方式
#define  HALL

//无霍尔方式
//#define  HALLLESS

//1:开环；2：速度环；3速度环、电流环；
#define  LOOP                           1 

//1：正转；2：反转
#define  INVERSION                      1

//PWM频率，单位KHZ;
#define  PWM_FREQ                      ((u16) 16) 

//配置目标转矩，增大可提高力矩 
#define  Torque                         0xBB 

//Hallless 滤波长度
#define  FILTER_LONG                    0xFF 

//速度PID
#define  Speed_Kp                       0.1 
#define  Speed_Ki                       0.01 
#define  Speed_Kc                       0.01 

//电流PID
#define  Current_Kp                     2 
#define  Current_Ki                     0.2
#define  Current_Kc                     0.01 

//H_PWM_L_PWM驱动模式
//#define  H_PWM_L_PWM

//H_PWM_L_ON驱动模式
#define  H_PWM_L_ON

//H_ON_L_PWM驱动模式
//#define  H_ON_L_PWM

#endif
