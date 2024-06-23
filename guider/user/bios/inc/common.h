#ifndef __common_h
#define	__common_h

/*-----------------------------------------------------------------
1、需要严格按照以下顺序放置头文件，防止找不到定义。(C库函数头文件、寄存器及其地址封装头文件、公共服务宏和结构体、底层驱动函数API)
2、若是freertos、usb协议栈之类等的头文件，最好也放在“//底层的驱动API”的前面，和“//上层和底层的公共服务、宏、结构体”的后面,因为执行这些协议栈不需要前2种头文件的支持。
3、注意C文件的头文件的放置顺序：在.c文件里先放common.h，再放.c文件的内部头文件。因为执行C文件时头文件会按顺序展开，又因为执行这些协议栈不需要前2种头文件的支持。
------------------------------------------------------------------*/

//底层用到的C库
#include <string.h>//内存拷贝函数用到
#include <stdio.h>//重定向函数用到
#include <stdlib.h>//malloc()、free()等用到


//底层的STM32库函数(硬件抽象层/底层，含映射寄存器及寄存器位宏定义)
//u8、u6、u32定义在里面
//包含了“stdint.h”这个头文件，可用uint16_t之类
#include "stm32f10x.h"

//底层的寄存器操作函数，外设地址没有结构体封装时候才用到！
#include "regwrApi.h"

//上层和底层的公共服务定义(需要排列在底层的驱动函数前，确保底层的驱动函数(传参)能够找到该公共服务定义)
#include "macro.h"
//上层和底层公共的宏和结构体(需要排列在底层的驱动函数前，确保底层的驱动函数(传参)能够找到该宏和结构体)
#include "pubdef.h"
//宏开关
#include "config.h"

typedef enum{
	DEV_ERR = 0,
	DEV_OK = 1,	
	DEV_BUSY = 2,
	DEV_TIMEOUT = 3
}DEV_STATUS;

//底层的驱动API
#include "gpioApi.h"
#include "keyCtrlApi.h"
#include "systick_rcc_Api.h"
#include "timerApi.h"
#include "ringbufApi.h"
#include "usartApi.h"
#include "ifApi.h"
#include "shellApi.h"
#include "dmaApi.h"
#include "iicApi.h"
#include "spiApi.h"
#include "eepromApi.h"
#include "flashApi.h"
#include "adda.h"



#include "bios.h"
#include "keyTTP224.h"//4键电容按键
#include "WaterSensor.h"//水位传感器
#include "DCmotor.h"//直流电机+继电器
#include "heater.h"//发热器+继电器
#include "DS18B20.h"//温度传感器+单总线
#include "OLED096.h"//OLED096屏幕



#endif//__common_h


