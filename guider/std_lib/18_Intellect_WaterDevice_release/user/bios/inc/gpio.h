#ifndef __gpio_h
#define	__gpio_h

//gpio
#define LED_CLOCK			RCC_APB2Periph_GPIOB
#define	LED_GPIO			GPIOB
#define	LED_PIN				GPIO_Pin_0//GPIO_Pin_5

#define LED_TOGGLE(p,i)	{p->ODR ^= i;}//相同为0.不同为1

#endif//__gpio_h




