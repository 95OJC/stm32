/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* W25Q64型号：厂商ID为EF，flash型号ID为0x4017*/
/* W25Q128型号：厂商ID为EF，flash型号ID为0x4018*/
/*FLASH 常用命令 */
#define W25X_WriteEnable 			0x06//写允许
#define W25X_WriteDisable 			0x04//写禁止
#define W25X_ReadStatusReg 			0x05//读状态寄存器
#define W25X_ReadData 				0x03//读
#define W25X_FastReadData 			0x0B//快速读
#define W25X_PageProgram 			0x02//页写
#define W25X_SectorErase 			0x20//sector擦除
#define W25X_BlockErase 			0xD8//block擦除
#define W25X_ChipErase 				0xC7//flash擦除
#define W25X_DeviceID 				0xAB//flash型号ID(低8位) 
#define W25X_ManufactDeviceID 		0x90//厂商ID + flash型号ID(低8位) 
#define W25X_JedecDeviceID 			0x9F//厂商ID + flash型号ID(高8位)  + flash型号ID(低8位)  

void spi_flash_connect(void)
{
		HAL_GPIO_WritePin(SFLASH_CS_GPIO_Port, SFLASH_CS_Pin, GPIO_PIN_RESET);
}

void spi_flash_disconnect(void)
{
		HAL_GPIO_WritePin(SFLASH_CS_GPIO_Port, SFLASH_CS_Pin, GPIO_PIN_SET);
}

void spi_flash_read_jid(void)
{
	uint8_t cmd[1];
	uint8_t jid[3] = {0};
	cmd[0] = W25X_JedecDeviceID;
	
	spi_flash_connect();
	HAL_SPI_Transmit(&hspi1,cmd,sizeof(cmd),1000);
	HAL_SPI_Receive(&hspi1,jid,sizeof(jid),1000);
	spi_flash_disconnect();

	printf("jid = 0x%x,0x%x,0x%x\r\n",jid[0],jid[1],jid[2]);
}
/* USER CODE END 1 */
