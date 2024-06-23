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


//OLED��ʾ������״̬������״̬��ȱˮ״̬
#define	SET_DEV_MODE(x)			{gDevInfo.modeState = (x);} 
#define	GET_DEV_MOED()			(gDevInfo.modeState) 

//��ǰˮ��
#define	SET_CUR_WATER_T(x)		{gDevInfo.cur_WaterT = (x);}
#define	GET_CUR_WATER_T()		(gDevInfo.cur_WaterT)

//Ԥ��ˮ��
#define	SET_PRE_WATER_T(x)		{gDevInfo.preset_WaterT = (x);}
#define	GET_PRE_WATER_T()		(gDevInfo.preset_WaterT)	

//ˮλ״̬
#define	IS_ERR_WATER()			(gDevInfo.errWater)
#define	SET_ERR_WATER()			{gDevInfo.errWater = TRUE;}
#define	CLS_ERR_WATER()			{gDevInfo.errWater = FALSE;}

//ˮ��״̬
#define	IS_PUMP_STATE()			(gDevInfo.pumpState)
#define	SET_PUMP_STATE()		{gDevInfo.pumpState = TRUE;}	
#define	CLS_PUMP_STATE()		{gDevInfo.pumpState = FALSE;}


/*�豸״̬��Ϣ*/
typedef struct _DEV_INFO
{
	BOOL errWater;				//ˮλ״̬(������������)
	u16	 cur_WaterT;			//��ǰˮ��
	u16	 preset_WaterT;			//Ԥ��ˮ��		
	mode_state modeState;		//OLED��ʾ������״̬(������������)������״̬(������������)��ȱˮ״̬(����ֹͣ����)
	BOOL pumpState;				//ˮ��״̬(������������)
	
}DEV_INFO;


extern DEV_INFO gDevInfo;

extern BOOL itemEnable(QITEM *item);

extern MSG_STAT bios_dev_init(INIT_INFO *init);
extern MSG_STAT bios_DCM_ctrl(DCM_INFO *dcm);
extern MSG_STAT bios_heater_ctrl(HEATER_INFO *heater);
extern MSG_STAT bios_tempAdd_ctrl(TEMP_ADD_INFO * tempAdd);
extern MSG_STAT bios_tempReduce_ctrl(TEMP_REDUCE_INFO * tempReduce);



#endif//__bios_h

