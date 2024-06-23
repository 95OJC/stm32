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
void shell_task(void);



/*其它*/
#define BIOS_DEV_INIT()             biosInit()
#define	BIOS_SHELL_TASK()			shell_task()
/*
端口
*/
#define BIOS_OPEN_PORT(t,b,c)       if_open(t,b,c)   //打开端口
#define BIOS_SET_RECV_PORT(n)       if_set_cur_port(n)  //设置当前端口类型
#define BIOS_GET_DATA(x,l)          if_read_data(x,l) //读接收端口
#define BIOS_SEND_DATA(x,l)         if_write_data(x,l)//向主机发送数据




#endif//__biosApi_h

