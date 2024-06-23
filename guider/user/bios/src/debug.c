#include "common.h"


static char va_buf[0x100];

#define	debug_port_init(pBuf,pCtrl)		shell_port_init(pBuf,pCtrl)//usart_sendByte
#define	debug_send_char(ch)				shell_usart_sendByte(ch)//usart_sendByte
//#define	debug_send_string(str)			shell_usart_sendString(str)//usart_sendString
//#define	debug_get_char()				shell_usart_getByte()//usart_getByte
//#define 	debug_asyn_get_char(byte)		shell_usart_asyn_getByte(byte)//readByte,RX中断接收数据给到缓存，等待用户取


void debug_open(void)
{
	static u8 DBGTxBuf[0x1000];
	static u8 DBGRxBuf[0x1000];

	UARTINFO uInfo;
	BUFINFO pBuf;

	pBuf.pRxBuf = DBGRxBuf;
	pBuf.pTxBuf = DBGTxBuf;
	pBuf.RxSize = sizeof(DBGRxBuf);
	pBuf.TxSize = sizeof(DBGTxBuf);
	pBuf.RxNearEmptySize = 100;
	pBuf.RxNearFullSize = sizeof(DBGRxBuf) - 16;
    pBuf.TxNearEmptySize = 100;
    pBuf.TxNearFullSize = sizeof(DBGTxBuf) - 16;

	uInfo.Ctrl.Baud = 115200;
	uInfo.Ctrl.Datas = USART_WordLength_8b;
	uInfo.Ctrl.Stops = USART_StopBits_1;
	uInfo.Ctrl.Prty = USART_Parity_No;
	uInfo.Ctrl.Flw = USART_HardwareFlowControl_None;
	uInfo.Ctrl.Mode = USART_Mode_Rx | USART_Mode_Tx;;

	debug_port_init(&pBuf,&uInfo);

	PRINTF("111111111111111111111111111111111");


}

void debug_out(char *fmt,...)
{
    int	n,i;
    
    va_list ap;
    va_start(ap, fmt);
    n = vsprintf(va_buf,fmt,ap);
    va_end(ap);
    
    for(i=0; i<n; i++)
    {
    	debug_send_char(va_buf[i]);
    }
}









