#include "common.h"

/*
�ļ�����ʽ����IC-TTP224����Ҫ���ù������ŵ�ƽ˵�����£�

LPMB���ţ�
1������ģʽ��ԭ��ͼĬ�ϣ�
0���͹���ģʽ

MOT0���ţ�
0���ά�ִ���16�루ԭ��ͼĬ�ϣ�
����̬�����޴���ʱ��

SM���ţ�
����̬������2�����ϵĶ������
0������ģʽ��ԭ��ͼĬ�ϣ�

TOG OD AHLB���ţ�
 1  0   1��ԭ��ͼĬ�ϣ�
����ģʽ���ϵ�״̬Ϊ����̬���͵�ƽ��Ч����ģ����º�Ϊ�ߵ�ƽ��

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


