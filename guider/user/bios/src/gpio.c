#include "common.h"

/*-----------------------------------------------------------------
验证gpio功能。

验证按键功能(STM32F103自带硬件消抖)和外部中断功能。
key外部中断步骤：
1、初始化产生中断的GPIO口
2、配置NVIC
3、打开GPIO时钟和AFIO时钟和配置AFIO
3、初始化EXTI
4、编写中断服务函数
------------------------------------------------------------------*/
void led_on(void)
{
	
    GPIO_ResetBits(LED_GPIO,LED_PIN);
}

void led_off(void)
{
    GPIO_SetBits(LED_GPIO,LED_PIN);
}

void led_toggle(void)
{
	LED_GPIO->ODR ^= LED_PIN;
}

void led_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	LED_BUSx_CLOCK(LED_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = LED_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO,&gpio_initStruct);

	led_off();
}

void key_exti_init(void)
{
	EXTI_InitTypeDef exti_initStruct;
	MEM_SET(&exti_initStruct,0,sizeof(EXTI_InitTypeDef));

	NVIC_ClearPendingIRQ(KEY_NVIC_IRQN);
	NVIC_SetPriority(KEY_NVIC_IRQN,KEY_NVIC_PRIO);//因为分组4没子优先级，若有则取bit几的抢占和子优先级算
	NVIC_EnableIRQ(KEY_NVIC_IRQN);

	KEY_AFIO_BUSx_CLOCK(KEY_AFIO_CLOCK,ENABLE);
	GPIO_EXTILineConfig(KEY_AFIO_GPIO,KEY_AFIO_PIN);
	
	exti_initStruct.EXTI_Line = KEY_EXTI_LINE;
	exti_initStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_initStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_initStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_initStruct);

}

void key_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	KEY_BUSx_CLOCK(KEY_CLOCK,ENABLE);

    gpio_initStruct.GPIO_Pin = KEY_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO,&gpio_initStruct);
}

void KEY_IRQHandler_FUNC(void)
{
	if(EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
	{
		led_toggle();
		EXTI_ClearITPendingBit(KEY_EXTI_LINE);
	}

}

void led_test(void)
{
	led_init();
	led_on();
}

void key_test(void)
{
	key_init();
	key_exti_init();
}


