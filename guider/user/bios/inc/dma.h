#ifndef __dma_h
#define	__dma_h

typedef struct _DMACTRL
{
	u32 PeripheralBaseAddr;
	u32 MemoryBaseAddr;
	u32 DIR; 
	u32 BufferSize;
	u32 PeripheralInc;
	u32 MemoryInc;
	u32 PeripheralDataSize;
	u32 MemoryDataSize;
	u32 Mode;
	u32 Priority;
	u32 M2M;
}DMACTRL;


typedef enum 
{
	M2P_USART 	= 0,
	M2P_TIM		= 1,	
	M2P_ADC		= 2,
	M2P_DAC		= 3,
	M2P_IIC		= 4,
	M2P_SPI		= 5,

}DMA_M2P_per;

typedef enum 
{
	P2M_USART 	= 0,
	P2M_TIM		= 1,	
	P2M_ADC		= 2,
	P2M_DAC		= 3,
	P2M_IIC		= 4,
	P2M_SPI		= 5,

}DMA_P2M_per;

BOOL dma_m2m_init(DMACTRL *dmaInfo);
void dma_m2m_test(void);

BOOL dma_m2m_interrupt_init(DMACTRL *dmaInfo);
void dma_m2m_interrupt_test(void);

BOOL dma_m2p_init(DMACTRL *dmaInfo, DMA_M2P_per per);
void dma_m2p_test(DMA_M2P_per per);

BOOL dma_m2p_interrupt_init(DMACTRL *dmaInfo, DMA_M2P_per per);
void dma_m2p_interrupt_test(DMA_M2P_per per);

BOOL dma_p2m_init(DMACTRL *dmaInfo, DMA_P2M_per per);
void dma_p2m_test(DMA_P2M_per per);

BOOL dma_p2m_interrupt_init(DMACTRL *dmaInfo, DMA_P2M_per per);
void dma_p2m_interrupt_test(DMA_P2M_per per);


#endif//__dma_h

