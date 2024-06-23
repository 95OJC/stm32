#include "common.h"

/*
四键电容式触摸IC-TTP224的主要配置功能引脚电平说明如下：

LPMB引脚：
1：快速模式（原理图默认）
0：低功耗模式

MOT0引脚：
0：最长维持触摸16秒（原理图默认）
高阻态：不限触摸时间

SM引脚：
高阻态：允许2个以上的多键触摸
0：单键模式（原理图默认）

TOG OD AHLB引脚：
 1  0   1（原理图默认）
触发模式，上电状态为高阻态，低电平有效（错的，按下后为高电平）

*/

#define KYE_TTP224_RCC1(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define KYE_TTP224_CLOCK1			RCC_APB2Periph_GPIOA
#define	KYE_TTP224_GPIO1			GPIOA
#define	KYE_TTP224_PIN1			GPIO_Pin_2

#define KYE_TTP224_RCC2(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define KYE_TTP224_CLOCK2			RCC_APB2Periph_GPIOA
#define	KYE_TTP224_GPIO2			GPIOA
#define	KYE_TTP224_PIN2			GPIO_Pin_3

#define KYE_TTP224_RCC3(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define KYE_TTP224_CLOCK3			RCC_APB2Periph_GPIOA
#define	KYE_TTP224_GPIO3			GPIOA
#define	KYE_TTP224_PIN3			GPIO_Pin_11

#define KYE_TTP224_RCC4(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define KYE_TTP224_CLOCK4			RCC_APB2Periph_GPIOA
#define	KYE_TTP224_GPIO4			GPIOA
#define	KYE_TTP224_PIN4			GPIO_Pin_12


void keyTTP224_init(void)
{
	GPIO_InitTypeDef structInit;
	
	KYE_TTP224_RCC1(KYE_TTP224_CLOCK1,ENABLE);
	structInit.GPIO_Pin = KYE_TTP224_PIN1;
	structInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	structInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KYE_TTP224_GPIO1,&structInit);

	KYE_TTP224_RCC1(KYE_TTP224_CLOCK2,ENABLE);
	structInit.GPIO_Pin = KYE_TTP224_PIN2;
	GPIO_Init(KYE_TTP224_GPIO2,&structInit);

	KYE_TTP224_RCC1(KYE_TTP224_CLOCK3,ENABLE);
	structInit.GPIO_Pin = KYE_TTP224_PIN3;
	GPIO_Init(KYE_TTP224_GPIO3,&structInit);

	KYE_TTP224_RCC1(KYE_TTP224_CLOCK4,ENABLE);
	structInit.GPIO_Pin = KYE_TTP224_PIN4;
	GPIO_Init(KYE_TTP224_GPIO4,&structInit);
}


u8 keyTTP224_scan(void)
{
	u8 ret = KEY_NULL;
	
	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO1,KYE_TTP224_PIN1))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO1,KYE_TTP224_PIN1));
		ret = KEY_PUMP;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO2,KYE_TTP224_PIN2))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO2,KYE_TTP224_PIN2));
		ret = KEY_HEAT;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO3,KYE_TTP224_PIN3))
	{
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO3,KYE_TTP224_PIN3));
		ret = KEY_TEMP_UP;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO4,KYE_TTP224_PIN4))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO4,KYE_TTP224_PIN4));
		ret = KEY_TEMP_DOWN;
	}

	return ret;
}

u8 shell_keyTTP224_test(void)
{
	u8 ret = 0;
	
	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO1,KYE_TTP224_PIN1))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO1,KYE_TTP224_PIN1));
		ret = 1;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO2,KYE_TTP224_PIN2))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO2,KYE_TTP224_PIN2));
		ret = 2;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO3,KYE_TTP224_PIN3))
	{
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO3,KYE_TTP224_PIN3));
		ret = 3;
	}

	if(GPIO_ReadInputDataBit(KYE_TTP224_GPIO4,KYE_TTP224_PIN4))
	{	
		while(GPIO_ReadInputDataBit(KYE_TTP224_GPIO4,KYE_TTP224_PIN4));
		ret = 4;
	}


	return ret;

}


