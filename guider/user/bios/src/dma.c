#include "common.h"


/*-----------------------------------------------------------------
验证dma功能。

需理解以下：
1、需要初始化uart的io口和外设功能(不用打开中断)才能用dma。

2、TX：初始化串口和DMA后，执行USART_DMACmd()串口通过向DMA申请发送数据，然后缓冲的首地址(addr++)会自动通过DMA传给
串口DR寄存器，发送一个字节后，不需要等待串口的TXE位和TC位，因为CPU内部的DMA和串口已经处理好该关系，我们不需要关
注，发送完后可以产生DMA传输完成中断标志。

3、RX:初始化串口和DMA后，执行USART_DMACmd()串口通过向DMA申请接收串口DR寄存器的数据，事先定义好接收多少长度的数
据，然后DMA会自动给到缓存首地址(addr++),接收完可以产生DMA传输完成中断标志。

4、usart、iic、spi都具有数据寄存器DR，可以用DMA传输，因为DMA需要数据寄存器地址即可传输。

5、当使用存储器到存储器模式时候，通道可以随便选，没有硬性的规定。

总结1：DMA表有支持的外设(可详细看哪个DMA通道)，还要外设有数据寄存器(DR)，就可以用DMA传输数据,只需要关注缓存数据和外设寄存器还有传输宽度和大小等等即可。
总结2：内存到内存的DMA传输，只需要知道2个内存的首地址和大小即可。
------------------------------------------------------------------*/
#define BUFFER_SIZE 	32
	
const u32 aSRC_Buffer[BUFFER_SIZE]= {
									0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
									0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
									0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
									0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
									0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
									0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
									0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
									0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
	
u32 aDST_Buffer[BUFFER_SIZE];

char m2p_buffer[4] = {'a','b','c','d'};

char p2m_buffer[4] = {0};

BOOL dma_m2m_init(DMACTRL *dmaInfo)
{
	/*
	  内存到内存的DMA传输不需要外设向DMA发出请求，初始化完后就会马上传输了。
	  如果没有使能中断，则等待循环while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET);退出即可知道DMA_M2M传输完成。
	  传输完成中断状态标志位和中断不是共同作用的，前者也可以单独产生标志位置1的。
	*/

	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_M2M_BUSx_CLOCK(DMA_M2M_CLOCK, ENABLE);

	//源地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//目标地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从外设到内存(这里的外设可以是falsh或sram)
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址增？
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址增？
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//使能内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	//配置DMA通道
	DMA_Init(DMA_M2M_CHANNEL,&dma_initStruct);
    //清除DMA数据流传输完成标志位
    DMA_ClearFlag(DMA_M2M_FLAG_TC);
	//使能DMA，开始传输
	DMA_Cmd(DMA_M2M_CHANNEL,ENABLE);

	return TRUE;
}

void dma_m2m_test(void)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)aSRC_Buffer;
	dma_test.MemoryBaseAddr = (u32)aDST_Buffer;
	dma_test.DIR = DMA_DIR_PeripheralSRC;
	dma_test.BufferSize = BUFFER_SIZE;
	dma_test.PeripheralInc = DMA_PeripheralInc_Enable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Word;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Word;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Enable;
	dma_m2m_init(&dma_test);

	//等待dma传输完成中断状态标志位置1
	while(DMA_GetFlagStatus(DMA_M2M_FLAG_TC) == RESET);

	PRINTF("dma m2m success!\r\n");

}

BOOL dma_m2m_interrupt_init(DMACTRL *dmaInfo)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_M2M_BUSx_CLOCK(DMA_M2M_CLOCK, ENABLE);

	NVIC_ClearPendingIRQ(DMA_M2M_NVIC_IRQN);
	NVIC_SetPriority(DMA_M2M_NVIC_IRQN,DMA_M2M_NVIC_PRIO);
	NVIC_EnableIRQ(DMA_M2M_NVIC_IRQN);

	//源地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//目标地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从外设到内存(这里的外设可以是falsh或sram)
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址增？
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址增？
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//使能内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	//配置DMA通道
	DMA_Init(DMA_M2M_CHANNEL,&dma_initStruct);
    //清除DMA数据流传输完成标志位
    DMA_ClearITPendingBit(DMA_M2M_IT_TC);	
	//允许DMA传输完成中断产生
	DMA_ITConfig(DMA_M2M_CHANNEL,DMA_IT_TC,ENABLE);	
	//使能DMA，开始传输
	DMA_Cmd(DMA_M2M_CHANNEL,ENABLE);

	return TRUE;
}

