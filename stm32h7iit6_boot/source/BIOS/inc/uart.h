#ifndef __UART_H
#define	__UART_H


#define UART_0_NUM 				USART1
#define	UART_0_CLK_EN()			__HAL_RCC_USART1_CLK_ENABLE()
#define	UART_0_GPIO_EN()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define	UART_0_IRQHandler		USART1_IRQHandler
#define UART_0_RX_TX_GPIO   	GPIOA
#define UART_0_RX_PIN    		GPIO_PIN_10
#define UART_0_TX_PIN    		GPIO_PIN_9
#define	UART_0_AF				GPIO_AF7_USART1










#endif//__UART_H



