//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#ifndef __DRIVE_PARAMETERS_H
#define __DRIVE_PARAMETERS_H

#define TEST_MANUELL                    0

//������ʽ
#define  HALL

//�޻�����ʽ
//#define  HALLLESS

//1:������2���ٶȻ���3�ٶȻ�����������
#define  LOOP                           1 

//1����ת��2����ת
#define  INVERSION                      1

//PWMƵ�ʣ���λKHZ;
#define  PWM_FREQ                      ((u16) 16) 

//����Ŀ��ת�أ������������� 
#define  Torque                         0xBB 

//Hallless �˲�����
#define  FILTER_LONG                    0xFF 

//�ٶ�PID
#define  Speed_Kp                       0.1 
#define  Speed_Ki                       0.01 
#define  Speed_Kc                       0.01 

//����PID
#define  Current_Kp                     2 
#define  Current_Ki                     0.2
#define  Current_Kc                     0.01 

//H_PWM_L_PWM����ģʽ
//#define  H_PWM_L_PWM

//H_PWM_L_ON����ģʽ
#define  H_PWM_L_ON

//H_ON_L_PWM����ģʽ
//#define  H_ON_L_PWM

#endif
