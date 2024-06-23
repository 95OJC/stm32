#ifndef	__timer_h
#define	__timer_h

//nvic
#define	TIMER_NVIC_IRQN			TIM7_IRQn//TIM6_IRQn
#define	TIMER_NVIC_PRIO			0x01

//timer num
#define TIMER_CLOCK				RCC_APB1Periph_TIM7//RCC_APB1Periph_TIM6
#define	TIMER_NUM				TIM7//TIM6

//timer�ж�ʱ������
#define	TIMER_PERIOD_COUNT		(72 - 1)//����
#define	TIMER_PRESCALER			(1000 - 1)//��Ƶ

//timer�жϴ�����������
#define	TIMER_INT_CNT			1000




#endif//__timer_h

