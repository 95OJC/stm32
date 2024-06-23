#include "common.h"


#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


#define MAX_PRNITEM		8
#define PRNITEM_MASK	(MAX_PRNITEM - 1)

#define	QITEM_PRIOR(x)		((x) & 0x0f)	//�������ȼ�����������ȼ�
#define	IS_PRLE_PRIOR(x)	((x) & 0x80)	//ǿ���滻�������ȼ����滻��ʽ�ύ

//����prnq������¼�����
#define PRNQ_SUBMIT_EVENT   (1 << 0)
#define PRNQ_REQ_EVENT      (1 << 1)

//ִ��item���start�ص�����
#define	CALL_QITEM_SFUNC(item)	do{\
									if((item)->s_func)\
									{\
										(item)->s_func((void*)item);\
										(item)->s_func = NULL;\
									}\
								}while(0)


//ִ��item���end�ص�����
#define	CALL_QITEM_EFUNC(item)	do{\
									if((item)->e_func)\
									{\
										(item)->e_func((void*)item);\
										(item)->e_func = NULL;\
									}\
								}while(0)	
								

typedef struct _PRNQUEUE
{
	QITEM	pnlItem;		//���ȼ���ߵ�item���Ҫ�Ǵ�������ύ��item�

	QITEM	prnQBuf[MAX_PRNITEM]; //������У������Ž�Ҫִ�е�item��ṹ��
	
	PRNQ_STAT	qStat;			//�ο�prnQ.qStat
								//������е�״̬��־������--
								//			���м��
								//			������ͣ��			

	PRNQ_REQ_STAT	qCtrlReq;	//�ο�prnQ.qCtrlReq
								//������еĿ�������,����--
								//			��ͣ�����ִ�С�
								//			�ָ������ִ�С�					
								//			���prnQBuf���������
								//			���pnlItem�����


	u8	qCnt;		//1 ~ MAX_PRNITEM��qInIdx++ʱqCnt++	��qOutIdx++ʱqCnt--
	u8	qOutIdx;	//0 ~ MAX_PRNITEM-1��׼�������������е�item���++
	u8	qInIdx;		//0 ~ MAX_PRNITEM-1������item���++
}PRNTQUEUE;


#define SET_QSTAT_STATUS(m) 	{sPrnQ.qStat = (m);}              
#define GET_QSTAT_STATUS()  	(sPrnQ.qStat)


static BOOL isPrnqEmpty = TRUE;
EventGroupHandle_t gPrnqEvent;
static PRNTQUEUE sPrnQ;


/*
* ���ܣ��ϲ��ύitem�������
* ���룺item--ĳ������/ģ���item��
*     	prior--�������ȼ�,����NORMAL_PRIOR,PANEL_PRIOR
* ���أ�TRUE��ʾ�ύ�ɹ�
*/
HWD submit_item(QITEM *item, u8 prior)
{

	HWD p = NULL;
	
	if(QITEM_PRIOR(prior) == PANEL_PRIOR)
	{
		if(sPrnQ.pnlItem.type != NO_MSG)
		{
#if 0//ǿ���滻���ݲ�ʹ��
			if(IS_PRLE_PRIOR(prior))//�����ǿ���滻����ж��Ƿ�����滻
			{
				if(sPrnQ.pnlItem.stat != ITEM_READY)
				{	
					//�Ѿ���ִ�У��������滻��Ҳ��ʱ�����ύ���item���ȴ�������ɲ����ύ
					//�ϲ���Ҫ�ж��Ƿ�ǿ���滻�ɹ�
					return NULL;
				}
				else//�ϸ����item�û�������У���ǰ�����item����滻
				{
					//ǿ���滻�ɹ���
					//��������ִ������
				}
				
			}
			else//�������ǿ���滻����Ѿ������item���ˣ������ύ���item���ȴ�������ɲ����ύ
			{
				return NULL;
			}
#else

			//�Ѿ������item���ˣ������ύ���item���ȴ�������ɲ����ύ
			//Ŀǰ�ϲ���Ҫ�ظ��ύ��Ȼ���ظ��ж��Ƿ��ύ�ɹ�
			return NULL;
#endif
		}

        p = (HWD)&sPrnQ.pnlItem;
        MEM_CPY(p,item,sizeof(QITEM));
		
        if(isPrnqEmpty)
        {
            xEventGroupSetBits(gPrnqEvent, PRNQ_SUBMIT_EVENT);
        }
        return p;		
	}
	else if(sPrnQ.qCnt > MAX_PRNITEM)
	{
		//item������������ϲ���Ҫ�ж��Ƿ��ύ�ɹ����ظ��ύ����
		return NULL;
	}

	p = (HWD)&sPrnQ.prnQBuf[sPrnQ.qInIdx];
	MEM_CPY(p,item,sizeof(QITEM));

    sPrnQ.qInIdx++;
    sPrnQ.qInIdx &= PRNITEM_MASK;
	sPrnQ.qCnt++;//��Ҫ����󣬷�ֹprnq����ִ���˿����ݣ�������ʱ��ȫ�ֱ���Ҫ������У�

	if(isPrnqEmpty)
	{
		xEventGroupSetBits(gPrnqEvent, PRNQ_SUBMIT_EVENT);
	}
	
	return p;
}


