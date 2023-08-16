#include "dma.h"	
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include <string.h>
#include <stdlib.h>

#include "usart.h"	


/**********************************************************
* @brief    串口接收端DMA配置函数（从外设->存储器模式/8位数据宽度/存储器增量模式 ）
* @param	DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
*			chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
*			par:外设地址
*			mar:存储器地址
*			ndtr:数据传输量 
***********************************************************/
void u2DMA_RX_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;  //中断优先级配置
	
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	//DMA2时钟使能 
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);	//DMA1时钟使能 
	}
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}		//等待DMA可配置 
		
	/*配置 DMA Stream*/
	DMA_InitStructure.DMA_Channel = chx;  							//通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;					//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;					//DMA存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;			//外设到存储器
	DMA_InitStructure.DMA_BufferSize = ndtr;						//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 					//正常模式，即满了就不在接收了，而不是循环存储
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;			//高优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//存储器突发单次传输？？？
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输？？？
	DMA_Init(DMA_Streamx, &DMA_InitStructure);						//初始化DMA Stream
		
	DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);						//清除传输完成中断
	//开启DMA错误和接收完成中断 +（使能接收完成中断是为了防止一次性接收过长数据）
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TE|DMA_IT_TC, ENABLE);
	
	
		//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;//DMA中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	DMA_Cmd (DMA2_Stream5,ENABLE);									//使能DMA
}





/**********************************************************
* @brief    串口发送端DMA配置函数（从存储器->外设模式/8位数据宽度/存储器增量模式 ）
* @param	DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
*			chx:DMA通道选择,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
*			par:外设地址
*			mar:存储器地址
*			ndtr:数据传输量 
***********************************************************/ 
void u2DMA_TX_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef  DMA_InitStructure;
	if((u32)DMA_Streamx>(u32)DMA2)		//得到当前stream是属于DMA2还是DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	//DMA2时钟使能 
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);	//DMA1时钟使能 
	}
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}		//等待DMA可配置 
		
	/*配置 DMA Stream*/
	DMA_InitStructure.DMA_Channel = chx;  							//通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;					//DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;					//DMA存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;			//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = ndtr;						//数据传输量 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//正常模式，即满了就不在接收了，而不是循环存储
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;			//中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//存储器突发单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_Init(DMA_Streamx, &DMA_InitStructure);						//初始化DMA Stream
}


#define RECEIVE_BUF  100		//定义最大接收字
/************************************************************
* @brief    DMA2_Stream5的所有中断,防止接收到的帧过长无法触发串口空闲中断
* @param	
***********************************************************/
void DMA2_Stream5_IRQHandler(void)
{
if(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_TCIF5) != RESET)
	{
		DMA_Cmd(DMA2_Stream5, DISABLE); 					//关闭DMA,防止处理其间有数据
		
		Rx_DMA_receive_size = RECEIVE_BUF - DMA_GetCurrDataCounter(DMA2_Stream5);
		if(Rx_DMA_receive_size !=0)
		{
		printf("\r\nDMA DATA:\r\n"); 		
//			Receive_DataPack();/*处理接收数据函数*/;
		}
		DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//清除DMA2_Steam7传输完成标志
		while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE){}	//确保DMA可以被设置
		DMA_SetCurrDataCounter(DMA2_Stream5, RECEIVE_BUF);
        DMA_Cmd(DMA2_Stream5, ENABLE);    			 		//打开DMA
	}
}












