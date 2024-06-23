#ifndef __logdev_h
#define	__logdev_h


#include "macro.h"
#include "pubdef.h"
#include "biosApi.h"


/*底层硬件初始化*/
#define BIOS_INIT() 			        BIOS_DEV_INIT()

/*端口*/
#define	SHELL_TASK						BIOS_SHELL_TASK
#define INIT_RECV_PORT(t,b,c)			BIOS_OPEN_PORT(t,b,c)//打开端口
#define SET_RECV_PORT(n)				BIOS_SET_RECV_PORT(n)//设置当前端口类型
#define GET_DATA(x,l)					BIOS_GET_DATA(x,l)//获取数据
#define SEND_DATA(x,l)					BIOS_SEND_DATA(x,l)//发送数据
#define PEEK_DATA(x,l)					BIOS_PEEK_DATA(x,l)	//查看接收端口数据
#define IS_RECV_EMPT()					BIOS_IS_BUF_EMPT(RX_BUF)//检测接收缓存是否为空
#define IS_TX_EMPTY()					BIOS_IS_BUF_EMPT(TX_BUF)//检查发送端口是否为空
#define GET_RECV_CNT()					BIOS_GET_BUF_CNT(RX_BUF)//接收缓冲中的数据量
#define GET_TX_CNT()					BIOS_GET_BUF_CNT(TX_BUF)//接收缓冲中的数据量
#define RESET_PORT()					BIOS_RESET_PORT()//清空接收和发送缓冲
#define CLS_RECV_BUF()					BIOS_CLS_BUF(IOCTRL_RESET_RX_BUF)//清除接收缓冲
#define CLS_TX_BUF()					BIOS_CLS_BUF(IOCTRL_RESET_TX_BUF)//清除发送缓冲
#define FIND_DATA_PORT()				BIOS_FIND_DPORT()//遍历所有端口，查看哪个收到数据

/*队列*/
#define	PRNQ_TASK						BIOS_PRNQ_TASK
#define SUBMIT_ITEM_MSG(item,p) 		BIOS_SUBMIT_ITEM_MSG(item,p)//提交ITEM
#define CHECK_ITEM_STATUS(h)            BIOS_CHECK_ITEM_STATUS(h)//检测item项的运行状态
#define GET_PRNQ_STAT()                 BIOS_GET_PRNQ_STAT()//获取打印队列状态，队列工作/暂停
#define SET_PRNQ_REQ(m)					BIOS_SET_PRNQ_REQ(m)//设置队列激活/暂停		
#define GET_PRNQ_CNT()                  BIOS_GET_PRNQ_CNT()//获取非面板item项队列中的ITEM项数量(包含正在运行的非面板item项)
#define IS_PRNQ_EMPTY()                 BIOS_IS_PRNQ_EMPTY()//检测队列是否为空
#define IS_PNLITEM_EMPTY()				BIOS_IS_PNLITEM_EMPTY()//检测面板item项是否为空
#define GET_FREE_PRNQ_CNT()             BIOS_GET_FREE_PRNQ_CNT()//队列中空ITEM项数量


#endif//__logdev_h



