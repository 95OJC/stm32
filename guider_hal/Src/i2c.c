/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//读写地址
#define AT24C02_Write 0xA0
#define AT24C02_Read  0xA1
//三次写入的字符串
unsigned char str1[]="jeck666";//{"jeck666"};
unsigned char str2[]="1234567";//{"1234567"};
unsigned char str3[]="abcdefg";//{"abcdefg"};
//读取缓存区
uint8_t ReadBuffer[50];

void test_iic_eeprom(void)
{
	printf("IIC1 Init Ok!\r\n");
	HAL_Delay(100);
#if 1
	//阻塞方式写入读取
	//每个字节发送后都会等待1000ms的数据完成标志位
	if(HAL_I2C_Mem_Write(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str1,sizeof(str1),1000)==HAL_OK)
		printf("STR1_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str1),1000);
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#elif 0
	//中断方式写入读取（需要在stm32_hal_conf.h打开宏USE_HAL_I2C_REGISTER_CALLBACKS才能够触发HAL_I2C_MemRxCpltCallback之类的回调函数）
	//验证每发送或接收一个字节会进中断一次(hi2c->XferISR     = I2C_Master_ISR_IT)中断里触发该回调函数
	//然后在中断里判断iic的数据完成标志位,而阻塞方式直接判断iic的数据完成标志位
	//HAL_I2C_Mem_Write_IT只是启动发送中断，手动第一次发送数据，然后在中断里面再发送余下数据，每字节都会产生一次中断发送一字节数据
	//目前验证出rx能够每接收到一字节产生中断和完成回调，但是发送每字节不能产生中断和完成回调，不知道为什么发送中断哪里配置不对？
	if(HAL_I2C_Mem_Write_IT(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str2,sizeof(str2))==HAL_OK)
		printf("STR2_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read_IT(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str2));
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#else
	//DMA中断方式写入读取
	//先产生dma中断(里面判断是一半传输还是完成传输或传输错误等类型的中断)，再产生iic ev中断的接收中断，但是iic ev中断的发送中断没有产生，和上面的中断方式写入读取一样的问题
	//因此先触发dma接收完成中断回调函数(有道笔记有介绍该回调函数怎么被HAL库注册了)，再触发iic的接收完成中断

	/* 这里的注册会被HAL库覆盖掉的...*/
	HAL_DMA_RegisterCallback(&hdma_i2c1_rx,HAL_DMA_XFER_CPLT_CB_ID,HAL_I2C_DMARxCpltCallback);
	HAL_DMA_RegisterCallback(&hdma_i2c1_tx,HAL_DMA_XFER_CPLT_CB_ID,HAL_I2C_DMATxCpltCallback);
	
	if(HAL_I2C_Mem_Write_DMA(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str3,sizeof(str3))==HAL_OK)
		printf("STR3_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read_DMA(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str3));
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#endif
}

//***停止信号前面的每一步都以SCL被拉低为结束(下一步先动SDA，因为SCL是低电平不影响)，最后停止信号将会把SCL和SDA都拉高。
//***每一步都是需要控制SCL一个脉冲，及时把SDA切换为输入和输出模式。
#define	SOFT_IIC_SCL_H()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
#define	SOFT_IIC_SCL_L()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
#define	SOFT_IIC_SDA_H()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
#define	SOFT_IIC_SDA_L()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
#define	SOFT_IIC_SDA_READ()		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

/** EEPROM 器件地址 */
#define EEPROM_ID       0xA0

//iic读写方向
#define IIC_DEVICE_ADDRESS_WRITE	0 
#define	IIC_DEVICE_ADDRESS_READ		1


static void soft_delay(__IO uint32_t cnt)
{
	for( ; cnt!=0; cnt--);
}

void soft_iic_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
	
		/**I2C1 GPIO Configuration
	PB6     ------> I2C1_SCL
	PB7     ------> I2C1_SDA
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void soft_iic_start(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	/*进入这里前全部为高电平或者全部为低电平*/
	
	SOFT_IIC_SDA_H();
	SOFT_IIC_SCL_H();//将时钟线和数据线全部拉高，让其处于空闲状态
	soft_delay(30);//AT24C02至少等待4.7us(保持空闲状态)
	
	SOFT_IIC_SDA_L();//SDA拉低，产生下降沿，产生起始信号
	soft_delay(30);//AT24C02至少等待4us(保持起始信号)
	
	SOFT_IIC_SCL_L();
	soft_delay(1);//AT24C02至少0.1us(TAA:SCL变低到数据输出有效)，最多4.5us就需要把SDA拉高，再发送有效数据

	/*退出时全部为低电平*/
}

static void soft_iic_stop(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	
	SOFT_IIC_SDA_L();
	SOFT_IIC_SCL_L();
	soft_delay(40);//AT24C02为了错开ack的波形，延迟久一点
	
	SOFT_IIC_SCL_H();//SCL最后拉高
	soft_delay(1);
	SOFT_IIC_SDA_H();//SDA最后拉高，产生上升沿，产生停止信号
	
	soft_delay(30);//at24c02在开始一个新的传输前总线被释放时间至少4.7us
	
	/*退出时全部为高电平*/
}

static void soft_iic_ack(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	
	SOFT_IIC_SDA_L();//CPU主动驱动SDA=0为应答信号
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	
	SOFT_IIC_SCL_H();//CPU产生一个SCL时钟
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(30);//？
	
	/*退出时全部为低电平*/
}

static void soft_iic_no_ack(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	
	SOFT_IIC_SDA_H();//CPU主动驱动SDA=1为非应答信号
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	
	SOFT_IIC_SCL_H();//CPU产生一个SCL时钟
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(1);//？
	
	/*退出时SCL为低电平，SDA为高电平*/
}

static uint8_t soft_iic_wait_ack(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	uint8_t ret;
	
	SOFT_IIC_SDA_H();//CPU释放SDA总线,对于AT24C02来说已经拿到SDA线权而应答了，所以波形一直显示为低电平。
	soft_delay(1);//不需要等待太久，因为AT24C02已经拿到SDA线权而应答了。
	
	//(若上面没有从机拿到线权才配置？)这里可添加配置SDA线为输入模式，再加个soft_delay();
	//........
	
	SOFT_IIC_SCL_H();//CPU主动驱动SCL=1, 此时器件会返回SDA线的ACK应答
	soft_delay(1);//在此期间，可以尽快读取应答信号或者非应答信号,因此等待时间可以尽量少。
	if(SOFT_IIC_SDA_READ())
	{
		ret = 0;//高电平表示非应答信号
	}
	else
	{
		ret = 1;//低电平表示应答信号
	}
	
	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(5);//此时应答结束，at24c02归还SDA线权给MCU，因此是高电平，也可能为低电平。
	//(若上面没有拿到线权才配置？)这里可添加配置SDA线为输出模式，再加个soft_delay();
	
	return ret;
	
	/*退出时SCL为低电平，SDA为高电平或者低电平*/
}

static void soft_iic_send_byte(uint8_t data)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		//切换有效数据电平状态
		if(data & 0x80)//先传最高位，因为是MSB格式
		{
			SOFT_IIC_SDA_H();
		}
		else
		{
			SOFT_IIC_SDA_L();
		}
		soft_delay(30);//at24c02等待有效数据电平稳定至少200ns，而SCL低电平时间至少4.7us
		
		SOFT_IIC_SCL_H();//SCL拉高，获取有效数据
		soft_delay(30);//at24c02的SCL高电平时间至少4us
		SOFT_IIC_SCL_L();//SCL最后拉低，获取有效数据完毕
		
		data <<= 1;//移到下一低位
		//soft_delay(1);//SCL拉低后，需维持有效数据电平一定时间,at24c02没有该时间要求
	}
	
	soft_delay(1);//下一个函数为等待应答，at24c02的应答输出时间至少需要100ns
	
	/*退出时SCL为低电平，SDA为高电平或者低电平*/
}

