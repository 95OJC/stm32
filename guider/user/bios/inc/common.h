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
#include <stdarg.h>//debug��va_start���õ�


//�ײ��STM32�⺯��(Ӳ�������/�ײ㣬��ӳ��Ĵ������Ĵ���λ�궨��)
//u8��u6��u32����������
//�����ˡ�stdint.h�����ͷ�ļ�������uint16_t֮��
#include "stm32f10x.h"
#include "regwrApi.h"//�ײ�Ĵ������������������ַû�нṹ���װʱ����õ�


/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* FATFS�ļ�ϵͳ */
#include "ff.h"


//�ϲ�͵ײ�Ĺ���������(��Ҫ�����ڵײ����������ǰ��ȷ���ײ����������(����)�ܹ��ҵ��ù���������)
#include "macro.h"
//�ϲ�͵ײ㹫���ĺ�ͽṹ��(��Ҫ�����ڵײ����������ǰ��ȷ���ײ����������(����)�ܹ��ҵ��ú�ͽṹ��)
#include "pubdef.h"


typedef enum{
	DEV_ERR = 0,
	DEV_OK = 1,	
	DEV_BUSY = 2,
	DEV_TIMEOUT = 3
}DEV_STATUS;
#include "config.h"
#include "gpio.h"
#include "systick_rcc.h"
#include "timer.h"
#include "usart.h"
#include "dma.h"
#include "wdg.h"
#include "adda.h"
#include "sensor.h"
#include "iic.h"
#include "spi.h"
#include "AT24C02eeprom.h"
#include "W25Q64flash.h"
#include "pwm.h"

#include "debug.h"
#include "shell.h"
#include "if.h"
#include "ringbuf.h"
#include "prnq.h"
#include "bios.h"




#endif//__common_h


