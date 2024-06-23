#include "common.h"


#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


BOOL bios_is_buf_empty(u8 m)
{
    u32 n = 1;
    
    if(m == RX_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);
    }else if(m == TX_BUF)
    {
        if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
    }else if(m == RT_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);
        if(n == 0)
        {
            if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
        }
    }
    if(n == 0)
    {
		return TRUE;
    }
	else
	{
		return FALSE;
	}
}


u32 bios_get_data_cnt(u8 m)
{
    u32 n;
    
    if(m == RX_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);

    }else if(m == TX_BUF)
    {
        if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
    }

    return n;
}


//判断ITEM项是否允许激活运行
BOOL itemEnable(QITEM *item)
{
	//获取各item项的动作/模块的状态，判断ITEM项是否允许激活
	//添加忽略错误时激活item项的代码
	return TRUE;
}


//上层提交item项后，prnq任务实际执行的item项函数
MSG_STAT bios_dev_init(INIT_INFO *init)
{

	DBG_TRACE("bios_dev_init OK!\r\n");
	return RUN_OK;
}


void biosInit(void)
{
	if_init();
	//usart_driver_register();//与debug共用串口，暂时屏蔽
	
}


