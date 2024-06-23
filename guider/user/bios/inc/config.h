#ifndef __config_h
#define	__config_h

/**********************************gpio******************************************/
//gpio_out
#define	LED_BUSx_CLOCK(x,s)			RCC_APB2PeriphClockCmd(x,s)		
#define LED_CLOCK					RCC_APB2Periph_GPIOB
#define	LED_GPIO					GPIOB
#define	LED_PIN						GPIO_Pin_0//GPIO_Pin_5

//gpio_in
#define	KEY_BUSx_CLOCK(x,s)			RCC_APB2PeriphClockCmd(x,s)		
#define KEY_CLOCK					RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOA
#define	KEY_GPIO					GPIOC//GPIOA
#define	KEY_PIN						GPIO_Pin_13//GPIO_Pin_0
//exti and afio
#define	KEY_AFIO_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	KEY_AFIO_CLOCK				RCC_APB2Periph_AFIO
#define	KEY_AFIO_GPIO				GPIO_PortSourceGPIOC//GPIO_PortSourceGPIOA
#define	KEY_AFIO_PIN				GPIO_PinSource13//GPIO_PinSource0
#define	KEY_EXTI_LINE				EXTI_Line13//EXTI_Line0
//nvic
#define	KEY_NVIC_IRQN				EXTI15_10_IRQn//EXTI0_IRQn
#define	KEY_NVIC_PRIO				0x01
#define	KEY_IRQHandler_FUNC			EXTI15_10_IRQHandler//EXTI0_IRQHandler


/***********************************systick****************************************/
//进1次中断需要多久时间
#define SYSTICK_UNIT_1US			(SystemCoreClock/1000000)
#define SYSTICK_UNIT_1MS			(SystemCoreClock/1000)
#define SYSTICK_UNIT_10MS			(SystemCoreClock/100)
//进多少次中断
#define	SYSHANDLE_COUNT				1000

/***********************************timer****************************************/
//timer num
#define	TIMER_BUSx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define TIMER_CLOCK					RCC_APB1Periph_TIM7//RCC_APB1Periph_TIM6
#define	TIMER_NUM					TIM7//TIM6
//timer中断时间配置
#define	TIMER_PERIOD_COUNT			(72 - 1)//次数
#define	TIMER_PRESCALER				(1000 - 1)//分频
//timer中断次数触发条件
#define	TIMER_INT_CNT				1000
//nvic
#define	TIMER_NVIC_IRQN				TIM7_IRQn//TIM6_IRQn
#define	TIMER_NVIC_PRIO				0x01
#define	TIMER_IRQHandler_FUNC		TIM7_IRQHandler//TIM6_IRQHandler


/***********************************usart****************************************/
//流控模拟io
#define	USART_RTS_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RTS_CLOCK				RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOB
#define USART_RTS_GPIO				GPIOC//GPIOB
#define USART_RTS_PIN				GPIO_Pin_7//GPIO_Pin_6
#if 1
#define	USART_NUM					USART1
#define	USART_BUSx_CLOCK(x,s)		RCC_APB2PeriphClockCmd(x,s)
#define	USART_CLOCK					RCC_APB2Periph_USART1

#define	USART_TX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_TX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_TX_GPIO				GPIOA
#define USART_TX_PIN				GPIO_Pin_9

#define	USART_RX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_RX_GPIO				GPIOA
#define USART_RX_PIN				GPIO_Pin_10
//nvic
#define	USART_NVIC_IRQN				USART1_IRQn
#define	USART_NVIC_PRIO				0x01
#define	USART_IRQHandler_FUNC		USART1_IRQHandler

#else
#define	USART_NUM					USART2
#define	USART_BUSx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define	USART_CLOCK					RCC_APB1Periph_USART2

#define	USART_TX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_TX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_TX_GPIO				GPIOA
#define USART_TX_PIN				GPIO_Pin_2

#define	USART_RX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_RX_GPIO				GPIOA
#define USART_RX_PIN				GPIO_Pin_3
//nvic
#define	USART_NVIC_IRQN				USART2_IRQn
#define	USART_NVIC_PRIO				0x01
#define	USART_IRQHandler_FUNC		USART2_IRQHandler

