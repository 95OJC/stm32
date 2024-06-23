#ifndef	__ifApi_h
#define	__ifApi_h

//���ײ�bios.c��init�������á�
void if_init(void);

//�����ײ�ͨѶ�ӿں�������(��ֵ�ӿڵĻص�����)��Ȼ��ײ�ͨѶ�ӿ��ļ��γ�����usart1_driver_register���������ײ�bios.c��init�������á�
void if_register_driver(
	PORT_NUM n,
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl),
	BOOL (*close)(void),
	u32  (*read)(u8 *buf, u32 len),
	u32  (*write)(u8 *buf, u32 len),
	u32  (*peek)(u8 *buf, u32 len),
	BOOL  (*io_ctrl)(IOCTRL_TYPE type, void *ctrl)
);

//��������Ϊ�˵ײ���ļ�Ҳ����ʹ��,��������������Ŀʱ�ײ�һ�㲻���õ���
//���º���һ�����ϲ����(biosApi.h�ļ��������ҷ�װΪ�ײ�꣬Ȼ��main.h��logdev.hҲ���biosApi.hͬʱ��logdev.h��װ�ϲ�꼴���ϲ����)��
BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl);
BOOL if_set_cur_port(PORT_NUM n);
BOOL if_close(void);
u32 if_read_data(u8 *buf, u32 len);
u32 if_write_data(u8 *buf, u32 len);
u32 if_peek_data(u8 *buf, u32 len);
BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl);


#endif//_ifApi_h

