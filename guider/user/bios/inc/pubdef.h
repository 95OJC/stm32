#ifndef __PUBDEF_H
#define __PUBDEF_H

typedef struct _BUFINFO
{
	u8 *pRxBuf;//���ջ���
	u8 *pTxBuf;//���ͻ���
	u32 RxSize;//���ջ����С
	u32 TxSize;//���ͻ����С
	u32 RxNearFullSize;//���ջ�����������
	u32 RxNearEmptySize;//���ջ���յ�����
	u32 TxNearFullSize;//���ͻ�����������
	u32 TxNearEmptySize;//���ͻ���յ�����

}BUFINFO;	

typedef struct _UARTCTRL
{
	u32 Baud;
	u8 Datas;
	u8 Stops;
	u8 Prty;
	u8 Flw;
	u8 Mode;
}UARTCTRL;	//uart�˿�����

typedef struct _UARTINFO
{
	UARTCTRL Ctrl;
	u32 State;	//��ǰuart�˿�״̬
	
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


