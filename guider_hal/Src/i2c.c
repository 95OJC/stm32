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
//��д��ַ
#define AT24C02_Write 0xA0
#define AT24C02_Read  0xA1
//����д����ַ���
unsigned char str1[]="jeck666";//{"jeck666"};
unsigned char str2[]="1234567";//{"1234567"};
unsigned char str3[]="abcdefg";//{"abcdefg"};
//��ȡ������
uint8_t ReadBuffer[50];

void test_iic_eeprom(void)
{
	printf("IIC1 Init Ok!\r\n");
	HAL_Delay(100);
#if 1
	//������ʽд���ȡ
	//ÿ���ֽڷ��ͺ󶼻�ȴ�1000ms��������ɱ�־λ
	if(HAL_I2C_Mem_Write(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str1,sizeof(str1),1000)==HAL_OK)
		printf("STR1_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str1),1000);
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#elif 0
	//�жϷ�ʽд���ȡ����Ҫ��stm32_hal_conf.h�򿪺�USE_HAL_I2C_REGISTER_CALLBACKS���ܹ�����HAL_I2C_MemRxCpltCallback֮��Ļص�������
	//��֤ÿ���ͻ����һ���ֽڻ���ж�һ��(hi2c->XferISR     = I2C_Master_ISR_IT)�ж��ﴥ���ûص�����
	//Ȼ�����ж����ж�iic��������ɱ�־λ,��������ʽֱ���ж�iic��������ɱ�־λ
	//HAL_I2C_Mem_Write_ITֻ�����������жϣ��ֶ���һ�η������ݣ�Ȼ�����ж������ٷ����������ݣ�ÿ�ֽڶ������һ���жϷ���һ�ֽ�����
	//Ŀǰ��֤��rx�ܹ�ÿ���յ�һ�ֽڲ����жϺ���ɻص������Ƿ���ÿ�ֽڲ��ܲ����жϺ���ɻص�����֪��Ϊʲô�����ж��������ò��ԣ�
	if(HAL_I2C_Mem_Write_IT(&hi2c1,AT24C02_Write,0,I2C_MEMADD_SIZE_8BIT,str2,sizeof(str2))==HAL_OK)
		printf("STR2_Write_OK\r\n");
	HAL_Delay(1000);
	HAL_I2C_Mem_Read_IT(&hi2c1,AT24C02_Read,0,I2C_MEMADD_SIZE_8BIT,ReadBuffer,sizeof(str2));
	HAL_Delay(1000);
	printf("%s\r\n",ReadBuffer);
	HAL_Delay(1000);
#else
	//DMA�жϷ�ʽд���ȡ
	//�Ȳ���dma�ж�(�����ж���һ�봫�仹����ɴ�������������͵��ж�)���ٲ���iic ev�жϵĽ����жϣ�����iic ev�жϵķ����ж�û�в�������������жϷ�ʽд���ȡһ��������
	//����ȴ���dma��������жϻص�����(�е��ʼ��н��ܸûص�������ô��HAL��ע����)���ٴ���iic�Ľ�������ж�

	/* �����ע��ᱻHAL�⸲�ǵ���...*/
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

//***ֹͣ�ź�ǰ���ÿһ������SCL������Ϊ����(��һ���ȶ�SDA����ΪSCL�ǵ͵�ƽ��Ӱ��)�����ֹͣ�źŽ����SCL��SDA�����ߡ�
//***ÿһ��������Ҫ����SCLһ�����壬��ʱ��SDA�л�Ϊ��������ģʽ��
#define	SOFT_IIC_SCL_H()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
#define	SOFT_IIC_SCL_L()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
#define	SOFT_IIC_SDA_H()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
#define	SOFT_IIC_SDA_L()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
#define	SOFT_IIC_SDA_READ()		HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

/** EEPROM ������ַ */
#define EEPROM_ID       0xA0

//iic��д����
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
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	/*��������ǰȫ��Ϊ�ߵ�ƽ����ȫ��Ϊ�͵�ƽ*/
	
	SOFT_IIC_SDA_H();
	SOFT_IIC_SCL_H();//��ʱ���ߺ�������ȫ�����ߣ����䴦�ڿ���״̬
	soft_delay(30);//AT24C02���ٵȴ�4.7us(���ֿ���״̬)
	
	SOFT_IIC_SDA_L();//SDA���ͣ������½��أ�������ʼ�ź�
	soft_delay(30);//AT24C02���ٵȴ�4us(������ʼ�ź�)
	
	SOFT_IIC_SCL_L();
	soft_delay(1);//AT24C02����0.1us(TAA:SCL��͵����������Ч)�����4.5us����Ҫ��SDA���ߣ��ٷ�����Ч����

	/*�˳�ʱȫ��Ϊ�͵�ƽ*/
}

static void soft_iic_stop(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	
	SOFT_IIC_SDA_L();
	SOFT_IIC_SCL_L();
	soft_delay(40);//AT24C02Ϊ�˴�ack�Ĳ��Σ��ӳپ�һ��
	
	SOFT_IIC_SCL_H();//SCL�������
	soft_delay(1);
	SOFT_IIC_SDA_H();//SDA������ߣ����������أ�����ֹͣ�ź�
	
	soft_delay(30);//at24c02�ڿ�ʼһ���µĴ���ǰ���߱��ͷ�ʱ������4.7us
	
	/*�˳�ʱȫ��Ϊ�ߵ�ƽ*/
}

static void soft_iic_ack(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	
	SOFT_IIC_SDA_L();//CPU��������SDA=0ΪӦ���ź�
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	
	SOFT_IIC_SCL_H();//CPU����һ��SCLʱ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(30);//��
	
	/*�˳�ʱȫ��Ϊ�͵�ƽ*/
}

static void soft_iic_no_ack(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	
	SOFT_IIC_SDA_H();//CPU��������SDA=1Ϊ��Ӧ���ź�
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	
	SOFT_IIC_SCL_H();//CPU����һ��SCLʱ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(1);//��
	
	/*�˳�ʱSCLΪ�͵�ƽ��SDAΪ�ߵ�ƽ*/
}

static uint8_t soft_iic_wait_ack(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	uint8_t ret;
	
	SOFT_IIC_SDA_H();//CPU�ͷ�SDA����,����AT24C02��˵�Ѿ��õ�SDA��Ȩ��Ӧ���ˣ����Բ���һֱ��ʾΪ�͵�ƽ��
	soft_delay(1);//����Ҫ�ȴ�̫�ã���ΪAT24C02�Ѿ��õ�SDA��Ȩ��Ӧ���ˡ�
	
	//(������û�дӻ��õ���Ȩ�����ã�)������������SDA��Ϊ����ģʽ���ټӸ�soft_delay();
	//........
	
	SOFT_IIC_SCL_H();//CPU��������SCL=1, ��ʱ�����᷵��SDA�ߵ�ACKӦ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ����ȡӦ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	if(SOFT_IIC_SDA_READ())
	{
		ret = 0;//�ߵ�ƽ��ʾ��Ӧ���ź�
	}
	else
	{
		ret = 1;//�͵�ƽ��ʾӦ���ź�
	}
	
	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(5);//��ʱӦ�������at24c02�黹SDA��Ȩ��MCU������Ǹߵ�ƽ��Ҳ����Ϊ�͵�ƽ��
	//(������û���õ���Ȩ�����ã�)������������SDA��Ϊ���ģʽ���ټӸ�soft_delay();
	
	return ret;
	
	/*�˳�ʱSCLΪ�͵�ƽ��SDAΪ�ߵ�ƽ���ߵ͵�ƽ*/
}

static void soft_iic_send_byte(uint8_t data)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		//�л���Ч���ݵ�ƽ״̬
		if(data & 0x80)//�ȴ����λ����Ϊ��MSB��ʽ
		{
			SOFT_IIC_SDA_H();
		}
		else
		{
			SOFT_IIC_SDA_L();
		}
		soft_delay(30);//at24c02�ȴ���Ч���ݵ�ƽ�ȶ�����200ns����SCL�͵�ƽʱ������4.7us
		
		SOFT_IIC_SCL_H();//SCL���ߣ���ȡ��Ч����
		soft_delay(30);//at24c02��SCL�ߵ�ƽʱ������4us
		SOFT_IIC_SCL_L();//SCL������ͣ���ȡ��Ч�������
		
		data <<= 1;//�Ƶ���һ��λ
		//soft_delay(1);//SCL���ͺ���ά����Ч���ݵ�ƽһ��ʱ��,at24c02û�и�ʱ��Ҫ��
	}
	
	soft_delay(1);//��һ������Ϊ�ȴ�Ӧ��at24c02��Ӧ�����ʱ��������Ҫ100ns
	
	/*�˳�ʱSCLΪ�͵�ƽ��SDAΪ�ߵ�ƽ���ߵ͵�ƽ*/
}

