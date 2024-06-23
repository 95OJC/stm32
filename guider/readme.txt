stm32f10x.h里面包含：//u8、u6、u32定义在里面
1、#include "core_cm3.h"//内核寄存器映射和控制函数，包含了“stdint.h”这个头文件，可用uint16_t
2、#include "system_stm32f10x.h"//SystemInit函数将系统时钟就被初始化成72M
3、#include "stm32f10x_conf.h"//可选择打开哪个外设头文件，里面包含各个外设头文件

common.h包含以下：（因此可用所有stm32底层库、自己写的驱动层函数、）
#include "stm32f10x.h"
#include "macro.h"
#include "gpioApi.h"
等等

--------------------------------------------
当前所用的IO资源分配：
led: 红色PB5  绿色PB0  蓝色PB1
KEY: KEY1-PA0  KEY2-PC13
USART1：TX-PA9  RX-PA10
IIC1-eeprom：SCL-PB6  SDA-PB7
SPI1-flash：CS-PC0  SCK-PA5  MISO-PA6  MOSI-PA7
蜂鸣器：PA8

总结：
PA-0-8-9-10-5-6-7
PB-0-1-5-6-7
PC-13-0

--------------------------------------------
TTP224四键电容式触摸IC-IO口控制：哪个最后接触就会触发哪个
PA-2-3-11-12 

3.3V的水位传感器：
PC-1  ：ADC2_channel-11
PB-8  ：IO口控制

5V接单片机的继电器 + 5V直流有刷电机 + 1.2V~12V(最大2A)供电电源
PB-10

5V接单片机的继电器 + 发热器3.3~12V(最大2.9A，最小0.9A) + 1.2V~12V(最大2A)供电电源
PC-7

3.3V的DS18B20温度传感器-单总线控制：
PA-4

096OLED屏-4pin：
SCL-PB6  SDA-PB7  ：硬IIC1或者软IIC 












