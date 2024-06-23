#ifndef __dma_h
#define	__dma_h

/*usart dma*/
#define	DMA_NVIC_IRQN					DMA1_Channel5_IRQn
#define	DMA_NVIC_PRIO					0x02

#define	DMA_AHB_CLOCK(x,s)				RCC_AHBPeriphClockCmd(x,s)	//dma时钟fun
#define	DMA_PERIPHERAL_REQ_FUN(p,q,s)	USART_DMACmd(p,q,s)			//dma外设请求fun

#define	DMA_TX_CLOCK					RCC_AHBPeriph_DMA1			//dma时钟
#define	DMAy_TX_CHANNELx				DMA1_Channel4				//dma外设通道
#define	DMA_PERIPHERAL_TXNUM			USART1						//dma外设
#define	DMA_PERIPHERAL_TXREQ			USART_DMAReq_Tx				//dma外设请求

#define	DMA_RX_CLOCK					RCC_AHBPeriph_DMA1			//dma时钟
#define	DMAy_RX_CHANNELx				DMA1_Channel5				//dma外设通道
#define	DMA_PERIPHERAL_RXNUM			USART1						//dma外设
#define	DMA_PERIPHERAL_RXREQ			USART_DMAReq_Rx				//dma外设请求	


#endif//__dma_h