static uint8_t soft_iic_read_byte(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ�ˣ���֤û����ʼ�źš�û��ֹͣ�źš�û��Ӧ���źš�������Ч*/
	uint8_t data = 0;
	uint8_t i;
	
	//������������SDA��Ϊ����ģʽ���ټӸ�soft_delay();
	soft_delay(25);//at24c02��SCL�͵�ƽʱ������4.7us
	for(i=0;i<8;i++)
	{
		data <<= 1;
		SOFT_IIC_SCL_H();//SCL���ߣ���ȡ��Ч����
		soft_delay(1);//at24c02������Ծ��������
		if(SOFT_IIC_SDA_READ())//SDA����ƽ��Ҫbit0����Ϊ��MSB�����ʽ
		{
			data++;//��ǰλΪ1
		}
		soft_delay(30);//at24c02��SCL�ߵ�ƽʱ������4us
		SOFT_IIC_SCL_L();//SCL�������
		soft_delay(30);//at24c02��SCL�͵�ƽʱ������4.7us
	}
	
	//������������SDA��Ϊ���ģʽ���ټӸ�soft_delay();
	return data;
	
	/*�˳�ʱSCLΪ�͵�ƽ��SDAΪ�ߵ�ƽ���ߵ͵�ƽ*/
}

void soft_iic_write_mem(uint32_t write_addr, uint8_t *pbuf)
{
	/* ��1��������I2C���������ź� */
	soft_iic_start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* ��3�����ȴ�ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem��3�����ȴ�ACK error!\r\n");
		while(1);
	}	
	
	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	soft_iic_send_byte((uint8_t)write_addr);
	
	/* ��5�����ȴ�ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem��5�����ȴ�ACK error!\r\n");
		while(1);
	}	
	
	/* ��6������ʼд������ */
	soft_iic_send_byte(*pbuf);

	/* ��7�����ȴ�ACK*/
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_write_mem��7�����ȴ�ACK error!\r\n");
		while(1);
	}

	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	soft_iic_stop();	
}

void soft_iic_read_mem(uint32_t read_addr, uint8_t *pbuf)
{
	/* ��1��������I2C���������ź� */
	soft_iic_start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* ��3�����ȴ�ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem��3�����ȴ�ACK error!\r\n");
		while(1);
	}		
	
	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	soft_iic_send_byte((uint8_t)read_addr);	
	
	/* ��5�����ȴ�ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem��5�����ȴ�ACK error!\r\n");
		while(1);
	}		
	
	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */	
	soft_iic_start();
	
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_send_byte(EEPROM_ID|IIC_DEVICE_ADDRESS_WRITE);
	
	/* ��8�����ȴ�ACK */
	if(!soft_iic_wait_ack())
	{
		printf("soft_iic_read_mem��8�����ȴ�ACK error!\r\n");
		while(1);
	}			
	
	/* ��9����ѭ����ȡ���� */
	//�������������һ���ֽ�����
	{
		*pbuf = soft_iic_read_byte();
		soft_iic_no_ack();
	}	
	
	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	soft_iic_stop();
}

/* USER CODE END 1 */
