#ifndef	__monitor_h
#define	__monitor_h


//无名结构体是union的简化写法，否则需要在外面定义一个结构体，union添加结构体变量
typedef union
{
	struct
	{
		u8 init:1;
		u8 dcm:1;
		u8 heater:1;
		u8 tempAdd:1;
		u8 tempReduce:1;
	}bit;	
	u8 dword;
}MECH_STATUS;

extern MECH_STATUS gMechStat;

#define	IS_BUSY_STATE()				(gMechStat.dword)
#define	CLS_BUSY_STATE()			{gMechStat.dword = 0x00;}

#define	IS_INIT_STATE()				(gMechStat.bit.init == TRUE)
#define	SET_INIT_STATE()			{gMechStat.bit.init = TRUE;}
#define	CLS_INIT_STATE()			{gMechStat.bit.init = FALSE;}

#define	IS_DCM_STATE()				(gMechStat.bit.dcm == TRUE)
#define	SET_DCM_STATE()				{gMechStat.bit.dcm = TRUE;}
#define	CLS_DCM_STATE()				{gMechStat.bit.dcm = FALSE;}

#define	IS_HEATER_STATE()			(gMechStat.bit.heater == TRUE)
#define	SET_HEATER_STATE()			{gMechStat.bit.heater = TRUE;}
#define	CLS_HEATER_STATE()			{gMechStat.bit.heater = FALSE;}

#define	IS_TEMP_ADD_STATE()			(gMechStat.bit.tempAdd == TRUE)
#define	SET_TEMP_ADD_STATE()		{gMechStat.bit.tempAdd = TRUE;}
#define	CLS_TEMP_ADD_STATE()		{gMechStat.bit.tempAdd = FALSE;}

#define	IS_TEMP_REDUCE_STATE()		(gMechStat.bit.tempReduce == TRUE)
#define	SET_TEMP_REDUCE_STATE()		{gMechStat.bit.tempReduce = TRUE;}
#define	CLS_TEMP_REDUCE_STATE()		{gMechStat.bit.tempReduce = FALSE;}

void monitorTask(void *p);




#endif//__monitor_h


