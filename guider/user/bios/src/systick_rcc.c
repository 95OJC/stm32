#include "common.h"
#include "systick_rcc.h"

/*-----------------------------------------------------------------
验证rcc功能和mco功能、systick定时功能

 * 使用HSE时，设置系统时钟的步骤
 * 1、开启HSE，并等待稳定，启动失败或超时，则跳到最后执行错误代码
 * 2、设置AHB、APB1、APB2的分频因子
 * 3、设置PLL的时钟来源，和PLL倍频因子，设置各种频率主要就是这里设置
 * 4、开启PLL，并等待稳定
 * 5、把PLLCLK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 * 7、启动时钟失败时，执行错误代码

 * 使用HSI时，设置系统时钟的步骤
 * 1、开启HSI，并等待稳定启动失败或超时，则跳到最后执行错误代码
 * 2、设置AHB、APB1、APB2的分频因子
 * 3、设置PLL的时钟来源，和PLL倍频因子，设置各种频率主要就是这里设置
 * 4、开启PLL，并等待稳定
 * 5、把PLLCLK切换为系统时钟SYSCLK
 * 6、读取时钟切换状态位，确保PLLCLK被选为系统时钟
 * 7、启动时钟失败时，执行错误代码


 * 设置 系统时钟:SYSCLK , AHB总线时钟:HCLK , APB2总线时钟:PCLK2 , APB1总线时钟:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2，最高只能是36M
 * HSE作为时钟来源，经过PLL倍频作为系统时钟，这是通常的做法


mco功能：
1、打开gpio时钟
2、配置mco复用功能
3、调用库函数RCC_MCOConfig(x),x为选择的采集时钟


systick定时功能：
1、SysTick_Config函数为ST库提供
2、填参数为72是1us，宏SYSTICK_UNIT_1MS是1ms
3、不能填超过0xffff的参数，因为load加载是16位的
4、宏SYSHANDLE_COUNT可作为进入定时器次数

 ------------------------------------------------------------------*/

void hse_sysclk_init(void)
{
	__IO uint32_t HSEstatus = 0;
	__IO uint32_t startUpCounter = 0;

	//把RCC外设初始化为复位状态，这句是必须的
  	RCC_DeInit();
	
	//使能HSE，开启外部晶振（野火开发板用的是8M）
	RCC->CR |= (uint32_t)RCC_CR_HSEON;

	//等待HSE启动稳定，并做超时处理
	do{
		HSEstatus = RCC->CR & RCC_CR_HSERDY;
		startUpCounter++;
	}while((HSEstatus == 0)
			&&(startUpCounter != HSE_STARTUP_TIMEOUT)
		  );

	//HSE启动稳定，则继续往下执行
	if(HSEstatus == RCC_CR_HSERDY)
	{
		#if 0	//这里是操作FLASH闪存用到的，如果不操作FLASH，注释掉也没影响
		//使能 FLASH 预存取缓冲区
		FLASH->ACR |= FLASH_ACR_PRFTBE;
		
		/*	
		 *	SYSCLK周期与闪存访问时间的比例设置，统一设置为2，
		 *  设为2时，SYSCLK低于48M也可以工作，如果设置为0或1，
		 *  SYSCLK超出范围则进入硬件错误。
		 *  0:	 0 < SYSCLK <= 24M
		 *  1:	24 < SYSCLK <= 48M
		 *  2:	48 < SYSCLK <= 72M	
		 */
		FLASH->ACR &= (uint32_t)(~FLASH_ACR_LATENCY);
		FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
		#endif
		
		//设置AHP、APB2、APB1分频因子
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;	//位7:4  	HCLK = SYSCLK
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;	//位13:11 	PCLK2 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;	//位10:8 	PCLK1 = HCLK/2

		//设置PLL时钟来源，设置PLL倍频因子，PLLCLK = HSE*9 = 72M Hz
		RCC->CFGR &= (uint32_t)(~(RCC_CFGR_PLLSRC		//位16		清各个时钟作为PLL输入时钟源
		 						| RCC_CFGR_PLLXTPRE		//位17		清HSE二分频给PLL
		 						| RCC_CFGR_PLLMULL));	//位21:18	清PLL倍频因子
		 						
		 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE	//位16		使能HSE作为PLL输入时钟源
		 						| RCC_CFGR_PLLMULL9);	//位21:18	PLL倍频因子为9

		//使能PLL
		RCC->CR |= RCC_CR_PLLON;

		//等待PLL稳定
		while((RCC->CR & RCC_CR_PLLRDY) == 0);

		//选择PLL作为系统时钟来源
		RCC->CFGR &= (~RCC_CFGR_SW);
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		//读取时钟切换状态位，确保PLLCLK被选为系统时钟
		while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

	}
	else
	{
		//如何HSE启动失败，添加错误代码
		while(1);
	}
	
}


void mco_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//设置MCO引脚输出时钟，用示波器即可在PA8测量到输出的时钟信号
	//MCO引脚输出可以是HSE,HSI,PLLCLK/2,SYSCLK
	//RCC_MCOConfig(RCC_MCO_HSE);	             // 8M	        
	//RCC_MCOConfig(RCC_MCO_HSI);	           // 8M 	        
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    // 36M		
	//RCC_MCOConfig(RCC_MCO_SYSCLK);		     // 72M  
}


void systick_init(void)
{
	//系统时钟为72M，即T=1/72M，传入72000则T=1/1000(s)则为每次进中断需1ms。
	//该参数不能超出16位。
	SysTick_Config(SYSTICK_UNIT_1MS);
}

void SysTick_Handler(void)
{
	static uint16_t count = 0;
	count++;
	if(count == SYSHANDLE_COUNT)
	{
		led_toggle();
		count = 0;
	}
}


