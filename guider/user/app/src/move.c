#include "main.h"


#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


/* 如果底层返回的item项stat是成功，说明item项被删除，可以退出循环 */
void WaitItemStop(HWD handle)
{   
    if(handle == NULL)
        return;
        
    while(CHECK_ITEM_STATUS(handle) != ITEM_CPLT)
    {
        DBG_TRACE("WaitItemStop:result=%x\r\n",CHECK_ITEM_STATUS(handle));
        vTaskDelay(1);
    }
}


/* 提交item项到底层，返回item项首地址指针handle */
HWD SubmitItem(QITEM *qItem,u8 prior,u32 time_out)
{    
    HWD handle = NULL;

    do{
        handle = SUBMIT_ITEM_MSG(qItem,prior);
        if(handle != NULL)
        {
            return handle;
        }
		else
        {
            if(time_out != WAIT_FOREVER)
            {
                time_out--;
            }
            vTaskDelay(1);
        }
    }while(time_out);
		
    return handle;
}


void* InitEndCallBack(void* p)
{
    p = NULL;

    CLS_INIT_STATE();
    return (void *)p;
}

HWD DevInit(void)
{
    QITEM qItem;
    INIT_INFO Init;
    HWD handle = NULL;

    SET_INIT_STATE();
    MEM_SET((void*)&qItem,0,sizeof(QITEM));  
	MEM_SET((void*)&Init,0,sizeof(INIT_INFO));
	qItem.type = INIT_MSG;
	qItem.s_func = NULL;
    qItem.e_func = InitEndCallBack;
	qItem.hook_func = NULL;
	qItem.dlyTime = 0;
	qItem.dlyTimeReload = 0;
	qItem.itemTag = ITEM_NORMAL;
	//qItem.stat = ITEM_READY;
    Init.ctrl = 0x00;
    MEM_CPY(qItem.buf,&Init,sizeof(INIT_INFO));
    handle = SubmitItem(&qItem,NORMAL_PRIOR,WAIT_FOREVER);    

	WaitItemStop(handle);
    return handle;
}






