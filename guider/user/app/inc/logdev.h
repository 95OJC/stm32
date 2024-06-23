#ifndef __logdev_h
#define	__logdev_h


#include "macro.h"
#include "pubdef.h"
#include "biosApi.h"


/*�ײ�Ӳ����ʼ��*/
#define BIOS_INIT() 			        BIOS_DEV_INIT()

/*�˿�*/
#define	SHELL_TASK						BIOS_SHELL_TASK
#define INIT_RECV_PORT(t,b,c)			BIOS_OPEN_PORT(t,b,c)//�򿪶˿�
#define SET_RECV_PORT(n)				BIOS_SET_RECV_PORT(n)//���õ�ǰ�˿�����
#define GET_DATA(x,l)					BIOS_GET_DATA(x,l)//��ȡ����
#define SEND_DATA(x,l)					BIOS_SEND_DATA(x,l)//��������
#define PEEK_DATA(x,l)					BIOS_PEEK_DATA(x,l)	//�鿴���ն˿�����
#define IS_RECV_EMPT()					BIOS_IS_BUF_EMPT(RX_BUF)//�����ջ����Ƿ�Ϊ��
#define IS_TX_EMPTY()					BIOS_IS_BUF_EMPT(TX_BUF)//��鷢�Ͷ˿��Ƿ�Ϊ��
#define GET_RECV_CNT()					BIOS_GET_BUF_CNT(RX_BUF)//���ջ����е�������
#define GET_TX_CNT()					BIOS_GET_BUF_CNT(TX_BUF)//���ջ����е�������
#define RESET_PORT()					BIOS_RESET_PORT()//��ս��պͷ��ͻ���
#define CLS_RECV_BUF()					BIOS_CLS_BUF(IOCTRL_RESET_RX_BUF)//������ջ���
#define CLS_TX_BUF()					BIOS_CLS_BUF(IOCTRL_RESET_TX_BUF)//������ͻ���
#define FIND_DATA_PORT()				BIOS_FIND_DPORT()//�������ж˿ڣ��鿴�ĸ��յ�����

/*����*/
#define	PRNQ_TASK						BIOS_PRNQ_TASK
#define SUBMIT_ITEM_MSG(item,p) 		BIOS_SUBMIT_ITEM_MSG(item,p)//�ύITEM
#define CHECK_ITEM_STATUS(h)            BIOS_CHECK_ITEM_STATUS(h)//���item�������״̬
#define GET_PRNQ_STAT()                 BIOS_GET_PRNQ_STAT()//��ȡ��ӡ����״̬�����й���/��ͣ
#define SET_PRNQ_REQ(m)					BIOS_SET_PRNQ_REQ(m)//���ö��м���/��ͣ		
#define GET_PRNQ_CNT()                  BIOS_GET_PRNQ_CNT()//��ȡ�����item������е�ITEM������(�����������еķ����item��)
#define IS_PRNQ_EMPTY()                 BIOS_IS_PRNQ_EMPTY()//�������Ƿ�Ϊ��
#define IS_PNLITEM_EMPTY()				BIOS_IS_PNLITEM_EMPTY()//������item���Ƿ�Ϊ��
#define GET_FREE_PRNQ_CNT()             BIOS_GET_FREE_PRNQ_CNT()//�����п�ITEM������


#endif//__logdev_h



