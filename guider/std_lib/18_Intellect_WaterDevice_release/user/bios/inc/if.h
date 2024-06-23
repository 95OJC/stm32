#ifndef __if_h
#define	__if_h

typedef struct{
	BOOL valid;//�˿��Ƿ���Ч������˿�ע�����Ч,��Ч����if_open
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl);
	BOOL (*close)(void);
	u32  (*read)(u8 *buf, u32 len);
	u32  (*write)(u8 *buf, u32 len);
	u32  (*peek)(u8 *buf,u32 len);
	BOOL  (*io_ctrl)(IOCTRL_TYPE type, void *ctrl);
	BOOL tag;//�˿��Ƿ�򿪣�if_open��true/if_close��false ,trueʱ����if_set_port
}IF_DRIVER;



#endif//__if_h


