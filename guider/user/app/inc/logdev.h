#ifndef __logdev_h
#define	__logdev_h

//#include "typedef.h"
#include "macro.h"
#include "pubdef.h"

#include "biosApi.h"

/*�ײ��ʼ��*/
#define BIOS_INIT() 			        BIOS_DEV_INIT()	//��ʼ��BIOS����

/*
�˿ڲ���
*/
#define INIT_RECV_PORT(t,b,c)			BIOS_OPEN_PORT(t,b,c)//���õ�ǰ���ն˿�
#define SET_RECV_PORT(n)				BIOS_SET_RECV_PORT(n)//���õ�ǰ�˿�����
#define GET_DATA(x,l)					BIOS_GET_DATA(x,l)//��ȡ����
#define SEND_DATA(x,l)					BIOS_SEND_DATA(x,l)//��������


#define	SHELL_TASK()					BIOS_SHELL_TASK()


#endif//__logdev_h



