#include "common.h"


/*-----------------------------------------------------------------
该文件主要功能是验证spi功能

步骤：

要点：
1、这里的stm32与外部flash通讯。stm32作为主机配置spi时，需要先了解从机端的spi模式，通过查阅flash数据手册<W25Q64>获取，
   它支持spi模式0和模式3，支持双线全双工，使用msb先行模式，支持最高通讯时钟为104Mhz，数据帧为8位。我们要把stm32的spi
   外设中的这些参数配置一致。
2、spi的读写函数不包含spi起始和停止信号，只是收发的主要过程，所有在调用读写函数前后要做好起始和停止信号的操作。   
3、若spi_flash，在flash.c调用spi_init，需要给flash配置spi的参数传入到spi_init。

------------------------------------------------------------------*/
#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
static __IO u32 SPITimeout = SPIT_FLAG_TIMEOUT;

static void spi_timeout_userCallBack(u8 errorCode)
{
	DBG_TRACE("spi_timeout! errorCode = %d",errorCode);
	while(1);
}


static void spi_gpio_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	SPI_RCC_GPIO_CLOCK(SPI_RCC_CS_PORT|SPI_RCC_SCK_PORT|SPI_RCC_MISO_PORT|SPI_RCC_MOSI_PORT,ENABLE);

	//spi_cs片选线(nss)
	gpio_initStruct.GPIO_Pin = SPI_CS_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_CS_PORT,&gpio_initStruct);
	
	//spi_sck时钟线
	gpio_initStruct.GPIO_Pin = SPI_SCK_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_SCK_PORT,&gpio_initStruct);
	
	//spi_miso线
	gpio_initStruct.GPIO_Pin = SPI_MISO_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_MISO_PORT,&gpio_initStruct);

	//spi_mosi线
	gpio_initStruct.GPIO_Pin = SPI_MOSI_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SPI_MOSI_PORT,&gpio_initStruct);	
}


static void spi_config_init(SPI_CFG_INFO *pInfo,u8 spinum)
{
	SPI_InitTypeDef	spi_initStruct;
	MEM_SET(&spi_initStruct,0,sizeof(spi_initStruct));

	SPI_RCC_CLOCK(SPI_RCC_PORT,ENABLE);

	spi_initStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工
	spi_initStruct.SPI_Mode = SPI_Mode_Master;//主机模式
	spi_initStruct.SPI_DataSize = SPI_DataSize_8b;//数据帧8位
	spi_initStruct.SPI_CPOL = SPI_CPOL_High;//sck起始为高电平，模式0~3的3
	spi_initStruct.SPI_CPHA = SPI_CPHA_2Edge;//偶边沿采集，模式0~3的3
	spi_initStruct.SPI_NSS = SPI_NSS_Soft;//cs线为软件控
	spi_initStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//16分频，72M/16=4.5M
	spi_initStruct.SPI_FirstBit = SPI_FirstBit_MSB;//msc先行模式
	spi_initStruct.SPI_CRCPolynomial = 7;//无需用到，无效

	SPI_Init(SPI_NUM,&spi_initStruct);

	//使能spi
	SPI_Cmd(SPI_NUM , ENABLE);

}

//结合野火的25.2.3的SPI通讯过程图理解，给SPI的DR发送数据后，等待RXNE标志置1接收数据即可，但每次开始发送数据前都等待DR为空先！
static DEV_STATUS spi_sendByte(u8 *txbuf)
{
	DEV_STATUS ret = DEV_OK;
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	//等待发送缓冲区置1为空，TXE事件
	//用户写数据给DR寄存器后，DR寄存器把数据发给MOSI后就会置1，用户就可以继续发下一个数据给DR寄存器
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(11);
	}

	//把要写入的数据写入发送缓冲区
	SPI_I2S_SendData(SPI_NUM, (uint16_t )(*txbuf));

	SPITimeout = SPIT_FLAG_TIMEOUT;
	//等待接收缓冲区置1为非空，RXNE事件
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(12);
	}

	//读取数据寄存器，获取接收缓冲区数据,这里仅读出即可，无需保存变量。
	SPI_I2S_ReceiveData(SPI_NUM);

	return ret;
}

static DEV_STATUS spi_getByte(u8 *rxbuf)
{
	DEV_STATUS ret = DEV_OK;
	
	SPITimeout = SPIT_FLAG_TIMEOUT;
	//等待发送缓冲区置1为空，TXE事件,
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_TXE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(0);
	}

	//把要写入的数据写入发送缓冲区
	SPI_I2S_SendData(SPI_NUM, 0xff);

	SPITimeout = SPIT_FLAG_TIMEOUT;
	//等待接收缓冲区置1为非空，RXNE事件
	while(SPI_I2S_GetFlagStatus(SPI_NUM, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if ((SPITimeout--) == 0) 
			spi_timeout_userCallBack(1);
	}

	//读取数据寄存器，获取接收缓冲区数据
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