#endif
/***********************************dma****************************************/
/*m2m*/
#define	DMA_M2M_BUSx_CLOCK(x,s)			RCC_AHBPeriphClockCmd(x,s)
#define	DMA_M2M_CLOCK					RCC_AHBPeriph_DMA1
#define	DMA_M2M_CHANNEL					DMA1_Channel7
#define DMA_M2M_FLAG_TC     			DMA1_FLAG_TC7
/*m2m interrupt*/
#define	DMA_M2M_NVIC_IRQN				DMA1_Channel7_IRQn
#define	DMA_M2M_NVIC_PRIO				0x02
#define	DMA_M2M_IT_TC					DMA1_IT_TC7
#define	DMA_M2M_IRQHandler_FUNC 		DMA1_Channel7_IRQHandler

/*m2p*/
#define	DMA_M2P_BUSx_CLOCK(x,s)			RCC_AHBPeriphClockCmd(x,s)
#define	DMA_M2P_CLOCK					RCC_AHBPeriph_DMA1
#define	DMA_M2P_CHANNEL					DMA1_Channel4
#define	DMA_M2P_TX						USART1
#define	DMA_M2P_TXDR_ADDR				(&USART1->DR)
#define DMA_M2P_FLAG_TC     			DMA1_FLAG_TC4
/*m2p interrupt*/
#define	DMA_M2P_NVIC_IRQN				DMA1_Channel4_IRQn
#define	DMA_M2P_NVIC_PRIO				0x02
#define	DMA_M2P_IT_TC					DMA1_IT_TC4
#define	DMA_M2P_IRQHandler_FUNC 		DMA1_Channel4_IRQHandler

/*p2m*/
#define	DMA_P2M_BUSx_CLOCK(x,s)			RCC_AHBPeriphClockCmd(x,s)
#define	DMA_P2M_CLOCK					RCC_AHBPeriph_DMA1			//dma时钟
#define	DMA_P2M_CHANNEL					DMA1_Channel5				//dma外设通道
#define	DMA_M2P_RX						USART1
#define	DMA_M2P_RXDR_ADDR				(&USART1->DR)
#define DMA_P2M_FLAG_TC     			DMA1_FLAG_TC5
/*p2m interrupt*/
#define	DMA_P2M_NVIC_IRQN				DMA1_Channel5_IRQn
#define	DMA_P2M_NVIC_PRIO				0x02
#define	DMA_P2M_IT_TC					DMA1_IT_TC5
#define	DMA_P2M_IRQHandler_FUNC			DMA1_Channel5_IRQHandler

/***********************************adc****************************************/
#define	ADC_GPIO_BUSx_CLOCK(x,s)		RCC_APB2PeriphClockCmd(x,s)
#define	ADC_GPIO_CLOCK					RCC_APB2Periph_GPIOC
#define	ADC_PORT                   		GPIOC
#define	ADC_PIN                     	GPIO_Pin_1	

#define	ADC_BUSx_CLOCK_FUN(r,s)			RCC_APB2PeriphClockCmd(r,s)		
#define ADC_CLOCK						RCC_APB2Periph_ADC2
#define ADCx							ADC2			
#define	ADC_CHANNEL                 	ADC_Channel_11
//nvic
#define	ADC_NVIC_IRQN               	ADC1_2_IRQn
#define	ADC_NVIC_PRIO					0x01

/***********************************dac****************************************/
#define	DAC_GPIO_BUSx_CLOCK(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define	DAC_GPIO_CLOCK					RCC_APB2Periph_GPIOA
#define	DAC_PORT                    	GPIOA
#define	DAC_PIN                     	(GPIO_Pin_4 | GPIO_Pin_5)

#define	DAC_BUSx_CLOCK(r,s)				RCC_APB1PeriphClockCmd(r,s)		
#define DAC_CLOCK						RCC_APB1Periph_DAC


