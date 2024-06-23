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


//�ж�ITEM���Ƿ�����������
BOOL itemEnable(QITEM *item)
{
	//��ȡ��item��Ķ���/ģ���״̬���ж�ITEM���Ƿ�������
	//��Ӻ��Դ���ʱ����item��Ĵ���
	return TRUE;
}


MSG_STAT bios_dev_init(INIT_INFO *init)
{
	MEM_SET(&gDevInfo, 0, sizeof(gDevInfo));
	
	SET_DEV_MODE(NORMAL_MODE);//�������������״̬
	SET_CUR_WATER_T(0);//��ʼ����ǰˮ��Ϊ0
	SET_PRE_WATER_T(0);//��ʼ��Ԥ��ˮ��Ϊ0	
	
	CLS_ERR_WATER();//���ˮλ״̬
	CLS_PUMP_STATE();//���ˮ��״̬

	DBG_TRACE("bios_dev_init OK!\r\n");
	return RUN_OK;
}

MSG_STAT bios_DCM_ctrl(DCM_INFO *dcm)
{
	if(GET_DEV_MOED() == NORMAL_MODE)
	{
		if(DCmotor_toggle())//����״̬�ſ��Գ�ˮ
		{
			SET_PUMP_STATE();
			DBG_TRACE("��ˮ��\r\n");
		}
		else
		{
			CLS_PUMP_STATE();
			DBG_TRACE("�ر�ˮ��\r\n");
		}
	}

	return RUN_OK;
}


MSG_STAT bios_heater_ctrl(HEATER_INFO *heater)
{
	u8 temp;
	
	temp = heater->heat_temp;
	
	if(!IS_PUMP_STATE())//�ǳ�ˮʱ�ſ��Լ���
	{
		if(heater_toggle())
		{
			SET_DEV_MODE(HEAT_MODE);
			DBG_TRACE("�򿪼���\r\n");
		}
		else
		{
			SET_DEV_MODE(NORMAL_MODE);
			DBG_TRACE("�رռ���\r\n");
		}

		if(temp <= 100 && temp > 0)//��Χ��0~100
		{
			SET_PRE_WATER_T(heater->heat_temp);
			DBG_TRACE("����ˮ�� = %d\r\n",heater->heat_temp);		
		}
	}

	return RUN_OK;
}



MSG_STAT bios_tempAdd_ctrl(TEMP_ADD_INFO * tempAdd)
{
	if(GET_PRE_WATER_T() < 100)//��Χ��0~100
	{
		SET_PRE_WATER_T(GET_PRE_WATER_T()++);
		DBG_TRACE("����+1\r\n");
	}
	
	return RUN_OK;
}


MSG_STAT bios_tempReduce_ctrl(TEMP_REDUCE_INFO * tempReduce)
{
	if(GET_PRE_WATER_T() <= 100 && GET_PRE_WATER_T() > 0)//��Χ��0~100
	{
		SET_PRE_WATER_T(GET_PRE_WATER_T()--);
		DBG_TRACE("����-1\r\n");
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
	//systick_init();//ÿ�뷭ת1��LED��
	//basic_timer_init();//ÿ�뷭ת1��LED��

	keyTTP224_init();			//���ݰ���IO_IN
	WaterSensor_init();			//ˮλ������IO_IN
	DCmotor_init();				//ֱ�����ˮ��IO_OUT��OFF״̬��
	heater_init();				//������IO_OUT��OFF״̬��
	DS18B20_init();				//�¶ȴ�����IO�����ߣ�������Ϊ�ߵ�ƽ��
	OLED_Init();				//��ʾ��IIC���ߣ�������
	
}


