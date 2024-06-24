stm32f10x.h里面包含：//u8、u6、u32定义在里面
1、#include "core_cm3.h"//内核寄存器映射和控制函数，包含了“stdint.h”这个头文件，可用uint16_t
2、#include "system_stm32f10x.h"//SystemInit函数将系统时钟就被初始化成72M
3、#include "stm32f10x_conf.h"//可选择打开哪个外设头文件，里面包含各个外设头文件