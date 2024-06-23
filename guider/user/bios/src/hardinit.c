#include "common.h"

/*-----------------------------------------------------------------
换新主板移植底层注意事项：
1、查看电路图的IO口是否有变化，如led、key只需要底层修改gpio.h的宏即可，如uart需要修改。。。
2、上层不用修改，因为上层只需要知道led1、2和key1、2之类传下去即可，同时底层返回led状态上来。(因此需要做到上层传到底层的led_num，底层根据num配置初始化led驱动和开关状态返回上层)。


//这里声明DevMain函数为了不添加上层头文件，而使用上层的DevMain函数(实现上层和底层的程序分层)。
//底层调用该函数初始化上层应用，只有进去该函数里面就是上层的世界了。
//而上层想要调用底层函数，必须在biosApi.h声明底层函数，因为上层包含了该头文件，因此上层可以用(biosApi.h不需要被common.h包含，而是被lovgdev.h包含)。
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级

	DBG_OPEN();
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

#if 0
	DevMain();//初始化硬件和创建多任务启动
#endif

#if 0
	pwm_test();
#endif

#if 0
	//SleepMode_test();
	//StopMode_test();
	//PVD_config_init();//配置PVD，当电压过低时，会进入中断服务函数，亮红灯
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
	da_init();//打开串口debug后不能正常输出电压，暂没查原因
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




