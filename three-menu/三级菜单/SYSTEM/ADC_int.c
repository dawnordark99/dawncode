//############################################################
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//��Ȩ���У�����ؾ�
//EtherCAT������ƿ�����
//Author��͢���������
//��ַ: https://shop461235811.taobao.com/
//############################################################
#include "ADC_int.h"
#include "Hall.h"
#include "Control stage parameters.h"
#include "State_Machine.h"
#include "Hallless.h"
#include "Drive parameters.h"

uint32_t  ADC_ConvertedValue[5]={0,0,0,0,0};//��ʼ��AD����ֵ

extern volatile uint16_t ADC_DualConvertedValueTab[5];    //����ʱ����ȡֵ

extern    State       StateContr;
extern    ADCSamp     ADCSampPare;

#ifdef HALL
extern    Hall        Hall_Three;
#endif

#ifdef HALLLESS
extern    Hallless   Hallless_Three;
#endif
void Adc_Configuration(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA |  RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PHASE_U_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_U_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PHASE_V_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_V_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PHASE_W_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(PHASE_W_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BusVolt_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(BusVolt_GPIO_PORT, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = POT_GPIO_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
//	GPIO_Init(POT_GPIO_PORT, &GPIO_InitStructure);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (ADC1->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_DualConvertedValueTab;  //ADC������������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
     
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;  //2��Ƶ
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 4;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_RegularChannelConfig(ADC1, PHASE_U_CURR_CHANNEL, 1, ADC_SampleTime_15Cycles);
  ADC_RegularChannelConfig(ADC1, PHASE_V_CURR_CHANNEL, 2, ADC_SampleTime_15Cycles);
  ADC_RegularChannelConfig(ADC1, PHASE_W_CURR_CHANNEL, 3, ADC_SampleTime_15Cycles);  
	ADC_RegularChannelConfig(ADC1, BusVolt_CHANNEL, 4, ADC_SampleTime_15Cycles); 
	//ADC_RegularChannelConfig(ADC1, POT_CHANNEL, 5, ADC_SampleTime_15Cycles); 
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE); 
  ADC_SoftwareStartConv(ADC1);
}

void Offset_CurrentReading(void)  
{
	static uint16_t ADC_PhaseU_Curr[64];
	static uint16_t ADC_PhaseV_Curr[64];
	static uint16_t ADC_PhaseW_Curr[64];
	static uint8_t i=0;
   
  /* ADC Channel used for current reading are read  in order to get zero currents ADC values*/
  //64�β�����ƽ��ֵ��������������ʼУ׼   	
	ADC_PhaseU_Curr[i] = ADC_DualConvertedValueTab[0];
	ADC_PhaseV_Curr[i] = ADC_DualConvertedValueTab[1];  
	ADC_PhaseW_Curr[i] = ADC_DualConvertedValueTab[2];

	i++;
	if(i>=64)
	i=0;
 //�����������ĸ�ߵ����ĵ��跨��������,��Ҫ�ϵ��ȡ��ʼƫִ��ѹ
	if(StateContr.drive_car == 0) 
	{
		uint32_t sum_U=0;
		uint32_t sum_V=0;
		uint32_t sum_W=0;
		uint8_t i;
		for(i=0; i < 64; i++)
		{
			sum_U += ADC_PhaseU_Curr[i];
			sum_V += ADC_PhaseV_Curr[i];
			sum_W += ADC_PhaseW_Curr[i];
		}
	     ADCSampPare.OffsetPhaseU_Curr = sum_U /64;
	     ADCSampPare.OffsetPhaseV_Curr = sum_V /64;
	     ADCSampPare.OffsetPhaseW_Curr = sum_W /64;
	}
}
#ifdef HALL
void HALL_ADC_Sample(void) // ����PWM�жϽ����󣬲���ʱ��Ϊ1us����,(7cycles)  ��֤��PWM�жϺ����������Ϊ��PWM�м�ɼ������
{ 
  ADCSampPare.PhaseU_Curr  =    (ADC_DualConvertedValueTab[0]-ADCSampPare.OffsetPhaseU_Curr)<<1;	
	ADCSampPare.PhaseV_Curr  =    (ADC_DualConvertedValueTab[1]-ADCSampPare.OffsetPhaseV_Curr)<<1;
	ADCSampPare.PhaseW_Curr  =    (ADC_DualConvertedValueTab[2]-ADCSampPare.OffsetPhaseW_Curr)<<1;  
  ADCSampPare.BUS_Voltage  =     ADC_DualConvertedValueTab[3];	
	if(Hall_Three.Hall_State == 0x05)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseV_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hall_Three.Hall_State == 0x01)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hall_Three.Hall_State == 0x03)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hall_Three.Hall_State == 0x02)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hall_Three.Hall_State == 0x06)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hall_Three.Hall_State == 0x04)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseW_Curr + ADCSampPare.PhaseV_Curr;
	}
}
#endif

#ifdef HALLLESS
void HALLLESS_ADC_Sample(void) // ����PWM�жϽ����󣬲���ʱ��Ϊ1us����,(7cycles)  ��֤��PWM�жϺ����������Ϊ��PWM�м�ɼ������
{    
  ADCSampPare.PhaseU_Curr  =    (ADC_DualConvertedValueTab[0]-ADCSampPare.OffsetPhaseU_Curr)<<1;	
	ADCSampPare.PhaseV_Curr  =    (ADC_DualConvertedValueTab[1]-ADCSampPare.OffsetPhaseV_Curr)<<1;
	ADCSampPare.PhaseW_Curr  =    (ADC_DualConvertedValueTab[2]-ADCSampPare.OffsetPhaseW_Curr)<<1;  
  ADCSampPare.BUS_Voltage  =     ADC_DualConvertedValueTab[3];	
	if(Hallless_Three.Hallless_State == 0x05)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseV_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x01)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x03)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseW_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x02)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x06)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseU_Curr + ADCSampPare.PhaseV_Curr;
	}
	else if(Hallless_Three.Hallless_State == 0x04)
	{
	ADCSampPare.BUS_Curr = ADCSampPare.PhaseW_Curr + ADCSampPare.PhaseV_Curr;
	}
}
#endif
