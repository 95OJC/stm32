#include "common.h"
#include "main.h"


/*-----------------------------------------------------------------
shell����(����cmd)��

���裺
1�����徲̬ȫ�ֽṹ��sCmdctrl���躬�У���������(���ڽ���cmd�Ͳ���)��ָ������(���ڷָ�cmd�Ͳ���)��ָ�����������ֵ(�жϲ����Ƿ񳬳�)
2�����徲̬constȫ�ֽṹ������sCmdctrl[],�û���������ӻص�������cmd��tip
3��д������������(����������)
4��д�����ָ�cmd�Ͳ�������ָ������(��������)
5��д�����жϷָ���cmd��gCmdTbl[]���Ƿ������ͬcmd��������ִ���û��ص�����(���뵱ǰcmdctrl�ṹ������)


������
����help����cmd����ʾ����FullCmd(ShortCmd)��
����help��cmd����ʾ��cmd��help��ʾ��


���
�����CMD���󣬻ᱨcmd error!
�����CMD����̫�࣬�ᱨargc out error!
�����CMD����̫�٣�����ʾ��cmd��tip��ʾ��

------------------------------------------------------------------*/

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


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


static cmdctrl_t sCmdctrl;
static const cmd_t sCmdTbl[];

static BOOL isSpace(u8 d)
{
	if((d == 0x20) || (d == 0x09))
		return TRUE;
	return FALSE;
}

