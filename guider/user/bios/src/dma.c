#include "common.h"
#include "dma.h"

/*-----------------------------------------------------------------
��֤dma���ܡ�

��������£�
1����Ҫ��ʼ��uart��io�ں����蹦��(���ô��ж�)������dma��

2��TX����ʼ�����ں�DMA��ִ��USART_DMACmd()����ͨ����DMA���뷢�����ݣ�Ȼ�󻺳���׵�ַ(addr++)���Զ�ͨ��DMA����
����DR�Ĵ���������һ���ֽں󣬲���Ҫ�ȴ����ڵ�TXEλ��TCλ����ΪCPU�ڲ���DMA�ʹ����Ѿ�����øù�ϵ�����ǲ���Ҫ��
ע�����������Բ���DMA��������жϱ�־��

3��RX:��ʼ�����ں�DMA��ִ��USART_DMACmd()����ͨ����DMA������մ���DR�Ĵ��������ݣ����ȶ���ý��ն��ٳ��ȵ���
�ݣ�Ȼ��DMA���Զ����������׵�ַ(addr++),��������Բ���DMA��������жϱ�־��

4��usart��iic��spi���������ݼĴ���DR��������DMA���䣬��ΪDMA��Ҫ���ݼĴ�����ַ���ɴ��䡣

5����ʹ�ô洢�����洢��ģʽʱ��ͨ���������ѡ��û��Ӳ�ԵĹ涨��

�ܽ�1��DMA����֧�ֵ�����(����ϸ���ĸ�DMAͨ��)����Ҫ���������ݼĴ���(DR)���Ϳ�����DMA��������,ֻ��Ҫ��ע�������ݺ�����Ĵ������д����Ⱥʹ�С�ȵȼ��ɡ�
�ܽ�2���ڴ浽�ڴ��DMA���䣬ֻ��Ҫ֪��2���ڴ���׵�ַ�ʹ�С���ɡ�
------------------------------------------------------------------*/

//���嵽����
BOOL dma_mem2peripheral_init(DMACTRL *dmaInfo)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_CLOCK(DMA_TX_CLOCK, ENABLE);

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;//USART1_BASE+0x04;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;//(u32)dma_txbuf;
	//���򣺴��ڴ浽����
	dma_initStruct.DMA_DIR = dmaInfo->DIR;//DMA_DIR_PeripheralDST;
	//�����С
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;//DMA_UART_TX_SIZE;
	//�����ַ����
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;//DMA_PeripheralInc_Disable;
	//�ڴ��ַ����
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;//DMA_MemoryInc_Enable;
	//�������ݿ��
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;//DMA_PeripheralDataSize_Byte;
	//�ڴ����ݿ��
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;//DMA_MemoryDataSize_Byte;
	//DMAģʽ��һ�λ�ѭ��ģʽ
	dma_initStruct.DMA_Mode = dmaInfo->Mode;//DMA_Mode_Normal;
	//���ȼ�����
	dma_initStruct.DMA_Priority = dmaInfo->Priority;//DMA_Priority_High;
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;//DMA_M2M_Disable;

	DMA_Init(DMAy_TX_CHANNELx,&dma_initStruct);

	DMA_Cmd(DMAy_TX_CHANNELx,ENABLE);

	/*
	  ������DMA����TX����,Ȼ���dmaInfo->MemoryBaseAddr���ݵ����ݷ���ȥ��
	  �൱���Զ��ѻ������ݸ�TX_DR�Ĵ���д���ݷ��ͳ�ȥ��(����CPU��Ԥÿ��ѭ��д���ݸ�TX_DR�Ĵ���)
	*/	
	DMA_PERIPHERAL_REQ_FUN(DMA_PERIPHERAL_TXNUM,DMA_PERIPHERAL_TXREQ,ENABLE);

	return TRUE;
}


