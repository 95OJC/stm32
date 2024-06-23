#ifndef __PUBDEF_H
#define __PUBDEF_H

typedef struct _BUFINFO
{
	u8 *pRxBuf;//接收缓冲
	u8 *pTxBuf;//发送缓冲
	u32 RxSize;//接收缓冲大小
	u32 TxSize;//发送缓冲大小
	u32 RxNearFullSize;//接收缓冲满的条件
	u32 RxNearEmptySize;//接收缓冲空的条件
	u32 TxNearFullSize;//发送缓冲满的条件
	u32 TxNearEmptySize;//发送缓冲空的条件

}BUFINFO;	

typedef struct _UARTCTRL
{
	u32 Baud;
	u8 Datas;
	u8 Stops;
	u8 Prty;
	u8 Flw;
	u8 Mode;
}UARTCTRL;	//uart端口配置

typedef struct _UARTINFO
{
	UARTCTRL Ctrl;
	u32 State;	//当前uart端口状态
	
}UARTINFO;

typedef enum{
	UART_PORT = 0,
	NULL_PORT = 0xFF,
}PORT_NUM;

typedef enum{
	IOCTRL_RX_DATA_SIZE = 0,
	IOCTRL_TX_DATA_SIZE,
	IOCTRL_RESET_RX_SIZE,
	IOCTRL_RESET_TX_SIZE,
	IOCTRL_RESET_RT_SIZE,
}IOCTRL_TYPE;

#endif	//__PUBDEF_H


