#ifndef __COMMON_H
#define	__COMMON_H

//是C标准库中的一个头文件，提供了许多通用工具函数，包括内存分配、进程控制、排序和搜索、以及字符串转换等
#include <stdlib.h>
//是C99标准引入的头文件，仅在 C99 及更高版本中可用，它提供了一些整数类型的定义，这些类型具有明确的大小和符号属性，确保在不同平台上的一致性。
#include <stdint.h>
//是C标准库中的一个头文件，提供了一组用于处理字符串和内存块的函数。这些函数涵盖了字符串复制、连接、比较、搜索和内存操作等。
#include <string.h>
//定义了一个变量类型 va_list 和三个宏，这三个宏可用于在参数个数未知（即参数个数可变）时获取函数中的参数。可变参数的函数通常在参数列表的末尾是使用省略号 ... 定义的。
#include <stdarg.h>
//包含了一系列用于输入输出操作的函数原型（比如 printf 和 scanf 函数）、宏定义（比如 EOF 和 NULL）和类型声明（比如 FILE 和 size_t）
#include <stdio.h>


#include "stm32h7xx.h"//定义宏USE_HAL_DRIVER使用HAL库、定义宏STM32H743xx使用H743寄存器位定义
#include "typedef.h"//定义数据类型缩写
#include "macro.h"//定义全局中断、代码和数据的存储空间、内存管理函数
#include "pubdef.h"//定义公共端口宏、任务队列、错误宏
#include "pridef.h"//中断优先级
#include "RingBufApi.h"
#include "IFApi.h"
#include "debug.h"
#include "uartApi.h"

#include "main.h"
#include "bootApi.h"





#endif//__COMMON_H


