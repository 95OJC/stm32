#include "common.h"
#include "timer.h"

/*-----------------------------------------------------------------
��֤timer���ܡ�

������ʱ�����裺
1������NVIC�ĸö�ʱ�����ȼ���
2���򿪶�ʱ��ʱ�ӡ�
3����ʼ��������ʱ���ṹ�壺������timer_clk��Ƶϵ�����Զ���װ��ֵ��
4��ʹ�ܶ�ʱ����
5����д�жϷ�������
------------------------------------------------------------------*/

void basic_timer_init(void)
{
	NVIC_ClearPendingIRQ(TIMER_NVIC_IRQN);
	NVIC_SetPriority(TIMER_NVIC_IRQN,TIMER_NVIC_PRIO);
	NVIC_EnableIRQ(TIMER_NVIC_IRQN);

	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	RCC_APB1PeriphClockCmd(TIMER_CLOCK, ENABLE);

	basicTimer_initStruct.TIM_Period = TIMER_PERIOD_COUNT;//��װ��ֵ
	basicTimer_initStruct.TIM_Prescaler = TIMER_PRESCALER;//Ԥ��Ƶ������ʱ��ʱ��
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);

	//����������жϱ�־λ
    TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);

	//�����������ж�
	TIM_ITConfig(TIMER_NUM,TIM_IT_Update,ENABLE);

	//ʹ�ܼ�����
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
	
	basicTimer_initStruct.TIM_Period = ((72-1)*ms);//��װ��ֵ
	basicTimer_initStruct.TIM_Prescaler = (1000-1);//Ԥ��Ƶ������ʱ��ʱ��
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//��timer��־λ
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//ʹ�ܼ�����
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//�ȴ���־λ
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//�رն�ʱ��
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
	
	basicTimer_initStruct.TIM_Period = ((72-1)*us);//��װ��ֵ
	basicTimer_initStruct.TIM_Prescaler = (1-1);//Ԥ��Ƶ������ʱ��ʱ��
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//��timer��־λ
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//ʹ�ܼ�����
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//�ȴ���־λ
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//�رն�ʱ��
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

