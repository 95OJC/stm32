#ifndef __main_h
#define	__mian_h

//�ϲ��õ���C��
#include <string.h>//�ڴ濽�������õ�
#include <stdio.h>//�ض������õ�
#include <stdlib.h>//malloc()��free()���õ�


//Ϊ���ϲ���Ը��ײ������������
//u8��u6��u32����������
//�����ˡ�stdint.h�����ͷ�ļ�������uint16_t֮��
#include "stm32f10x.h"


//�����ϲ�͵ײ�Ĺ��������壬����memcpy�Ⱥ�������
//�����ϲ�͵ײ�Ľӿڹ����ĺ�ͽṹ��
//�����ײ㴫���ϲ�ʹ�õĺ���
#include "logdev.h"


//����ͻ��嶨��
#include "system.h"//�����ϲ����õ�ͨѶTX����cache��RX����buffer���ṹ���ringbuf���ݺ�
#include "sysRam.h"//(�ⲿ)�����ϲ����õ�ͨѶTX����cache��RX����buffer


//�ϲ��ļ���ͷ�ļ�
#include "monitor.h"


#endif//__main_h

