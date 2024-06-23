#include "common.h"


/* 
IWDG���ú���:
	MCU�ڲ���LSIʱ�ӻ���30~60kHz֮��仯��������40kHzʱ�Ӹ���iwgd�� 
	Ȼ��40��prv�ó�����1�ε�ʱ��Ƶ�ʣ��ٳ�rlv���ó����ٵ�ι��ʱ�䡣

 * prv������[4,8,16,32,64,128,256]
 * prv:Ԥ��Ƶ��ֵ��ȡֵ���£�
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:Ԥ��Ƶ��ֵ��ȡֵ��ΧΪ��0-0XFFF
 * �������þ�����
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���	
*/
BOOL iwdg_config_init(uint8_t prv ,uint16_t rlv)
{
	// ʹ�� Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// ����Ԥ��Ƶ��ֵ
	IWDG_SetPrescaler( prv );
	
	// ������װ�ؼĴ���ֵ
	IWDG_SetReload( rlv );
	
	// ����װ�ؼĴ�����ֵ�ŵ���������
	IWDG_ReloadCounter();
	
	// ʹ�� IWDG
	IWDG_Enable();	

	return TRUE;
}

void iwdg_feed(void)
{
	// ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
	// ����������ֵ����0��ʱ������ϵͳ��λ
	IWDG_ReloadCounter();
}

void iwdg_test(void)
{
	//IWDG_Prescaler_64, 625---����1��ι��1�η�ֹ��λ
	iwdg_config_init(IWDG_Prescaler_64, 625);

	while(1)
	{
		delay_timer_ms(500);
		iwdg_feed();
	}
}


/* WWDG ���ú���
 * tr ���ݼ���ʱ����ֵ�� ȡֵ��ΧΪ��0x7f~0x40,һ���������ó����0X7F
 * wr ������ֵ��ȡֵ��ΧΪ��0x7f~0x40
 * prv��Ԥ��Ƶ��ֵ��ȡֵ������
 *      @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1(36MHZ)/4096)/1
 *      @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1(36mhz)/4096)/2
 *      @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1(36mhz)/4096)/4
 *      @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1(36mhz)/4096)/8
 */
BOOL wwdg_config_init(uint8_t tr, uint8_t wr, uint32_t prv)
{
	// ���� WWDG ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	// ����WWDG�ж����ȼ�
	NVIC_ClearPendingIRQ(WWDG_IRQn);
	NVIC_SetPriority(WWDG_IRQn,0x01);
	NVIC_EnableIRQ(WWDG_IRQn);
	
	// ���õݼ���������ֵ
	WWDG_SetCounter( tr );

	// �����ϴ���ֵ
	WWDG_SetWindowValue( wr );
	
	// ����Ԥ��Ƶ����ֵ
	WWDG_SetPrescaler( prv );
	
	// ���õݼ���������ֵ(��WWDG_SetCounter�����ظ���)��ʹ��WWDG
	WWDG_Enable( tr );	
	
	// �����ǰ�����жϱ�־λ
	WWDG_ClearFlag();
	
	// ��WWDG �ж�
	WWDG_EnableIT();
	
	return TRUE;
}

void wwdg_feed(void)
{
	// ι����ˢ�µݼ���������ֵ�����ó����WDG_CNT=0X7F
	WWDG_SetCounter( WWDG_CNT_MAX );
}

void wwdg_test(void)
{
	//0X7F, 0X5F, WWDG_Prescaler_8--- 1ms��1��0x7f����0x3f�͸�λ����0x40�Ͳ����ж�
	wwdg_config_init(WWDG_CNT_MAX, 0X5F, WWDG_Prescaler_8);

	while(1)
	{
		delay_timer_ms(35);
		wwdg_feed();
	}
}

void WWDG_IRQHandler(void)
{
	// ����жϱ�־λ
	WWDG_ClearFlag();
	PRINTF("WWDG_IRQHandler\r\n");
}

