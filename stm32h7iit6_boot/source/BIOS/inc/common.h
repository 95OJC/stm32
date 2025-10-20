#ifndef __COMMON_H
#define	__COMMON_H

//��C��׼���е�һ��ͷ�ļ����ṩ�����ͨ�ù��ߺ����������ڴ���䡢���̿��ơ�������������Լ��ַ���ת����
#include <stdlib.h>
//��C99��׼�����ͷ�ļ������� C99 �����߰汾�п��ã����ṩ��һЩ�������͵Ķ��壬��Щ���;�����ȷ�Ĵ�С�ͷ������ԣ�ȷ���ڲ�ͬƽ̨�ϵ�һ���ԡ�
#include <stdint.h>
//��C��׼���е�һ��ͷ�ļ����ṩ��һ�����ڴ����ַ������ڴ��ĺ�������Щ�����������ַ������ơ����ӡ��Ƚϡ��������ڴ�����ȡ�
#include <string.h>
//������һ���������� va_list �������꣬��������������ڲ�������δ֪�������������ɱ䣩ʱ��ȡ�����еĲ������ɱ�����ĺ���ͨ���ڲ����б��ĩβ��ʹ��ʡ�Ժ� ... ����ġ�
#include <stdarg.h>
//������һϵ������������������ĺ���ԭ�ͣ����� printf �� scanf ���������궨�壨���� EOF �� NULL������������������ FILE �� size_t��
#include <stdio.h>


#include "stm32h7xx.h"//�����USE_HAL_DRIVERʹ��HAL�⡢�����STM32H743xxʹ��H743�Ĵ���λ����
#include "typedef.h"//��������������д
#include "macro.h"//����ȫ���жϡ���������ݵĴ洢�ռ䡢�ڴ������
#include "pubdef.h"//���幫���˿ںꡢ������С������
#include "pridef.h"//�ж����ȼ�
#include "RingBufApi.h"
#include "IFApi.h"
#include "debug.h"
#include "uartApi.h"

#include "main.h"
#include "bootApi.h"





#endif//__COMMON_H


