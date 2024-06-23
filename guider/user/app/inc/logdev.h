#ifndef __logdev_h
#define	__logdev_h

//#include "typedef.h"
#include "macro.h"
#include "pubdef.h"

#include "biosApi.h"

/*底层初始化*/
#define BIOS_INIT() 			        BIOS_DEV_INIT()	//初始化BIOS参数

/*
端口操作
*/
#define INIT_RECV_PORT(t,b,c)			BIOS_OPEN_PORT(t,b,c)//设置当前接收端口
#define SET_RECV_PORT(n)				BIOS_SET_RECV_PORT(n)//设置当前端口类型
#define GET_DATA(x,l)					BIOS_GET_DATA(x,l)//获取数据
#define SEND_DATA(x,l)					BIOS_SEND_DATA(x,l)//发送数据


#define	SHELL_TASK()					BIOS_SHELL_TASK()


#endif//__logdev_h



