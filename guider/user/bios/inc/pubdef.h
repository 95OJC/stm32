#ifndef __PUBDEF_H
#define __PUBDEF_H

/*�������ȼ���һ�����ϲ�ʹ�ã�*/
#define NORMAL_PRIOR	0x00	//prnQBuf[MAX_PRNITEM]���ȼ�
#define PANEL_PRIOR		0x01	//pnlItem���ȼ�
//#define PRIOR_RPLE		0x80	//ǿ���滻�������ȼ����滻��ʽ�ύ�������滻��ǰ������ȼ���item��ײ������ˣ���ʱ��ʹ�ã�����Ҫ����һ��ʵ�ʹ��ܣ�


//�����������
/***********prnQ.qCtrlReq*************/
typedef enum
{
	QREQ_NONE		= 	0,
	QREQ_PAUSE		=	1,
	QREQ_RESUME		=	2,
	QREQ_CLS		=	3,
	QREQ_PNL_CLS	=	4
}PRNQ_REQ_STAT;


//��������״̬
/************prnQ.qStat***************/
typedef enum
{
	QSTAT_ACT	=	0,	//���й���
	QSTAT_PAUSE	=	1	//������ͣ
}PRNQ_STAT;


/*********QITEM.type***********/
typedef enum
{
	NO_MSG,				/*< NO MSG */
	INIT_MSG,			/*< ��ʼ�� */

}MSG_TYPE;


/*********QITEM.MSG_INFO***********/
//��ӦQITEM.type�ж��ٸ����Ͷ���/ģ�飬��Ҫ�ж��ٸ�����/ģ���MSG_INFO�ṹ��
//Ϊ���ϲ㴫��Ϣ���ײ����Щ����/ģ�飬Ȼ��ײ�ͨ����Щ��Ϣ��������/ģ�鹤��
//��Щ����/ģ���MSG_INFO�ṹ�壬Ŀǰ���涼ֻ��ctrl(��������QITEM.itemTag������item��Ķ���/ģ��)�����������һЩ��������ָ���
typedef struct
{
	u32 	ctrl;
}INIT_INFO;


typedef union
{
	INIT_INFO	init;

}MSG_INFO;


/*********QITEM.itemTag***********/
/*ע��ֻ�и�16BIT���õ�16bit����*/
//�ײ�����ϲ��ύ��itemTag���ԣ��ɸ�Ԥitem��Ķ���/ģ�顣
#define ITEM_TAG_MASK   0xFFFF0000
#define ITEM_NORMAL     0x00000000/*< ����    */
#define ITEM_IGNERR     0x00010000/*< ���Դ��󣬺��Դ��󼤻�item�� */
#define ITEM_HOLD       0x00020000/*< ���������˺ֽʱ���������Ĭ������60S��*/


/***********QITEM.stat ****************/
//�ײ������������/ģ���PRNQ��������Ӧ�����н�������ϲ�
typedef enum
{
    ITEM_READY      = 0,    /*< ITEM ������л�û�м���         */
    ITEM_ACTING     = 1,    /*< ITEM �������й�����        */
    /*����ȫ��״̬������ITEM�ɾ�� */
    ITEM_CPLT       = 0x82, /*< ITEM �������     */
    ITEM_CANCEL     = 0x83, /*< �û�ȡ��ITEM��      */
}ITEM_STAT;


/*********** һ��item��Ľṹ�� ****************/
typedef struct _QITEM
{
	MSG_TYPE type;				/*< 1����Ϣ������ */
	u32 padding;				/* ռλ����֤����4�ֽڶ��� */
	u8 buf[sizeof(MSG_INFO)];	/* 2����Ϣ�Ĳ������ο�QITEM.MSG_INFO ˵��*/
	CallBackFunc s_func;		/*< ITEM�����ˣ���ʼ�ص��������ڻص��п����ٴ��޸�QITEM��������������Ϊ *QITEM */
	CallBackFunc e_func;		/*< ITEM���������ˣ������ص��������ڻص�����������ȡ���ϲ�ö���/ģ���æ��־����������Ϊ *QITEM */
    CallBackFunc hook_func; 	/*< ITEM��û����ɹ���hook�ص����������ȴ�����������,ֱ��ITEM��������У�����һֱ��⴫����֮��� */
	u16	dlyTime;				/*< ����item����ӳ����е�λms */
	u16	dlyTimeReload;			/*< �ӳ���װ */
	u32	itemTag;				/*< item�����������(һ�����ϲ�����): �ο�QITEM.itemTag˵�� */
	u8 pt;            			//����
	ITEM_STAT stat;         	/*< ��PRNQ�������,ITEM������н�����ڴˣ��ο�QITEM.stat˵�� */
}QITEM;


