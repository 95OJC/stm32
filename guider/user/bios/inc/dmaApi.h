#ifndef __dmaApi_h
#define	__dmaApi_h

/*m2m dma*/
#define	DMA_AHB_M2M_CLOCK(x,s)			RCC_AHBPeriphClockCmd(x,s)			//dmaʱ��fun
#define	DMA_M2M_CLOCK					RCC_AHBPeriph_DMA1					//dmaʱ��
#define	DMAy_M2M_CHANNELx				DMA1_Channel7//DMA1_Channel6		//dmaͨ��
#define DMA_FLAG_TC     				DMA1_FLAG_TC7//DMA1_FLAG_TC6		//������ɱ�־

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


BOOL dma_mem2peripheral_init(DMACTRL *dmaInfo);
BOOL dma_peripheral2mem_init(DMACTRL *dmaInfo);
BOOL dma_m2m_init(DMACTRL *dmaInfo);




#endif//__dmaApi_h

