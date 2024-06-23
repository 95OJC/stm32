#include "common.h"

/*
	用ADC输入3.3V供电的可调电阻的电压值。
	
	DAC通过定时器和DMA方式发出正弦波的电压值。
	过程：DMA自动把缓存数据给到DAC_DHRx寄存器(DHRx已经转移数据到DORx)，由于
		  DAC配置了TIM2当触发源，定时器计算至指定的值时产生事件，触发DAC
		  把DHRx寄存器的数据转移到DORx，从而开始进行转换，这样DMA才能一直循环
		  发送缓冲数据给到DAC_DHRx寄存器。
	
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

	ADC_structInit.ADC_Mode = ADC_Mode_Independent;//只使用一个ADC，属于独立模式
	ADC_structInit.ADC_ScanConvMode = DISABLE;//禁止扫描模式，多通道才要，单通道不需要
	ADC_structInit.ADC_ContinuousConvMode = ENABLE;//连续转换模式
	ADC_structInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不用外部触发转换，软件开启即可
	ADC_structInit.ADC_DataAlign = ADC_DataAlign_Right;//转换结果右对齐
	ADC_structInit.ADC_NbrOfChannel = 1;//转换通道为1个
	ADC_Init(ADCx,&ADC_structInit);

	//配置ADC时钟为PCLK2的8分频，即9MHz(ADC时钟最大不能超过14M，实际最多配12M?)
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

	//配置ADC_CHANNEL的通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	
}

void ad_init(void)
{
	//优先级配置
	NVIC_ClearPendingIRQ(ADC_NVIC_IRQN);
	NVIC_SetPriority(ADC_NVIC_IRQN,ADC_NVIC_PRIO);
	NVIC_EnableIRQ(ADC_NVIC_IRQN);

	ad_io_init();
	ad_config_init();
	
	//ADC转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);

	//开启ADC，并开始转换
	ADC_Cmd(ADCx, ENABLE);

	//初始化ADC校准寄存器，等待校准寄存器初始化完成  
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx));

	//ADC开始校准，等待校准完成
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));

	//由于没有采用外部触发，所以使用软件触发ADC转换 
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

	/* 配置DAC 通道1 */
	DA_structInit.DAC_Trigger = DAC_Trigger_T2_TRGO;//使用TIM2(TRGO事件)作为触发源
	DA_structInit.DAC_WaveGeneration = DAC_WaveGeneration_None;//不使用波形发生器,即不输出伪噪声或三角波
	DA_structInit.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//不使用DAC输出缓冲
	DAC_Init(DAC_Channel_1,&DA_structInit);

	/* 配置DAC 通道2 */
  	DAC_Init(DAC_Channel_2, &DA_structInit);

	/* 使能通道1 由PA4输出 , 使能通道2 由PA5输出 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
}

//1ms产生1次TRGO事件
static void da_timer_config(void)
{
	TIM_TimeBaseInitTypeDef	TIM_structInit;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_structInit.TIM_Period = (72-1);//重装载值
	TIM_structInit.TIM_Prescaler = (1000-1);//预分频器：定时器时钟
	TIM_structInit.TIM_ClockDivision = 0;
	TIM_structInit.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInit(TIM2,&TIM_structInit);

	/* 配置TIM2触发源 */
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//产生TRGO事件

	/* 使能TIM2 */
	TIM_Cmd(TIM2, ENABLE);	
}

static void da_dma_config(void)
{
	u32 MaddrVal = 2048;
	DMA_InitTypeDef  DMA_structInit;
	
	/* 使能DMA2时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	  
	/* 配置DMA2 */
	DMA_structInit.DMA_PeripheralBaseAddr = (DAC_BASE+0x20);//DAC DHR12RD寄存器，12位、右对齐、双通道			  //外设数据地址
	DMA_structInit.DMA_MemoryBaseAddr = (u32)&MaddrVal;//内存数据地址
	DMA_structInit.DMA_DIR = DMA_DIR_PeripheralDST;//数据传输方向内存至外设
	DMA_structInit.DMA_BufferSize = 1;//缓存大小为1字节	  
	DMA_structInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设数据地址固定  
	DMA_structInit.DMA_MemoryInc = DMA_MemoryInc_Disable;//内存数据地址固定
	DMA_structInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//外设数据以字为单位
	DMA_structInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//内存数据以字为单位   
	DMA_structInit.DMA_Mode = DMA_Mode_Circular;//循环模式
	DMA_structInit.DMA_Priority = DMA_Priority_High;//高DMA通道优先级
	DMA_structInit.DMA_M2M = DMA_M2M_Disable;//非内存至内存模式  
	
	DMA_Init(DMA2_Channel4, &DMA_structInit);
	  
	/* 使能DMA2-14通道 */
	DMA_Cmd(DMA2_Channel4, ENABLE);


}

//PA4好像正常，但是PA5暂时不能输出DA值，需要调试
void da_init(void)
{
	da_io_init();
	da_config_init();

	/* 使能DAC2的DMA请求 */
	DAC_DMACmd(DAC_Channel_2, ENABLE);
	da_dma_config();
	
	da_timer_config();

}

void ADC1_2_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
		AD_Value = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}