static BOOL isHexVal(const u8 *pStr)
{
	if(*pStr++ == '0')
	{
		if((*pStr == 'x') || ( *pStr == 'X'))
		{
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL isDigit(u8 d,u8 radix)
{
	BOOL ret = FALSE;

	if(radix == DEC)
	{
		if(('0' <= d) && (d <= '9'))
			ret = TRUE;
	}
	else if(radix == HEX)
	{
		if((('0' <= d) && (d <= '9')) ||
				(('a' <= d) && (d <= 'f')) ||
				(('A' <= d) && (d <= 'F')))
		ret = TRUE;
	}

	return ret;
}

static u8 c2b(u8 c,u8 radix)
{
	u8 data = 0;
	
	if(radix == DEC)
	{
		if(('0' <= c) && (c <= '9'))
			data = c - '0';
	}
	else if(radix == HEX)
	{
		if(('0' <= c) && (c <= '9'))
			data = c - '0';
		else if(('a' <= c) && (c <= 'f'))
			data = c- 'a' + 10;
		else if(('A' <= c) && (c <= 'F'))
			data = c - 'A' + 10;
	}

	return data;
}

static s32 string_2_integer(const u8 *pstr)
{

	
	BOOL isSign = FALSE;
	BOOL isHex = FALSE;
	s32 num = 0;
	u8 c;
	
	while(isSpace(*pstr))	++pstr;

	if(*pstr == '-')//����
	{
		isSign = TRUE;
		pstr++;
	}
	else if(*pstr == '+')//����
	{
		++pstr;
	}

	if(isHexVal(pstr))//ʮ��������
	{
		isHex = TRUE;
		pstr += 2;
	}

	while(1)
	{
		c = *pstr++;
		if(isHex)
		{
			if(isDigit(c,HEX))//0~9��a~f
			{
				num = num*16 + c2b(c,HEX);
			}
			else
			{
				break;
			}
		}
		else
		{
			if(isDigit(c,DEC))//0~9
			{
				num = num*10 + c2b(c,DEC);
			}
			else
			{
				break;
			}

		}
	}

	return (isSign? -num : num);
}


static u16  shell_string_cmp(const u8 *s1, const u8 *s2)
{
	while(*s1 == *s2)
	{
		if(!*s1)//s1��s2��������������0
		{
			return 0;
		}
		s1++;
		s2++;
	}

	return (*s1 - *s2);	
}

static cmd_t* look_up_cmd_list(u8 *str)
{
	cmd_t *cmd;
	
	cmd = (cmd_t *)&sCmdTbl[0];

	while(cmd->pFullName != NULL)
	{
		if(!shell_string_cmp(str,cmd->pFullName))
		{
			return cmd;
		}

		if(!shell_string_cmp(str,cmd->PshortName))
		{
			return cmd;
		}

		cmd++;
	}

	return NULL;
}

//����TRUE��cmd�Ͳ����ָ����
//����FALSE��û��cmd�Ͳ�����Ҫ�ָ��ָ��쳣
static u8 parse_cmd(cmdctrl_t *pCtrl)
{
	u8 n = 0;
	u8 *p = pCtrl->buf;
	
	while(n < SHELL_MAX_ARGS)
	{
		while((*p == ' ') || (*p == '\t') || (*p == SEMICOLON))
		{
			p++;
		}

		if(*p == '\0')
		{
			return FALSE;
		}
		
		//��һ�μ���cmd���ڶ���ѭ�������ټ������
		pCtrl->argv[n++] = p;

		//������������,�ո�,Tab,�����˳�;û��������ִ����һ��,����һ��cmd
		while( (*p != '\0') && (*p != ' ') && (*p != '\t') && (*p != SEMICOLON))
		{
			p++;
		}

		if(*p == '\0')//�ָ���ɣ��˳�����
		{
			pCtrl->argc = n;//��ǰcmd����
			return TRUE;
		}

		*p++ = '\0';//�ѿո�,Tab,���Ÿ�Ϊ������,Ȼ���ٽ��в����ķָ�
	}

	return FALSE;
}

//����TRUE��cmd�Ͳ�����ȷ
//����FALSE��cmd�Ͳ�����ȡ����cmd�Ͳ�������SHELL_ARGS_LEN��С
static u8 receive_input_data(u8 *prompt,cmdctrl_t *pCtrl)
{
	u8 *p = pCtrl->buf;
	u8 c = 0;
	u8 n = 0;

	if(prompt)
	{
		//shell_send_string("\r\n");
		shell_send_string(prompt);
	}

	while(1)
	{
		c = shell_get_char();
		switch(c)
		{
		case ENTER:
			*p = '\0';
			shell_send_string("\r\n");
			return TRUE;
			
		case ESC:
			*p = '\0';
			shell_send_string("\r\n");
			return FALSE;
			
		default:
			if(n < SHELL_ARGS_LEN - 2)
			{
				*p++ = c;
				n++;
				shell_send_char(c);//�����յ����ַ�����PC������
			}
			else//���������յ�cmd
			{
            	*p = '\0';
            	return FALSE;					
			}
			break;
		}
	}
}


void shell_task(void *p)
{
	cmdctrl_t *pInfo = &sCmdctrl;//���ﲻ�ö���ȫ�ֱ�������Ϊֻ���ڸú����ڵ��ã����Ǿֲ�������Ҫ��ʼ��0ֵ
	
	while(1)
	{
		//��ȡ�û�����cmd�Ͳ���
		if(receive_input_data("cmd>>",pInfo))
		{
			//�ָ�cmd�Ͳ���
			if(parse_cmd(pInfo))
			{
				//����cmd
				pInfo->pCmd = look_up_cmd_list(pInfo->argv[0]);
				if(pInfo->pCmd != NULL)
				{
					if(pInfo->argc > pInfo->pCmd->bMaxArgs)
					{
						DBG_TRACE("argc out error!\r\n");
					}
					else
					{
						if(pInfo->pCmd->func)
						{
							if(pInfo->pCmd->func(pInfo) != NULL)
							{
								//DBG_TRACE("argc little error!\r\n");
							}
							else
							{
								//ִ�е����˵���ɹ�ִ���û��ص�������
							}
						}
					}
				}
				else
				{
					DBG_TRACE("cmd error!\r\n");
					
				}
			}
		}

	}

}


static void * cmd_help(cmdctrl_t *pCtrl)
{
	cmd_t *pcmd;

	pcmd = (cmd_t *)&sCmdTbl[0];
	
	if(pCtrl->argc == 1)
	{
		DBG_TRACE("\r\ncmd list!\r\n");
		while(pcmd->pFullName != NULL)
		{
			DBG_TRACE("%s(%s)\r\n",pcmd->pFullName,pcmd->PshortName);
			pcmd++;
		}
	}
	else if(pCtrl->argc == 2)
	{
		pcmd = look_up_cmd_list(pCtrl->argv[1]);
		if(pcmd)
		{
			if (pcmd->func)
			{
				DBG_TRACE("param:%s\r\n",pcmd->pHelp);
			}

		}
		else
		{
			DBG_TRACE("Unknown Command!\r\n");
		}
		//s32 data;
		//data = string_2_integer(pCtrl->argv[1]);
		//printf("data = %d\r\n",data);
		//printf("%s\r\n",pCtrl->pCmd->pTip);
		//return (void *) 1;
	}

	return (void *) 0;
}


static const cmd_t sCmdTbl[] = {
{"help","hp",cmd_help,2,HELP_TIP, HELP_HELP},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0}

};


