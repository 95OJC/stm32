#include "common.h"

/*
ͨ�ö�ʱ�����PWM��ԭ��
1�����ö�ʱ����ʱ��Ԥ��Ƶ��(PWM�ĸߵ�ֵ)����װ��ֵ(PWM������ֵ)
2��ͨ�����ò���/�ȽϼĴ���TIMx_CCRx����PWM�ĸߵ�ʱ��ֵ(��ʱ������װ��ֵΪ���ֵ)
3������������ֵС������ȽϼĴ��� CCRx ��ֵʱ��PWM ͨ������͵�ƽ������ LED �ơ�
4�������ʱ���������ϼ���ģʽ����ʼ�����PWM�͵�ƽ���ﵽ����ȽϼĴ��� CCRx ��ֵʱ�������PWM�ߵ�ƽ��
*/

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

static void pwm_gpio_init(void)
{
	GPIO_InitTypeDef gpio_initStruct;
	
	PWM_GPIO_BUSx_CLOCK(PWM_GPIO_CLOCK,ENABLE);
	PWM_GPIO_REMAP_FUN();//TIM3_CH2������ӳ��

	gpio_initStruct.GPIO_Pin = PWM_GPIO_RED_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PWM_GPIO_RED_PORT,&gpio_initStruct);

	gpio_initStruct.GPIO_Pin = PWM_GPIO_GREEN_PIN;
	GPIO_Init(PWM_GPIO_GREEN_PORT,&gpio_initStruct);

	gpio_initStruct.GPIO_Pin = PWM_GPIO_BLUE_PIN;
	GPIO_Init(PWM_GPIO_BULE_PORT,&gpio_initStruct);	
}

static void pwm_config_init(void)
{
	TIM_TimeBaseInitTypeDef  basicTimer_initStruct;
	TIM_OCInitTypeDef  TIM_OCInitStructure;	
	
	PWM_TIMER_BUSx_CLOCK(PWM_TIMER_CLOCK,ENABLE);

	basicTimer_initStruct.TIM_Period = PWM_TIMER_PERIOD_COUNT;//��װ��ֵ
	basicTimer_initStruct.TIM_Prescaler = PWM_TIMER_PRESCALER;//Ԥ��Ƶ������ʱ��ʱ��
	basicTimer_initStruct.TIM_ClockDivision = TIM_CKD_DIV1 ;//����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���)
	basicTimer_initStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInit(PWM_TIMER_NUM,&basicTimer_initStruct);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ʹ�����
	TIM_OCInitStructure.TIM_Pulse = 0;//���ó�ʼPWM������Ϊ0   
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//����ʱ������ֵС��CCR_ValʱΪ�͵�ƽ
	//ʹ��ͨ����Ԥװ��
	TIM_OC2Init(PWM_TIMER_NUM, &TIM_OCInitStructure);							  
	TIM_OC2PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable);					  
	//ʹ��ͨ����Ԥװ��
	TIM_OC3Init(PWM_TIMER_NUM, &TIM_OCInitStructure);										  
	TIM_OC3PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable); 					  
	//ʹ��ͨ����Ԥװ��
	TIM_OC4Init(PWM_TIMER_NUM, &TIM_OCInitStructure);									  
	TIM_OC4PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable);						  

	TIM_ARRPreloadConfig(PWM_TIMER_NUM, ENABLE);//ʹ��COLOR_TIMx���ؼĴ���ARR
	
	/* COLOR_TIMx enable counter */
	TIM_Cmd(PWM_TIMER_NUM, ENABLE);//ʹ�ܶ�ʱ�� 	  

}

void pwm_init(void)
{
	pwm_gpio_init();
	pwm_config_init();
	
}

void pwm_SetRGBcolor(uint8_t r,uint8_t g,uint8_t b)
{
	//������ɫֵ�޸Ķ�ʱ���ıȽϼĴ���ֵ
	PWM_TIMER_NUM->CCR2 = r;	//R
	PWM_TIMER_NUM->CCR3 = g;	//G     
	PWM_TIMER_NUM->CCR4 = b;	//B
}

void pwm_test(void)
{
	pwm_init();

	while(1)
	{
		//pwm_SetRGBcolor(181,230,29);
		//delay_timer_ms(500);
		//pwm_SetRGBcolor(255,128,64);
		//delay_timer_ms(500);
		pwm_SetRGBcolor(22,0,0);
	}
	
}

