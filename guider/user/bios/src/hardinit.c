#include "common.h"

/*-----------------------------------------------------------------
换新主板移植底层注意事项：
1、查看电路图的IO口是否有变化，如led、key只需要底层修改gpio.h的宏即可，如uart需要修改。。。
2、上层不用修改，因为上层只需要知道led1、2和key1、2之类传下去即可，同时底层返回led状态上来。(因此需要做到上层传到底层的led_num，底层根据num配置初始化led驱动和开关状态返回上层)。


//这里声明DevMain函数为了不添加上层头文件，而使用上层的DevMain函数(实现上层和底层的程序分层)。
//底层调用该函数初始化上层应用，只有进去该函数里面就是上层的世界了。
//而上层想要调用底层函数，必须在biosApi.h声明底层函数，因为上层包含了该头文件，因此上层可以用(biosApi.h不需要被common.h包含，而是被lovgdev.h包含)。
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
	//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	

	DBG_OPEN();
	
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

	//初始化硬件 和 创建多任务启动
	DevMain();

	while(1);
}

#else
static void OLED_normal_mode(void)
{
	OLED_ShowCHinese(0,0,5);//常
	OLED_ShowCHinese(16,0,11);//温
	OLED_ShowCHinese(32,0,3);//状
	OLED_ShowCHinese(48,0,4);//态
}

static void OLED_heat_mode(void)
{
	OLED_ShowCHinese(0,0,1);//加
	OLED_ShowCHinese(16,0,2);//热
	OLED_ShowCHinese(32,0,3);//状
	OLED_ShowCHinese(48,0,4);//态
}

static void OLED_errWater_mode(void)
{
	OLED_ShowCHinese(0,0,0);//缺
	OLED_ShowCHinese(16,0,10);//水
	OLED_ShowCHinese(32,0,3);//状
	OLED_ShowCHinese(48,0,4);//态
}


static void OLED_WaterTemp_mode(void)
{
	OLED_ShowCHinese(0,4,8);//当
	OLED_ShowCHinese(16,4,9);//前
	OLED_ShowCHinese(32,4,10);//水
	OLED_ShowCHinese(48,4,11);//温
	OLED_ShowChar(64,4,':',16);//:
	OLED_ShowNum(80,4,HUNDRED_PLACE(GET_CUR_WATER_T()),1,16);//水温百位
	OLED_ShowNum(96,4,TEN_PLACE(GET_CUR_WATER_T()),1,16);//水温十位
	OLED_ShowNum(112,4,UNIT_PLACE(GET_CUR_WATER_T()),1,16);//水温个位	
	
	OLED_ShowCHinese(0,2,6);//预
	OLED_ShowCHinese(16,2,7);//设
	OLED_ShowCHinese(32,2,10);//水
	OLED_ShowCHinese(48,2,11);//温
	OLED_ShowChar(64,2,':',16);//:
	OLED_ShowNum(80,2,HUNDRED_PLACE(GET_PRE_WATER_T()),1,16);//水温百位
	OLED_ShowNum(96,2,TEN_PLACE(GET_PRE_WATER_T()),1,16);//水温十位
	OLED_ShowNum(112,2,UNIT_PLACE(GET_PRE_WATER_T()),1,16);//水温个位
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
				case KEY_PUMP://水泵
					if(GET_DEV_MOED() == NORMAL_MODE)
					{
						if(DCmotor_toggle())//常温状态才可以出水
						{
							SET_PUMP_STATE();
							printf("打开水泵\r\n");
						}
						else
						{
							CLS_PUMP_STATE();
							printf("关闭水泵\r\n");
						}
					}
					break;		
				case KEY_HEAT://加热
					if(!IS_PUMP_STATE())//非出水时才可以加热
					{
						if(heater_toggle())
						{
							SET_DEV_MODE(HEAT_MODE);
							printf("打开加热\r\n");
						}
						else
						{
							SET_DEV_MODE(NORMAL_MODE);
							printf("关闭加热\r\n");
						}
					}
					break;	
				case KEY_TEMP_UP://预设水温+
					if(GET_PRE_WATER_T() < 100 && GET_PRE_WATER_T() >= 0)//范围：0~100
					{
						SET_PRE_WATER_T(GET_PRE_WATER_T()++);
						printf("设温+1\r\n");
					}
					break;
				case KEY_TEMP_DOWN://预设水温-
					if(GET_PRE_WATER_T() <= 100 && GET_PRE_WATER_T() > 0)//范围：0~100
					{
						SET_PRE_WATER_T(GET_PRE_WATER_T()--);
						printf("设温-1\r\n");
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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级
	DevMain();//初始化硬件
	bios_dev_init();//初始化设备信息
	
	while(1)
	{
		Wtemp = 0;
		/* 实时监控水位，读取和设置水温 */
		if(1)//if(WaterSensor_monitor())
		{
			CLS_ERR_WATER();
			Wtemp = DS18B20_ReadTemperature();
			SET_CUR_WATER_T(Wtemp);//设置当前水温的全局变量
		}
		else
		{
			SET_ERR_WATER();		
		}
	
		/* 实时显示OLED */
		if(!IS_ERR_WATER())
		{
			/* 当前水温达到预设水温之后,并且是加热模式时，则停止加热 */
			if( (GET_CUR_WATER_T() >= GET_PRE_WATER_T()) && (GET_DEV_MOED() == HEAT_MODE) )
			{
				heater_off();//关闭加热
				SET_DEV_MODE(NORMAL_MODE);
				printf("关闭加热\r\n");
			}
			monitor_OLED();
		}
		else
		{//缺水状态,初始化设备信息，并且停止工作。
			bios_dev_init();
			DCmotor_off();
			heater_off();
			
			SET_DEV_MODE(ERR_WATER_MODE);			
			monitor_OLED();
			while(1);//需要加水后，重新开机！
		}
		/* 实时检测按键输入，改变显示：工作状态和(预设)水温 */
		fourKey_scan();
	}

#if 0
	da_init();//每秒输出1次DAC转换值（目前是固定2048，即3.3V的一半）
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



