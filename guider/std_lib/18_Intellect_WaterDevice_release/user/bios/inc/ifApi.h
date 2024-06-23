#ifndef	__ifApi_h
#define	__ifApi_h

//给底层bios.c的init函数调用。
void if_init(void);

//给各底层通讯接口函数调用(赋值接口的回调函数)，然后底层通讯接口文件形成例如usart1_driver_register函数，给底层bios.c的init函数调用。
void if_register_driver(
	PORT_NUM n,
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl),
	BOOL (*close)(void),
	u32  (*read)(u8 *buf, u32 len),
	u32  (*write)(u8 *buf, u32 len),
	u32  (*peek)(u8 *buf, u32 len),
	BOOL  (*io_ctrl)(IOCTRL_TYPE type, void *ctrl)
);

//这里声明为了底层的文件也可以使用,但是做真正的项目时底层一般不会用到。
//以下函数一般是上层调用(biosApi.h文件声明并且封装为底层宏，然后main.h的logdev.h也添加biosApi.h同时在logdev.h封装上层宏即可上层调用)。
BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl);
BOOL if_set_cur_port(PORT_NUM n);
BOOL if_close(void);
u32 if_read_data(u8 *buf, u32 len);
u32 if_write_data(u8 *buf, u32 len);
u32 if_peek_data(u8 *buf, u32 len);
BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl);


#endif//_ifApi_h

