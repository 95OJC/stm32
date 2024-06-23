#include "common.h"

/*

��5V�̵�������2.5~6Vֱ����ˢ���(���ʣ�0.36W��������0.18A)��

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

#define	DC_RCC(r,s)			RCC_APB2PeriphClockCmd(r,s)
#define DC_CLOCK			RCC_APB2Periph_GPIOB
#define	DC_GPIO				GPIOB
#define	DC_PIN				GPIO_Pin_10

#define DC_TOGGLE(p,i)	{p->ODR ^= i;}//��ͬΪ0.��ͬΪ1

void DCmotor_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	DC_RCC(DC_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = DC_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DC_GPIO,&gpio_initStruct);

	DCmotor_off();
}

void DCmotor_on(void)
{
	
    GPIO_SetBits(DC_GPIO,DC_PIN);
}

void DCmotor_off(void)
{
    GPIO_ResetBits(DC_GPIO,DC_PIN);
}

BOOL DCmotor_toggle(void)
{
	BOOL ret;
	
	DC_TOGGLE(DC_GPIO,DC_PIN);
	ret = GPIO_ReadOutputDataBit(DC_GPIO,DC_PIN);

	return ret;
}