//��ȡitem�������
#define	GET_QITEM_TYPE(m)		((m)->type)

//��ȡitem���itemTag
#define	GET_QITEM_TAG(m)		((m)->itemTag)


//���Դ��󼤻�tiem��
#define SET_QITEM_IGNERR(m)     {(m)->itemTag |= ITEM_IGNERR;}
#define CLS_QITEM_IGNERR(m)     {(m)->itemTag &= ~ITEM_IGNERR;}
#define IS_QITEM_IGNERR(m)      ((m)->itemTag & ITEM_IGNERR)


//�������
#define SET_QITEM_HOLD(m)       {(m)->itemTag |= ITEM_HOLD;}
#define CLS_QITEM_HOLDE(m)      {(m)->itemTag &= ~ITEM_HOLD;}
#define IS_QITEM_HOLD(m)        ((m)->itemTag & ITEM_HOLD)






/*����*/
#define TX_BUF      0
#define RX_BUF      1
#define RT_BUF      2

//�˿�ȫ��״̬(������ȫ��)
#define IF_GLB_BIT              0x0000FF00
#define IF_ACT                  0x00000100//�˿ڴ�
#define IF_CONNECT              0x00000200//�˿�����

//�˿ڽ���״̬(�����Ǿ�̬ȫ��)
#define IF_RX_STAT_BIT          0x00FF0000
#define IF_RX_BUSY		        0x00010000
#define IF_RX_EMPTY             0x00020000

//�˿ڷ���״̬(�����Ǿ�̬ȫ��)
#define IF_TX_STAT_BIT          0xFF000000
#define IF_TX_BUSY		        0x01000000
#define IF_TX_EMPTY             0x02000000

//�˿�ȫ�ּ���
#define IF_SET_ACT(m) 		{(m) |= IF_ACT;}
#define IF_CLS_ACT(m) 		{(m) &= ~IF_ACT;}
#define IF_GET_ACT(m) 		(m & IF_ACT)

//�˿�ȫ������
#define IF_SET_CONNECT(m)   {(m) |= IF_CONNECT;}
#define IF_CLS_CONNECT(m)   {(m) &= ~IF_CONNECT;}
#define IF_GET_CONNECT(m)   ((m) & IF_CONNECT)

//�˿ڽ���״̬(BUSY���ڼ�¼��������)
#define IF_SET_RX_BUSY(m)   {(m) |= IF_RX_BUSY;}
#define IF_CLS_RX_BUSY(m)   {(m) &= ~IF_RX_BUSY;}
#define IF_GET_RX_BUSY(m)   ((m) & IF_RX_BUSY)

//�˿ڷ���״̬(EMPTY���ڿ��Ʒ���)
#define IF_SET_TX_EMPTY(m)  {(m) |= IF_TX_EMPTY;}
#define IF_CLS_TX_EMPTY(m)  {(m) &= ~IF_TX_EMPTY;}
#define IF_GET_TX_EMPTY(m)  ((m) & IF_TX_EMPTY)

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
	u32 State;	//��ǰuart�˿�״̬���ϲ����״̬��֪���Ƿ񼤻�Ƿ�����
	
}UARTINFO;

typedef enum
{
	UART_PORT = 0,
	NULL_PORT = 0xFF,
}PORT_NUM;

typedef enum
{
	IOCTRL_RX_DATA_SIZE = 0,
	IOCTRL_TX_DATA_SIZE,
	IOCTRL_RESET_RX_BUF,
	IOCTRL_RESET_TX_BUF,
	IOCTRL_RESET_RT_BUF,
	IOCTRL_GET_PORT_STAT,
}IOCTRL_TYPE;


#endif	//__PUBDEF_H


