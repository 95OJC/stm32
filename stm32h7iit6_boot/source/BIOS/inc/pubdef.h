#ifndef __PUBDEF_H
#define	__PUBDEF_H

#include "typedef.h"

typedef struct _BUFINFO{
	u8 *pRxBuf;//接收缓冲
	u8 *pTxBuf;//发送缓冲
	u32 RxSize;//接收缓冲大小
	u32 TxSize;//发送缓冲大小
	u32 RxNearFullSize; //接收缓冲满的条件
	u32 RxNearEmptySize;//接收缓冲空的条件
	u32 TxNearFullSize; //发送缓冲满的条件
	u32 TxNearEmptySize;//发送缓冲空的条件
}BUFINFO;

typedef struct UARTCTRL{
    u32 Other;
    int Baud;
    int Datas;
    int Stops;
    int Prty;
    int Flw;
}UARTCTRL;

typedef struct _UARTINFO{
    UARTCTRL Ctrl;
    u32 Stat;
}UARTINFO;//记录波特率等参数和激活连接等固定状态

/*串口ctrl*/
//波特率
#define BAUD1200            1200U
#define BAUD2400            2400U
#define BAUD4800            4800U
#define BAUD9600            9600U
#define BAUD19200           19200U
#define BAUD38400           38400U
#define BAUD57600           57600U
#define BAUD115200          115200U
//数据位
#define UART_8DATAS         UART_WORDLENGTH_8B
#define UART_7DATAS         UART_WORDLENGTH_7B
//停止位
#define UART_1STOP		    UART_STOPBITS_1
#define UART_2STOP		    UART_STOPBITS_2
//奇偶校验
#define NO_PARITY		    UART_PARITY_NONE
#define ODD_PARITY		    UART_PARITY_ODD
#define EVEN_PARITY		    UART_PARITY_EVEN
//流控制
#define UART_FLOW_NONE	    0
#define UART_XON_XOFF	    1
#define UART_RTS_CTS	    2
#define UART_DTR_DSR	    3


/* 端口stat */
#define	IF_DEV_STAT_BIT			0x000000FF
#define	IF_PORT_ERR				0x00000001
//端口全局状态
#define	IF_GLOBAL_BIT			0x0000FF00
#define	IF_ACT					0x00000100
#define	IF_CONNECT				0x00000200
//接收端口状态
#define	IF_RX_STAT_BIT			0x00FF0000
#define	IF_RX_BUSY				0x00010000
#define	IF_RX_EMPTY				0x00020000
#define	IF_RX_FORCE_BUSY		0x00040000//强制忙
//发送端口状态
#define	IF_TX_STAT_BIT			0xFF000000
#define	IF_TX_BUSY				0x01000000
#define	IF_TX_EMPTY				0x02000000


#define	IF_SET_ACT(m)			{(m) |= IF_ACT;}
#define	IF_CLS_ACT(m)			{(m) &= ~IF_ACT;}
#define	IF_GET_ACT(m)			((m) & IF_ACT)

#define IF_SET_CONNECT(m)   	{(m) |= IF_CONNECT;}
#define IF_CLS_CONNECT(m)   	{(m) &= ~IF_CONNECT;}
#define IF_GET_CONNECT(m)   	((m) & IF_CONNECT)

#define	IF_SET_RX_BUSY(m)		{(m) |= IF_RX_BUSY;}
#define	IF_CLS_RX_BUSY(m)		{(m) &= ~IF_RX_BUSY;}
#define	IF_GET_RX_BUSY(m)		((m) & IF_RX_BUSY)
#define IF_SET_RX_EMPTY(m)  	{(m) |= IF_RX_EMPTY;}
#define IF_CLS_RX_EMPTY(m)  	{(m) &= ~IF_RX_EMPTY;}
#define IF_GET_RX_EMPTY(m)  	((m) & IF_RX_EMPTY)

#define IF_SET_TX_BUSY(m)   {(m) |= IF_TX_BUSY;}
#define IF_CLS_TX_BUSY(m)   {(m) &= ~IF_TX_BUSY;}
#define IF_GET_TX_BUSY(m)   ((m) & IF_TX_BUSY)
#define IF_SET_TX_EMPTY(m)  {(m) |= IF_TX_EMPTY;}
#define IF_CLS_TX_EMPTY(m)  {(m) &= ~IF_TX_EMPTY;}
#define IF_GET_TX_EMPTY(m)  ((m) & IF_TX_EMPTY)



#endif//__PUBDEF_H

