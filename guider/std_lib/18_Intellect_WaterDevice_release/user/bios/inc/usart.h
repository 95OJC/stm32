#ifndef __uart_h
#define	__uart_h

#if 1
//nvic
#define	USART_NVIC_IRQN				USART1_IRQn
#define	USART_NVIC_PRIO				0x01

//usart
#define	USART_NUM					USART1
#define	USART_APBx_CLOCK(x,s)		RCC_APB2PeriphClockCmd(x,s)
#define	USART_CLOCK					RCC_APB2Periph_USART1

#define	USART_TX_APBx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_TX_CLOCK_GPIOx		RCC_APB2Periph_GPIOA
#define USART_TX_GPIO				GPIOA
#define USART_TX_PIN				GPIO_Pin_9

#define	USART_RX_APBx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RX_CLOCK_GPIOx		RCC_APB2Periph_GPIOA
#define USART_RX_GPIO				GPIOA
#define USART_RX_PIN				GPIO_Pin_10

#else
//nvic
#define	USART_NVIC_IRQN				USART2_IRQn
#define	USART_NVIC_PRIO				0x01

//usart
#define	USART_NUM					USART2
#define	USART_APBx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define	USART_CLOCK					RCC_APB1Periph_USART2

#define	USART_TX_APBx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_TX_CLOCK_GPIOx		RCC_APB2Periph_GPIOA
#define USART_TX_GPIO				GPIOA
#define USART_TX_PIN				GPIO_Pin_2

#define	USART_RX_APBx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RX_CLOCK_GPIOx		RCC_APB2Periph_GPIOA
#define USART_RX_GPIO				GPIOA
#define USART_RX_PIN				GPIO_Pin_3

#endif


//Á÷¿ØÄ£Äâio
#define	USART_RTS_APBx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RTS_CLOCK_GPIOx		RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOB
#define USART_RTS_GPIO				GPIOC//GPIOB
#define USART_RTS_PIN				GPIO_Pin_7//GPIO_Pin_6


#endif//__uart_h
