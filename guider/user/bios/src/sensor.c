#include "common.h"

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

/*
˯��ģʽ��1���ر��ں�ʱ�ӣ��ں�ֹͣ���������������У�������ϱ���Ϊ����ִ���µĴ��롣���״̬�ᱣ��˯��ǰ���ں˼Ĵ������ڴ�����ݡ�
		  2��û�л����ӳ١�
		  3�����жϻ��ѣ��Ƚ����жϣ��˳��жϷ������󣬽���ִ��WFIָ���ĳ��������¼����ѣ�ֱ�ӽ���ִ��WFE��ĳ���

ֹͣģʽ��1���ں�ֹͣ��Ƭ������Ҳֹͣ�����״̬�ᱣ��ֹͣǰ���ں˼Ĵ������ڴ�����ݡ�
		  2���������ӳ٣������ӳ�ΪHSI����������ʱ�䣬����ѹ�������ڵ͹���ģʽ������Ҫ���ϵ�ѹ���ӵ͹����л�������ģʽ�µ�ʱ�䡣
		  3�������жϻ��ѣ��Ƚ����жϣ��˳��жϷ������󣬽���ִ��WFIָ���ĳ��������¼����ѣ�ֱ�ӽ���ִ��WFE��ĳ���
		  4�����Ѻ�stm32��ʹ��HSI��Ϊϵͳʱ�ӡ�

����ģʽ��1���ں�ֹͣ��Ƭ������Ҳֹͣ���ں˼Ĵ������ڴ�����ݻᶪʧ������λ���š�RTC_AFI���ż�WKUP���ţ�����I/O�ڻṤ���ڸ���̬��
		  2�����ѵ��ӳ�ΪоƬ��λ���¼���
		  3�����Ѻ��൱��оƬ��λ���ڳ������Ϊ��ͷ��ʼִ�д��롣

���������Դ�������磬�������ڵ�RTC�������������У��������ڵļĴ��������ݻᱻ���棬���ܹ���ģʽӰ�졣
*/

#if 1
void EnterInto_SleepMode(void)
{
	//����˯��ģʽ,��һ�жϼ��ɻ��ѣ�
	__WFI();
}

void EnterInto2_SleepMode(void)
{
	//����˯��ģʽ,��һ�¼����ɻ��ѣ�
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
	/* ����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��жϻ��� */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);	
}

void EnterInto2_StopMode(void)
{
	/* ����ֹͣģʽ�����õ�ѹ������Ϊ�͹���ģʽ���ȴ��¼����� */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);	
}

/**
  * @brief  ͣ�����Ѻ�����ϵͳʱ��: ʹ�� HSE, PLL
  *         ����ѡ��PLL��Ϊϵͳʱ��.
  * @param  None
  * @retval None
  */
static void SYSCLKConfig_STOP(void)
{
  /* After wake-up from STOP reconfigure the system clock */
  /* ʹ�� HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  /* �ȴ� HSE ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {
  }
  
  /* ʹ�� PLL */ 
  RCC_PLLCmd(ENABLE);
  
  /* �ȴ� PLL ׼������ */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }
  
  /* ѡ��PLL��Ϊϵͳʱ��Դ */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  /* �ȴ�PLL��ѡ��Ϊϵͳʱ��Դ */
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
	
	//-��ȡ�ձ�����ʱ��ʱ��״̬	
	//ʱ��Դ
	clock_source_wakeup = RCC_GetSYSCLKSource ();
	//ʱ��Ƶ��
	RCC_GetClocksFreq(&clock_status_wakeup);
	
	//��ֹͣģʽ�±����Ѻ�ʹ�õ���HSIʱ�ӣ��˴�����HSEʱ��,ʹ��PLLCLK
	SYSCLKConfig_STOP();
	
	//��ȡ�������ú��ʱ��״̬	
	//ʱ��Դ
	clock_source_config = RCC_GetSYSCLKSource ();
	//ʱ��Ƶ��
	RCC_GetClocksFreq(&clock_status_config);	
	
	DBG_TRACE("wup!!!\r\n");
}
#endif

#if 1
void PVD_config_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/*ʹ�� PWR ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	/* ʹ�� PVD �ж� */
	NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	/* ���� EXTI16��(PVD ���) �����������½����ж�*/
	EXTI_ClearITPendingBit(EXTI_Line16);
	EXTI_InitStructure.EXTI_Line = EXTI_Line16;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* ����PVD����PWR_PVDLevel_2V6 (PVD����ѹ����ֵΪ2.6V��VDD��ѹ����2.6Vʱ����PVD�ж�) */
	/*���弶������Լ���ʵ��Ӧ��Ҫ������*/
	PWR_PVDLevelConfig(PWR_PVDLevel_2V6);
	
	/* ʹ��PVD��� */
	PWR_PVDCmd(ENABLE);
}

void PVD_IRQHandler(void)
{
		/*����Ƿ������PVD�����ź�*/
		if(PWR_GetFlagStatus (PWR_FLAG_PVDO)==SET)			
		{
			/* ʵ��Ӧ����Ӧ�������״̬���� */
		
		}
    /* ����ж��ź�*/
    EXTI_ClearITPendingBit(EXTI_Line16);

}
#endif

