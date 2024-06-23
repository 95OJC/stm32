#ifndef __prnq_h
#define	__prnq_h

/* item项的 bios_xxx_fun(QITEM.MSG_INFO参数) 执行情况 */	
typedef enum
{
	NO_RUN	=	0,	    //没有执行
	RUN_OK	=	1,	    //完整执行完成
	RUNNING	=	2,	    //正在执行
	RUN_ERR	=	0x0F,    //执行错误
}MSG_STAT;


#endif//__prnq_h

