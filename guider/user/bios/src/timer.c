#include "common.h"
#include "timer.h"

/*-----------------------------------------------------------------
验证timer功能。

基本定时器步骤：
1、配置NVIC的该定时器优先级。
2、打开定时器时钟。
3、初始化基本定时器结构体：仅配置timer_clk分频系数和自动重装载值。
4、使能定时器。
5、编写中断服务函数。
------------------------------------------------------------------*/

void basic_timer_init(void)
{
	NVIC_ClearPendingIRQ(TIMER_NVIC_IRQN);
	NVIC_SetPriority(TIMER_NVIC_IRQN,TIMER_NVIC_PRIO);
	NVIC_EnableIRQ(TIMER_NVIC_IRQN);

	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	RCC_APB1PeriphClockCmd(TIMER_CLOCK, ENABLE);

	basicTimer_initStruct.TIM_Period = TIMER_PERIOD_COUNT;//重装载值
	basicTimer_initStruct.TIM_Prescaler = TIMER_PRESCALER;//预分频器：定时器时钟
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);

	//清楚计数器中断标志位
    TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);

	//开启计数器中断
	TIM_ITConfig(TIMER_NUM,TIM_IT_Update,ENABLE);

	//使能计数器
	TIM_Cmd(TIMER_NUM,ENABLE);	
	
}

void delay_timer_ms(u32 ms)
{
	if(ms > 500)
	{
		ms = 500;
	}
	
	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	RCC_APB1PeriphClockCmd(TIMER_CLOCK, ENABLE);
	
	basicTimer_initStruct.TIM_Period = ((72-1)*ms);//重装载值
	basicTimer_initStruct.TIM_Prescaler = (1000-1);//预分频器：定时器时钟
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//清timer标志位
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//使能计数器
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//等待标志位
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//关闭定时器
	TIM_Cmd(TIMER_NUM,DISABLE);
}

void delay_timer_us(u32 us)
{
	if(us > 500)
	{
		us = 500;
	}
	
	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	RCC_APB1PeriphClockCmd(TIMER_CLOCK, ENABLE);
	
	basicTimer_initStruct.TIM_Period = ((72-1)*us);//重装载值
	basicTimer_initStruct.TIM_Prescaler = (1-1);//预分频器：定时器时钟
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//清timer标志位
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//使能计数器
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//等待标志位
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//关闭定时器
	TIM_Cmd(TIMER_NUM,DISABLE);
}

void TIM6_IRQHandler(void)
{
	static uint16_t cnt = 0;
	if (TIM_GetITStatus(TIMER_NUM,TIM_IT_Update) != RESET)
	{
		cnt++;
		if(cnt == TIMER_INT_CNT)
		{
			led_toggle();
			cnt = 0;
		}
		TIM_ClearITPendingBit(TIMER_NUM,TIM_FLAG_Update);  
	}
		
}

void TIM7_IRQHandler(void)
{
	static uint16_t cnt = 0;
	if (TIM_GetITStatus(TIMER_NUM,TIM_IT_Update) != RESET)
	{
		cnt++;
		if(cnt == TIMER_INT_CNT)
		{
			led_toggle();
			cnt = 0;
		}
		TIM_ClearITPendingBit(TIMER_NUM,TIM_FLAG_Update);  
	}
		
}

