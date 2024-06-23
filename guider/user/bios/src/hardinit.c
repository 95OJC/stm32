#include "common.h"

/*-----------------------------------------------------------------
����������ֲ�ײ�ע�����
1���鿴��·ͼ��IO���Ƿ��б仯����led��keyֻ��Ҫ�ײ��޸�gpio.h�ĺ꼴�ɣ���uart��Ҫ�޸ġ�����
2���ϲ㲻���޸ģ���Ϊ�ϲ�ֻ��Ҫ֪��led1��2��key1��2֮�ഫ��ȥ���ɣ�ͬʱ�ײ㷵��led״̬������(�����Ҫ�����ϲ㴫���ײ��led_num���ײ����num���ó�ʼ��led�����Ϳ���״̬�����ϲ�)��


//��������DevMain����Ϊ�˲�����ϲ�ͷ�ļ�����ʹ���ϲ��DevMain����(ʵ���ϲ�͵ײ�ĳ���ֲ�)��
//�ײ���øú�����ʼ���ϲ�Ӧ�ã�ֻ�н�ȥ�ú�����������ϲ�������ˡ�
//���ϲ���Ҫ���õײ㺯����������biosApi.h�����ײ㺯������Ϊ�ϲ�����˸�ͷ�ļ�������ϲ������(biosApi.h����Ҫ��common.h���������Ǳ�lovgdev.h����)��
------------------------------------------------------------------*/
#if 1

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
	//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	

	DBG_OPEN();
	
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

	//��ʼ��Ӳ�� �� ��������������
	DevMain();

	while(1);
}

#else
static void OLED_normal_mode(void)
{
	OLED_ShowCHinese(0,0,5);//��
	OLED_ShowCHinese(16,0,11);//��
	OLED_ShowCHinese(32,0,3);//״
	OLED_ShowCHinese(48,0,4);//̬
}

static void OLED_heat_mode(void)
{
	OLED_ShowCHinese(0,0,1);//��
	OLED_ShowCHinese(16,0,2);//��
	OLED_ShowCHinese(32,0,3);//״
	OLED_ShowCHinese(48,0,4);//̬
}

static void OLED_errWater_mode(void)
{
	OLED_ShowCHinese(0,0,0);//ȱ
	OLED_ShowCHinese(16,0,10);//ˮ
	OLED_ShowCHinese(32,0,3);//״
	OLED_ShowCHinese(48,0,4);//̬
}


static void OLED_WaterTemp_mode(void)
{
	OLED_ShowCHinese(0,4,8);//��
	OLED_ShowCHinese(16,4,9);//ǰ
	OLED_ShowCHinese(32,4,10);//ˮ
	OLED_ShowCHinese(48,4,11);//��
	OLED_ShowChar(64,4,':',16);//:
	OLED_ShowNum(80,4,HUNDRED_PLACE(GET_CUR_WATER_T()),1,16);//ˮ�°�λ
	OLED_ShowNum(96,4,TEN_PLACE(GET_CUR_WATER_T()),1,16);//ˮ��ʮλ
	OLED_ShowNum(112,4,UNIT_PLACE(GET_CUR_WATER_T()),1,16);//ˮ�¸�λ	
	
	OLED_ShowCHinese(0,2,6);//Ԥ
	OLED_ShowCHinese(16,2,7);//��
	OLED_ShowCHinese(32,2,10);//ˮ
	OLED_ShowCHinese(48,2,11);//��
	OLED_ShowChar(64,2,':',16);//:
	OLED_ShowNum(80,2,HUNDRED_PLACE(GET_PRE_WATER_T()),1,16);//ˮ�°�λ
	OLED_ShowNum(96,2,TEN_PLACE(GET_PRE_WATER_T()),1,16);//ˮ��ʮλ
	OLED_ShowNum(112,2,UNIT_PLACE(GET_PRE_WATER_T()),1,16);//ˮ�¸�λ
}

static void monitor_OLED(void)
{
	mode_state modeState;
	
	modeState = GET_DEV_MOED();
	if(modeState == NORMAL_MODE)
	{
		OLED_normal_mode();
	}
	else if(modeState == HEAT_MODE)
	{
		OLED_heat_mode();
	}
	else if(modeState == ERR_WATER_MODE)
	{
		OLED_errWater_mode();
	}

	OLED_WaterTemp_mode();
	
}

