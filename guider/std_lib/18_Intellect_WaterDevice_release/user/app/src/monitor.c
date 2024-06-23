#include "main.h"

static void readUsartSet(UARTINFO* usartInfo) 
{
	usartInfo->Ctrl.Baud = 115200;
	usartInfo->Ctrl.Datas = USART_WordLength_8b;
	usartInfo->Ctrl.Stops = USART_StopBits_1;
	usartInfo->Ctrl.Prty = USART_Parity_No;
	usartInfo->Ctrl.Flw = USART_HardwareFlowControl_None;
	usartInfo->Ctrl.Mode = USART_Mode_Rx | USART_Mode_Tx;
}


void initRecvPort(void)
{
	BUFINFO stdBufInf;
	UARTINFO usartInfo;

	MEM_SET(&stdBufInf,0,sizeof(stdBufInf));
	MEM_SET(&usartInfo,0,sizeof(usartInfo));

	stdBufInf.pRxBuf = gSysPool.sysExtRxBuf;
	stdBufInf.pTxBuf = gSysPool.sysExtTxBuf;
	stdBufInf.RxSize = SYS_RXBUF_LEN;
	stdBufInf.TxSize = SYS_TXBUF_LEN;
	stdBufInf.RxNearEmptySize = RX_NEAR_EMPTY_SIZE;
	stdBufInf.RxNearFullSize =  RX_NEAR_FULL_SIZE;
	stdBufInf.TxNearEmptySize = TX_NEAR_EMPTY_SIZE;
	stdBufInf.TxNearFullSize = TX_NEAR_FULL_SIZE;

	readUsartSet(&usartInfo);
	INIT_RECV_PORT(UART_PORT,&stdBufInf,&usartInfo);
	SET_RECV_PORT(UART_PORT);


}


void monitorTask(void)
{
	initRecvPort();

#if 0
	u8 data;

	while(1)
	{
		if(GET_DATA(&data,1))
		{
			SEND_DATA(&data,1);
		}
	}
#endif
}




