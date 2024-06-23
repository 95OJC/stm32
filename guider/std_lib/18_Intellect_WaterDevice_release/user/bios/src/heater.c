#include "common.h"

/*

用5V继电器控制发热圈(功率：3.7W~100W，电压：3.7-24V，电流：1.1~2.98A)：

若电源给传感器过大电压可能被击穿，导致短路。(外部供电电压不能太大？)
若传感器过大电流可能热量过高，导致烧坏。（电压过大或电阻坏了？导致流过用电器的电流高，因为电压是外部供电不变的）

继电器一边：
1、继电器的D+和D-分别接到单片机的5V和GND;
2、IN接单片机的IO口控制高低电平的输入;

继电器另一边：
1、NO：常开端
2、COM：公共端
3、NC：常闭端
4、使用方法是单片机给IN输入高低电平，进而控制开和关NO、COM、NC之间的关系，形成闭合回路形成电流
   来开关大负载，继电器上面有表明连接负载及电源的通过最大交流/直流电压和对应的最大电流。

*/

#define	HT_RCC(r,s)			RCC_APB2PeriphClockCmd(r,s)				
#define HT_CLOCK			RCC_APB2Periph_GPIOC
#define	HT_GPIO				GPIOC
#define	HT_PIN				GPIO_Pin_7

#define HT_TOGGLE(p,i)	{p->ODR ^= i;}//相同为0.不同为1

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





