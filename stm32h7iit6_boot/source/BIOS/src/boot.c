#include "common.h"
#include "boot.h"

#define AppAddr  0x08100000    /* APP地址 */

void JumpToApp(void)
{
	uint32_t i=0;
	
	typedef void (*AppJump)(void); /* 声明一个函数指针 */
	
	/* 关闭全局中断 */
	DI();
	
	/* 设置所有时钟到默认状态，使用HSI时钟 */
	HAL_RCC_DeInit();
	
	/* 关闭滴答定时器，复位到默认值 */
	SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;	
	
	/* 关闭所有中断，清除所有中断挂起标志 */
	for (i = 0; i < 8; i++)
	{
		NVIC->ICER[i]=0xFFFFFFFF;
		NVIC->ICPR[i]=0xFFFFFFFF;
	}	
	
	/* 跳转到应用程序，首地址是MSP，地址+4是复位中断服务程序地址，然后执行该地址的值就是复位中断服务程序 */
	//AppJump = (void (*)(void)) (*((uint32_t *) (AppAddr + 4)));
	AppJump AppJumpp = (AppJump) ( *( (uint32_t *)(AppAddr + 4) ) );
	
	/* 设置主堆栈指针 */
	__set_MSP(*(uint32_t *)AppAddr);	
	
	/* 设置中断向量表地址 */
	SCB->VTOR = AppAddr;
	/* 跳转到app */
	//AppJump(); 
	AppJumpp();
	
	/* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
	while (1)
	{

	}	
}

