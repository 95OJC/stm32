#ifndef __PUBDEF_H
#define __PUBDEF_H

/*队列优先级（一般是上层使用）*/
#define NORMAL_PRIOR	0x00	//prnQBuf[MAX_PRNITEM]优先级
#define PANEL_PRIOR		0x01	//pnlItem优先级
//#define PRIOR_RPLE		0x80	//强制替换项：面板优先级以替换方式提交，用于替换当前面板优先级的item项（底层屏蔽了，暂时不使用，还需要考虑一下实际功能）


//队列请求控制
/***********prnQ.qCtrlReq*************/
typedef enum
{
	QREQ_NONE		= 	0,
	QREQ_PAUSE		=	1,
	QREQ_RESUME		=	2,
	QREQ_CLS		=	3,
	QREQ_PNL_CLS	=	4
}PRNQ_REQ_STAT;


//队列请求状态
/************prnQ.qStat***************/
typedef enum
{
	QSTAT_ACT	=	0,	//队列工作
	QSTAT_PAUSE	=	1	//队列暂停
}PRNQ_STAT;


/*********QITEM.type***********/
typedef enum
{
	NO_MSG,				/*< NO MSG */
	INIT_MSG,			/*< 初始化 */

}MSG_TYPE;


/*********QITEM.MSG_INFO***********/
//对应QITEM.type有多少个类型动作/模块，就要有多少个动作/模块的MSG_INFO结构体
//为了上层传信息给底层的这些动作/模块，然后底层通过这些信息驱动动作/模块工作
//这些动作/模块的MSG_INFO结构体，目前里面都只有ctrl(用于联动QITEM.itemTag来控制item项的动作/模块)，还可以添加一些变量或者指针等
typedef struct
{
	u32 	ctrl;
}INIT_INFO;


typedef union
{
	INIT_INFO	init;

}MSG_INFO;


/*********QITEM.itemTag***********/
/*注意只有高16BIT可用低16bit保留*/
//底层根据上层提交的itemTag属性，可干预item项的动作/模块。
#define ITEM_TAG_MASK   0xFFFF0000
#define ITEM_NORMAL     0x00000000/*< 正常    */
#define ITEM_IGNERR     0x00010000/*< 忽略错误，忽略错误激活item项 */
#define ITEM_HOLD       0x00020000/*< 锁定电机（撕纸时锁定电机，默认锁定60S）*/


/***********QITEM.stat ****************/
//底层完成驱动动作/模块后，PRNQ负责填充对应的运行结果返回上层
typedef enum
{
    ITEM_READY      = 0,    /*< ITEM 放入队列还没有激活         */
    ITEM_ACTING     = 1,    /*< ITEM 正在运行过程中        */
    /*以下全部状态会引起ITEM项被删除 */
    ITEM_CPLT       = 0x82, /*< ITEM 运行完成     */
    ITEM_CANCEL     = 0x83, /*< 用户取消ITEM项      */
}ITEM_STAT;


/*********** 一个item项的结构体 ****************/
typedef struct _QITEM
{
	MSG_TYPE type;				/*< 1、消息的类型 */
	u32 padding;				/* 占位符保证下面4字节对齐 */
	u8 buf[sizeof(MSG_INFO)];	/* 2、消息的参数：参考QITEM.MSG_INFO 说明*/
	CallBackFunc s_func;		/*< ITEM项激活好了，起始回调函数，在回调中可以再次修改QITEM参数，函数参数为 *QITEM */
	CallBackFunc e_func;		/*< ITEM项运行完了，结束回调函数，在回调函数参数中取消上层该动作/模块的忙标志，函数参数为 *QITEM */
    CallBackFunc hook_func; 	/*< ITEM项没激活成功，hook回调函数可以先处理其他事情,直到ITEM项被激活运行，例如一直检测传感器之类的 */
	u16	dlyTime;				/*< 激活item项后，延迟运行单位ms */
	u16	dlyTimeReload;			/*< 延迟重装 */
	u32	itemTag;				/*< item项的属性设置(一般由上层设置): 参考QITEM.itemTag说明 */
	u8 pt;            			//保留
	ITEM_STAT stat;         	/*< 由PRNQ负责填充,ITEM项的运行结果放在此，参考QITEM.stat说明 */
}QITEM;


//获取item项的类型
#define	GET_QITEM_TYPE(m)		((m)->type)

//获取item项的itemTag
#define	GET_QITEM_TAG(m)		((m)->itemTag)


//忽略错误激活tiem项
#define SET_QITEM_IGNERR(m)     {(m)->itemTag |= ITEM_IGNERR;}
#define CLS_QITEM_IGNERR(m)     {(m)->itemTag &= ~ITEM_IGNERR;}
#define IS_QITEM_IGNERR(m)      ((m)->itemTag & ITEM_IGNERR)


//锁定电机
#define SET_QITEM_HOLD(m)       {(m)->itemTag |= ITEM_HOLD;}
#define CLS_QITEM_HOLDE(m)      {(m)->itemTag &= ~ITEM_HOLD;}
#define IS_QITEM_HOLD(m)        ((m)->itemTag & ITEM_HOLD)






/*缓冲*/
#define TX_BUF      0
#define RX_BUF      1
#define RT_BUF      2

//端口全局状态(变量是全局)
#define IF_GLB_BIT              0x0000FF00
#define IF_ACT                  0x00000100//端口打开
#define IF_CONNECT              0x00000200//端口连接

//端口接收状态(变量是静态全局)
#define IF_RX_STAT_BIT          0x00FF0000
#define IF_RX_BUSY		        0x00010000
#define IF_RX_EMPTY             0x00020000

//端口发送状态(变量是静态全局)
#define IF_TX_STAT_BIT          0xFF000000
#define IF_TX_BUSY		        0x01000000
#define IF_TX_EMPTY             0x02000000

//端口全局激活
#define IF_SET_ACT(m) 		{(m) |= IF_ACT;}
#define IF_CLS_ACT(m) 		{(m) &= ~IF_ACT;}
#define IF_GET_ACT(m) 		(m & IF_ACT)

//端口全局连接
#define IF_SET_CONNECT(m)   {(m) |= IF_CONNECT;}
#define IF_CLS_CONNECT(m)   {(m) &= ~IF_CONNECT;}
#define IF_GET_CONNECT(m)   ((m) & IF_CONNECT)

//端口接收状态(BUSY用于记录接收流控)
#define IF_SET_RX_BUSY(m)   {(m) |= IF_RX_BUSY;}
#define IF_CLS_RX_BUSY(m)   {(m) &= ~IF_RX_BUSY;}
#define IF_GET_RX_BUSY(m)   ((m) & IF_RX_BUSY)

//端口发送状态(EMPTY用于控制发送)
#define IF_SET_TX_EMPTY(m)  {(m) |= IF_TX_EMPTY;}
#define IF_CLS_TX_EMPTY(m)  {(m) &= ~IF_TX_EMPTY;}
#define IF_GET_TX_EMPTY(m)  ((m) & IF_TX_EMPTY)

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
	u32 State;	//当前uart端口状态，上层读该状态可知：是否激活、是否连接
	
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


