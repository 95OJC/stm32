#include "main.h"



#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif



//任务优先级(任务数不能大于configMAX_PRIORITIES，任务的优先级需大于TASK_PRIO_IDLE)
#define TASK_HIGHEST_PRIO	(configMAX_PRIORITIES)	//预留
#define TASK_TIMER_PRIO     (TASK_HIGHEST_PRIO - 1)	//预留
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

//任务句柄
static TaskHandle_t	QueueHandle;
static TaskHandle_t MonitorHandle;
static TaskHandle_t	PanelHandle;
static TaskHandle_t ConsoleHandle;

//任务栈的大小
#define	QUEUE_STACK_SIZE	1024
#define	MONITOR_STACK_SIZE	1024
#define	PANEL_STACK_SIZE	1024
#define	CONSOLE_STACK_SIZE	1024


//at41功能以下：
//初始化上层的全局结构体
//底层硬件外设初始化BIOS_INIT()，才可把通讯接口缓冲传给底层通讯接口作缓冲
//初始化底层的能耗控制
//读取eeprom的EDS项，如果连续3次失败，就把const.c的EDS结构体值赋到EDS项，然后检测到版本号修改了，需重新保存eeprom的EDS项
//通过AD读取某些传感器的值，确认传感器是否安装
//通过AD读取各传感器值，赋值给上层sens结构体全局变量（多少个sens就有多少个bit）
//初始化key和led的结构体全局变量
//给上层的全局结构体赋值，系统属性和中西字属性等，初始化prnq任务的item项动作/模块的上层结构体全局变量
//上层设置sens的背景值给到底层sens结构体静态全局变量（让底层对sens的检测知道有效区间）
//gPageInfo纸张物理结构体全局变量的赋值（上层gFPageInfo的const结构体赋值），这里的上层很多宏值是根据机构实际状态定义的
//初始化gLineInfo行信息
//初始化点调表
//注册掉电时保存EDS项到eeprom的回调函数和启动底层硬件掉电功能
//初始化ctrl任务的接受数据的结构体全局变量索引
//初始化emul任务的接受数据的结构体全局变量索引
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

	//at41功能以下：
	//初始化上层的全局结构体
	//底层硬件外设初始化BIOS_INIT()，才可把通讯接口缓冲传给底层通讯接口作缓冲
	//初始化底层的能耗控制
	//读取eeprom的EDS项，如果连续3次失败，就把const.c的EDS结构体值赋到EDS项，然后检测到版本号修改了，需重新保存eeprom的EDS项
	//通过AD读取某些传感器的值，确认传感器是否安装
	//通过AD读取各传感器值，赋值给上层sens结构体全局变量（多少个sens就有多少个bit）
	//初始化key和led的结构体全局变量
	//给上层的全局结构体赋值，系统属性和中西字属性等，初始化prnq任务的item项动作/模块的上层结构体全局变量
	//上层设置sens的背景值给到底层sens结构体静态全局变量（让底层对sens的检测知道有效区间）
	//gPageInfo纸张物理结构体全局变量的赋值（上层gFPageInfo的const结构体赋值），这里的上层很多宏值是根据机构实际状态定义的
	//初始化gLineInfo行信息
	//初始化点调表
	//注册掉电时保存EDS项到eeprom的回调函数和启动底层硬件掉电功能
	//初始化ctrl任务的接受数据的结构体全局变量索引
	//初始化emul任务的接受数据的结构体全局变量索引
	systemInit();
	
	creatTask(QUEUE_TASK);
	creatTask(MONITOR_TASK);
	creatTask(PANEL_TASK);
	creatTask(CONSOLE_TASK);
	DBG_TRACE("Now Run Task...\r\n");

	//添加定时器：
	//25ms检测一次按键输入
	//50ms刷新一次sens值
	//500ms刷新一次led端口值给底层亮灭
	
	
	//启动调度器:初始化空闲任务(可选初始化定时器任务)，启动系统定时器(不同硬件的port.c代码不一样)
	//Cortex-M3架构中，freertos为了任务启动和任务切换使用了3个异常:SVC，PendSV，systick
	vTaskStartScheduler();

}





