#include "common.h"
#include "dma.h"

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

//缓冲到外设
BOOL dma_mem2peripheral_init(DMACTRL *dmaInfo)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_CLOCK(DMA_TX_CLOCK, ENABLE);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;//USART1_BASE+0x04;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;//(u32)dma_txbuf;
	//方向：从内存到外设
	dma_initStruct.DMA_DIR = dmaInfo->DIR;//DMA_DIR_PeripheralDST;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;//DMA_UART_TX_SIZE;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;//DMA_PeripheralInc_Disable;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;//DMA_MemoryInc_Enable;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;//DMA_PeripheralDataSize_Byte;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;//DMA_MemoryDataSize_Byte;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;//DMA_Mode_Normal;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;//DMA_Priority_High;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;//DMA_M2M_Disable;

	DMA_Init(DMAy_TX_CHANNELx,&dma_initStruct);

	DMA_Cmd(DMAy_TX_CHANNELx,ENABLE);

	/*
	  外设向DMA发出TX请求,然后把dmaInfo->MemoryBaseAddr数据的内容发出去。
	  相当于自动把缓冲数据给TX_DR寄存器写数据发送出去。(不用CPU干预每次循环写数据给TX_DR寄存器)
	*/	
	DMA_PERIPHERAL_REQ_FUN(DMA_PERIPHERAL_TXNUM,DMA_PERIPHERAL_TXREQ,ENABLE);

	return TRUE;
}


//外设到缓存
BOOL dma_peripheral2mem_init(DMACTRL *dmaInfo)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_CLOCK(DMA_RX_CLOCK, ENABLE);

	//DMA优先级配置
	NVIC_ClearPendingIRQ(DMA_NVIC_IRQN);
	NVIC_SetPriority(DMA_NVIC_IRQN,DMA_NVIC_PRIO);
	NVIC_EnableIRQ(DMA_NVIC_IRQN);

	//外设地址
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;//USART1_BASE+0x04;
	//内存地址
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;//(u32)dma_rxbuf;
	//方向：从外设到内存
	dma_initStruct.DMA_DIR = dmaInfo->DIR;//DMA_DIR_PeripheralSRC;
	//传输大小
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;//DMA_UART_RX_SIZE;
	//外设地址不增
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;//DMA_PeripheralInc_Disable;
	//内存地址自增
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;//DMA_MemoryInc_Enable;
	//外设数据宽度
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;//DMA_PeripheralDataSize_Byte;
	//内存数据宽度
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;//DMA_MemoryDataSize_Word;
	//DMA模式，一次或循环模式
	dma_initStruct.DMA_Mode = dmaInfo->Mode;//DMA_Mode_Normal;
	//优先级：高
	dma_initStruct.DMA_Priority = dmaInfo->Priority;//DMA_Priority_High;
	//禁止内存到内存的传输
	dma_initStruct.DMA_M2M = dmaInfo->M2M;//DMA_M2M_Disable;

	DMA_Init(DMAy_RX_CHANNELx,&dma_initStruct);

	//使能DMA接收完成中断
	DMA_ITConfig(DMAy_RX_CHANNELx,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(DMAy_RX_CHANNELx,ENABLE);

	/*
	  外设向DMA发出RX请求,然后等待dmaInfo->MemoryBaseAddr数据接收完毕产生DMA接收完成中断
	  相当于RX_DR寄存器有数据，自动把数据放到缓存里。(不用CPU干预每次循环读RX_DR寄存器的数据)
	*/
	USART_DMACmd(DMA_PERIPHERAL_RXNUM,DMA_PERIPHERAL_RXREQ,ENABLE);

	return TRUE;
}


//内存到内存
BOOL dma_m2m_init(DMACTRL *dmaInfo)
{
	/*
	  内存到内存的DMA传输不需要外设向DMA发出请求，初始化完后就会马上传输了。
	  如果没有使能中断，则等待循环while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET);退出即可知道DMA_M2M传输完成。
	  传输完成状态标志位和中断不是共同作用的，前者也可以单独产生标志位置1的。
	*/

	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_M2M_CLOCK(DMA_M2M_CLOCK, ENABLE);

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

	DMA_Init(DMAy_M2M_CHANNELx,&dma_initStruct);

    //清除DMA数据流传输完成标志位
    DMA_ClearFlag(DMA_FLAG_TC);

	DMA_Cmd(DMAy_M2M_CHANNELx,ENABLE);

	return TRUE;
}


//串口RX_DMA中断函数
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)!=RESET)
	{
		DMA_ClearFlag(DMA1_IT_TC5);
		
	}
}

