#include "main.h"


#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


//����monitor�������ı���ص���������Ϣ���У�
//����������ͨ���ܣ��������ѻ������еȵȣ�
void panelTask(void *p)
{


	while(1)
	{
		DBG_TRACE("panelTask\r\n");
		vTaskDelay(1000);

	}
}










