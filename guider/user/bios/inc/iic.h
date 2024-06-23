#ifndef __iic_h_
#define	__iic_h_

/*-------------------hard iic---------------------------*/
#define	IIC_RCC_GPIO_CLOCK(r,s)			RCC_APB2PeriphClockCmd(r,s)			
#define	IIC_RCC_GPIO_PORT				RCC_APB2Periph_GPIOB
#define IIC_GPIO_PORT					GPIOB
#define IIC_SCL_PIN						GPIO_Pin_6
#define	IIC_SDA_PIN						GPIO_Pin_7

#define	IIC_RCC_CLOCK(r,s)				RCC_APB1PeriphClockCmd(r,s)
#define	IIC_RCC_PORT					RCC_APB1Periph_I2C1
#define	IIC_NUM							I2C1

/*-------------------soft iic----------------------------*/
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




#endif	//__iic_h


