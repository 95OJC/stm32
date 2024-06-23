#include "common.h"
#include "if.h"

/*-----------------------------------------------------------------
ʵ��ͨѶ�ӿڷֲ�Ĺ��ܣ�ʹ�ײ���ϲ�ͨѶ�ӿں����ֿ���

���裺
1���ṹ������driver_tbl����MAX_DRIVER_NUM��IF_DRIVER�ṹ��ע��ص�������
2���ṹ��ָ��pCurrentDriver���ڵ�ǰ�����ĸ�port��ִ�лص������������ݡ�
3���谴������˳��:
   �ٵײ�if_init���ѳ�ʼ���ṹ��ָ��pCurrentDriver����ΪNULL��driver_tbl[i].valid��driver_tbl[i].tag��
   �ڵײ�if_register_driver��ע��PORT_NUM��Ӧ�Ļص�����������driver_tbl[i].valid��Ч��
   ���ϲ�if_open����ʼ���ϲ㴫��ö��ֵPORT_NUM��ͨѶ�ӿں�����driver_tbl[i].tag�򿪡�
   ���ϲ�if_set_cur_port�������ϲ㴫��ö��ֵPORT_NUMΪdriver_tbl��������ָ��pCurrentDriver��
   ���ϲ�close��read��write��peek����ʹ��ǰ�����ж�ָ��pCurrentDriver�Ƿ�Ϊ�ա�
   

�������shell�õ��Ļص���
static const cmd_t sCmdTbl[] = {
{"help","hp",help,1,"tip", "help"},
{0,0,0,0,0,0},
{0,0,0,0,0,0}
};
����Ľṹ�������������鸳ֵ����ʽ��ֵ�ص�������Ȼ��pInfo->pCmd = &sCmdTbl[0]��pInfo->pCmd->func���ɵ��ûص�������


�ܽ�if��shell�Ļص������÷���
ע��ص�����������shell��ֱ���ڽṹ������ĳ�ʼ����ֵ���е�ע��ص���������if�����ṩ��һ��ע�ắ����ͨѶ�ӿ��ļ����ã���Ҫ
			  ����ö��ֵPORT_NUM��ֵ�ĸ��ṹ��ʹ���������ע��Ļص�������
			      
ʹ�ûص�����������shell��һ���ýṹ������ָ��pInfo->pCmdֱ��ָ��ýṹ������ĳ���±꣬Ȼ��ȥʹ��pInfo->pCmd->func1/func2/func3��ִ�лص�������
			  (func1/func2/func3��ͬһ���±�Ļص���������Ӧif���pCurrentDriver->read/write/peek�Ȼص�����)
			  ��if��������һ���ýṹ������ָ��pCurrentDriverͨ��if_set_cur_port����ָ��ýṹ������ĳ���±�(�±�:�ϲ����ö˿ڶ�������ö��ֵPORT_NUM)��
			  Ȼ���дread��write��peek��io_ctrl�Ⱥ�����������pCurrentDriver->read��(����...)ִ�лص�����


ע�⣺if_open��if_io_ctrl�Ⱥ���������������ͺ�ͨѶ�ӿڻص��������������������һ���ģ���˶��������ʱ��Ҫ���ϲ�ʹ�ú͵ײ�ע��/ʹ�õĹ���ͷ�ļ���


���ͨ��ָ����ûص��ҵ�ԭ�����õ�ԭʼ������(һ�������ǿ�������ָ��ȥִ�лص��������뿴�ص���ԭʼ��������Ҫ��������2�������)
1���ҵ��õ��ûص���ָ�룬����ָ���ĸ��ص��������͵ı����������±ꡣ
2���ҵ��ص��������͵ı����������±��ע�ắ���������ҵ��ص���ԭʼ������

------------------------------------------------------------------*/

#define	MAX_DRIVER_NUM	10

static IF_DRIVER driver_tbl[MAX_DRIVER_NUM];//�ýṹ��ѡ�����±꣬���ⲿ��ֵ
static IF_DRIVER *pCurrentDriver;//�ýṹ����ڲ���ֵ��ѡ��ǰͨѶ��

void if_init(void)
{
	u8 i;

	pCurrentDriver = NULL;

	for(i=0; i<MAX_DRIVER_NUM; i++)
	{
		driver_tbl[i].valid = FALSE;
		driver_tbl[i].tag = FALSE;
	}

}

void if_register_driver(
	PORT_NUM n,
	BOOL (*open)(BUFINFO *pInfo, void *pCtrl),
	BOOL (*close)(void),
	u32  (*read)(u8 *buf, u32 len),
	u32  (*write)(u8 *buf, u32 len),
	u32  (*peek)(u8 *buf, u32 len),
	BOOL (*io_ctrl)(IOCTRL_TYPE type, void *ctrl)
)
{
	IF_DRIVER *p;

	p = &driver_tbl[n];

	p->valid = TRUE;
	p->tag = FALSE;
	p->open = open;
	p->close = close;
	p->read = read;
	p->write = write;
	p->peek = peek;
	p->io_ctrl = io_ctrl;

}


PORT_NUM if_find_data_port(void)
{
    PORT_NUM port = NULL_PORT;
    u8 i;
    u32 n;
    
    for(i=0; i<MAX_DRIVER_NUM; i++)
    {
        if(!driver_tbl[i].valid || !(driver_tbl[i].tag & IF_OPEN_BIT))
        {
            continue;
        }

        driver_tbl[i].io_ctrl(IOCTRL_RX_DATA_SIZE,&n);
        
        if(n > 0)
        {
            port = (PORT_NUM)i;
            break;
        }
    }
    
    return port;
}


BOOL if_open(PORT_NUM n, BUFINFO * pInfo, void *pCtrl)
{
	BOOL ret = FALSE;
	IF_DRIVER *pDriver;

	if(n >= MAX_DRIVER_NUM)
	{
		return FALSE;
	}

	pDriver = &driver_tbl[n];

	if(!pDriver->valid)
	{
		return FALSE;
	}

	if(pDriver->open(pInfo,pCtrl))
	{
		pDriver->tag = TRUE;
		ret = TRUE;
	}

	return ret;
}


BOOL if_set_cur_port(PORT_NUM n)
{
	if(n >= MAX_DRIVER_NUM)
	{
		return FALSE;
	}

	if(!driver_tbl[n].tag)
	{
		return FALSE;
	}

	pCurrentDriver = &driver_tbl[n];

	return TRUE;
}

BOOL if_close(void)
{
	BOOL ret = FALSE;

	if(pCurrentDriver)
	{
		if(pCurrentDriver->close())
		{
			pCurrentDriver->tag = FALSE;
			ret = TRUE;
		}
	}

	return ret;
}

u32 if_read_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->read(buf,len);
	}

	return ret;
}

u32 if_write_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->write(buf,len);
	}

	return ret;
}

u32 if_peek_data(u8 *buf, u32 len)
{
	u32 ret = 0;

	if(pCurrentDriver)
	{
		ret = pCurrentDriver->peek(buf,len);
	}

	return ret;
}

BOOL if_io_ctrl(IOCTRL_TYPE type, void *ctrl)
{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->io_ctrl(type,ctrl);
    }

    return ret;   
}


