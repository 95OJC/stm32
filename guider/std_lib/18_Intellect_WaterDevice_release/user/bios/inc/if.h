#ifndef __if_h
#define	__if_h

typedef struct{
	BOOL valid;//端口是否有效，外设端口注册后有效,有效才能if_open
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl);
	BOOL (*close)(void);
	u32  (*read)(u8 *buf, u32 len);
	u32  (*write)(u8 *buf, u32 len);
	u32  (*peek)(u8 *buf,u32 len);
	BOOL  (*io_ctrl)(IOCTRL_TYPE type, void *ctrl);
	BOOL tag;//端口是否打开，if_open置true/if_close置false ,true时才能if_set_port
}IF_DRIVER;



#endif//__if_h


