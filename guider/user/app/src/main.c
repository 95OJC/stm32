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

	//����freertos����ִ��
	monitorTask();
	//SHELL_TASK();

}





