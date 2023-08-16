//#include "sys.h"
#include "usart.h"	
#include "stm32f4xx_rcc.h"
#include "dma.h"	
#include "MITSUBISHI_FX2.h"  

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "slave_rtu.h"

#define USE_USART_DMA_RX 2
         
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}

 

#define RECEIVE_BUF_SIZE  100		//定义最大接收字

extern  u8 Rx_DMA_receive[];  		//100bytes buffer
/************************************************************
* @brief    串口 1 初始化函数
* @param	bound:波特率
***********************************************************/
void uart_init(u32 bound)
	{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;  //中断优先级配置
		
	//开启GPIO与串口时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 		//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1); 	//GPIOB6复用为USART1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);	//GPIOB7复用为USART1
	
	//串口1的GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 	//GPIOA9,GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); 						//初始化PA9,PA10

    //USART1初始化设置
	USART_InitStructure.USART_BaudRate = bound;					//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_2;		//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//收发模式
	USART_Init(USART1, &USART_InitStructure); 					//初始化串口1
	
#if USE_USART_DMA_RX	
	//Usart1 开启中断 + NVIC 配置
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);				//开启串口空闲中断									
	

		//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_DMACmd(USART1,USART_DMAReq_Rx, ENABLE); 				//使能的DMA收
	//DMA2,STEAM5,CH4,外设为串口1,存储器为SendBuff,方向为外设到存储器，长度为:RECEIVE_BUF_SIZE.
	u2DMA_RX_Config(DMA2_Stream5,DMA_Channel_4,(u32)&USART1->DR,(u32)Rx_DMA_receive,100);
#else
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//开启串口接收中断
	NVIC_Configuration(3,3);
#endif
	
	
#if USE_USART_DMA_TX
	USART_DMACmd(USART1,USART_DMAReq_Tx, ENABLE); 				//使能的DMA发
	//DMA2,STEAM7,CH4,外设为串口1,存储器为Rxbuf,方向为存储器到外设，长度为:RECEIVE_BUF_SIZE.
 	u2DMA_TX_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)Rxbuf,0);	
#endif
	
	USART_Cmd(USART1, ENABLE); 					  				//使能串口1,要放最后
}




u8 dum;
u8 Rx_DMA_receive[RECEIVE_BUF_SIZE];  //100bytes
u8 Rx_DMA_receive_size = 0;


#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))
/************************************************************
* @brief    串口 1 中断函数
* @param	
***********************************************************/
void USART1_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
#if USE_USART_DMA_RX
	/*空闲中断 */
	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{	
		dum = USART1->SR;
		dum = USART1->DR;	//读取SR DR数据可清除“空闲中断”标志
		Rx_DMA_receive_size = RECEIVE_BUF_SIZE - DMA_GetCurrDataCounter(DMA2_Stream5); //本帧数据长度=DMA准备的接收数据长度-DMA已接收数据长度
		
		if( 0 != Rx_DMA_receive_size)
		{
			if(Rx_DMA_receive[1] < 0x30)    //modbus格式
			{	   Modbus_Poll(Rx_DMA_receive,Rx_DMA_receive_size);}
			else 
			{	
		    IScommandvalid();               //校验和格式
		    processResponse(getdataadress(),getdataCount());				
			}
//			for (int i = 0; i < Rx_DMA_receive_size; i++) 
//		{
////			printf("%x ", Rx_DMA_receive[i]);
//		}
////		    printf("%d ", Rx_DMA_receive_size);
			
		}
		
		DMA_Cmd(DMA2_Stream5,DISABLE);							//暂停DMA传输 
		// 清除DMA中断标志位
		DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//清除DMA2_Steam7传输完成标志
		while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE){}		//确保DMA可以被设置  
		DMA_SetCurrDataCounter(DMA2_Stream5,RECEIVE_BUF_SIZE); 	//设置DMA数据传输量 
		DMA_Cmd(DMA2_Stream5, ENABLE);                     		//开启DMA传输 
	}

#endif
	
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 


//			printf("Rx_DMA_receive_size:%d\n\r",Rx_DMA_receive_size); 		
//			printf("%c%c%c%c%c%c%c%c%c%c%c%c\n",Rx_DMA_receive[0],Rx_DMA_receive[1],Rx_DMA_receive[2],Rx_DMA_receive[3],Rx_DMA_receive[4],Rx_DMA_receive[5],Rx_DMA_receive[6],Rx_DMA_receive[7],Rx_DMA_receive[8],Rx_DMA_receive[9],Rx_DMA_receive[10],Rx_DMA_receive[11]); 		
//			Receive_DataPack();/*DMA接收模式时处理接收数据函数*/			
//          restoreHexNumber(Rx_DMA_receive[0],Rx_DMA_receive[1],dataStorage);
//			printf("Rx_DMA_receive_size:%d\n\r",dataStorage[0]); 	
//            dataLength = strlen((char*)Rx_DMA_receive);
 
