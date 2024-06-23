#ifndef	__biosApi_h
#define	__biosApi_h

//该文件的函数声明是给上层app文件调用的，可以不需要给common.h包含，而是给main.h包含的logdev.h所包含。
void biosInit(void);
BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl);
BOOL if_set_cur_port(PORT_NUM n);
BOOL if_close(void);
u32 if_read_data(u8 *buf, u32 len);
u32 if_write_data(u8 *buf, u32 len);
u32 if_peek_data(u8 *buf, u32 len);
BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl);
PORT_NUM if_find_data_port(void);


void shell_task(void *p);
void prnq_task(void *p);
HWD submit_item(QITEM *item,u8 prior);
ITEM_STAT check_msg_status(HWD h);
BOOL set_prnq_req(PRNQ_REQ_STAT req);
PRNQ_STAT get_prnq_status(void);
u8 get_prnq_cnt(void);
BOOL is_prnq_empty(void);
BOOL is_prnq_pnl_empty(void);
u8 get_prnq_free_cnt(void);


BOOL bios_is_buf_empty(u8 m);
u32 bios_get_data_cnt(u8 m);


/*底层硬件初始化*/
#define BIOS_DEV_INIT()             biosInit()

/*端口*/
#define	BIOS_SHELL_TASK				shell_task
#define BIOS_OPEN_PORT(t,b,c)       if_open(t,b,c)
#define BIOS_SET_RECV_PORT(n)       if_set_cur_port(n)
#define BIOS_GET_DATA(x,l)          if_read_data(x,l)
#define BIOS_SEND_DATA(x,l)         if_write_data(x,l)
#define BIOS_PEEK_DATA(x,l)         if_peek_data(x,l)
#define BIOS_IS_BUF_EMPT(n)         bios_is_buf_empty(n)
#define BIOS_GET_BUF_CNT(n)         bios_get_data_cnt(n)
#define BIOS_RESET_PORT()           if_io_ctrl(IOCTRL_RESET_RT_BUF,NULL)
#define BIOS_CLS_BUF(ioctrl)        if_io_ctrl(ioctrl,NULL)
#define BIOS_FIND_DPORT()           if_find_data_port()

/* 队列 */
#define	BIOS_PRNQ_TASK					prnq_task
#define BIOS_SUBMIT_ITEM_MSG(item,p)	submit_item(item,p)
#define BIOS_CHECK_ITEM_STATUS(h)   	check_msg_status(h)
#define BIOS_GET_PRNQ_STAT()        	get_prnq_status()
#define BIOS_SET_PRNQ_REQ(m)        	set_prnq_req(m)
#define BIOS_GET_PRNQ_CNT()         	get_prnq_cnt()
#define BIOS_IS_PRNQ_EMPTY()        	is_prnq_empty()
#define BIOS_IS_PNLITEM_EMPTY()      	is_prnq_pnl_empty()
#define BIOS_GET_FREE_PRNQ_CNT()    	get_prnq_free_cnt()


#endif//__biosApi_h

