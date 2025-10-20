/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#include "common.h"

#ifdef debug

static c8 va_buf[0x100];

extern BOOL debug_port_init(BUFINFO *pBuf,void *pCtrl);
extern u32 debug_get_data(u8 *d);
extern u32 debug_send_data(u8 d);

#define GET_CHAR(buf)   debug_get_data(buf)
#define SEND_CHAR(d)	while(!debug_send_data(d))
#define PORT_INIT(b,c)  debug_port_init(b,c)

u8 get_char(void)
{
    u8 d = 0;
    
    while(!GET_CHAR(&d))
    {
        HAL_Delay(5);//vTaskDelay(5);
    }

    return d;
}

void put_char(u8 d)
{
    SEND_CHAR(d);
}

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
	pBuf.RxNearFullSize = sizeof(DBGRxBuf) - 100;
    pBuf.TxNearEmptySize = 100;
    pBuf.TxNearFullSize = sizeof(DBGTxBuf) - 100;

	uInfo.Ctrl.Baud = BAUD115200;
	uInfo.Ctrl.Datas = UART_8DATAS;
	uInfo.Ctrl.Stops = UART_1STOP;
	uInfo.Ctrl.Prty = NO_PARITY;
	uInfo.Ctrl.Flw = UART_XON_XOFF;
	uInfo.Ctrl.Other = 0;
	
    PORT_INIT(&pBuf,&uInfo);

    PRINTF("111111111111111111111111111111111\r\n");

}

void debug_out(c8 *fmt,...)
{
    int	n,i;
    
    va_list ap;
    va_start(ap, fmt);
    n = vsprintf(va_buf,fmt,ap);
    va_end(ap);
    
    for(i=0; i<n; i++)
    {
    	put_char(va_buf[i]);
    }
}

#else

#endif

