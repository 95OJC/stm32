#include "main.h"



#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif



//�������ȼ�(���������ܴ���configMAX_PRIORITIES����������ȼ������TASK_PRIO_IDLE)
#define TASK_HIGHEST_PRIO	(configMAX_PRIORITIES)	//Ԥ��
#define TASK_TIMER_PRIO     (TASK_HIGHEST_PRIO - 1)	//Ԥ��
#define TASK_PRIO_8			(TASK_HIGHEST_PRIO - 2)
#define TASK_PRIO_7			(TASK_HIGHEST_PRIO - 3)
#define TASK_PRIO_6			(TASK_HIGHEST_PRIO - 4)
#define TASK_PRIO_5			(TASK_HIGHEST_PRIO - 5)
#define TASK_PRIO_4			(TASK_HIGHEST_PRIO - 6)
#define TASK_PRIO_3			(TASK_HIGHEST_PRIO - 7)
#define TASK_PRIO_2			(TASK_HIGHEST_PRIO - 8)
#define TASK_PRIO_1			(TASK_HIGHEST_PRIO - 9)		
#define TASK_PRIO_0			(TASK_HIGHEST_PRIO - 10)//Idle Task

#define QUEUE_TASK			TASK_PRIO_7
#define MONITOR_TASK		TASK_PRIO_6
#define PANEL_TASK			TASK_PRIO_3
#define CONSOLE_TASK        TASK_PRIO_1
#define TASK_PRIO_IDLE		TASK_PRIO_0

//������
static TaskHandle_t	QueueHandle;
static TaskHandle_t MonitorHandle;
static TaskHandle_t	PanelHandle;
static TaskHandle_t ConsoleHandle;

//����ջ�Ĵ�С
#define	QUEUE_STACK_SIZE	1024
#define	MONITOR_STACK_SIZE	1024
#define	PANEL_STACK_SIZE	1024
#define	CONSOLE_STACK_SIZE	1024


//at41�������£�
//��ʼ���ϲ��ȫ�ֽṹ��
//�ײ�Ӳ�������ʼ��BIOS_INIT()���ſɰ�ͨѶ�ӿڻ��崫���ײ�ͨѶ�ӿ�������
//��ʼ���ײ���ܺĿ���
//��ȡeeprom��EDS��������3��ʧ�ܣ��Ͱ�const.c��EDS�ṹ��ֵ����EDS�Ȼ���⵽�汾���޸��ˣ������±���eeprom��EDS��
//ͨ��AD��ȡĳЩ��������ֵ��ȷ�ϴ������Ƿ�װ
//ͨ��AD��ȡ��������ֵ����ֵ���ϲ�sens�ṹ��ȫ�ֱ��������ٸ�sens���ж��ٸ�bit��
//��ʼ��key��led�Ľṹ��ȫ�ֱ���
//���ϲ��ȫ�ֽṹ�帳ֵ��ϵͳ���Ժ����������Եȣ���ʼ��prnq�����item���/ģ����ϲ�ṹ��ȫ�ֱ���
//�ϲ�����sens�ı���ֵ�����ײ�sens�ṹ�徲̬ȫ�ֱ������õײ��sens�ļ��֪����Ч���䣩
//gPageInfoֽ������ṹ��ȫ�ֱ����ĸ�ֵ���ϲ�gFPageInfo��const�ṹ�帳ֵ����������ϲ�ܶ��ֵ�Ǹ��ݻ���ʵ��״̬�����
//��ʼ��gLineInfo����Ϣ
//��ʼ�������
//ע�����ʱ����EDS�eeprom�Ļص������������ײ�Ӳ�����繦��
//��ʼ��ctrl����Ľ������ݵĽṹ��ȫ�ֱ�������
//��ʼ��emul����Ľ������ݵĽṹ��ȫ�ֱ�������
static void systemInit(void)
{
	BIOS_INIT();
	CLS_BUSY_STATE();
}