void dma_m2m_interrupt_test(void)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)aSRC_Buffer;
	dma_test.MemoryBaseAddr = (u32)aDST_Buffer;
	dma_test.DIR = DMA_DIR_PeripheralSRC;
	dma_test.BufferSize = BUFFER_SIZE;
	dma_test.PeripheralInc = DMA_PeripheralInc_Enable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Word;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Word;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Enable;
	dma_m2m_interrupt_init(&dma_test);

}

void DMA_M2M_IRQHandler_FUNC(void)
{
	if(DMA_GetITStatus(DMA_M2M_IT_TC)!=RESET)
	{
		DMA_ClearFlag(DMA_M2M_IT_TC);
		PRINTF("dma m2m interrupt success!\r\n");
		
	}

}

BOOL dma_m2p_init(DMACTRL *dmaInfo, DMA_M2P_per per)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_M2P_BUSx_CLOCK(DMA_M2P_CLOCK, ENABLE);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从内存到外设
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_M2P_CHANNEL,&dma_initStruct);

	if(per == M2P_USART)
	{
		/*
		  外设向DMA发出TX请求,然后把dmaInfo->MemoryBaseAddr数据的内容发出去。
		  相当于自动把缓冲数据给TX_DR寄存器写数据发送出去。(不用CPU干预每次循环写数据给TX_DR寄存器)
		*/	
		USART_DMACmd(DMA_M2P_TX,USART_DMAReq_Tx,ENABLE);
	}

	DMA_Cmd(DMA_M2P_CHANNEL,ENABLE);	
	return TRUE;
}

void dma_m2p_test(DMA_M2P_per per)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)DMA_M2P_TXDR_ADDR;
	dma_test.MemoryBaseAddr = (u32)m2p_buffer;
	dma_test.DIR = DMA_DIR_PeripheralDST;
	dma_test.BufferSize = sizeof(m2p_buffer);
	dma_test.PeripheralInc = DMA_PeripheralInc_Disable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Disable;
	dma_m2p_init(&dma_test,per);

	//等待dma传输完成中断状态标志位置1
	while(DMA_GetFlagStatus(DMA_M2P_FLAG_TC) == RESET);

	PRINTF(" dma m2p success!\r\n");	
}

BOOL dma_m2p_interrupt_init(DMACTRL *dmaInfo, DMA_M2P_per per)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_M2P_BUSx_CLOCK(DMA_M2P_CLOCK, ENABLE);

	NVIC_ClearPendingIRQ(DMA_M2P_NVIC_IRQN);
	NVIC_SetPriority(DMA_M2P_NVIC_IRQN,DMA_M2P_NVIC_PRIO);
	NVIC_EnableIRQ(DMA_M2P_NVIC_IRQN);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从内存到外设
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_M2P_CHANNEL,&dma_initStruct);
    //清除DMA数据流传输完成标志位
    DMA_ClearITPendingBit(DMA_M2P_IT_TC);	
	//允许DMA传输完成中断产生
	DMA_ITConfig(DMA_M2P_CHANNEL,DMA_IT_TC,ENABLE);	
	
	if(per == M2P_USART)
	{
		/*
		  外设向DMA发出TX请求,然后把dmaInfo->MemoryBaseAddr数据的内容发出去。
		  相当于自动把缓冲数据给TX_DR寄存器写数据发送出去。(不用CPU干预每次循环写数据给TX_DR寄存器)
		*/	
		USART_DMACmd(DMA_M2P_TX,USART_DMAReq_Tx,ENABLE);
	}

	DMA_Cmd(DMA_M2P_CHANNEL,ENABLE);

	return TRUE;
}

void dma_m2p_interrupt_test(DMA_M2P_per per)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)DMA_M2P_TXDR_ADDR;
	dma_test.MemoryBaseAddr = (u32)m2p_buffer;
	dma_test.DIR = DMA_DIR_PeripheralDST;
	dma_test.BufferSize = sizeof(m2p_buffer);
	dma_test.PeripheralInc = DMA_PeripheralInc_Disable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Disable;
	dma_m2p_interrupt_init(&dma_test,per);

}

void DMA_M2P_IRQHandler_FUNC(void)
{
	if(DMA_GetITStatus(DMA_M2P_IT_TC)!=RESET)
	{
		DMA_ClearFlag(DMA_M2P_IT_TC);
		PRINTF(" dma m2p interrupt success!\r\n");
		
	}
}

