//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT电机控制开发板
//Author：廷华电子设计
//网址: https://shop461235811.taobao.com/
//############################################################
#include "Tim1_ISR_MCLoop.h"
#include "ADC_int.h"
#include "Hall.h"
#include "State_Machine.h"
#include "PI_Cale.h"
#include "VvvfControl.h"
#include "Hallless.h"
#include "Drive parameters.h"
#ifdef HALL
extern    Hall          Hall_Three;
#endif
#ifdef HALLLESS
extern    Hallless      Hallless_Three;
#endif
extern    PIDREG_T      pi_spd;
extern    PIDREG_T      pi_ICurr;
extern    State         StateContr;
extern    ADCSamp       ADCSampPare;
extern    VvvF_start    VvvF_startPare;

void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	if(StateContr.drive_car == 1)
	{	
#ifdef HALLLESS
	//开环起动
	if(StateContr.Start_order == 1)
	{
		Anwerfen();
		StateContr.Start_order = 2;	
	}
	if(StateContr.Start_order == 2)
	{	
		HALLLESS_ADC_Sample();
		Hallless_SW();

	//开环运行
	if(StateContr.Control_Mode == 1)
	{
		StateContr.Duty = StateContr.Aim_Duty;
	}
	//速度环
	if(StateContr.Control_Mode == 2)
	{
		StateContr.Speed_Count++;
		if(StateContr.Speed_Count > 2)
		{
			 pi_spd.Fdb = Hallless_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
			 StateContr.Duty= pi_spd.OutF;
		}
	}
	//电流环+速度环
	if(StateContr.Control_Mode == 3)
	{
		StateContr.Speed_Count++;
		StateContr.Current_Count++;
		if(StateContr.Speed_Count > 2)
		{
			 pi_spd.Fdb = Hallless_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
		}
		if(StateContr.Current_Count > 1)
		{
			 pi_ICurr.Ref = Torque;                                     //空载默认0.2A电流
			 pi_ICurr.Fdb = ADCSampPare.BUS_Curr;   
			 PID_CALC(pi_ICurr);
			 FirstOrder_LPF_Cacl(pi_ICurr.Out,pi_ICurr.OutF,0.08379f);
			 StateContr.Current_Count = 0;
		}
		if(pi_ICurr.OutF>pi_spd.OutF)	 
		{
			 StateContr.Duty= pi_spd.OutF;              
			 pi_ICurr.Ui  = pi_spd.Ui;
		} 
		else 
		{
			 StateContr.Duty= pi_ICurr.OutF;          
			 pi_spd.Ui  = pi_ICurr.Ui;
		}
	}
	}
	
#endif	
	
#ifdef HALL
  //HALL_ADC_Sample();
	Hall_SW();
	//开环
	if(StateContr.Control_Mode == 1)
	{
		StateContr.Duty = StateContr.Aim_Duty;
	}
	//速度环
	if(StateContr.Control_Mode == 2)
	{
		StateContr.Speed_Count++;
		if(StateContr.Speed_Count > 2)
		{
			 pi_spd.Fdb = Hall_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
			 StateContr.Duty= pi_spd.OutF;
		}
	}
	//电流环+速度环
	if(StateContr.Control_Mode == 3)
	{
		StateContr.Speed_Count++;
		StateContr.Current_Count++;
		if(StateContr.Speed_Count > 2)
		{
			 pi_spd.Fdb = Hall_Three.Speed_RPMF ;   
			 PID_CALC(pi_spd);
			 FirstOrder_LPF_Cacl(pi_spd.Out,pi_spd.OutF,0.08379f);
			 StateContr.Speed_Count = 0;
		}
		if(StateContr.Current_Count > 1)
		{
			 pi_ICurr.Ref = Torque;                                     //空载默认0.2A电流
			 pi_ICurr.Fdb = ADCSampPare.BUS_Curr ;   
			 PID_CALC(pi_ICurr);
			 FirstOrder_LPF_Cacl(pi_ICurr.Out,pi_ICurr.OutF,0.08379f);
			 StateContr.Current_Count = 0;
		}
		if(pi_ICurr.OutF>pi_spd.OutF)	 
		{
			 StateContr.Duty= pi_spd.OutF;              
			 pi_ICurr.Ui  = pi_spd.Ui;
		} 
		else 
		{
			 StateContr.Duty= pi_ICurr.OutF;          
			 pi_spd.Ui  = pi_ICurr.Ui;
		}
	}
#endif
}
}
