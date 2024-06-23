#include "main.h"


#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


//处理monitor任务传来的报错回调函数（消息队列）
//处理按键的普通功能（联机、脱机、跳行等等）
void panelTask(void *p)
{


	while(1)
	{
		DBG_TRACE("panelTask\r\n");
		vTaskDelay(1000);

	}
}










