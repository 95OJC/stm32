#ifndef __keyCtrl_h
#define __keyCtrl_h

//nvic
#define	KEY_NVIC_IRQN		EXTI15_10_IRQn//EXTI0_IRQn
#define	KEY_NVIC_PRIO		0x01

//gpio
#define KYE_CLOCK			RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOA
#define	KYE_GPIO			GPIOC//GPIOA
#define	KYE_PIN				GPIO_Pin_13//GPIO_Pin_0

//exti and afio
#define	KEY_AFIO_CLOCK		RCC_APB2Periph_AFIO
#define	KEY_AFIO_GPIO		GPIO_PortSourceGPIOC//GPIO_PortSourceGPIOA
#define	KEY_AFIO_PIN		GPIO_PinSource13//GPIO_PinSource0
#define	KEY_EXTI_LINE		EXTI_Line13//EXTI_Line0

#endif//__keyCtrl_h

