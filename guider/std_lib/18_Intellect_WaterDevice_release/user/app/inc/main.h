#ifndef __main_h
#define	__mian_h

//上层用到的C库
#include <string.h>//内存拷贝函数用到
#include <stdio.h>//重定向函数用到
#include <stdlib.h>//malloc()、free()等用到


//为了上层可以给底层配置外设参数
//u8、u6、u32定义在里面
//包含了“stdint.h”这个头文件，可用uint16_t之类
#include "stm32f10x.h"


//包含上层和底层的公共服务定义，还有memcpy等函数功能
//包含上层和底层的接口公共的宏和结构体
//包含底层传给上层使用的函数
#include "logdev.h"


//缓存和缓冲定义
#include "system.h"//定义上层需用的通讯TX缓存cache和RX缓冲buffer，结构体和ringbuf数据宏
#include "sysRam.h"//(外部)声明上层需用的通讯TX缓存cache和RX缓冲buffer


//上层文件名头文件
#include "monitor.h"


#endif//__main_h

