/**********************************************************************************************************************
 * @file   DataScope_DP.c
 * @brief  配合 MiniBalance 上位机使用 串口1
**********************************************************************************************************************/

#include "DataScope_DP.h"
#include "usart.h"
#include "State_Machine.h"
#include "GPIO_int.h"
unsigned char DataScope_OutPut_Buffer[42] = {0};	   //串口发送缓冲区

/*--------------------------------------------------------------------------------------------------
    与上位机通信的数据帧长度固定为4N*1，帧数据字节之间传输延时不可超过1ms，否则将认为当前帧结束。
    UI刷新没有限制显示延时
----------------------------------------------------------------------------------------------------*/

//函数说明：将单精度浮点数据转成4字节数据并存入指定地址
//附加说明：用户无需直接操作此函数
//target:目标单精度数据
//buf:待写入数组
//beg:指定从数组第几个元素开始写入
//函数无返回值
void Float2Byte(float *target,unsigned char *buf,unsigned char beg)
{
    unsigned char *point;
    point = (unsigned char*)target;	  //得到float的地址
    buf[beg]   = point[0];
    buf[beg+1] = point[1];
    buf[beg+2] = point[2];
    buf[beg+3] = point[3];
}
 
//函数说明：将待发送通道的单精度浮点数据写入发送缓冲区
//Data:通道数据
//Channel:选择通道(1-10)
//函数无返回 
void DataScope_Get_Channel_Data(float Data,unsigned char Channel)
{
	if ( (Channel > 10) || (Channel == 0) ) return;  //通道个数大于10或者等于0，直接跳出函数
  else
  {
     switch (Channel)
		{
      case 1:  Float2Byte(&Data,DataScope_OutPut_Buffer,1); break;
      case 2:  Float2Byte(&Data,DataScope_OutPut_Buffer,5); break;
		  case 3:  Float2Byte(&Data,DataScope_OutPut_Buffer,9); break;
		  case 4:  Float2Byte(&Data,DataScope_OutPut_Buffer,13); break;
		  case 5:  Float2Byte(&Data,DataScope_OutPut_Buffer,17); break;
		  case 6:  Float2Byte(&Data,DataScope_OutPut_Buffer,21); break;
		  case 7:  Float2Byte(&Data,DataScope_OutPut_Buffer,25); break;
		  case 8:  Float2Byte(&Data,DataScope_OutPut_Buffer,29); break;
		  case 9:  Float2Byte(&Data,DataScope_OutPut_Buffer,33); break;
		  case 10: Float2Byte(&Data,DataScope_OutPut_Buffer,37); break;
		}
  }	 
}


//函数说明：生成DataScopeV1.0能正确识别的帧格式
//Channel_Number,需要发送的通道个数
//返回发送缓冲区数据的个数
//返回0表示帧格式生成失败
unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
	if ( (Channel_Number > 10) || (Channel_Number == 0) ) { return 0; }  //通道个数大于10或者等于0，直接跳出函数
  else
  {	
	 DataScope_OutPut_Buffer[0] = '$';  //帧头
		
	 switch(Channel_Number)   
   { 
		 case 1:   DataScope_OutPut_Buffer[5]  =  5; return  6;  
		 case 2:   DataScope_OutPut_Buffer[9]  =  9; return 10;
		 case 3:   DataScope_OutPut_Buffer[13] = 13; return 14; 
		 case 4:   DataScope_OutPut_Buffer[17] = 17; return 18;
		 case 5:   DataScope_OutPut_Buffer[21] = 21; return 22;  
		 case 6:   DataScope_OutPut_Buffer[25] = 25; return 26;
		 case 7:   DataScope_OutPut_Buffer[29] = 29; return 30; 
		 case 8:   DataScope_OutPut_Buffer[33] = 33; return 34; 
		 case 9:   DataScope_OutPut_Buffer[37] = 37; return 38;
     case 10:  DataScope_OutPut_Buffer[41] = 41; return 42; 
   }	 
  }
	return 0;
}

/*-------------------主函数调试-------------------------------------

unsigned char i;          //计数变量
unsigned char Send_Count; //串口需要发送的数据个数
static float a;

	while(1)
		{
			a+=0.1;
			if(a>3.14)  a=-3.14; 
			DataScope_Get_Channel_Data(500*sin(a), 1 );
			DataScope_Get_Channel_Data(500* tan(a), 2 );
			DataScope_Get_Channel_Data( 500*cos(a), 3 ); 
			DataScope_Get_Channel_Data( 100*a , 4 );   
			DataScope_Get_Channel_Data(0, 5 );
			DataScope_Get_Channel_Data(0 , 6 );
			DataScope_Get_Channel_Data(0, 7 );
			DataScope_Get_Channel_Data( 0, 8 ); 
			DataScope_Get_Channel_Data(0, 9 );  
			DataScope_Get_Channel_Data( 0 , 10);
			Send_Count = DataScope_Data_Generate(10);
			for( i = 0 ; i < Send_Count; i++) 
			{
			while((USART1->SR&0X40)==0);  
			USART1->DR = DataScope_OutPut_Buffer[i]; 
			}
			delay_ms(50); //20HZ
		} 

------------------------------------------------------------------------------*/
void Printf_Task(void)
{
	    unsigned char i;          //计数变量
       unsigned char Send_Count; //串口需要发送的数据个数
//		DataScope_Get_Channel_Data( Key_Scanning() /1.000f , 1 );
//		DataScope_Get_Channel_Data( stru_HallDef.buffer.State[1] /1.000f , 2 );
//		DataScope_Get_Channel_Data( 3 /1.000f , 3 );
//		DataScope_Get_Channel_Data( 4 /1.000f , 4 );
//		DataScope_Get_Channel_Data( 5 /1.000f , 5 );

			Send_Count = DataScope_Data_Generate(10);
			for( i = 0 ; i < Send_Count; i++) 
			{
			while((USART1->SR&0X40)==0);  
			USART1->DR = DataScope_OutPut_Buffer[i]; 
			}
		Delay(100000);	


}
