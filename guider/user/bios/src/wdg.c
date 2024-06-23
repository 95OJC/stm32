#include "common.h"


/* 
IWDG配置函数:
	MCU内部的LSI时钟会在30~60kHz之间变化，我们以40kHz时钟给出iwgd， 
	然后40除prv得出计数1次的时钟频率，再乘rlv，得出至少的喂狗时间。

 * prv可以是[4,8,16,32,64,128,256]
 * prv:预分频器值，取值如下：
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:预分频器值，取值范围为：0-0XFFF
 * 函数调用举例：
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出	
*/
BOOL iwdg_config_init(uint8_t prv ,uint16_t rlv)
{
	// 使能 预分频寄存器PR和重装载寄存器RLR可写
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// 设置预分频器值
	IWDG_SetPrescaler( prv );
	
	// 设置重装载寄存器值
	IWDG_SetReload( rlv );
	
	// 把重装载寄存器的值放到计数器中
	IWDG_ReloadCounter();
	
	// 使能 IWDG
	IWDG_Enable();	

	return TRUE;
}

void iwdg_feed(void)
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}

void iwdg_test(void)
{
	//IWDG_Prescaler_64, 625---至少1秒喂狗1次防止复位
	iwdg_config_init(IWDG_Prescaler_64, 625);

	while(1)
	{
		delay_timer_ms(500);
		iwdg_feed();
	}
}


/* WWDG 配置函数
 * tr ：递减计时器的值， 取值范围为：0x7f~0x40,一般我们设置成最大0X7F
 * wr ：窗口值，取值范围为：0x7f~0x40
 * prv：预分频器值，取值可以是
 *      @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1(36mhz)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1(36mhz)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1(36mhz)/4096)/8
 */
BOOL wwdg_config_init(uint8_t tr, uint8_t wr, uint32_t prv)
{
	// 开启 WWDG 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	// 配置WWDG中断优先级
	NVIC_ClearPendingIRQ(WWDG_IRQn);
	NVIC_SetPriority(WWDG_IRQn,0x01);
	NVIC_EnableIRQ(WWDG_IRQn);
	
	// 设置递减计数器的值
	WWDG_SetCounter( tr );

	// 设置上窗口值
	WWDG_SetWindowValue( wr );
	
	// 设置预分频器的值
	WWDG_SetPrescaler( prv );
	
	// 设置递减计数器的值(和WWDG_SetCounter函数重复了)，使能WWDG
	WWDG_Enable( tr );	
	
	// 清除提前唤醒中断标志位
	WWDG_ClearFlag();
	
	// 开WWDG 中断
	WWDG_EnableIT();
	
	return TRUE;
}

void wwdg_feed(void)
{
	// 喂狗，刷新递减计数器的值，设置成最大WDG_CNT=0X7F
	WWDG_SetCounter( WWDG_CNT_MAX );
}

void wwdg_test(void)
{
	//0X7F, 0X5F, WWDG_Prescaler_8--- 1ms减1，0x7f减到0x3f就复位，到0x40就产生中断
	wwdg_config_init(WWDG_CNT_MAX, 0X5F, WWDG_Prescaler_8);

	while(1)
	{
		delay_timer_ms(35);
		wwdg_feed();
	}
}

void WWDG_IRQHandler(void)
{
	// 清除中断标志位
	WWDG_ClearFlag();
	PRINTF("WWDG_IRQHandler\r\n");
}

