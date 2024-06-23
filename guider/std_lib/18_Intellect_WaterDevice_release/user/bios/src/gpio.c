#include "common.h"
#include "gpio.h"

/*-----------------------------------------------------------------
验证gpio功能。
------------------------------------------------------------------*/
void led_off(void);
void led_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	RCC_APB2PeriphClockCmd(LED_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = LED_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO,&gpio_initStruct);

	led_off();
}

void led_on(void)
{
	
    GPIO_ResetBits(LED_GPIO,LED_PIN);
}

void led_off(void)
{
    GPIO_SetBits(LED_GPIO,LED_PIN);
}

void led_toggle(void)
{
	LED_TOGGLE(LED_GPIO,LED_PIN);
}