BOOL dma_p2m_init(DMACTRL *dmaInfo, DMA_P2M_per per)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_P2M_BUSx_CLOCK(DMA_P2M_CLOCK, ENABLE);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从外设到内存
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_P2M_CHANNEL,&dma_initStruct);

	if(per == P2M_USART)
	{
		/*
		  外设向DMA发出RX请求,然后等待dmaInfo->MemoryBaseAddr数据接收完毕产生DMA接收完成中断
		  相当于RX_DR寄存器有数据，自动把数据放到缓存里。(不用CPU干预每次循环读RX_DR寄存器的数据)
		*/
		USART_DMACmd(DMA_M2P_RX,USART_DMAReq_Rx,ENABLE);
	}
	
	DMA_Cmd(DMA_P2M_CHANNEL,ENABLE);

	return TRUE;

}

void dma_p2m_test(DMA_P2M_per per)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)DMA_M2P_RXDR_ADDR;
	dma_test.MemoryBaseAddr = (u32)p2m_buffer;
	dma_test.DIR = DMA_DIR_PeripheralSRC;
	dma_test.BufferSize = sizeof(p2m_buffer);
	dma_test.PeripheralInc = DMA_PeripheralInc_Disable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Disable;
	dma_p2m_init(&dma_test,per);

	//等待dma传输完成中断状态标志位置1
	while(DMA_GetFlagStatus(DMA_P2M_FLAG_TC) == RESET);

	PRINTF(" dma p2m success!\r\n");
	PRINTF("p2m_buffer[0] = %c \r\n",p2m_buffer[0]);
	PRINTF("p2m_buffer[1] = %c \r\n",p2m_buffer[1]);
	PRINTF("p2m_buffer[2] = %c \r\n",p2m_buffer[2]);
	PRINTF("p2m_buffer[3] = %c \r\n",p2m_buffer[3]);	
}

BOOL dma_p2m_interrupt_init(DMACTRL *dmaInfo, DMA_P2M_per per)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_P2M_BUSx_CLOCK(DMA_P2M_CLOCK, ENABLE);

	NVIC_ClearPendingIRQ(DMA_P2M_NVIC_IRQN);
	NVIC_SetPriority(DMA_P2M_NVIC_IRQN,DMA_P2M_NVIC_PRIO);
	NVIC_EnableIRQ(DMA_P2M_NVIC_IRQN);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//方向：从外设到内存
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_P2M_CHANNEL,&dma_initStruct);
    //清除DMA数据流传输完成标志位
    DMA_ClearITPendingBit(DMA_P2M_IT_TC);	
	//允许DMA传输完成中断产生
	DMA_ITConfig(DMA_P2M_CHANNEL,DMA_IT_TC,ENABLE);

	if(per == P2M_USART)
	{
		/*
		  外设向DMA发出RX请求,然后等待dmaInfo->MemoryBaseAddr数据接收完毕产生DMA接收完成中断
		  相当于RX_DR寄存器有数据，自动把数据放到缓存里。(不用CPU干预每次循环读RX_DR寄存器的数据)
		*/
		USART_DMACmd(DMA_M2P_RX,USART_DMAReq_Rx,ENABLE);
	}
	
	DMA_Cmd(DMA_P2M_CHANNEL,ENABLE);

	return TRUE;
}

void dma_p2m_interrupt_test(DMA_P2M_per per)
{
	DMACTRL dma_test;
	dma_test.PeripheralBaseAddr = (u32)DMA_M2P_RXDR_ADDR;
	dma_test.MemoryBaseAddr = (u32)p2m_buffer;
	dma_test.DIR = DMA_DIR_PeripheralSRC;
	dma_test.BufferSize = sizeof(p2m_buffer);
	dma_test.PeripheralInc = DMA_PeripheralInc_Disable;
	dma_test.MemoryInc = DMA_MemoryInc_Enable;
	dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma_test.MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma_test.Mode = DMA_Mode_Normal;
	dma_test.Priority = DMA_Priority_High;
	dma_test.M2M = DMA_M2M_Disable;
	dma_p2m_interrupt_init(&dma_test,per);
}

void DMA_P2M_IRQHandler_FUNC(void)
{
	if(DMA_GetITStatus(DMA_P2M_IT_TC)!=RESET)
	{
		DMA_ClearFlag(DMA_P2M_IT_TC);
		PRINTF("dma p2m interrupt success!\r\n");
		PRINTF("p2m_buffer[0] = %c \r\n",p2m_buffer[0]);
		PRINTF("p2m_buffer[1] = %c \r\n",p2m_buffer[1]);
		PRINTF("p2m_buffer[2] = %c \r\n",p2m_buffer[2]);
		PRINTF("p2m_buffer[3] = %c \r\n",p2m_buffer[3]);
	}
}