//���赽����
BOOL dma_peripheral2mem_init(DMACTRL *dmaInfo)
{
	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_CLOCK(DMA_RX_CLOCK, ENABLE);

	//DMA���ȼ�����
	NVIC_ClearPendingIRQ(DMA_NVIC_IRQN);
	NVIC_SetPriority(DMA_NVIC_IRQN,DMA_NVIC_PRIO);
	NVIC_EnableIRQ(DMA_NVIC_IRQN);

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;//USART1_BASE+0x04;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;//(u32)dma_rxbuf;
	//���򣺴����赽�ڴ�
	dma_initStruct.DMA_DIR = dmaInfo->DIR;//DMA_DIR_PeripheralSRC;
	//�����С
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;//DMA_UART_RX_SIZE;
	//�����ַ����
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;//DMA_PeripheralInc_Disable;
	//�ڴ��ַ����
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;//DMA_MemoryInc_Enable;
	//�������ݿ��
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;//DMA_PeripheralDataSize_Byte;
	//�ڴ����ݿ��
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;//DMA_MemoryDataSize_Word;
	//DMAģʽ��һ�λ�ѭ��ģʽ
	dma_initStruct.DMA_Mode = dmaInfo->Mode;//DMA_Mode_Normal;
	//���ȼ�����
	dma_initStruct.DMA_Priority = dmaInfo->Priority;//DMA_Priority_High;
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;//DMA_M2M_Disable;

	DMA_Init(DMAy_RX_CHANNELx,&dma_initStruct);

	//ʹ��DMA��������ж�
	DMA_ITConfig(DMAy_RX_CHANNELx,DMA_IT_TC,ENABLE);
	
	DMA_Cmd(DMAy_RX_CHANNELx,ENABLE);

	/*
	  ������DMA����RX����,Ȼ��ȴ�dmaInfo->MemoryBaseAddr���ݽ�����ϲ���DMA��������ж�
	  �൱��RX_DR�Ĵ��������ݣ��Զ������ݷŵ������(����CPU��Ԥÿ��ѭ����RX_DR�Ĵ���������)
	*/
	USART_DMACmd(DMA_PERIPHERAL_RXNUM,DMA_PERIPHERAL_RXREQ,ENABLE);

	return TRUE;
}


//�ڴ浽�ڴ�
BOOL dma_m2m_init(DMACTRL *dmaInfo)
{
	/*
	  �ڴ浽�ڴ��DMA���䲻��Ҫ������DMA�������󣬳�ʼ�����ͻ����ϴ����ˡ�
	  ���û��ʹ���жϣ���ȴ�ѭ��while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET);�˳�����֪��DMA_M2M������ɡ�
	  �������״̬��־λ���жϲ��ǹ�ͬ���õģ�ǰ��Ҳ���Ե���������־λ��1�ġ�
	*/

	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_AHB_M2M_CLOCK(DMA_M2M_CLOCK, ENABLE);

	//Դ��ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//Ŀ���ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//���򣺴����赽�ڴ�(��������������falsh��sram)
	dma_initStruct.DMA_DIR = dmaInfo->DIR;
	//�����С
	dma_initStruct.DMA_BufferSize = dmaInfo->BufferSize;
	//�����ַ����
	dma_initStruct.DMA_PeripheralInc = dmaInfo->PeripheralInc;
	//�ڴ��ַ����
	dma_initStruct.DMA_MemoryInc = dmaInfo->MemoryInc;
	//�������ݿ��
	dma_initStruct.DMA_PeripheralDataSize = dmaInfo->PeripheralDataSize;
	//�ڴ����ݿ��
	dma_initStruct.DMA_MemoryDataSize = dmaInfo->MemoryDataSize;
	//DMAģʽ��һ�λ�ѭ��ģʽ
	dma_initStruct.DMA_Mode = dmaInfo->Mode;
	//���ȼ�����
	dma_initStruct.DMA_Priority = dmaInfo->Priority;
	//ʹ���ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;

	DMA_Init(DMAy_M2M_CHANNELx,&dma_initStruct);

    //���DMA������������ɱ�־λ
    DMA_ClearFlag(DMA_FLAG_TC);

	DMA_Cmd(DMAy_M2M_CHANNELx,ENABLE);

	return TRUE;
}


//����RX_DMA�жϺ���
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5)!=RESET)
	{
		DMA_ClearFlag(DMA1_IT_TC5);
		
	}
}

