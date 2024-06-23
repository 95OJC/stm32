#ifndef	__biosApi_h
#define	__biosApi_h


//���ļ��ĺ��������Ǹ��ϲ�app�ļ����õģ����Բ���Ҫ��common.h���������Ǹ�main.h������logdev.h��������
void biosInit(void);
BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl);
BOOL if_set_cur_port(PORT_NUM n);
BOOL if_close(void);
u32 if_read_data(u8 *buf, u32 len);
u32 if_write_data(u8 *buf, u32 len);
u32 if_peek_data(u8 *buf, u32 len);
BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl);
void shell_task(void);



/*����*/
#define BIOS_DEV_INIT()             biosInit()
#define	BIOS_SHELL_TASK()			shell_task()
/*
�˿�
*/
#define BIOS_OPEN_PORT(t,b,c)       if_open(t,b,c)   //�򿪶˿�
#define BIOS_SET_RECV_PORT(n)       if_set_cur_port(n)  //���õ�ǰ�˿�����
#define BIOS_GET_DATA(x,l)          if_read_data(x,l) //�����ն˿�
#define BIOS_SEND_DATA(x,l)         if_write_data(x,l)//��������������




#endif//__biosApi_h

