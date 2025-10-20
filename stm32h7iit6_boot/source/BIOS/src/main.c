#include "common.h"

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


void Error_Handler(void)
{
	DI();
	while (1)
	{}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Supply configuration update enable
	*/
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

	/** Configure the main internal regulator output voltage
	*/
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 15;
	RCC_OscInitStruct.PLL.PLLN = 216;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_0;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
							  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
	Error_Handler();
	}
}

static void MPU_Config( void )
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* 禁止 MPU */
	HAL_MPU_Disable();
#if 0
   	/* 最强性能。配置AXI SRAM的MPU属性为Write back, Read allocate，Write allocate */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;//允许配置MPU
	MPU_InitStruct.BaseAddress      = 0x24000000;//配置起始地址，地址需要32字节对齐
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;//配置大小，需32字节的整数倍
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;//特权用户和普通用户均能读写
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;//打开缓冲buffer，可写cache
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;//打开缓存cache，可读cache
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;//关闭共享
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;//内存区域1，最大16
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;//配置cache的策略：等级0为非normal模式，不支持非对齐访问；等级1为normal模式，可以使用cache并且支持非对齐访问
	MPU_InitStruct.SubRegionDisable = 0x00;//子区域一般填0禁用
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;//所有用户可以执行该区域代码
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
 #else
	/* 适合初学者使用。配置AXI SRAM的MPU属性为NORMAL, NO Read allocate，NO Write allocate */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;//允许配置MPU
	MPU_InitStruct.BaseAddress      = 0x24000000;//配置起始地址，地址需要32字节对齐
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;//配置大小，需32字节的整数倍
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;//特权用户和普通用户均能读写
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;//关闭缓冲buffer
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;//关闭缓存cache
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;//关闭共享
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;//内存区域1，最大16
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;//配置cache的策略：等级0为非normal模式，不支持非对齐访问；等级1为normal模式，可以使用cache并且支持非对齐访问
	MPU_InitStruct.SubRegionDisable = 0x00;//子区域一般填0禁用
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;//所有用户可以执行该区域代码
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif
	/*使能 MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

int main(void)
{
	u8 data = 0;
	
	/* 无需480Mhz的时钟，64Mhz时钟即可配置MPU和cache */
	MPU_Config();    /* 配置MPU */
	SCB_EnableICache();	/* 使能 I-Cache */
	SCB_EnableDCache();	/* 使能 D-Cache */
	
	HAL_Init();
	SystemClock_Config();

    if_init();
	DBG_OPEN();


	while (1)
	{
		DBG_TRACE("boot:JumpToApp...\r\n\r\n");
		//JumpToApp();
		
		while(1)
		{
			/* 跳转成功的话，不会执行到这里 */
			DBG_TRACE("JumpToApp error!\r\n");
			HAL_Delay(1000);
		}
	}

}



