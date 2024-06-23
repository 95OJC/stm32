#include "common.h"

DEV_INFO gDevInfo;

void biosInit(void)
{
	if_init();
	usart_driver_register();
	
	//led_init();
	//key_init();
	//key_exti_init();
	//systick_init();//ÿ�뷭ת1��LED��
	//basic_timer_init();//ÿ�뷭ת1��LED��

	keyTTP224_init();//���ݰ���IO_IN
	WaterSensor_init();//ˮλ������IO_IN
	DCmotor_init();//ֱ�����ˮ��IO_OUT��OFF״̬��
	heater_init();//������IO_OUT��OFF״̬��
	DS18B20_init();//�¶ȴ�����IO�����ߣ�������Ϊ�ߵ�ƽ��
	OLED_Init();//��ʾ��IIC���ߣ�������
	
}

void bios_dev_init(void)
{
	MEM_SET(&gDevInfo, 0, sizeof(gDevInfo));
	
	SET_DEV_MODE(NORMAL_MODE);//�������������״̬
	SET_CUR_WATER_T(0);//��ʼ����ǰˮ��Ϊ0
	SET_PRE_WATER_T(0);//��ʼ��Ԥ��ˮ��Ϊ0	
	
	CLS_ERR_WATER();//���ˮλ״̬
	CLS_PUMP_STATE();//���ˮ��״̬

}


