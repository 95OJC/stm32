#include "common.h"

/*-----------------------------------------------------------------
����������ֲ�ײ�ע�����
1���鿴��·ͼ��IO���Ƿ��б仯����led��keyֻ��Ҫ�ײ��޸�gpio.h�ĺ꼴�ɣ���uart��Ҫ�޸ġ�����
2���ϲ㲻���޸ģ���Ϊ�ϲ�ֻ��Ҫ֪��led1��2��key1��2֮�ഫ��ȥ���ɣ�ͬʱ�ײ㷵��led״̬������(�����Ҫ�����ϲ㴫���ײ��led_num���ײ����num���ó�ʼ��led�����Ϳ���״̬�����ϲ�)��


//��������DevMain����Ϊ�˲�����ϲ�ͷ�ļ�����ʹ���ϲ��DevMain����(ʵ���ϲ�͵ײ�ĳ���ֲ�)��
//�ײ���øú�����ʼ���ϲ�Ӧ�ã�ֻ�н�ȥ�ú�����������ϲ�������ˡ�
//���ϲ���Ҫ���õײ㺯����������biosApi.h�����ײ㺯������Ϊ�ϲ�����˸�ͷ�ļ�������ϲ������(biosApi.h����Ҫ��common.h���������Ǳ�lovgdev.h����)��
------------------------------------------------------------------*/

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

static uint8_t *compiler_data = __DATE__;
static uint8_t *compiler_time = __TIME__;



void DevMain(void);
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�

	DBG_OPEN();
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

#if 0
	DevMain();//��ʼ��Ӳ���ʹ�������������
#endif

#if 0
	pwm_test();
#endif

#if 0
	//SleepMode_test();
	//StopMode_test();
	//PVD_config_init();//����PVD������ѹ����ʱ��������жϷ������������
#endif

#if 0
	//iwdg_test();
	//wwdg_test();
#endif

#if 0
	//dma_m2m_test();
	//dma_m2m_interrupt_test();
	//dma_m2p_test(M2P_USART);
	//dma_m2p_interrupt_test(M2P_USART);
	//dma_p2m_test(P2M_USART);
	//dma_p2m_interrupt_test(P2M_USART);
#endif

#if 0
		extern __IO u16 AD_Value;
		float AD_getV;
		ad_init();
		while(1)
		{
			AD_getV = (float)(AD_Value*3.3)/4095;
			printf("AD_Value = %d\r\n",AD_Value);
			printf("ADC_get_V = %f\r\n",AD_getV);
			printf("\r\n");
			delay_timer_ms(500);
		}	
	
#endif

#if 0
	da_init();//�򿪴���debug�������������ѹ����û��ԭ��
#endif

#if 0//test iic eeprom
	u8 test_TxBuf[10];
	MEM_SET(test_TxBuf, 1, sizeof(test_TxBuf));
	u8 test_RxBuf[10];
	MEM_SET(test_RxBuf, 0, sizeof(test_RxBuf));
	#if 1
	eeprom_init();
	eeprom_write(0,test_TxBuf);
	Delay_ojc(0x4fffff);
	eeprom_read(0,test_RxBuf);
	printf("test_RxBuf[0] = %d\r\n",test_RxBuf[0]);
	#else
	AT24C02eeprom_init();
	AT24C02eeprom_write(0xaa,test_TxBuf);
	Delay_ojc(0x4fffff);
	AT24C02eeprom_read(0xaa,test_RxBuf);
	printf("test_RxBuf[0] = %d\r\n",test_RxBuf[0]);
	#endif
#endif

#if 0//test spi flash
	DEV_STATUS ret;
	u8 rxbuf[512];

	spi_flash_init();
	
	//ret = spi_flash_read(0x101010,rxbuf,sizeof(rxbuf));
	ret = spi_flash_read_jid(rxbuf);

	if(ret == DEV_OK)
	{
		printf("flash read OK!\r\n");
		printf("rxbuf[0] = 0x%x\r\n",rxbuf[0]);
		printf("rxbuf[1] = 0x%x\r\n",rxbuf[1]);
		printf("rxbuf[2] = 0x%x\r\n",rxbuf[2]);
	}
	
#endif

	while(1);
}