/*
* ���ܣ��ϲ���ã����ϲ㷵�ص�ǰitem������н��
* ���룺h--��ǰִ�е�item��
* ���أ���ǰitem������н��(�ο�QITEM.stat˵��)
* ʹ��ע�⣺���뱣֤H��ITEM�󣬵�ǰ�������������û���ύ����MAX_PRNITEM��ITEM�����H��ITEM��Ϣ�ᱻ�ƻ�
			���ע��ɺ��ԣ���Ϊsubmit_item�������ύ����MAX_PRNITEM��ITEM����жϡ�
*/
ITEM_STAT check_msg_status(HWD h)
{
	ITEM_STAT s = ITEM_READY;
	QITEM *pItem;

	if(h != NULL)
	{
		pItem = (QITEM *)h;
		s = pItem->stat;
	}

	return s;
}


PRNQ_STAT get_prnq_status(void)
{
    return sPrnQ.qStat;
}


u8 get_prnq_cnt(void)
{
    return sPrnQ.qCnt;
}


BOOL is_prnq_empty(void)
{
	BOOL ret = TRUE;
	
	if(sPrnQ.qCnt)
		ret = FALSE;
		
    return ret;
}


BOOL is_prnq_pnl_empty(void)
{
	BOOL ret = FALSE;
	
	if(sPrnQ.pnlItem.type == NO_MSG)
		ret = TRUE;
		
    return ret;
}


u8 get_prnq_free_cnt(void)
{
    return MAX_PRNITEM - sPrnQ.qCnt;
}


/*
* ���ܣ����item���MSG_INFO��Ϣ
* ���룺item--������
*     	s--���item������н�����ɵײ�PRNQ�������
* ���أ�TRUE��ʾ����ɹ�
*/
static BOOL release_prnq_item(QITEM *item, ITEM_STAT s)
{
	BOOL ret = TRUE;
	MSG_INFO *pInfo;
	
	item->stat = s;
	pInfo = (MSG_INFO *)item->buf;
	
	CALL_QITEM_SFUNC(item);

	switch(item->type)
	{
		case INIT_MSG:
			MEM_SET(&pInfo->init, 0, sizeof(INIT_INFO));
			break;
		
			
		default :
			break;
	}
	
	CALL_QITEM_EFUNC(item);

	return ret;
}


/* �ϲ����ö������󣬿��ƶ��У�����/��ͣ/ɾ�� */
BOOL set_prnq_req(PRNQ_REQ_STAT req)
{
	BOOL retVal = TRUE;

	sPrnQ.qCtrlReq = QREQ_NONE;

	switch (req)
		{
		case QREQ_PNL_CLS:
			sPrnQ.qCtrlReq = QREQ_PNL_CLS;
			break;

		case QREQ_CLS:
			sPrnQ.qCtrlReq = QREQ_CLS;
			break;

		case QREQ_PAUSE:
			sPrnQ.qCtrlReq = QREQ_PAUSE;
			break;

		case QREQ_RESUME:
			if(GET_QSTAT_STATUS() == QSTAT_PAUSE)
			{
				sPrnQ.qCtrlReq = QREQ_RESUME;
			}
			break;

		default:
			break;
		}

	xEventGroupSetBits(gPrnqEvent, PRNQ_REQ_EVENT);

	while(sPrnQ.qCtrlReq != QREQ_NONE)
    {
        vTaskDelay(1);
    }
	
	return retVal;
}


