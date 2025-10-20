#ifndef __PUBDEF_H
#define	__PUBDEF_H

#include "typedef.h"

typedef struct _BUFINFO{
	u8 *pRxBuf;//���ջ���
	u8 *pTxBuf;//���ͻ���
	u32 RxSize;//���ջ����С
	u32 TxSize;//���ͻ����С
	u32 RxNearFullSize; //���ջ�����������
	u32 RxNearEmptySize;//���ջ���յ�����
	u32 TxNearFullSize; //���ͻ�����������
	u32 TxNearEmptySize;//���ͻ���յ�����
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
}UARTINFO;//��¼�����ʵȲ����ͼ������ӵȹ̶�״̬

/*����ctrl*/
//������
#define BAUD1200            1200U
#define BAUD2400            2400U
#define BAUD4800            4800U
#define BAUD9600            9600U
#define BAUD19200           19200U
#define BAUD38400           38400U
#define BAUD57600           57600U
#define BAUD115200          115200U
//����λ
#define UART_8DATAS         UART_WORDLENGTH_8B
#define UART_7DATAS         UART_WORDLENGTH_7B
//ֹͣλ
#define UART_1STOP		    UART_STOPBITS_1
#define UART_2STOP		    UART_STOPBITS_2
//��żУ��
#define NO_PARITY		    UART_PARITY_NONE
#define ODD_PARITY		    UART_PARITY_ODD
#define EVEN_PARITY		    UART_PARITY_EVEN
//������
#define UART_FLOW_NONE	    0
#define UART_XON_XOFF	    1
#define UART_RTS_CTS	    2
#define UART_DTR_DSR	    3


/* �˿�stat */
#define	IF_DEV_STAT_BIT			0x000000FF
#define	IF_PORT_ERR				0x00000001
//�˿�ȫ��״̬
#define	IF_GLOBAL_BIT			0x0000FF00
#define	IF_ACT					0x00000100
#define	IF_CONNECT				0x00000200
//���ն˿�״̬
#define	IF_RX_STAT_BIT			0x00FF0000
#define	IF_RX_BUSY				0x00010000
#define	IF_RX_EMPTY				0x00020000
#define	IF_RX_FORCE_BUSY		0x00040000//ǿ��æ
//���Ͷ˿�״̬
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

