#ifndef __prnq_h
#define	__prnq_h

/* item��� bios_xxx_fun(QITEM.MSG_INFO����) ִ����� */	
typedef enum
{
	NO_RUN	=	0,	    //û��ִ��
	RUN_OK	=	1,	    //����ִ�����
	RUNNING	=	2,	    //����ִ��
	RUN_ERR	=	0x0F,    //ִ�д���
}MSG_STAT;


#endif//__prnq_h