/***********************************iic****************************************/
//hard iic
#define	IIC_RCC_GPIO_CLOCK(r,s)			RCC_APB2PeriphClockCmd(r,s)			
#define	IIC_RCC_GPIO_PORT				RCC_APB2Periph_GPIOB
#define IIC_GPIO_PORT					GPIOB
#define IIC_SCL_PIN						GPIO_Pin_6
#define	IIC_SDA_PIN						GPIO_Pin_7

#define	IIC_RCC_CLOCK(r,s)				RCC_APB1PeriphClockCmd(r,s)
#define	IIC_RCC_PORT					RCC_APB1Periph_I2C1
#define	IIC_NUM							I2C1

//soft iic
#define	SOFT_IIC_RCC_GPIO_CLOCK(r,s)	RCC_APB2PeriphClockCmd(r,s)
#define	SOFT_IIC_RCC_GPIO_PORT			RCC_APB2Periph_GPIOB
#define SOFT_IIC_GPIO_PORT				GPIOB
#define SOFT_IIC_SCL_PIN				GPIO_Pin_6
#define	SOFT_IIC_SDA_PIN				GPIO_Pin_7

#define	SOFT_IIC_SCL_H()				GPIO_SetBits(SOFT_IIC_GPIO_PORT, SOFT_IIC_SCL_PIN)
#define	SOFT_IIC_SCL_L()				GPIO_ResetBits(SOFT_IIC_GPIO_PORT, SOFT_IIC_SCL_PIN)
#define	SOFT_IIC_SDA_H()				GPIO_SetBits(SOFT_IIC_GPIO_PORT, SOFT_IIC_SDA_PIN)
#define	SOFT_IIC_SDA_L()				GPIO_ResetBits(SOFT_IIC_GPIO_PORT, SOFT_IIC_SDA_PIN)
#define	SOFT_IIC_SDA_READ()				GPIO_ReadInputDataBit(SOFT_IIC_GPIO_PORT, SOFT_IIC_SDA_PIN)


/***********************************spi****************************************/
#define	SPI_RCC_GPIO_CLOCK(r,s)			RCC_APB2PeriphClockCmd(r,s)			
#define	SPI_RCC_CS_PORT					RCC_APB2Periph_GPIOC
#define	SPI_RCC_SCK_PORT				RCC_APB2Periph_GPIOA
#define	SPI_RCC_MISO_PORT				RCC_APB2Periph_GPIOA
#define	SPI_RCC_MOSI_PORT				RCC_APB2Periph_GPIOA
#define SPI_CS_PORT						GPIOC
#define SPI_SCK_PORT					GPIOA
#define SPI_MISO_PORT					GPIOA
#define SPI_MOSI_PORT					GPIOA
#define SPI_CS_PIN						GPIO_Pin_0
#define	SPI_SCK_PIN						GPIO_Pin_5
#define SPI_MISO_PIN					GPIO_Pin_6
#define	SPI_MOSI_PIN					GPIO_Pin_7

#define	SPI_RCC_CLOCK(r,s)				RCC_APB2PeriphClockCmd(r,s)
#define	SPI_RCC_PORT					RCC_APB2Periph_SPI1

#define	SPI_NUM							SPI1

/***********************************pwm****************************************/
#define	PWM_GPIO_BUSx_CLOCK(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define	PWM_GPIO_CLOCK					(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define PWM_GPIO_REMAP_FUN()			GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE) 
#define PWM_GPIO_RED_PORT               GPIOB
#define PWM_GPIO_RED_PIN                GPIO_Pin_5
#define PWM_GPIO_GREEN_PORT             GPIOB
#define PWM_GPIO_GREEN_PIN              GPIO_Pin_0
#define PWM_GPIO_BULE_PORT              GPIOB
#define PWM_GPIO_BLUE_PIN               GPIO_Pin_1

#define	PWM_TIMER_BUSx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define PWM_TIMER_CLOCK					RCC_APB1Periph_TIM3
#define	PWM_TIMER_NUM					TIM3
#define	PWM_TIMER_PERIOD_COUNT			(256 - 1)//次数
#define	PWM_TIMER_PRESCALER				(2000 - 1)//分频




/***********************************macro config****************************************/
#define	USE_OLED_LIB





#endif//__config_h