/* �ײ㴦��������� */
static void prnq_req_proc(void)
{
	PRNQ_REQ_STAT req;
	QITEM *pItem;

	req = sPrnQ.qCtrlReq;

	switch (req)
	{
		case QREQ_PAUSE:
			if(sPrnQ.qCnt)
			{
				//��������item����ҵ�ǰitem�û��ʼ����
				if(sPrnQ.prnQBuf[sPrnQ.qOutIdx].stat == ITEM_READY)
				{
					SET_QSTAT_STATUS(QSTAT_PAUSE);
					sPrnQ.qCtrlReq = QREQ_NONE;
				}
				
				//����Ѿ������˾͵��¸�item���ǰ�ٽ���ֹͣ	
			}
			else
			{//������û��item��
				SET_QSTAT_STATUS(QSTAT_PAUSE);
				sPrnQ.qCtrlReq = QREQ_NONE;		
			}
			break;

		case QREQ_RESUME:
			if(GET_QSTAT_STATUS() == QSTAT_PAUSE)
			{
				SET_QSTAT_STATUS(QSTAT_ACT);
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			break;

		case QREQ_CLS:
			//�����������item��������µ�item�û�������У�����ȫ��ɾ��
			if(sPrnQ.qCnt && (sPrnQ.prnQBuf[sPrnQ.qOutIdx].stat == ITEM_READY))
			{
				while(sPrnQ.qCnt)
				{
					pItem = &sPrnQ.prnQBuf[sPrnQ.qOutIdx++];
					sPrnQ.qOutIdx &= PRNITEM_MASK;
					release_prnq_item(pItem,ITEM_CANCEL);
					sPrnQ.qCnt--;	
				}
				
	            sPrnQ.qCnt = 0;
	            sPrnQ.qInIdx = 0;
	            sPrnQ.qOutIdx = 0;
				isPrnqEmpty = TRUE;//���������ȫ��item�������꣬����TRUE�ģ�����ȫ��ɾ����������TRUE
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			else if(sPrnQ.qCnt)
			{
				//����Ѿ������˾͵��¸�item���ǰ�ٽ�������ɾ��
			}
			else
			{
				//�������else if(sPrnQ.qCnt)ִ��������˵���ո�ֻ��һ��,�������е��У��������ִ������
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			break;

		case QREQ_PNL_CLS:
			if(sPrnQ.pnlItem.type != NO_MSG)
			{
				if(sPrnQ.pnlItem.stat == ITEM_READY)//�����item���û�������ɾ��
				{
					release_prnq_item(&sPrnQ.pnlItem,ITEM_CANCEL);
					sPrnQ.pnlItem.type = NO_MSG;
					sPrnQ.qCtrlReq = QREQ_NONE;
					isPrnqEmpty = TRUE;
				}
			}
			else
			{	//û�����item���ˣ����qCtrlReqɾ����־
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			break;
		default:
			break;
	}
}


/* 
 * ���ܣ���������е�item�� ,��������У�һֱ�ı�item���stat���н��
 * ���룺��ǰitem�� 
 * ���أ�item��stat�����״̬
*/
static ITEM_STAT prnq_dispatch(QITEM *item)
{
	u32 itemTag_ctrl;
	MSG_INFO *pInfo;
	MSG_STAT r = NO_RUN;
	ITEM_STAT ret;

	if(itemEnable(item))
	{
		/* ׼��Ҫ���У����� ACTING ��־ */
		item->stat = ITEM_ACTING;

		/* �ӳټ����� */
        if(item->dlyTime > 0)
        {
            vTaskDelay(item->dlyTime);
            item->dlyTime = 0;
        }		

		itemTag_ctrl = GET_QITEM_TAG(item) & ITEM_TAG_MASK;//��ȡ�ϲ����õ�item������,����bios_xxx_fun��������
		
		if(item->s_func)
		{
			CALL_QITEM_SFUNC(item);//ִ��start�ص��������ٴ��޸�item�����
		}

		pInfo = (MSG_INFO *)item->buf;

		switch (GET_QITEM_TYPE(item))
		{
			case INIT_MSG:
				pInfo->init.ctrl |= itemTag_ctrl;
				while(1)
				{
					r = bios_dev_init(&pInfo->init);
					if(r == RUN_OK || r == RUN_ERR)
					{
						break;
					}
					else//r == RUNNING
					{
						vTaskDelay(5);
					}
				}
				break;


			default:
				r = RUN_OK;
				break;
		}

		if(r == RUN_OK)
		{
			if(item->e_func)
			{
				CALL_QITEM_EFUNC(item);//ִ��end�ص���ȡ���ϲ�ö���/ģ���æ��־
			}

			item->stat = ITEM_CPLT;
			ret = item->stat;
		}
		else if(r == RUN_ERR)
		{
			/* �Ѽ���item�����У���������ʧ�ܣ��������е�ǰitem�� */
			item->stat = ITEM_ACTING;
			ret = item->stat;		
		}
	}
	else
	{
		if(item->hook_func)
		{
			item->hook_func(item);
			//item->hook_func = NULL;
		}

		/* û�ܼ���item�����У������ȴ����� */
		item->stat = ITEM_READY;
		ret = item->stat;
	}

	return ret;
}


/*
 ���ܣ����ȴ���sPrnQ.pnlItem��item�		���سɹ�ʱ��ɾ�������еĸ�item��(��sPrnQ.pnlItem.type = NO_MSG)��������ظ�ִ�и�item�
		�ٴ�������е�item����سɹ�ʱ��ɾ�������еĸ�item��(��sPrnQ.qCnt--��sPrnQ.qOutIdx++)��������ظ�ִ�и�item�
 ���أ�TRUE��ʾitem��������ɡ�
*/
static BOOL act_prnq_item(void)
{
	BOOL ret = FALSE;

	ITEM_STAT status;

	if(sPrnQ.pnlItem.type != NO_MSG)
	{
		status = prnq_dispatch(&sPrnQ.pnlItem);
		if(status == ITEM_CPLT)
		{
			sPrnQ.pnlItem.type = NO_MSG;
			ret = TRUE;
		}
		
		return ret;
	}

	if(sPrnQ.qCnt && (GET_QSTAT_STATUS() != QSTAT_PAUSE))
	{
		status = prnq_dispatch(&sPrnQ.prnQBuf[sPrnQ.qOutIdx]);

		if(status == ITEM_CPLT)
		{
			sPrnQ.qCnt--;
			
			sPrnQ.qOutIdx++;
            sPrnQ.qOutIdx &= PRNITEM_MASK;
			
            ret = TRUE;
		}
	}

	return ret;
}


void prnq_task(void *p)
{
	EventBits_t event;
	
	MEM_SET(&sPrnQ, 0, sizeof(sPrnQ));
	gPrnqEvent = xEventGroupCreate();
	isPrnqEmpty = TRUE;
	
	while(1)
	{
		do{
			event = xEventGroupWaitBits(gPrnqEvent,PRNQ_SUBMIT_EVENT|PRNQ_REQ_EVENT,pdTRUE,pdFAIL,portMAX_DELAY);
		}while(!event);


		//ֻҪ�������������submit_item���ͷ��¼��󣬾ͻ�����(1�������л�ʱ�䣿)����prnq_task����ִ��
		//���ȷ����������ֻ��1���¼��Ǽ���prnq_task����ģ�������⣨���PRNQ_REQ_EVENTʱ���룩
		if(event & PRNQ_SUBMIT_EVENT|PRNQ_REQ_EVENT)
		{
			while(1)
			{		
				if(sPrnQ.qCtrlReq != QREQ_NONE)
				{
					/* ͨ���ϲ�����sPrnQ.qCtrlReq������������� */
					prnq_req_proc();
				}
		
				if(sPrnQ.qCnt || sPrnQ.pnlItem.type != NO_MSG)
				{
					/* ����item�� */
				
					isPrnqEmpty = FALSE;

					if(!act_prnq_item())
					{
						vTaskDelay(5);
					}
				}
				else
				{
					/* ����item��ȫ��������� */
				
					isPrnqEmpty = TRUE;
					break;
				}
			}
		}


	}
}




