#include "common.h"

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

/*
睡眠模式：1、关闭内核时钟，内核停止，而外设正常运行，在软件上表现为不再执行新的代码。这个状态会保留睡眠前的内核寄存器、内存的数据。
		  2、没有唤醒延迟。
		  3、若中断唤醒，先进入中断，退出中断服务程序后，接着执行WFI指令后的程序；若由事件唤醒，直接接着执行WFE后的程序。

停止模式：1、内核停止，片上外设也停止。这个状态会保留停止前的内核寄存器、内存的数据。
		  2、唤醒有延迟，基础延迟为HSI震荡器的启动时间，若调压器工作在低功耗模式，还需要加上调压器从低功耗切换至正常模式下的时间。
		  3、若由中断唤醒，先进入中断，退出中断服务程序后，接着执行WFI指令后的程序；若由事件唤醒，直接接着执行WFE后的程序。
		  4、唤醒后，stm32会使用HSI作为系统时钟。

待机模式：1、内核停止，片上外设也停止；内核寄存器、内存的数据会丢失。除复位引脚、RTC_AFI引脚及WKUP引脚，其他I/O口会工作在高阻态。
		  2、唤醒的延迟为芯片复位的事件。
		  3、唤醒后，相当于芯片复位，在程序表现为从头开始执行代码。

若备份域电源正常供电，备份域内的RTC都可以正常运行，备份域内的寄存器的数据会被保存，不受功耗模式影响。
*/

#if 1
void EnterInto_SleepMode(void)
{
	//进入睡眠模式,任一中断即可唤醒！
	__WFI();
}

void EnterInto2_SleepMode(void)
{
	//进入睡眠模式,任一事件即可唤醒！
	__WFE();
}

void SleepMode_test(void)
{
	led_test();
	key_test();
	DBG_TRACE("sleep 3\r\n");	
	delay_timer_ms(500);
	delay_timer_ms(500);
	DBG_TRACE("sleep 2\r\n");
	delay_timer_ms(500);
	delay_timer_ms(500);
	DBG_TRACE("sleep 1\r\n");
	delay_timer_ms(500);
	delay_timer_ms(500);	
	DBG_TRACE("sleep 0\r\n");
	EnterInto_SleepMode();
	DBG_TRACE("wup!!!\r\n");
}
#endif

#if 1
void EnterInto_StopMode(void)
{
	/* 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
}

void EnterInto2_StopMode(void)
{
	/* 进入停止模式，设置电压调节器为低功耗模式，等待事件唤醒 */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);	
}

/**
  * @brief  停机唤醒后配置系统时钟: 使能 HSE, PLL
  *         并且选择PLL作为系统时钟.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* 使能 HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* 等待 HSE 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* 使能 PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* 等待 PLL 准备就绪 */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* 选择PLL作为系统时钟源 */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* 等待PLL被选择为系统时钟源 */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  }
}

void StopMode_test(void)
{
	RCC_ClocksTypeDef clock_status_wakeup,clock_status_config;
	uint8_t clock_source_wakeup,clock_source_config; 
	
	led_test();
	key_test();
	DBG_TRACE("sleep 3\r\n");	
	delay_timer_ms(500);
	delay_timer_ms(500);
	DBG_TRACE("sleep 2\r\n");
	delay_timer_ms(500);
	delay_timer_ms(500);
	DBG_TRACE("sleep 1\r\n");
	delay_timer_ms(500);
	delay_timer_ms(500);	
	DBG_TRACE("sleep 0\r\n");
	
	EnterInto_StopMode();
	
	//-获取刚被唤醒时的时钟状态	
	//时钟源
	clock_source_wakeup = RCC_GetSYSCLKSource ();
	//时钟频率
	RCC_GetClocksFreq(&clock_status_wakeup);
	
	//从停止模式下被唤醒后使用的是HSI时钟，此处重启HSE时钟,使用PLLCLK
	SYSCLKConfig_STOP();
	
	//获取重新配置后的时钟状态	
	//时钟源
	clock_source_config = RCC_GetSYSCLKSource ();
	//时钟频率
	RCC_GetClocksFreq(&clock_status_config);	
	
	DBG_TRACE("wup!!!\r\n");
}
#endif

#if 1
void PVD_config_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/*使能 PWR 时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* 使能 PVD 中断 */
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	/* 配置 EXTI16线(PVD 输出) 来产生上升下降沿中断*/
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* 配置PVD级别PWR_PVDLevel_2V6 (PVD检测电压的阈值为2.6V，VDD电压低于2.6V时产生PVD中断) */
	/*具体级别根据自己的实际应用要求配置*/
	PWR_PVDLevelConfig(PWR_PVDLevel_2V6);
	
	/* 使能PVD输出 */
	PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
		/*检测是否产生了PVD警告信号*/
		if(PWR_GetFlagStatus (PWR_FLAG_PVDO)==SET)			
		{
			/* 实际应用中应进入紧急状态处理 */
		
		}
    /* 清除中断信号*/
    EXTI_ClearITPendingBit(EXTI_Line16);

}
#endif

