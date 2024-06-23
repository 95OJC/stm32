#include "common.h"
#include "systick_rcc.h"

/*-----------------------------------------------------------------
��֤rcc���ܺ�mco���ܡ�systick��ʱ����

 * ʹ��HSEʱ������ϵͳʱ�ӵĲ���
 * 1������HSE�����ȴ��ȶ�������ʧ�ܻ�ʱ�����������ִ�д������
 * 2������AHB��APB1��APB2�ķ�Ƶ����
 * 3������PLL��ʱ����Դ����PLL��Ƶ���ӣ����ø���Ƶ����Ҫ������������
 * 4������PLL�����ȴ��ȶ�
 * 5����PLLCLK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 * 7������ʱ��ʧ��ʱ��ִ�д������

 * ʹ��HSIʱ������ϵͳʱ�ӵĲ���
 * 1������HSI�����ȴ��ȶ�����ʧ�ܻ�ʱ�����������ִ�д������
 * 2������AHB��APB1��APB2�ķ�Ƶ����
 * 3������PLL��ʱ����Դ����PLL��Ƶ���ӣ����ø���Ƶ����Ҫ������������
 * 4������PLL�����ȴ��ȶ�
 * 5����PLLCLK�л�Ϊϵͳʱ��SYSCLK
 * 6����ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
 * 7������ʱ��ʧ��ʱ��ִ�д������


 * ���� ϵͳʱ��:SYSCLK , AHB����ʱ��:HCLK , APB2����ʱ��:PCLK2 , APB1����ʱ��:PCLK1
 * PCLK2 = HCLK = SYSCLK
 * PCLK1 = HCLK/2�����ֻ����36M
 * HSE��Ϊʱ����Դ������PLL��Ƶ��Ϊϵͳʱ�ӣ�����ͨ��������


mco���ܣ�
1����gpioʱ��
2������mco���ù���
3�����ÿ⺯��RCC_MCOConfig(x),xΪѡ��Ĳɼ�ʱ��


systick��ʱ���ܣ�
1��SysTick_Config����ΪST���ṩ
2�������Ϊ72��1us����SYSTICK_UNIT_1MS��1ms
3���������0xffff�Ĳ�������Ϊload������16λ��
4����SYSHANDLE_COUNT����Ϊ���붨ʱ������

 ------------------------------------------------------------------*/

void hse_sysclk_init(void)
{
	__IO uint32_t HSEstatus = 0;
	__IO uint32_t startUpCounter = 0;

	//��RCC�����ʼ��Ϊ��λ״̬������Ǳ����
  	RCC_DeInit();
	
	//ʹ��HSE�������ⲿ����Ұ�𿪷����õ���8M��
	RCC->CR |= (uint32_t)RCC_CR_HSEON;

	//�ȴ�HSE�����ȶ���������ʱ����
	do{
		HSEstatus = RCC->CR & RCC_CR_HSERDY;
		startUpCounter++;
	}while((HSEstatus == 0)
			&&(startUpCounter != HSE_STARTUP_TIMEOUT)
		  );

	//HSE�����ȶ������������ִ��
	if(HSEstatus == RCC_CR_HSERDY)
	{
		#if 0	//�����ǲ���FLASH�����õ��ģ����������FLASH��ע�͵�ҲûӰ��
		//ʹ�� FLASH Ԥ��ȡ������
		FLASH->ACR |= FLASH_ACR_PRFTBE;
		
		/*	
		 *	SYSCLK�������������ʱ��ı������ã�ͳһ����Ϊ2��
		 *  ��Ϊ2ʱ��SYSCLK����48MҲ���Թ������������Ϊ0��1��
		 *  SYSCLK������Χ�����Ӳ������
		 *  0:	 0 < SYSCLK <= 24M
		 *  1:	24 < SYSCLK <= 48M
		 *  2:	48 < SYSCLK <= 72M	
		 */
		FLASH->ACR &= (uint32_t)(~FLASH_ACR_LATENCY);
		FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
		#endif
		
		//����AHP��APB2��APB1��Ƶ����
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;	//λ7:4  	HCLK = SYSCLK
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;	//λ13:11 	PCLK2 = HCLK
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;	//λ10:8 	PCLK1 = HCLK/2

		//����PLLʱ����Դ������PLL��Ƶ���ӣ�PLLCLK = HSE*9 = 72M Hz
		RCC->CFGR &= (uint32_t)(~(RCC_CFGR_PLLSRC		//λ16		�����ʱ����ΪPLL����ʱ��Դ
		 						| RCC_CFGR_PLLXTPRE		//λ17		��HSE����Ƶ��PLL
		 						| RCC_CFGR_PLLMULL));	//λ21:18	��PLL��Ƶ����
		 						
		 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE	//λ16		ʹ��HSE��ΪPLL����ʱ��Դ
		 						| RCC_CFGR_PLLMULL9);	//λ21:18	PLL��Ƶ����Ϊ9

		//ʹ��PLL
		RCC->CR |= RCC_CR_PLLON;

		//�ȴ�PLL�ȶ�
		while((RCC->CR & RCC_CR_PLLRDY) == 0);

		//ѡ��PLL��Ϊϵͳʱ����Դ
		RCC->CFGR &= (~RCC_CFGR_SW);
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		//��ȡʱ���л�״̬λ��ȷ��PLLCLK��ѡΪϵͳʱ��
		while((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

	}
	else
	{
		//���HSE����ʧ�ܣ���Ӵ������
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
	
	//����MCO�������ʱ�ӣ���ʾ����������PA8�����������ʱ���ź�
	//MCO�������������HSE,HSI,PLLCLK/2,SYSCLK
	//RCC_MCOConfig(RCC_MCO_HSE);	             // 8M	        
	//RCC_MCOConfig(RCC_MCO_HSI);	           // 8M 	        
	RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    // 36M		
	//RCC_MCOConfig(RCC_MCO_SYSCLK);		     // 72M  
}


void systick_init(void)
{
	//ϵͳʱ��Ϊ72M����T=1/72M������72000��T=1/1000(s)��Ϊÿ�ν��ж���1ms��
	//�ò������ܳ���16λ��
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


