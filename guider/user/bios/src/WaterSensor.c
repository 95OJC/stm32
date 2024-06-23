#include "common.h"

/*
ˮλ�������� 
IO�ڽ��գ���ˮ�ߵ�ƽ��ûˮ�͵�ƽ?
*/


/*--------------------------------------------------------------------------------------------------------------------------------------------------------
ˮλ�������� 
1��DMA��ʽ��ȡADCֵ
2����ȡ�ߵ͵�ƽ

ת����ʽ��
AD_value(mA) = 3300000/4096*�ɼ���ADֵ
ˮλ���(mm) =(exp(0.0056*AD_value))*0.467

�½�init������Ȼ��������adc�Ľṹ�������ad_init���ٸ�bios����ִ�С�

����½�read��������������á�
//�½�read������Ȼ���붨�崫�����꣬���ú����¶��壬ֻ��sensor.c�ļ����á�
//ר����һ��sensor.c�ļ���GetSensorPort(BOOL��������)->BOOLֵ��GetSensAd(AD��������)->adֵ �� ��̬ȫ�ֱ���(��BOOL��bios��������SetSensLevel(�ߵͷ�ֵ)����ǰadֵ)

�������½�һ���ṹ�����飬�������ͺ�AD�͵�AD\DA�Ĳ�����Ϊһ�������±꣬Ȼ����ARRAY_SIZE(�ṹ��������)ѭ��ִ�г�ʼ���͸�ֵ��ص�AD\DAֵ��
//����get��set�������ⲿ���ã��ⲿ�Ϳ��Ե�����set����getĳ����������AD\DAֵ�ˡ�
--------------------------------------------------------------------------------------------------------------------------------------------------------*/

#define WATERSENSOR_RCC(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define WATERSENSOR_CLOCK			RCC_APB2Periph_GPIOB
#define	WATERSENSOR_GPIO			GPIOB
#define	WATERSENSOR_PIN				GPIO_Pin_8


void WaterSensor_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	WATERSENSOR_RCC(WATERSENSOR_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = WATERSENSOR_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WATERSENSOR_GPIO,&gpio_initStruct);

}

BOOL WaterSensor_monitor(void)
{
	BOOL ret = FALSE;
	
	if(GPIO_ReadInputDataBit(WATERSENSOR_GPIO,WATERSENSOR_PIN))
	{	
		ret = TRUE;
	}

	return ret;
}


