#include "common.h"

DEV_INFO gDevInfo;

void biosInit(void)
{
	if_init();
	usart_driver_register();
	
	//led_init();
	//key_init();
	//key_exti_init();
	//systick_init();//每秒翻转1次LED。
	//basic_timer_init();//每秒翻转1次LED。

	keyTTP224_init();//电容按键IO_IN
	WaterSensor_init();//水位传感器IO_IN
	DCmotor_init();//直流电机水泵IO_OUT（OFF状态）
	heater_init();//发热器IO_OUT（OFF状态）
	DS18B20_init();//温度传感器IO单总线（被上拉为高电平）
	OLED_Init();//显示屏IIC总线（清屏）
	
}

void bios_dev_init(void)
{
	MEM_SET(&gDevInfo, 0, sizeof(gDevInfo));
	
	SET_DEV_MODE(NORMAL_MODE);//并且清除加热器状态
	SET_CUR_WATER_T(0);//初始化当前水温为0
	SET_PRE_WATER_T(0);//初始化预设水温为0	
	
	CLS_ERR_WATER();//清除水位状态
	CLS_PUMP_STATE();//清除水泵状态

}


