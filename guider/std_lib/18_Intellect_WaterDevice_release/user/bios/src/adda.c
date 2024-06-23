#include "common.h"

/*
	��ADC����3.3V����Ŀɵ�����ĵ�ѹֵ��
	
	DACͨ����ʱ����DMA��ʽ�������Ҳ��ĵ�ѹֵ��
	���̣�DMA�Զ��ѻ������ݸ���DAC_DHRx�Ĵ���(DHRx�Ѿ�ת�����ݵ�DORx)������
		  DAC������TIM2������Դ����ʱ��������ָ����ֵʱ�����¼�������DAC
		  ��DHRx�Ĵ���������ת�Ƶ�DORx���Ӷ���ʼ����ת��������DMA����һֱѭ��
		  ���ͻ������ݸ���DAC_DHRx�Ĵ�����
	
*/

__IO u16 AD_Value;

static void ad_io_init(void)
{
	GPIO_InitTypeDef IO_structInit;

	ADC_GPIO_CLOCK_FUN(ADC_GPIO_CLOCK,ENABLE);

	IO_structInit.GPIO_Pin = ADC_PIN;
	IO_structInit.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(ADC_PORT,&IO_structInit);
	
}

static void ad_config_init(void)
{
	ADC_InitTypeDef ADC_structInit;

	ADC_CLOCK_FUN(ADC_CLOCK,ENABLE);

	ADC_structInit.ADC_Mode = ADC_Mode_Independent;//ֻʹ��һ��ADC�����ڶ���ģʽ
	ADC_structInit.ADC_ScanConvMode = DISABLE;//��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_structInit.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽ
	ADC_structInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�����ⲿ����ת���������������
	ADC_structInit.ADC_DataAlign = ADC_DataAlign_Right;//ת������Ҷ���
	ADC_structInit.ADC_NbrOfChannel = 1;//ת��ͨ��Ϊ1��
	ADC_Init(ADCx,&ADC_structInit);

	//����ADCʱ��ΪPCLK2��8��Ƶ����9MHz(ADCʱ������ܳ���14M��ʵ�������12M?)
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	//����ADC_CHANNEL��ͨ��ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
}

void ad_init(void)
{
	//���ȼ�����
	NVIC_ClearPendingIRQ(ADC_NVIC_IRQN);
	NVIC_SetPriority(ADC_NVIC_IRQN,ADC_NVIC_PRIO);
	NVIC_EnableIRQ(ADC_NVIC_IRQN);

	ad_io_init();
	ad_config_init();
	
	//ADCת�����������жϣ����жϷ�������ж�ȡת��ֵ
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);

	//����ADC������ʼת��
	ADC_Cmd(ADCx, ENABLE);

	//��ʼ��ADCУ׼�Ĵ������ȴ�У׼�Ĵ�����ʼ�����  
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx));

	//ADC��ʼУ׼���ȴ�У׼���
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));

	//����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);	
}

static void da_io_init(void)
{
	GPIO_InitTypeDef IO_structInit;
	
	DAC_GPIO_CLOCK_FUN(DAC_GPIO_CLOCK, ENABLE);	
	
	IO_structInit.GPIO_Pin =  DAC_PIN;
	IO_structInit.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &IO_structInit);
}

static void da_config_init(void)
{
	DAC_InitTypeDef	DA_structInit;
	 
	DAC_CLOCK_FUN(DAC_CLOCK, ENABLE);

	/* ����DAC ͨ��1 */
	DA_structInit.DAC_Trigger = DAC_Trigger_T2_TRGO;//ʹ��TIM2(TRGO�¼�)��Ϊ����Դ
	DA_structInit.DAC_WaveGeneration = DAC_WaveGeneration_None;//��ʹ�ò��η�����,�������α���������ǲ�
	DA_structInit.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//��ʹ��DAC�������
	DAC_Init(DAC_Channel_1,&DA_structInit);

	/* ����DAC ͨ��2 */
  	DAC_Init(DAC_Channel_2, &DA_structInit);

	/* ʹ��ͨ��1 ��PA4��� , ʹ��ͨ��2 ��PA5��� */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
}

//1ms����1��TRGO�¼�
static void da_timer_config(void)
{
	TIM_TimeBaseInitTypeDef	TIM_structInit;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_structInit.TIM_Period = (72-1);//��װ��ֵ
	TIM_structInit.TIM_Prescaler = (1000-1);//Ԥ��Ƶ������ʱ��ʱ��
	TIM_structInit.TIM_ClockDivision = 0;
	TIM_structInit.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2,&TIM_structInit);

	/* ����TIM2����Դ */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//����TRGO�¼�

	/* ʹ��TIM2 */
	TIM_Cmd(TIM2, ENABLE);	
}

static void da_dma_config(void)
{
	u32 MaddrVal = 2048;
	DMA_InitTypeDef  DMA_structInit;
	
	/* ʹ��DMA2ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	  
	/* ����DMA2 */
	DMA_structInit.DMA_PeripheralBaseAddr = (DAC_BASE+0x20);//DAC DHR12RD�Ĵ�����12λ���Ҷ��롢˫ͨ��			  //�������ݵ�ַ
	DMA_structInit.DMA_MemoryBaseAddr = (u32)&MaddrVal;//�ڴ����ݵ�ַ
	DMA_structInit.DMA_DIR = DMA_DIR_PeripheralDST;//���ݴ��䷽���ڴ�������
	DMA_structInit.DMA_BufferSize = 1;//�����СΪ1�ֽ�	  
	DMA_structInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�������ݵ�ַ�̶�  
	DMA_structInit.DMA_MemoryInc = DMA_MemoryInc_Disable;//�ڴ����ݵ�ַ�̶�
	DMA_structInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//������������Ϊ��λ
	DMA_structInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//�ڴ���������Ϊ��λ   
	DMA_structInit.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ
	DMA_structInit.DMA_Priority = DMA_Priority_High;//��DMAͨ�����ȼ�
	DMA_structInit.DMA_M2M = DMA_M2M_Disable;//���ڴ����ڴ�ģʽ  
	
	DMA_Init(DMA2_Channel4, &DMA_structInit);
	  
	/* ʹ��DMA2-14ͨ�� */
	DMA_Cmd(DMA2_Channel4, ENABLE);


}

//PA4��������������PA5��ʱ�������DAֵ����Ҫ����
void da_init(void)
{
	da_io_init();
	da_config_init();

	/* ʹ��DAC2��DMA���� */
	DAC_DMACmd(DAC_Channel_2, ENABLE);
	da_dma_config();
	
	da_timer_config();

}

void ADC1_2_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) 
	{
		// ��ȡADC��ת��ֵ
		AD_Value = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}