static void creatTask(u8 PRIO)
{
	if(PRIO == QUEUE_TASK)
	{
		if(pdTRUE == xTaskCreate(	PRNQ_TASK,
							 		(const char *)"queue",
								 	QUEUE_STACK_SIZE,
									NULL,//&QueueHandle,
									QUEUE_TASK,
									&QueueHandle	))
		{
			DBG_TRACE("creat queue ok\r\n");	
		}
	}
	else if(PRIO == MONITOR_TASK)
	{
		if(pdTRUE == xTaskCreate(	monitorTask,
							 		(const char *)"monitor",
								 	MONITOR_STACK_SIZE,
									NULL,//"Monitor Task Runing\r\n",
									MONITOR_TASK,
									&MonitorHandle	))
		{
			DBG_TRACE("creat monitorTask ok\r\n");	
		}
	}
	else if(PRIO == PANEL_TASK)
	{
		if(pdTRUE == xTaskCreate(	panelTask,
							 		(const char *)"panel",
								 	PANEL_STACK_SIZE,
									NULL,//"panel Task Runing\r\n",
									PANEL_TASK,
									&PanelHandle	))
		{
			DBG_TRACE("creat panelTask ok\r\n");	
		}
	}	
	else if(PRIO == CONSOLE_TASK)
	{
		if(pdTRUE == xTaskCreate(	SHELL_TASK,
							 		(const char *)"console",
								 	CONSOLE_STACK_SIZE,
									NULL,//"shell Task Runing\r\n",
									CONSOLE_TASK,
									&ConsoleHandle	))
		{
			DBG_TRACE("creat shell ok\r\n");	
		}
	}							
}


void DevMain(void)
{

	//at41�������£�
	//��ʼ���ϲ��ȫ�ֽṹ��
	//�ײ�Ӳ�������ʼ��BIOS_INIT()���ſɰ�ͨѶ�ӿڻ��崫���ײ�ͨѶ�ӿ�������
	//��ʼ���ײ���ܺĿ���
	//��ȡeeprom��EDS��������3��ʧ�ܣ��Ͱ�const.c��EDS�ṹ��ֵ����EDS�Ȼ���⵽�汾���޸��ˣ������±���eeprom��EDS��
	//ͨ��AD��ȡĳЩ��������ֵ��ȷ�ϴ������Ƿ�װ
	//ͨ��AD��ȡ��������ֵ����ֵ���ϲ�sens�ṹ��ȫ�ֱ��������ٸ�sens���ж��ٸ�bit��
	//��ʼ��key��led�Ľṹ��ȫ�ֱ���
	//���ϲ��ȫ�ֽṹ�帳ֵ��ϵͳ���Ժ����������Եȣ���ʼ��prnq�����item���/ģ����ϲ�ṹ��ȫ�ֱ���
	//�ϲ�����sens�ı���ֵ�����ײ�sens�ṹ�徲̬ȫ�ֱ������õײ��sens�ļ��֪����Ч���䣩
	//gPageInfoֽ������ṹ��ȫ�ֱ����ĸ�ֵ���ϲ�gFPageInfo��const�ṹ�帳ֵ����������ϲ�ܶ��ֵ�Ǹ��ݻ���ʵ��״̬�����
	//��ʼ��gLineInfo����Ϣ
	//��ʼ�������
	//ע�����ʱ����EDS�eeprom�Ļص������������ײ�Ӳ�����繦��
	//��ʼ��ctrl����Ľ������ݵĽṹ��ȫ�ֱ�������
	//��ʼ��emul����Ľ������ݵĽṹ��ȫ�ֱ�������
	systemInit();
	
	creatTask(QUEUE_TASK);
	creatTask(MONITOR_TASK);
	creatTask(PANEL_TASK);
	creatTask(CONSOLE_TASK);
	DBG_TRACE("Now Run Task...\r\n");

	//��Ӷ�ʱ����
	//25ms���һ�ΰ�������
	//50msˢ��һ��sensֵ
	//500msˢ��һ��led�˿�ֵ���ײ�����
	
	
	//����������:��ʼ����������(��ѡ��ʼ����ʱ������)������ϵͳ��ʱ��(��ͬӲ����port.c���벻һ��)
	//Cortex-M3�ܹ��У�freertosΪ�����������������л�ʹ����3���쳣:SVC��PendSV��systick
	vTaskStartScheduler();

}





