#include "common.h"
#include "keyCtrl.h"

/*-----------------------------------------------------------------
验证按键功能(STM32F103自带硬件消抖)和外部中断功能。

key外部中断步骤：
1、初始化产生中断的GPIO口
2、配置NVIC
3、打开GPIO时钟和AFIO时钟和配置AFIO
3、初始化EXTI
4、编写中断服务函数
------------------------------------------------------------------*/

void key_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	RCC_APB2PeriphClockCmd(KYE_CLOCK,ENABLE);

    gpio_initStruct.GPIO_Pin = KYE_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KYE_GPIO,&gpio_initStruct);
}

void key_exti_init(void)
{
	EXTI_InitTypeDef exti_initStruct;
	MEM_SET(&exti_initStruct,0,sizeof(EXTI_InitTypeDef));
#if 0
	NVIC_InitTypeDef nvic_initStruct;
	MEM_SET(&nvic_initStruct,0,sizeof(NVIC_InitTypeDef));
	
	nvic_initStruct.NVIC_IRQChannel = KEY_NVIC_IRQN;//按键1为PA0
	nvic_initStruct.NVIC_IRQChannelPreemptionPriority = KEY_NVIC_PRIO;//抢占优先级
	nvic_initStruct.NVIC_IRQChannelSubPriority = 0;//子优先级
	nvic_initStruct.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic_initStruct);
#else
	NVIC_ClearPendingIRQ(KEY_NVIC_IRQN);
	NVIC_SetPriority(KEY_NVIC_IRQN,KEY_NVIC_PRIO);//因为分组4没子优先级，若有则取bit几的抢占和子优先级算
	NVIC_EnableIRQ(KEY_NVIC_IRQN);
#endif

	RCC_APB2PeriphClockCmd(KEY_AFIO_CLOCK,ENABLE);
	GPIO_EXTILineConfig(KEY_AFIO_GPIO,KEY_AFIO_PIN);
	
	exti_initStruct.EXTI_Line = KEY_EXTI_LINE;
	exti_initStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_initStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_initStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_initStruct);

}

//没有中断的按键功能
BOOL key_scan(void)
{
	if(GPIO_ReadInputDataBit(KYE_GPIO,KYE_PIN))
	{
		while(GPIO_ReadInputDataBit(KYE_GPIO,KYE_PIN));//直到松开按键,返回TRUE
		led_toggle();
		return TRUE;
	}
	else
	{
		return FALSE
	}
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
	{
		led_toggle();
		EXTI_ClearITPendingBit(KEY_EXTI_LINE);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
	{
		led_toggle();
		EXTI_ClearITPendingBit(KEY_EXTI_LINE);
	}

}

