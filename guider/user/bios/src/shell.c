#include "common.h"
#include "main.h"
#include "shell.h"

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

#if 1
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

static void * cmd_TTP224_test(cmdctrl_t *pCtrl)
{
	u8 data;
	u8 pc_in;
	u8 key_ret;
	
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}
	
	data = (u8)string_2_integer(pCtrl->argv[1]);
	
	if(data == 0)
	{
		keyTTP224_init();
		DBG_TRACE("TTP224_init ok!\r\n");
	}
	else
	{
		do{
			key_ret = shell_keyTTP224_test();
			if(key_ret)
			{
				if(key_ret == 1)
					DBG_TRACE("1\r\n");
				if(key_ret == 2)
					DBG_TRACE("2\r\n");
				if(key_ret == 3)
					DBG_TRACE("3\r\n");
				if(key_ret == 4)
					DBG_TRACE("4\r\n");				
			}
			shell_asyn_get_char(&pc_in);
			
		}while(pc_in != ESC);
	}
	return (void *) 0;
}

static void * cmd_WaterSensor_test(cmdctrl_t *pCtrl)
{
	u8 data;
	u8 pc_in;
	u8 WS_ret;	
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}

	data = (u8)string_2_integer(pCtrl->argv[1]);

	if(data == 0)
	{
		WaterSensor_init();
		DBG_TRACE("WaterSensor_init ok!\r\n");
	}
	else
	{
		do{	
			WS_ret = WaterSensor_monitor();
			if(WS_ret)
			{
				DBG_TRACE("��ˮ!\r\n");
			}
			shell_asyn_get_char(&pc_in);

		}while(pc_in != ESC);		
	}

	return (void *) 0;
}

static void * cmd_DCmotor_test(cmdctrl_t *pCtrl)
{
	u8 data;
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}

	if(data == 0)
	{
		DCmotor_init();
		DBG_TRACE("heater_init ok!\r\n");
	}
	else
	{
		DCmotor_toggle();
		DBG_TRACE("��ˮ/ȡ��\r\n");
	}

	return (void *) 0;
}

static void * cmd_heater_test(cmdctrl_t *pCtrl)
{
	u8 data;
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}

	if(data == 0)
	{
		heater_init();
		DBG_TRACE("heater_init ok!\r\n");
	}
	else
	{
		heater_toggle();
		DBG_TRACE("����/����\r\n");
	}

	return (void *) 0;
}

static void * cmd_DS18B20_test(cmdctrl_t *pCtrl)
{
	u8 data;
	u8 pc_in;
	u16 temp;
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}
	
	data = (u8)string_2_integer(pCtrl->argv[1]);
	
	if(data == 0)
	{
		DS18B20_init();
		DBG_TRACE("DS18B20_init ok!\r\n");
	}
	else
	{
		do{	
			temp = DS18B20_ReadTemperature();
			DBG_TRACE("temp = %d\r\n",temp);
			
			shell_asyn_get_char(&pc_in);

			vTaskDelay(200);
		}while(pc_in != ESC);			
	}

	return (void *) 0;
}

static void * cmd_OLED096_test(cmdctrl_t *pCtrl)
{
#ifdef USE_OLED_LIB
	u8 data;
	u8 i,j;	
	
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}

	data = (u8)string_2_integer(pCtrl->argv[1]);

	if(data ==0)
	{
		OLED_Init();
		DBG_TRACE("OLED_Init ok!\r\n");
	}
	else if(data == 1)//����8*16�ַ�
	{
		for(j=0;j<8;j+=2)
		{
			for(i=0;i<128;i+=8)
			{
				OLED_ShowChar(i,j,'a',16);
			}
		}
	}
	else if(data == 2)//����6*8�ַ�
	{
		for(j=0;j<8;j++)
		{
			for(i=0;i<=120;i+=6)
			{
				OLED_ShowChar(i,j,'a',12);
			}
		}
	}
	else if(data == 3)//����8*16����
	{
		for(j=0;j<8;j+=2)
		{
			for(i=0;i<128;i+=16)
			{
				OLED_ShowNum(i,j,12,2,16);
			}
		}
		
		vTaskDelay(500);
		
		for(j=0;j<8;j+=2)
		{
			for(i=0;i<128;i+=8)
			{
				OLED_ShowNum(i,j,1,1,16);
			}
		}
	}
	else if(data == 4)//����16*16����
	{
		for(j=0;j<8;j+=2)
		{
			for(i=0;i<128;i+=16)
			{
				OLED_ShowCHinese(i,j,0);
			}
		}
	}
	
#else
	u8 data;
	u8 pc_in;
		
	if(pCtrl->argc < 2)
	{
		DBG_TRACE("%s\r\n",pCtrl->pCmd->pTip);
		return (void *) 1;
	}

	if(data == 0)
	{
		OLED096_init();
		DBG_TRACE("OLED096_init ok!\r\n");
	}
	else
	{
		do{	
			OLED096_full_test();
			vTaskDelay(500);
			OLED096_clear_test();
			
			shell_asyn_get_char(&pc_in);

			vTaskDelay(200);
		}while(pc_in != ESC);			
	}

#endif
	return (void *) 0;

}

#endif

static const cmd_t sCmdTbl[] = {
{"help","hp",cmd_help,2,HELP_TIP, HELP_HELP},
{"TTP224","TTP224",cmd_TTP224_test,2,TTP224_TIP, TTP224_HELP},
{"WaterSensor","WaterSensor",cmd_WaterSensor_test,2,WaterSensor_TIP, WaterSensor_HELP},
{"heater","heater",cmd_DCmotor_test,2,HEATER_TIP, HEATER_HELP},
{"heater","heater",cmd_heater_test,2,HEATER_TIP, HEATER_HELP},
{"DS18B20","DS18B20",cmd_DS18B20_test,2,DS18B20_TIP, DS18B20_HELP},
{"OLED096","OLED096",cmd_OLED096_test,2,OLED096_TIP, OLED096_HELP},
{0,0,0,0,0,0},
{0,0,0,0,0,0},
{0,0,0,0,0,0}

};


