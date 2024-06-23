#include "common.h"

/*
通用定时器输出PWM的原理：
1、设置定时器的时钟预分频器(PWM的高低值)和重装载值(PWM总周期值)
2、通过设置捕获/比较寄存器TIMx_CCRx来设PWM的高低时间值(定时器的重装载值为最大值)
3、当计数器的值小于输出比较寄存器 CCRx 的值时，PWM 通道输出低电平，点亮 LED 灯。
4、如果定时器设置向上计数模式，则开始会输出PWM低电平，达到输出比较寄存器 CCRx 的值时，再输出PWM高电平。
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
	PWM_GPIO_REMAP_FUN();//TIM3_CH2部分重映射

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

	basicTimer_initStruct.TIM_Period = PWM_TIMER_PERIOD_COUNT;//重装载值
	basicTimer_initStruct.TIM_Prescaler = PWM_TIMER_PRESCALER;//预分频器：定时器时钟
	basicTimer_initStruct.TIM_ClockDivision = TIM_CKD_DIV1 ;//设置时钟分频系数：不分频(这里用不到)
	basicTimer_initStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInit(PWM_TIMER_NUM,&basicTimer_initStruct);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//配置为PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;//设置初始PWM脉冲宽度为0   
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//当定时器计数值小于CCR_Val时为低电平
	//使能通道和预装载
	TIM_OC2Init(PWM_TIMER_NUM, &TIM_OCInitStructure);							  
	TIM_OC2PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable);					  
	//使能通道和预装载
	TIM_OC3Init(PWM_TIMER_NUM, &TIM_OCInitStructure);										  
	TIM_OC3PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable); 					  
	//使能通道和预装载
	TIM_OC4Init(PWM_TIMER_NUM, &TIM_OCInitStructure);									  
	TIM_OC4PreloadConfig(PWM_TIMER_NUM, TIM_OCPreload_Enable);						  

	TIM_ARRPreloadConfig(PWM_TIMER_NUM, ENABLE);//使能COLOR_TIMx重载寄存器ARR
	
	/* COLOR_TIMx enable counter */
	TIM_Cmd(PWM_TIMER_NUM, ENABLE);//使能定时器 	  

}

void pwm_init(void)
{
	pwm_gpio_init();
	pwm_config_init();
	
}

void pwm_SetRGBcolor(uint8_t r,uint8_t g,uint8_t b)
{
	//根据颜色值修改定时器的比较寄存器值
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

