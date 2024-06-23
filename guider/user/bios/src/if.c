#include "common.h"
#include "if.h"

/*-----------------------------------------------------------------
实现通讯接口分层的功能，使底层和上层通讯接口函数分开：

步骤：
1、结构体数组driver_tbl，给MAX_DRIVER_NUM个IF_DRIVER结构体注册回调函数。
2、结构体指针pCurrentDriver用于当前在用哪个port上执行回调函数操作数据。
3、需按照以下顺序:
   ①底层if_init，把初始化结构体指针pCurrentDriver设置为NULL，driver_tbl[i].valid和driver_tbl[i].tag。
   ②底层if_register_driver，注册PORT_NUM对应的回调函数和设置driver_tbl[i].valid有效。
   ③上层if_open，初始化上层传来枚举值PORT_NUM的通讯接口和设置driver_tbl[i].tag打开。
   ④上层if_set_cur_port，设置上层传来枚举值PORT_NUM为driver_tbl索引，给指针pCurrentDriver。
   ⑤上层close、read、write、peek函数使用前都先判断指针pCurrentDriver是否为空。
   

延伸理解shell用到的回调：
static const cmd_t sCmdTbl[] = {
{"help","hp",help,1,"tip", "help"},
{0,0,0,0,0,0},
{0,0,0,0,0,0}
};
上面的结构体数组是以数组赋值的形式赋值回调函数，然后pInfo->pCmd = &sCmdTbl[0]，pInfo->pCmd->func即可调用回调函数。


总结if和shell的回调函数用法：
注册回调函数：首先shell是直接在结构体数组的初始化赋值所有的注册回调函数。而if则是提供了一个注册函数给通讯接口文件调用，需要
			  传入枚举值PORT_NUM赋值哪个结构体和传入所有需注册的回调函数。
			      
使用回调函数：首先shell是一个该结构体类型指针pInfo->pCmd直接指向该结构体数组某个下标，然后去使用pInfo->pCmd->func1/func2/func3等执行回调函数。
			  (func1/func2/func3是同一个下标的回调函数，对应if层的pCurrentDriver->read/write/peek等回调函数)
			  而if则是先用一个该结构体类型指针pCurrentDriver通过if_set_cur_port函数指向该结构体数组某个下标(下标:上层设置端口而传来的枚举值PORT_NUM)，
			  然后编写read、write、peek、io_ctrl等函数，里面用pCurrentDriver->read等(参数...)执行回调函数


注意：if_open、if_io_ctrl等函数传入参数的类型和通讯接口回调函数传入参数的类型是一样的，因此定义该类型时需要在上层使用和底层注册/使用的公共头文件。


如何通过指针调用回调找到原来调用的原始函数？(一般我们是看到调用指针去执行回调函数，想看回调的原始函数，需要根据以下2步骤查找)
1、找到该调用回调的指针，它是指向哪个回调函数类型的变量或数组下标。
2、找到回调函数类型的变量或数组下标的注册函数，即可找到回调的原始函数。

------------------------------------------------------------------*/

#define	MAX_DRIVER_NUM	10

static IF_DRIVER driver_tbl[MAX_DRIVER_NUM];//该结构体选数组下标，给外部赋值
static IF_DRIVER *pCurrentDriver;//该结构体给内部赋值，选择当前通讯口

void if_init(void)
{
	u8 i;

	pCurrentDriver = NULL;

	for(i=0; i<MAX_DRIVER_NUM; i++)
	{
		driver_tbl[i].valid = FALSE;
		driver_tbl[i].tag = FALSE;
	}

}

void if_register_driver(
	PORT_NUM n,
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl),
	BOOL (*close)(void),
	u32  (*read)(u8 *buf, u32 len),
	u32  (*write)(u8 *buf, u32 len),
	u32  (*peek)(u8 *buf, u32 len),
	BOOL (*io_ctrl)(IOCTRL_TYPE type, void *ctrl)
)
{
	IF_DRIVER *p;

	p = &driver_tbl[n];

	p->valid = TRUE;
	p->tag = FALSE;
	p->open = open;
	p->close = close;
	p->read = read;
	p->write = write;
	p->peek = peek;
	p->io_ctrl = io_ctrl;

}


PORT_NUM if_find_data_port(void)
{
    PORT_NUM port = NULL_PORT;
    u8 i;
    u32 n;
    
    for(i=0; i<MAX_DRIVER_NUM; i++)
    {
        if(!driver_tbl[i].valid || !(driver_tbl[i].tag & IF_OPEN_BIT))
        {
            continue;
        }

        driver_tbl[i].io_ctrl(IOCTRL_RX_DATA_SIZE,&n);
        
        if(n > 0)
        {
            port = (PORT_NUM)i;
            break;
        }
    }
    
    return port;
}


BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl)
{
	BOOL ret = FALSE;
	IF_DRIVER *pDriver;

	if(n >= MAX_DRIVER_NUM)
	{
		return FALSE;
	}

	pDriver = &driver_tbl[n];

	if(!pDriver->valid)
	{
		return FALSE;
	}

	if(pDriver->open(pInfo,pCtrl))
	{
		pDriver->tag = TRUE;
		ret = TRUE;
	}

	return ret;
}


BOOL if_set_cur_port(PORT_NUM n)
{
	if(n >= MAX_DRIVER_NUM)
	{
		return FALSE;
	}

	if(!driver_tbl[n].tag)
	{
		return FALSE;
	}

	pCurrentDriver = &driver_tbl[n];

	return TRUE;
}

BOOL if_close(void)
{
	BOOL ret = FALSE;

	if(pCurrentDriver)
	{
		if(pCurrentDriver->close())
		{
			pCurrentDriver->tag = FALSE;
			ret = TRUE;
		}
	}

	return ret;
}

u32 if_read_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->read(buf,len);
	}

	return ret;
}

u32 if_write_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->write(buf,len);
	}

	return ret;
}

u32 if_peek_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->peek(buf,len);
	}

	return ret;
}

BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl)
{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->io_ctrl(type,ctrl);
    }

    return ret;   
}


