#include "common.h"


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
	  �ڴ浽�ڴ��DMA���䲻��Ҫ������DMA�������󣬳�ʼ�����ͻ����ϴ����ˡ�
	  ���û��ʹ���жϣ���ȴ�ѭ��while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET);�˳�����֪��DMA_M2M������ɡ�
	  ��������ж�״̬��־λ���жϲ��ǹ�ͬ���õģ�ǰ��Ҳ���Ե���������־λ��1�ġ�
	*/

	DMA_InitTypeDef dma_initStruct;
	MEM_SET(&dma_initStruct,0,sizeof(DMA_InitTypeDef));

	DMA_M2M_BUSx_CLOCK(DMA_M2M_CLOCK, ENABLE);

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
	//����DMAͨ��
	DMA_Init(DMA_M2M_CHANNEL,&dma_initStruct);
    //���DMA������������ɱ�־λ
    DMA_ClearFlag(DMA_M2M_FLAG_TC);
	//ʹ��DMA����ʼ����
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

	//�ȴ�dma��������ж�״̬��־λ��1
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
	//����DMAͨ��
	DMA_Init(DMA_M2M_CHANNEL,&dma_initStruct);
    //���DMA������������ɱ�־λ
    DMA_ClearITPendingBit(DMA_M2M_IT_TC);	
	//����DMA��������жϲ���
	DMA_ITConfig(DMA_M2M_CHANNEL,DMA_IT_TC,ENABLE);	
	//ʹ��DMA����ʼ����
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

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//���򣺴��ڴ浽����
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
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_M2P_CHANNEL,&dma_initStruct);

	if(per == M2P_USART)
	{
		/*
		  ������DMA����TX����,Ȼ���dmaInfo->MemoryBaseAddr���ݵ����ݷ���ȥ��
		  �൱���Զ��ѻ������ݸ�TX_DR�Ĵ���д���ݷ��ͳ�ȥ��(����CPU��Ԥÿ��ѭ��д���ݸ�TX_DR�Ĵ���)
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

	//�ȴ�dma��������ж�״̬��־λ��1
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

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//���򣺴��ڴ浽����
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
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_M2P_CHANNEL,&dma_initStruct);
    //���DMA������������ɱ�־λ
    DMA_ClearITPendingBit(DMA_M2P_IT_TC);	
	//����DMA��������жϲ���
	DMA_ITConfig(DMA_M2P_CHANNEL,DMA_IT_TC,ENABLE);	
	
	if(per == M2P_USART)
	{
		/*
		  ������DMA����TX����,Ȼ���dmaInfo->MemoryBaseAddr���ݵ����ݷ���ȥ��
		  �൱���Զ��ѻ������ݸ�TX_DR�Ĵ���д���ݷ��ͳ�ȥ��(����CPU��Ԥÿ��ѭ��д���ݸ�TX_DR�Ĵ���)
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

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//���򣺴����赽�ڴ�
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
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_P2M_CHANNEL,&dma_initStruct);

	if(per == P2M_USART)
	{
		/*
		  ������DMA����RX����,Ȼ��ȴ�dmaInfo->MemoryBaseAddr���ݽ�����ϲ���DMA��������ж�
		  �൱��RX_DR�Ĵ��������ݣ��Զ������ݷŵ������(����CPU��Ԥÿ��ѭ����RX_DR�Ĵ���������)
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

	//�ȴ�dma��������ж�״̬��־λ��1
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

	//�����ַ
	dma_initStruct.DMA_PeripheralBaseAddr = dmaInfo->PeripheralBaseAddr;
	//�ڴ��ַ
	dma_initStruct.DMA_MemoryBaseAddr = dmaInfo->MemoryBaseAddr;
	//���򣺴����赽�ڴ�
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
	//��ֹ�ڴ浽�ڴ�Ĵ���
	dma_initStruct.DMA_M2M = dmaInfo->M2M;
	DMA_Init(DMA_P2M_CHANNEL,&dma_initStruct);
    //���DMA������������ɱ�־λ
    DMA_ClearITPendingBit(DMA_P2M_IT_TC);	
	//����DMA��������жϲ���
	DMA_ITConfig(DMA_P2M_CHANNEL,DMA_IT_TC,ENABLE);

	if(per == P2M_USART)
	{
		/*
		  ������DMA����RX����,Ȼ��ȴ�dmaInfo->MemoryBaseAddr���ݽ�����ϲ���DMA��������ж�
		  �൱��RX_DR�Ĵ��������ݣ��Զ������ݷŵ������(����CPU��Ԥÿ��ѭ����RX_DR�Ĵ���������)
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



