#include "common.h"

/*
水位传感器： 
IO口接收，有水高电平，没水低电平?
*/


/*--------------------------------------------------------------------------------------------------------------------------------------------------------
水位传感器： 
1、DMA方式读取ADC值
2、读取高低电平

转换公式：
AD_value(mA) = 3300000/4096*采集的AD值
水位深度(mm) =(exp(0.0056*AD_value))*0.467

新建init函数，然后传入配置adc的结构体参数给ad_init，再给bios调用执行。

针打：新建read函数，给外面调用。
//新建read函数，然后传入定义传感器宏，再用宏重新定义，只给sensor.c文件调用。
//专门有一个sensor.c文件的GetSensorPort(BOOL传感器宏)->BOOL值、GetSensAd(AD传感器宏)->ad值 和 静态全局变量(有BOOL、bios函数设置SetSensLevel(高低阀值)、当前ad值)

热敏：新建一个结构体数组，带开关型和AD型的AD\DA的参数作为一个数组下标，然后用ARRAY_SIZE(结构体数组名)循环执行初始化和赋值相关的AD\DA值。
//利用get和set函数给外部调用，外部就可以单独的set或者get某个传感器的AD\DA值了。
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


