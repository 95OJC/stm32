#include "common.h"


/*-----------------------------------------------------------------
���ļ���Ҫ��������֤spi����

���裺

Ҫ�㣺
1�������stm32���ⲿflashͨѶ��stm32��Ϊ��������spiʱ����Ҫ���˽�ӻ��˵�spiģʽ��ͨ������flash�����ֲ�<W25Q64>��ȡ��
   ��֧��spiģʽ0��ģʽ3��֧��˫��ȫ˫����ʹ��msb����ģʽ��֧�����ͨѶʱ��Ϊ104Mhz������֡Ϊ8λ������Ҫ��stm32��spi
   �����е���Щ��������һ�¡�
2��spi�Ķ�д����������spi��ʼ��ֹͣ�źţ�ֻ���շ�����Ҫ���̣������ڵ��ö�д����ǰ��Ҫ������ʼ��ֹͣ�źŵĲ�����   
3����spi_flash����flash.c����spi_init����Ҫ��flash����spi�Ĳ������뵽spi_init��

------------------------------------------------------------------*/
#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

/*�ȴ���ʱʱ��*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
static __IO u32 SPITimeout = SPIT_FLAG_TIMEOUT;

static void spi_timeout_userCallBack(u8 errorCode)
{
	DBG_TRACE("spi_timeout! errorCode = %d",errorCode);
	while(1);
}


static void spi_gpio_init(void)
{
	//gpio��ʼ��
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	SPI_RCC_GPIO_CLOCK(SPI_RCC_CS_PORT|SPI_RCC_SCK_PORT|SPI_RCC_MISO_PORT|SPI_RCC_MOSI_PORT,ENABLE);

	//spi_csƬѡ��(nss)
	gpio_initStruct.GPIO_Pin = SPI_CS_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_CS_PORT,&gpio_initStruct);
	
	//spi_sckʱ����
	gpio_initStruct.GPIO_Pin = SPI_SCK_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_SCK_PORT,&gpio_initStruct);
	
	//spi_miso��
	gpio_initStruct.GPIO_Pin = SPI_MISO_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_MISO_PORT,&gpio_initStruct);

	//spi_mosi��
	gpio_initStruct.GPIO_Pin = SPI_MOSI_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_MOSI_PORT,&gpio_initStruct);	
}


static void spi_config_init(SPI_CFG_INFO *pInfo,u8 spinum)
{
	SPI_InitTypeDef	spi_initStruct;
	MEM_SET(&spi_initStruct,0,sizeof(spi_initStruct));

	SPI_RCC_CLOCK(SPI_RCC_PORT,ENABLE);

	spi_initStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//˫��ȫ˫��
	spi_initStruct.SPI_Mode = SPI_Mode_Master;//����ģʽ
	spi_initStruct.SPI_DataSize = SPI_DataSize_8b;//����֡8λ
	spi_initStruct.SPI_CPOL = SPI_CPOL_High;//sck��ʼΪ�ߵ�ƽ��ģʽ0~3��3
	spi_initStruct.SPI_CPHA = SPI_CPHA_2Edge;//ż���زɼ���ģʽ0~3��3
	spi_initStruct.SPI_NSS = SPI_NSS_Soft;//cs��Ϊ�����
	spi_initStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//16��Ƶ��72M/16=4.5M
	spi_initStruct.SPI_FirstBit = SPI_FirstBit_MSB;//msc����ģʽ
	spi_initStruct.SPI_CRCPolynomial = 7;//�����õ�����Ч

	SPI_Init(SPI_NUM,&spi_initStruct);

	//ʹ��spi
	SPI_Cmd(SPI_NUM , ENABLE);

}

//���Ұ���25.2.3��SPIͨѶ����ͼ��⣬��SPI��DR�������ݺ󣬵ȴ�RXNE��־��1�������ݼ��ɣ���ÿ�ο�ʼ��������ǰ���ȴ�DRΪ���ȣ�
static DEV_STATUS spi_sendByte(u8 *txbuf)
{
	DEV_STATUS ret = DEV_OK;
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	//�ȴ����ͻ�������1Ϊ�գ�TXE�¼�
	//�û�д���ݸ�DR�Ĵ�����DR�Ĵ��������ݷ���MOSI��ͻ���1���û��Ϳ��Լ�������һ�����ݸ�DR�Ĵ���
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(11);
	}

	//��Ҫд�������д�뷢�ͻ�����
	SPI_I2S_SendData(SPI_NUM, (uint16_t )(*txbuf));

	SPITimeout = SPIT_FLAG_TIMEOUT;
	//�ȴ����ջ�������1Ϊ�ǿգ�RXNE�¼�
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(12);
	}

	//��ȡ���ݼĴ�������ȡ���ջ���������,������������ɣ����豣�������
	SPI_I2S_ReceiveData(SPI_NUM);

	return ret;
}

static DEV_STATUS spi_getByte(u8 *rxbuf)
{
	DEV_STATUS ret = DEV_OK;
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	//�ȴ����ͻ�������1Ϊ�գ�TXE�¼�,
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(0);
	}

	//��Ҫд�������д�뷢�ͻ�����
	SPI_I2S_SendData(SPI_NUM, 0xff);

	SPITimeout = SPIT_FLAG_TIMEOUT;
	//�ȴ����ջ�������1Ϊ�ǿգ�RXNE�¼�
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(1);
	}

	//��ȡ���ݼĴ�������ȡ���ջ���������
	*rxbuf = (u8)SPI_I2S_ReceiveData(SPI_NUM);
	
	return ret;

}

DEV_STATUS spi_cs_connect(u8 cs,u8 spinum)
{
	GPIO_ResetBits(SPI_CS_PORT,SPI_CS_PIN);
	
	return DEV_OK;
}

DEV_STATUS spi_cs_disconnect(u8 cs,u8 spinum)
{
	GPIO_SetBits(SPI_CS_PORT,SPI_CS_PIN);
	
	return DEV_OK;
}

DEV_STATUS spi_init(SPI_CFG_INFO *pInfo,u8 spinum)
{
	spi_gpio_init();
	spi_cs_disconnect(0,0);
	spi_config_init(pInfo,spinum);

	
	return DEV_OK;
}

DEV_STATUS spi_write(u8 *txBuf, u32 size, u8 spinum)
{
	u32 i;
	DEV_STATUS ret;
	
	for(i=0;i<size;i++)
	{
		ret = spi_sendByte(txBuf++);
		if(ret != DEV_OK)
		{
			break;
		}
	}

	return ret;
}

DEV_STATUS spi_read(u8 *rxBuf, u32 size, u8 spinum)
{
	u32 i;
	DEV_STATUS ret;
	
	for(i=0;i<size;i++)
	{
		ret = spi_getByte(rxBuf++);
		if(ret != DEV_OK)
		{
			break;
		}
	}

	return ret;


}


