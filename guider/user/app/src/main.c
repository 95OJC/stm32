#include "main.h"

//void systemInit(void)
//{
//	BIOS_INIT();
//
//}

void DevMain(void)
{
	//systemInit();
	BIOS_INIT();

	//创建freertos任务执行
	monitorTask();
	//SHELL_TASK();

}





