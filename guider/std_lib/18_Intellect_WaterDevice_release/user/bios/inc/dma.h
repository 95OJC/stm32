#ifndef __dma_h
#define	__dma_h

/*usart dma*/
#define	DMA_NVIC_IRQN					DMA1_Channel5_IRQn
#define	DMA_NVIC_PRIO					0x02

#define	DMA_AHB_CLOCK(x,s)				RCC_AHBPeriphClockCmd(x,s)	//dmaʱ��fun
#define	DMA_PERIPHERAL_REQ_FUN(p,q,s)	USART_DMACmd(p,q,s)			//dma��������fun

#define	DMA_TX_CLOCK					RCC_AHBPeriph_DMA1			//dmaʱ��
#define	DMAy_TX_CHANNELx				DMA1_Channel4				//dma����ͨ��
#define	DMA_PERIPHERAL_TXNUM			USART1						//dma����
#define	DMA_PERIPHERAL_TXREQ			USART_DMAReq_Tx				//dma��������

#define	DMA_RX_CLOCK					RCC_AHBPeriph_DMA1			//dmaʱ��
#define	DMAy_RX_CHANNELx				DMA1_Channel5				//dma����ͨ��
#define	DMA_PERIPHERAL_RXNUM			USART1						//dma����
#define	DMA_PERIPHERAL_RXREQ			USART_DMAReq_Rx				//dma��������	


#endif//__dma_h

