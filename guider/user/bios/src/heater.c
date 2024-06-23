#include "common.h"

/*

��5V�̵������Ʒ���Ȧ(���ʣ�3.7W~100W����ѹ��3.7-24V��������1.1~2.98A)��

����Դ�������������ѹ���ܱ����������¶�·��(�ⲿ�����ѹ����̫��)
��������������������������ߣ������ջ�������ѹ�������軵�ˣ����������õ����ĵ����ߣ���Ϊ��ѹ���ⲿ���粻��ģ�

�̵���һ�ߣ�
1���̵�����D+��D-�ֱ�ӵ���Ƭ����5V��GND;
2��IN�ӵ�Ƭ����IO�ڿ��Ƹߵ͵�ƽ������;

�̵�����һ�ߣ�
1��NO��������
2��COM��������
3��NC�����ն�
4��ʹ�÷����ǵ�Ƭ����IN����ߵ͵�ƽ���������ƿ��͹�NO��COM��NC֮��Ĺ�ϵ���γɱպϻ�·�γɵ���
   �����ش��أ��̵��������б������Ӹ��ؼ���Դ��ͨ�������/ֱ����ѹ�Ͷ�Ӧ����������

*/

#define	HT_RCC(r,s)			RCC_APB2PeriphClockCmd(r,s)				
#define HT_CLOCK			RCC_APB2Periph_GPIOC
#define	HT_GPIO				GPIOC
#define	HT_PIN				GPIO_Pin_7

#define HT_TOGGLE(p,i)	{p->ODR ^= i;}//��ͬΪ0.��ͬΪ1

void heater_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	HT_RCC(HT_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = HT_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HT_GPIO,&gpio_initStruct);

	heater_off();
}

void heater_on(void)
{
	
    GPIO_SetBits(HT_GPIO,HT_PIN);
}

void heater_off(void)
{
    GPIO_ResetBits(HT_GPIO,HT_PIN);
}

BOOL heater_toggle(void)
{
	BOOL ret;
	
	HT_TOGGLE(HT_GPIO,HT_PIN);
	ret = GPIO_ReadOutputDataBit(HT_GPIO,HT_PIN);

	return ret;
}





