#ifndef	__shell_h
#define	__shell_h

struct _cmd_t;//声明结构体

#define	SHELL_MAX_ARGS	32
#define	SHELL_ARGS_LEN	0x80
typedef	struct _cmdctrl_t{
	//cmd_t *pCmd;
	struct _cmd_t *pCmd;		//指向对应sCmdTbl表，根据下面的buf、argv得到对应存在的表
	u8 buf[SHELL_ARGS_LEN];		//接收的cmd+参数
	u8 argc;					//cmd+参数的索引count	，最大为	SHELL_MAX_ARGS
	u8 *argv[SHELL_MAX_ARGS];	//cmd+参数的内容,最多存SHELL_MAX_ARGS的cmd+参数+参数...
}cmdctrl_t;

typedef	void* (*cmd_func)(struct _cmdctrl_t *pctrl);

typedef struct _cmd_t{
	u8 *pFullName;
	u8 *PshortName;
	cmd_func func;	//调用sCmdTbl表的回调函数，输入sCmdctrl结构体作参数，就可以用所有shell输入参数及指针指向的内容
	u8 bMaxArgs;
	u8 *pTip;
	u8 *pHelp;
}cmd_t;

#define	ENTER		0x0d//回车键
#define	ESC			0x1b//ESC键
#define	EXIT		0X30//数字0
#define	SPACE		0x20//空格键
#define	TAB			0x09//TAB
#define	DEL			0x7f//删除
#define	BACKSPACE	0x08//退格键
#define	SEMICOLON	0x2c//逗号

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
#define shell_asyn_get_char(byte)		shell_usart_asyn_getByte(byte)//readByte,RX中断接收数据给到缓存，等待用户取,用于退出测试函数的do{}while()

#endif//__shell_h


