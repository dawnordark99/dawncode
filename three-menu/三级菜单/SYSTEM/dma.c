#include "dma.h"	
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include <string.h>
#include <stdlib.h>

#include "usart.h"	


/**********************************************************
* @brief    ���ڽ��ն�DMA���ú�����������->�洢��ģʽ/8λ���ݿ��/�洢������ģʽ ��
* @param	DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
*			chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
*			par:�����ַ
*			mar:�洢����ַ
*			ndtr:���ݴ����� 
***********************************************************/
void u2DMA_RX_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;  //�ж����ȼ�����
	
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	//DMA2ʱ��ʹ�� 
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);	//DMA1ʱ��ʹ�� 
	}
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}		//�ȴ�DMA������ 
		
	/*���� DMA Stream*/
	DMA_InitStructure.DMA_Channel = chx;  							//ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;					//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;					//DMA�洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;			//���赽�洢��
	DMA_InitStructure.DMA_BufferSize = ndtr;						//���ݴ����� 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 					//����ģʽ�������˾Ͳ��ڽ����ˣ�������ѭ���洢
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;			//�����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//�洢��ͻ�����δ��䣿����
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ��䣿����
	DMA_Init(DMA_Streamx, &DMA_InitStructure);						//��ʼ��DMA Stream
		
	DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);						//�����������ж�
	//����DMA����ͽ�������ж� +��ʹ�ܽ�������ж���Ϊ�˷�ֹһ���Խ��չ������ݣ�
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TE|DMA_IT_TC, ENABLE);
	
	
		//Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;//DMA�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	DMA_Cmd (DMA2_Stream5,ENABLE);									//ʹ��DMA
}





/**********************************************************
* @brief    ���ڷ��Ͷ�DMA���ú������Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ ��
* @param	DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
*			chx:DMAͨ��ѡ��,@ref DMA_channel DMA_Channel_0~DMA_Channel_7
*			par:�����ַ
*			mar:�洢����ַ
*			ndtr:���ݴ����� 
***********************************************************/ 
void u2DMA_TX_Config(DMA_Stream_TypeDef *DMA_Streamx,u32 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_InitTypeDef  DMA_InitStructure;
	if((u32)DMA_Streamx>(u32)DMA2)		//�õ���ǰstream������DMA2����DMA1
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);	//DMA2ʱ��ʹ�� 
	}else 
	{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);	//DMA1ʱ��ʹ�� 
	}
	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}		//�ȴ�DMA������ 
		
	/*���� DMA Stream*/
	DMA_InitStructure.DMA_Channel = chx;  							//ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;					//DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;					//DMA�洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;			//�洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize = ndtr;						//���ݴ����� 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;					//����ģʽ�������˾Ͳ��ڽ����ˣ�������ѭ���洢
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;			//�е����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;		//�洢��ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
	DMA_Init(DMA_Streamx, &DMA_InitStructure);						//��ʼ��DMA Stream
}


#define RECEIVE_BUF  100		//������������
/************************************************************
* @brief    DMA2_Stream5�������ж�,��ֹ���յ���֡�����޷��������ڿ����ж�
* @param	
***********************************************************/
void DMA2_Stream5_IRQHandler(void)
{
if(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_TCIF5) != RESET)
	{
		DMA_Cmd(DMA2_Stream5, DISABLE); 					//�ر�DMA,��ֹ�������������
		
		Rx_DMA_receive_size = RECEIVE_BUF - DMA_GetCurrDataCounter(DMA2_Stream5);
		if(Rx_DMA_receive_size !=0)
		{
		printf("\r\nDMA DATA:\r\n"); 		
//			Receive_DataPack();/*����������ݺ���*/;
		}
		DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//���DMA2_Steam7������ɱ�־
		while (DMA_GetCmdStatus(DMA2_Stream5) != DISABLE){}	//ȷ��DMA���Ա�����
		DMA_SetCurrDataCounter(DMA2_Stream5, RECEIVE_BUF);
        DMA_Cmd(DMA2_Stream5, ENABLE);    			 		//��DMA
	}
}












