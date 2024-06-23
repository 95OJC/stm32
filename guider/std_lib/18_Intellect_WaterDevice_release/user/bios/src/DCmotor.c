#include "common.h"

/*

用5V继电器控制2.5~6V直流有刷电机(功率：0.36W，电流：0.18A)：

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

#define	DC_RCC(r,s)			RCC_APB2PeriphClockCmd(r,s)
#define DC_CLOCK			RCC_APB2Periph_GPIOB
#define	DC_GPIO				GPIOB
#define	DC_PIN				GPIO_Pin_10

#define DC_TOGGLE(p,i)	{p->ODR ^= i;}//相同为0.不同为1

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