static void fourKey_scan(void)
{
		u8 key_val;
	
		key_val = keyTTP224_scan();
		switch (key_val)
			{
				case KEY_PUMP://ˮ��
					if(GET_DEV_MOED() == NORMAL_MODE)
					{
						if(DCmotor_toggle())//����״̬�ſ��Գ�ˮ
						{
							SET_PUMP_STATE();
							printf("��ˮ��\r\n");
						}
						else
						{
							CLS_PUMP_STATE();
							printf("�ر�ˮ��\r\n");
						}
					}
					break;		
				case KEY_HEAT://����
					if(!IS_PUMP_STATE())//�ǳ�ˮʱ�ſ��Լ���
					{
						if(heater_toggle())
						{
							SET_DEV_MODE(HEAT_MODE);
							printf("�򿪼���\r\n");
						}
						else
						{
							SET_DEV_MODE(NORMAL_MODE);
							printf("�رռ���\r\n");
						}
					}
					break;	
				case KEY_TEMP_UP://Ԥ��ˮ��+
					if(GET_PRE_WATER_T() < 100 && GET_PRE_WATER_T() >= 0)//��Χ��0~100
					{
						SET_PRE_WATER_T(GET_PRE_WATER_T()++);
						printf("����+1\r\n");
					}
					break;
				case KEY_TEMP_DOWN://Ԥ��ˮ��-
					if(GET_PRE_WATER_T() <= 100 && GET_PRE_WATER_T() > 0)//��Χ��0~100
					{
						SET_PRE_WATER_T(GET_PRE_WATER_T()--);
						printf("����-1\r\n");
					}
					break;
				default:
					break;
			}
}

void DevMain(void);
int main(void)
{	
	u16 Wtemp = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�
	DevMain();//��ʼ��Ӳ��
	bios_dev_init();//��ʼ���豸��Ϣ
	
	while(1)
	{
		Wtemp = 0;
		/* ʵʱ���ˮλ����ȡ������ˮ�� */
		if(1)//if(WaterSensor_monitor())
		{
			CLS_ERR_WATER();
			Wtemp = DS18B20_ReadTemperature();
			SET_CUR_WATER_T(Wtemp);//���õ�ǰˮ�µ�ȫ�ֱ���
		}
		else
		{
			SET_ERR_WATER();		
		}
	
		/* ʵʱ��ʾOLED */
		if(!IS_ERR_WATER())
		{
			/* ��ǰˮ�´ﵽԤ��ˮ��֮��,�����Ǽ���ģʽʱ����ֹͣ���� */
			if( (GET_CUR_WATER_T() >= GET_PRE_WATER_T()) && (GET_DEV_MOED() == HEAT_MODE) )
			{
				heater_off();//�رռ���
				SET_DEV_MODE(NORMAL_MODE);
				printf("�رռ���\r\n");
			}
			monitor_OLED();
		}
		else
		{//ȱˮ״̬,��ʼ���豸��Ϣ������ֹͣ������
			bios_dev_init();
			DCmotor_off();
			heater_off();
			
			SET_DEV_MODE(ERR_WATER_MODE);			
			monitor_OLED();
			while(1);//��Ҫ��ˮ�����¿�����
		}
		/* ʵʱ��ⰴ�����룬�ı���ʾ������״̬��(Ԥ��)ˮ�� */
		fourKey_scan();
	}

#if 0
	da_init();//ÿ�����1��DACת��ֵ��Ŀǰ�ǹ̶�2048����3.3V��һ�룩
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

#if 0//test dma
	#define BUFFER_SIZE     32
			u32 aSRC_Const_Buffer[BUFFER_SIZE]= {
												0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
												0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
												0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
												0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
												0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
												0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
												0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
												0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
			
			u32 aDST_Buffer[BUFFER_SIZE];
		
			DMACTRL dma_test;
			dma_test.PeripheralBaseAddr = (u32)aSRC_Const_Buffer;
			dma_test.MemoryBaseAddr = (u32)aDST_Buffer;
			dma_test.DIR = DMA_DIR_PeripheralSRC;
			dma_test.BufferSize = BUFFER_SIZE;
			dma_test.PeripheralInc = DMA_PeripheralInc_Enable;
			dma_test.MemoryInc = DMA_MemoryInc_Enable;
			dma_test.PeripheralDataSize = DMA_PeripheralDataSize_Word;
			dma_test.MemoryDataSize = DMA_MemoryDataSize_Word;
			dma_test.Mode = DMA_Mode_Normal;
			dma_test.Priority = DMA_Priority_High;
			dma_test.M2M = DMA_M2M_Enable;
			dma_m2m_init(&dma_test);
		
			while(DMA_GetFlagStatus(DMA_FLAG_TC)==RESET);
		
			printf("dma m2m success!\r\n");
#endif

#if 0//test macro.h
		typedef struct _NEW_STRUCT{
			uint8_t a;
			uint8_t b;
			uint8_t c;
		}NEW_STRUCT;
	
		NEW_STRUCT abc[] =
		{
			{
				0x1,
				0x2,
				.c = 0x3,
			},
	
			{
				.a = 0x4,
				.b = 0x5,
				.c = 0x6,
			},
	
		};
	
		uint8_t d = 0;
		d = ARRAY_SIZE(abc);
		printf("d = %d\r\n",d);
		
#endif

	//while(1)
	//{
	//}
}

#endif



