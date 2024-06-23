#ifndef	__monitor_h
#define	__monitor_h


//�����ṹ����union�ļ�д����������Ҫ�����涨��һ���ṹ�壬union��ӽṹ�����
typedef union
{
	struct
	{
		u8 init:1;

	}bit;	
	u8 dword;
}MECH_STATUS;

extern MECH_STATUS gMechStat;

#define	IS_BUSY_STATE()				(gMechStat.dword)
#define	CLS_BUSY_STATE()			{gMechStat.dword = 0x00;}

#define	IS_INIT_STATE()				(gMechStat.bit.init == TRUE)
#define	SET_INIT_STATE()			{gMechStat.bit.init = TRUE;}
#define	CLS_INIT_STATE()			{gMechStat.bit.init = FALSE;}

void monitorTask(void *p);




#endif//__monitor_h


