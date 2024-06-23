#ifndef	__shell_h
#define	__shell_h

struct _cmd_t;//�����ṹ��

#define	SHELL_MAX_ARGS	32
#define	SHELL_ARGS_LEN	0x80
typedef	struct _cmdctrl_t{
	//cmd_t *pCmd;
	struct _cmd_t *pCmd;		//ָ���ӦsCmdTbl�����������buf��argv�õ���Ӧ���ڵı�
	u8 buf[SHELL_ARGS_LEN];		//���յ�cmd+����
	u8 argc;					//cmd+����������count	�����Ϊ	SHELL_MAX_ARGS
	u8 *argv[SHELL_MAX_ARGS];	//cmd+����������,����SHELL_MAX_ARGS��cmd+����+����...
}cmdctrl_t;

typedef	void* (*cmd_func)(struct _cmdctrl_t *pctrl);

typedef struct _cmd_t{
	u8 *pFullName;
	u8 *PshortName;
	cmd_func func;	//����sCmdTbl��Ļص�����������sCmdctrl�ṹ�����������Ϳ���������shell���������ָ��ָ�������
	u8 bMaxArgs;
	u8 *pTip;
	u8 *pHelp;
}cmd_t;

#define	ENTER		0x0d//�س���
#define	ESC			0x1b//ESC��
#define	EXIT		0X30//����0
#define	SPACE		0x20//�ո��
#define	TAB			0x09//TAB
#define	DEL			0x7f//ɾ��
#define	BACKSPACE	0x08//�˸��
#define	SEMICOLON	0x2c//����

#define DEC 	0
#define HEX 	1


#define HELP_TIP			"help tip"
#define	HELP_HELP			"help into"

#define TTP224_TIP			"u8 num"
#define	TTP224_HELP \
"u8 num\r\n\
num = 0  (init)\r\n\
num = !0 (test)\r\n"

#define WaterSensor_TIP		"u8 num"
#define	WaterSensor_HELP \
"u8 num\r\n\
num = 0  (init)\r\n\
num = !0 (test)\r\n"

#define DS18B20_TIP			"u8 num"
#define	DS18B20_HELP \
"u8 num\r\n\
num = 0  (init)\r\n\
num = !0 (test)\r\n"

#define OLED096_TIP			"u8 num"
#define	OLED096_HELP \
"u8 num\r\n\
num = 0  (init)\r\n\
num = !0 (test)\r\n"

#define HEATER_TIP			"u8 num"
#define	HEATER_HELP \
"u8 num\r\n\
num = 0  (init)\r\n\
num = !0 (test)\r\n"

#define	shell_send_char(ch)				shell_usart_sendByte(ch)//usart_sendByte
#define	shell_send_string(str)			shell_usart_sendString(str)//usart_sendString
#define	shell_get_char()				shell_usart_getByte()//usart_getByte
#define shell_asyn_get_char(byte)		shell_usart_asyn_getByte(byte)//readByte,RX�жϽ������ݸ������棬�ȴ��û�ȡ,�����˳����Ժ�����do{}while()

#endif//__shell_h


