#ifndef	__bios_h
#define	__bios_h

#define	UNIT_PLACE(x)			((x)/1%10)
#define	TEN_PLACE(x)			((x)/10%10)
#define	HUNDRED_PLACE(x)		((x)/100%10)

typedef enum{
	NORMAL_MODE	   = 0x00,
	HEAT_MODE      = 0x01,
	ERR_WATER_MODE = 0x02,
}mode_state;


//OLED显示：常温状态，加热状态，缺水状态
#define	SET_DEV_MODE(x)			{gDevInfo.modeState = (x);} 
#define	GET_DEV_MOED()			(gDevInfo.modeState) 

//当前水温
#define	SET_CUR_WATER_T(x)		{gDevInfo.cur_WaterT = (x);}
#define	GET_CUR_WATER_T()		(gDevInfo.cur_WaterT)

//预设水温
#define	SET_PRE_WATER_T(x)		{gDevInfo.preset_WaterT = (x);}
#define	GET_PRE_WATER_T()		(gDevInfo.preset_WaterT)	

//水位状态
#define	IS_ERR_WATER()			(gDevInfo.errWater)
#define	SET_ERR_WATER()			{gDevInfo.errWater = TRUE;}
#define	CLS_ERR_WATER()			{gDevInfo.errWater = FALSE;}

//水泵状态
#define	IS_PUMP_STATE()			(gDevInfo.pumpState)
#define	SET_PUMP_STATE()		{gDevInfo.pumpState = TRUE;}	
#define	CLS_PUMP_STATE()		{gDevInfo.pumpState = FALSE;}


/*设备状态信息*/
typedef struct _DEV_INFO
{
	BOOL errWater;				//水位状态(用于限制条件)
	u16	 cur_WaterT;			//当前水温
	u16	 preset_WaterT;			//预设水温		
	mode_state modeState;		//OLED显示：常温状态(用于限制条件)，加热状态(用于限制条件)，缺水状态(用于停止工作)
	BOOL pumpState;				//水泵状态(用于限制条件)
	
}DEV_INFO;


extern DEV_INFO gDevInfo;

extern BOOL itemEnable(QITEM *item);

extern MSG_STAT bios_dev_init(INIT_INFO *init);
extern MSG_STAT bios_DCM_ctrl(DCM_INFO *dcm);
extern MSG_STAT bios_heater_ctrl(HEATER_INFO *heater);
extern MSG_STAT bios_tempAdd_ctrl(TEMP_ADD_INFO * tempAdd);
extern MSG_STAT bios_tempReduce_ctrl(TEMP_REDUCE_INFO * tempReduce);



#endif//__bios_h

