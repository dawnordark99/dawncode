//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#include "PI_Cale.h"
#include "Drive parameters.h"

extern    PIDREG_T       pi_spd;
extern    PIDREG_T       pi_ICurr;

void PID_init(void)
{
  pi_spd.Kp = Speed_Kp;    
  pi_spd.Ki = Speed_Ki;  
	pi_spd.Kc = Speed_Kc;
  pi_spd.OutMax =10000;
#ifdef HALLLESS	
  pi_spd.OutMin =1000;
#endif	
#ifdef HALL
  pi_spd.OutMin =1000;
#endif	
	pi_spd.Ref = 0;

  pi_ICurr.Kp = Current_Kp;    
  pi_ICurr.Ki = Current_Ki;  
	pi_ICurr.Kc = Current_Kc;
  pi_ICurr.OutMax =10000; 
  pi_ICurr.OutMin =1000;
	pi_ICurr.Ref = 0;	
	
 }


