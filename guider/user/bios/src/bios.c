#include "common.h"


#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

DEV_INFO gDevInfo;

BOOL bios_is_buf_empty(u8 m)
{
    u32 n = 1;
    
    if(m == RX_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);
    }else if(m == TX_BUF)
    {
        if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
    }else if(m == RT_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);
        if(n == 0)
        {
            if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
        }
    }
    if(n == 0)
    {
		return TRUE;
    }
	else
	{
		return FALSE;
	}
}


u32 bios_get_data_cnt(u8 m)
{
    u32 n;
    
    if(m == RX_BUF)
    {
        if_io_ctrl(IOCTRL_RX_DATA_SIZE, &n);

    }else if(m == TX_BUF)
    {
        if_io_ctrl(IOCTRL_TX_DATA_SIZE, &n);
    }

    return n;
}


//判断ITEM项是否允许激活运行
BOOL itemEnable(QITEM *item)
{
	//获取各item项的动作/模块的状态，判断ITEM项是否允许激活
	//添加忽略错误时激活item项的代码
	return TRUE;
}


MSG_STAT bios_dev_init(INIT_INFO *init)
{
	MEM_SET(&gDevInfo, 0, sizeof(gDevInfo));
	
	SET_DEV_MODE(NORMAL_MODE);//并且清除加热器状态
	SET_CUR_WATER_T(0);//初始化当前水温为0
	SET_PRE_WATER_T(0);//初始化预设水温为0	
	
	CLS_ERR_WATER();//清除水位状态
	CLS_PUMP_STATE();//清除水泵状态

	DBG_TRACE("bios_dev_init OK!\r\n");
	return RUN_OK;
}

MSG_STAT bios_DCM_ctrl(DCM_INFO *dcm)
{
	if(GET_DEV_MOED() == NORMAL_MODE)
	{
		if(DCmotor_toggle())//常温状态才可以出水
		{
			SET_PUMP_STATE();
			DBG_TRACE("打开水泵\r\n");
		}
		else
		{
			CLS_PUMP_STATE();
			DBG_TRACE("关闭水泵\r\n");
		}
	}

	return RUN_OK;
}


MSG_STAT bios_heater_ctrl(HEATER_INFO *heater)
{
	u8 temp;
	
	temp = heater->heat_temp;
	
	if(!IS_PUMP_STATE())//非出水时才可以加热
	{
		if(heater_toggle())
		{
			SET_DEV_MODE(HEAT_MODE);
			DBG_TRACE("打开加热\r\n");
		}
		else
		{
			SET_DEV_MODE(NORMAL_MODE);
			DBG_TRACE("关闭加热\r\n");
		}

		if(temp <= 100 && temp > 0)//范围：0~100
		{
			SET_PRE_WATER_T(heater->heat_temp);
			DBG_TRACE("设温水温 = %d\r\n",heater->heat_temp);		
		}
	}

	return RUN_OK;
}



MSG_STAT bios_tempAdd_ctrl(TEMP_ADD_INFO * tempAdd)
{
	if(GET_PRE_WATER_T() < 100)//范围：0~100
	{
		SET_PRE_WATER_T(GET_PRE_WATER_T()++);
		DBG_TRACE("设温+1\r\n");
	}
	
	return RUN_OK;
}


MSG_STAT bios_tempReduce_ctrl(TEMP_REDUCE_INFO * tempReduce)
{
	if(GET_PRE_WATER_T() <= 100 && GET_PRE_WATER_T() > 0)//范围：0~100
	{
		SET_PRE_WATER_T(GET_PRE_WATER_T()--);
		DBG_TRACE("设温-1\r\n");
	}
	
	return RUN_OK;
}


void biosInit(void)
{
	//if_init();
	//usart_driver_register();
	
	//led_init();
	//key_init();
	//key_exti_init();
	//systick_init();//每秒翻转1次LED。
	//basic_timer_init();//每秒翻转1次LED。

	keyTTP224_init();			//电容按键IO_IN
	WaterSensor_init();			//水位传感器IO_IN
	DCmotor_init();				//直流电机水泵IO_OUT（OFF状态）
	heater_init();				//发热器IO_OUT（OFF状态）
	DS18B20_init();				//温度传感器IO单总线（被上拉为高电平）
	OLED_Init();				//显示屏IIC总线（清屏）
	
}