uint16_t dataStorage[] = {
	0x0005, //  一级MNT位置第5个
	0x0009, //  一级U位置第9个
	0x0012, //  一级OP第18个
	0x0014, //  一级F第20个
	0x0021, //  一级P第33个
	
	0x0003,0x0000,0x0001,0x0002,  // 二级MNT 
	0x0008,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000a, //二级U0 - U7  3 - 10
	0x0001,0x000b, //二级OP   11
	0x000c,0x000c,0x000d,0x000e,0x000f,0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017, //二级F0 - FP 12 - 23
	0x0005,0x0018,0x0019,0x001a,0x001b,0x001c, //二级P1 - P5  24  - 28
	
	0x0005,  0x001d,0x001e,0x001f,0x0020,0x0021,   //U0.01 - U0.05  //29 - 33
	0x0010,  0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f,0x0030,0x0031,//U1.01 - U1.16 //34 - 49
	0x0007,  0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,  //U2.01 - U2.07 //50 - 56
	0x0012,  0x0039,0x003a,0x003b,0x003c,0x003d,0x003e,0x003f,0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004a,//U3.01 - U3.18 //57 - 74
	0x0012,  0x004b,0x004c,0x004d,0x004e,0x004f,0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005a,0x005b,0x005c,//U4.01 - U4.18 //75 - 92
	0x0012,  0x005d,0x005e,0x005f,0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,//U5.01 - U5.18 //93 - 110
	0x0012,  0x006f,0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x007b,0x007c,0x007d,0x007e,0x007f,0x0080,//U6.01 - U6.18 //111 - 128
	0x0002,  0x0081,0x0082, //U7.01 - U7.02 //129 - 130
	
	0x0009,  0x0083,0x0084, 0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,  //OP1 - OP9 //131 - 139
	
	0x0016,  0x008c,0x008d,0x008e,0x008f,0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f,0x00a0,0x00a1, //F0.01 - F0.22V //140 - 161
	0x0015,  0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6, //F1.01 - F1.21 // 162 - 182
	0x001f,  0x00b7,0x00b8,0x00b9,0x00ba,0x00bb,0x00bc,0x00bd,0x00be,0x00bf,0x00c0,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf,0x00d0,0x00d1,0x00d2,0x00d3,0x00d4,0x00d5,//F2.01 - F2.31 // 183 - 213
	0x000f,  0x00d6,0x00d7,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x00dd,0x00de,0x00df,0x00e0,0x00e1,0x00e2,0x00e3,0x00e4,//F3.01 - F3.15 // 214 - 228
	0x0018,  0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x00ec,0x00ed,0x00ee,0x00ef,0x00f0,0x00f1,0x00f2,0x00f3,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x00fa,0x00fb,0x00fc,//F4.01 - F4.24 // 229 - 252
	0x0014,  0x00fd,0x00fe,0x00ff,0x0100,0x0101,0x0102,0x0103,0x0104,0x0105,0x0106,0x0107,0x0108,0x0109,0x010a,0x010b,0x010c,0x010d,0x010e,0x010f,0x0110,//F5.01 - F5.20 //253 - 272
	0x0019,  0x0111,0x0112,0x0113,0x0114,0x0115,0x0116,0x0117,0x0118,0x0119,0x011a,0x011b,0x011c,0x011d,0x011e,0x011f,0x0120,0x0121,0x0122,0x0123,0x0124,0x0125,0x0126,0x0127,0x0128,0x0129,//F6.01 - F6.25 // 273 - 297
	0x0006,  0x012a,0x012b,0x012c,0x012d,0x012e,0x012f,//F7.01 - F7.06 // 298 - 303
	0x000c,  0x0130,0x0131,0x0132,0x0133,0x0134,0x0135,0x0136,0x0137,0x0138,0x0139,0x013a,0x013b,//F8.01 - F8.12 // 304 - 315
	0x000b,  0x013c,0x013d,0x013e,0x013f,0x0140,0x0141,0x0142,0x0143,0x0144,0x0145,0x0146,//F9.01 - F9.11 //  316 - 326
	0x001a,  0x0147,0x0148,0x0149,0x014a,0x014b,0x014c,0x014d,0x014e,0x014f,0x0150,0x0151,0x0152,0x0153,0x0154,0x0155,0x0156,0x0157,0x0158,0x0159,0x015a,0x015b,0x015c,0x015d,0x015e,0x015f,0x0160,//FA.01 - FA.26 // 327 - 352
	0x0004,  0x0161,0x0162,0x0163,0x0164,//FP.01 - FP.04 // 353 - 356
	
	0x0002,  0x0D65,0x0166, //P1.01 - P1.02 // 357 - 358
	0x0003,  0x0167,0x0168,0x0169,//P2.01 - P2.03 // 359 - 361
	0x0002,  0x016a,0x016b, //P3.01 - P3.02 // 362 - 363
	0x0006,  0x016c,0x016d,0x016e,0x016f,0x0170,0x0171,//P4.01 - P4.06 // 364 - 369
	0x0008,  0x0172,0x0173,0x0174,0x0175,0x0176,0x0177,0x0178,0x0179,//P5.01 - P5.08 // 370 - 377
};


#define array_length =  sizeof(dataStorage) / sizeof(dataStorage[0]) * 2;
