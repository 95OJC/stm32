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

	/* ��ֹ MPU */
	HAL_MPU_Disable();
#if 0
   	/* ��ǿ���ܡ�����AXI SRAM��MPU����ΪWrite back, Read allocate��Write allocate */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;//��������MPU
	MPU_InitStruct.BaseAddress      = 0x24000000;//������ʼ��ַ����ַ��Ҫ32�ֽڶ���
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;//���ô�С����32�ֽڵ�������
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;//��Ȩ�û�����ͨ�û����ܶ�д
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;//�򿪻���buffer����дcache
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;//�򿪻���cache���ɶ�cache
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;//�رչ���
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;//�ڴ�����1�����16
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;//����cache�Ĳ��ԣ��ȼ�0Ϊ��normalģʽ����֧�ַǶ�����ʣ��ȼ�1Ϊnormalģʽ������ʹ��cache����֧�ַǶ������
	MPU_InitStruct.SubRegionDisable = 0x00;//������һ����0����
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;//�����û�����ִ�и��������
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
 #else
	/* �ʺϳ�ѧ��ʹ�á�����AXI SRAM��MPU����ΪNORMAL, NO Read allocate��NO Write allocate */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;//��������MPU
	MPU_InitStruct.BaseAddress      = 0x24000000;//������ʼ��ַ����ַ��Ҫ32�ֽڶ���
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;//���ô�С����32�ֽڵ�������
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;//��Ȩ�û�����ͨ�û����ܶ�д
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;//�رջ���buffer
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;//�رջ���cache
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;//�رչ���
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;//�ڴ�����1�����16
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;//����cache�Ĳ��ԣ��ȼ�0Ϊ��normalģʽ����֧�ַǶ�����ʣ��ȼ�1Ϊnormalģʽ������ʹ��cache����֧�ַǶ������
	MPU_InitStruct.SubRegionDisable = 0x00;//������һ����0����
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;//�����û�����ִ�и��������
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif
	/*ʹ�� MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

int main(void)
{
	u8 data = 0;
	
	/* ����480Mhz��ʱ�ӣ�64Mhzʱ�Ӽ�������MPU��cache */
	MPU_Config();    /* ����MPU */
	SCB_EnableICache();	/* ʹ�� I-Cache */
	SCB_EnableDCache();	/* ʹ�� D-Cache */
	
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
			/* ��ת�ɹ��Ļ�������ִ�е����� */
			DBG_TRACE("JumpToApp error!\r\n");
			HAL_Delay(1000);
		}
	}

}



