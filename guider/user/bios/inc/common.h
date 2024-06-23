#ifndef __common_h
#define	__common_h

/*-----------------------------------------------------------------
1����Ҫ�ϸ�������˳�����ͷ�ļ�����ֹ�Ҳ������塣(C�⺯��ͷ�ļ����Ĵ��������ַ��װͷ�ļ������������ͽṹ�塢�ײ���������API)
2������freertos��usbЭ��ջ֮��ȵ�ͷ�ļ������Ҳ���ڡ�//�ײ������API����ǰ�棬�͡�//�ϲ�͵ײ�Ĺ������񡢺ꡢ�ṹ�塱�ĺ���,��Ϊִ����ЩЭ��ջ����Ҫǰ2��ͷ�ļ���֧�֡�
3��ע��C�ļ���ͷ�ļ��ķ���˳����.c�ļ����ȷ�common.h���ٷ�.c�ļ����ڲ�ͷ�ļ�����Ϊִ��C�ļ�ʱͷ�ļ��ᰴ˳��չ��������Ϊִ����ЩЭ��ջ����Ҫǰ2��ͷ�ļ���֧�֡�
------------------------------------------------------------------*/

//�ײ��õ���C��
#include <string.h>//�ڴ濽�������õ�
#include <stdio.h>//�ض������õ�
#include <stdlib.h>//malloc()��free()���õ�


//�ײ��STM32�⺯��(Ӳ�������/�ײ㣬��ӳ��Ĵ������Ĵ���λ�궨��)
//u8��u6��u32����������
//�����ˡ�stdint.h�����ͷ�ļ�������uint16_t֮��
#include "stm32f10x.h"

//�ײ�ļĴ������������������ַû�нṹ���װʱ����õ���
#include "regwrApi.h"

//�ϲ�͵ײ�Ĺ���������(��Ҫ�����ڵײ����������ǰ��ȷ���ײ����������(����)�ܹ��ҵ��ù���������)
#include "macro.h"
//�ϲ�͵ײ㹫���ĺ�ͽṹ��(��Ҫ�����ڵײ����������ǰ��ȷ���ײ����������(����)�ܹ��ҵ��ú�ͽṹ��)
#include "pubdef.h"
//�꿪��
#include "config.h"

typedef enum{
	DEV_ERR = 0,
	DEV_OK = 1,	
	DEV_BUSY = 2,
	DEV_TIMEOUT = 3
}DEV_STATUS;

//�ײ������API
#include "gpioApi.h"
#include "keyCtrlApi.h"
#include "systick_rcc_Api.h"
#include "timerApi.h"
#include "ringbufApi.h"
#include "usartApi.h"
#include "ifApi.h"
#include "shellApi.h"
#include "dmaApi.h"
#include "iicApi.h"
#include "spiApi.h"
#include "eepromApi.h"
#include "flashApi.h"
#include "adda.h"



#include "bios.h"
#include "keyTTP224.h"//4�����ݰ���
#include "WaterSensor.h"//ˮλ������
#include "DCmotor.h"//ֱ�����+�̵���
#include "heater.h"//������+�̵���
#include "DS18B20.h"//�¶ȴ�����+������
#include "OLED096.h"//OLED096��Ļ



#endif//__common_h


