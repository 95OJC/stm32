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
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

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

    /* I2C1 DMA Init */
    /* I2C1_RX Init */
    hdma_i2c1_rx.Instance = DMA1_Channel7;
    hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_i2c1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmarx,hdma_i2c1_rx);

    /* I2C1_TX Init */
    hdma_i2c1_tx.Instance = DMA1_Channel6;
    hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
    hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_i2c1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i2cHandle,hdmatx,hdma_i2c1_tx);

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
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

    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(i2cHandle->hdmarx);
    HAL_DMA_DeInit(i2cHandle->hdmatx);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
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
#if 0
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
	//中断方式写入读取（需要在stm32_hal_conf.h打开宏USE_HAL_I2C_REGISTER_CALLBACKS才能够触发HAL_I2C_MemRxCpltCallback之类的回调函数?好像不是）
	//验证每发送或接收一个字节会进中断一次(hi2c->XferISR     = I2C_Master_ISR_IT)中断里触发该回调函数
	//然后在中断里判断iic的数据完成标志位,而阻塞方式直接判断iic的数据完成标志位
	//HAL_I2C_Mem_Write_IT只是启动发送中断，手动第一次发送数据，然后在中断里面再发送余下数据，每字节都会产生一次中断发送一字节数据
	//目前验证出rx能够每接收到一字节产生中断和完成回调，但是发送每字节不能产生中断和完成回调，不知道为什么发送中断哪里配置不对？
	if(HAL_I2C_Mem_Write_IT(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str2,sizeof(str2))==HAL_OK)
		printf("STR2_Write_OK\r\n");
	HAL_Delay(1000);
	//收到数据后，先产生中断，然后中断里面读取数据，最后接收完设置size的数据后，产生完成回调函数
	HAL_I2C_Mem_Read_IT(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str2));//需要主动发read命令，才能接收到数据
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#else
	//DMA中断方式写入读取
	//接收size数据后，会产生一次dma中断(里面判断是一半传输还是完成传输或传输错误等类型的中断)，再触发回调函数

	/* 这里的注册会被HAL库(HAL_I2C_Mem_Write_DMA里面或HAL_I2C_Mem_Read_DMA里面)覆盖掉的...*/
	//HAL_DMA_RegisterCallback(&hdma_i2c1_rx,HAL_DMA_XFER_CPLT_CB_ID,HAL_I2C_DMARxCpltCallback);
	//HAL_DMA_RegisterCallback(&hdma_i2c1_tx,HAL_DMA_XFER_CPLT_CB_ID,HAL_I2C_DMATxCpltCallback);
	if(HAL_I2C_Mem_Write_DMA(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str3,sizeof(str3))==HAL_OK)
	//if(HAL_I2C_Mem_Write(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str1,sizeof(str1),1000)==HAL_OK)
		printf("STR3_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read_DMA(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str3));
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#endif
}
/* USER CODE END 1 */