static uint8_t soft_iic_read_byte(void)
{
	/*进入这里前SCL已经是低电平了，保证没有起始信号、没有停止信号、没有应答信号、数据无效*/
	uint8_t data = 0;
	uint8_t i;
	
	//这里可添加配置SDA线为输入模式，再加个soft_delay();
	soft_delay(25);//at24c02的SCL低电平时间至少4.7us
	for(i=0;i<8;i++)
	{
		data <<= 1;
		SOFT_IIC_SCL_H();//SCL拉高，读取有效数据
		soft_delay(1);//at24c02这里可以尽快读数据
		if(SOFT_IIC_SDA_READ())//SDA最后电平需要bit0，因为是MSB传输格式
		{
			data++;//当前位为1
		}
		soft_delay(30);//at24c02的SCL高电平时间至少4us
		SOFT_IIC_SCL_L();//SCL最后拉低
		soft_delay(30);//at24c02的SCL低电平时间至少4.7us
	}
	
	//这里可添加配置SDA线为输出模式，再加个soft_delay();
	return data;
	
	/*退出时SCL为低电平，SDA为高电平或者低电平*/
}

void soft_iic_write_mem(uint32_t write_addr, uint8_t *pbuf)
{
	/* 第1步：发起I2C总线启动信号 */
	soft_iic_start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* 第3步：等待ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem第3步：等待ACK error!\r\n");
		while(1);
	}	
	
	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	soft_iic_send_byte((uint8_t)write_addr);
	
	/* 第5步：等待ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem第5步：等待ACK error!\r\n");
		while(1);
	}	
	
	/* 第6步：开始写入数据 */
	soft_iic_send_byte(*pbuf);

	/* 第7步：等待ACK*/
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem第7步：等待ACK error!\r\n");
		while(1);
	}

	/* 命令执行成功，发送I2C总线停止信号 */
	soft_iic_stop();	
}

void soft_iic_read_mem(uint32_t read_addr, uint8_t *pbuf)
{
	/* 第1步：发起I2C总线启动信号 */
	soft_iic_start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* 第3步：等待ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem第3步：等待ACK error!\r\n");
		while(1);
	}		
	
	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	soft_iic_send_byte((uint8_t)read_addr);	
	
	/* 第5步：等待ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem第5步：等待ACK error!\r\n");
		while(1);
	}		
	
	/* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */	
	soft_iic_start();
	
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* 第8步：等待ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem第8步：等待ACK error!\r\n");
		while(1);
	}			
	
	/* 第9步：循环读取数据 */
	//该括号里面接收一个字节数据
	{
		*pbuf = soft_iic_read_byte();
		soft_iic_no_ack();
	}	
	
	/* 命令执行成功，发送I2C总线停止信号 */
	soft_iic_stop();
}

/* USER CODE END 1 */
