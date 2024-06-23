#include "common.h"


#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


#define MAX_PRNITEM		8
#define PRNITEM_MASK	(MAX_PRNITEM - 1)

#define	QITEM_PRIOR(x)		((x) & 0x0f)	//正常优先级还是面板优先级
#define	IS_PRLE_PRIOR(x)	((x) & 0x80)	//强制替换项：面板优先级以替换方式提交

//激活prnq任务的事件请求
#define PRNQ_SUBMIT_EVENT   (1 << 0)
#define PRNQ_REQ_EVENT      (1 << 1)

//执行item项的start回调函数
#define	CALL_QITEM_SFUNC(item)	do{\
									if((item)->s_func)\
									{\
										(item)->s_func((void*)item);\
										(item)->s_func = NULL;\
									}\
								}while(0)


//执行item项的end回调函数
#define	CALL_QITEM_EFUNC(item)	do{\
									if((item)->e_func)\
									{\
										(item)->e_func((void*)item);\
										(item)->e_func = NULL;\
									}\
								}while(0)	
								

typedef struct _PRNQUEUE
{
	QITEM	pnlItem;		//优先级最高的item项，主要是处理面板提交的item项。

	QITEM	prnQBuf[MAX_PRNITEM]; //任务队列，保存着将要执行的item项结构体
	
	PRNQ_STAT	qStat;			//参考prnQ.qStat
								//任务队列的状态标志，包括--
								//			队列激活、
								//			队列暂停、			

	PRNQ_REQ_STAT	qCtrlReq;	//参考prnQ.qCtrlReq
								//任务队列的控制请求,包括--
								//			暂停任务的执行、
								//			恢复任务的执行、					
								//			清除prnQBuf所有任务项、
								//			清除pnlItem任务项、


	u8	qCnt;		//1 ~ MAX_PRNITEM，qInIdx++时qCnt++	，qOutIdx++时qCnt--
	u8	qOutIdx;	//0 ~ MAX_PRNITEM-1，准备或者正在运行的item项后++
	u8	qInIdx;		//0 ~ MAX_PRNITEM-1，加入item项后++
}PRNTQUEUE;


#define SET_QSTAT_STATUS(m) 	{sPrnQ.qStat = (m);}              
#define GET_QSTAT_STATUS()  	(sPrnQ.qStat)


static BOOL isPrnqEmpty = TRUE;
EventGroupHandle_t gPrnqEvent;
static PRNTQUEUE sPrnQ;


/*
* 功能：上层提交item项到队列中
* 输入：item--某个动作/模块的item项
*     	prior--任务优先级,包括NORMAL_PRIOR,PANEL_PRIOR
* 返回：TRUE表示提交成功
*/
HWD submit_item(QITEM *item, u8 prior)
{

	HWD p = NULL;
	
	if(QITEM_PRIOR(prior) == PANEL_PRIOR)
	{
		if(sPrnQ.pnlItem.type != NO_MSG)
		{
#if 0//强制替换项暂不使用
			if(IS_PRLE_PRIOR(prior))//如果是强制替换项，则判断是否可以替换
			{
				if(sPrnQ.pnlItem.stat != ITEM_READY)
				{	
					//已经在执行，不可以替换，也暂时不能提交面板item项，需等待运行完成才能提交
					//上层需要判断是否强制替换成功
					return NULL;
				}
				else//上个面板item项还没激活运行，当前的面板item项可替换
				{
					//强制替换成功！
					//这里无需执行内容
				}
				
			}
			else//如果不是强制替换项，则已经有面板item项了，不能提交面板item项，需等待运行完成才能提交
			{
				return NULL;
			}
#else

			//已经有面板item项了，不能提交面板item项，需等待运行完成才能提交
			//目前上层需要重复提交，然后重复判断是否提交成功
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
		//item项队列已满，上层需要判断是否提交成功，重复提交才行
		return NULL;
	}

	p = (HWD)&sPrnQ.prnQBuf[sPrnQ.qInIdx];
	MEM_CPY(p,item,sizeof(QITEM));

    sPrnQ.qInIdx++;
    sPrnQ.qInIdx &= PRNITEM_MASK;
	sPrnQ.qCnt++;//需要在最后，防止prnq任务执行了空内容（多任务时，全局变量要后调先判）

	if(isPrnqEmpty)
	{
		xEventGroupSetBits(gPrnqEvent, PRNQ_SUBMIT_EVENT);
	}
	
	return p;
}


/*
* 功能：上层调用，给上层返回当前item项的运行结果
* 输入：h--当前执行的item项
* 返回：当前item项的运行结果(参考QITEM.stat说明)
* 使用注意：必须保证H的ITEM后，当前任务和其他任务没有提交超过MAX_PRNITEM个ITEM项，否则H的ITEM信息会被破坏
			这个注意可忽略，因为submit_item函数有提交超过MAX_PRNITEM个ITEM项的判断。
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
* 功能：清除item项的MSG_INFO信息
* 输入：item--任务项
*     	s--填充item项的运行结果，由底层PRNQ负责填充
* 返回：TRUE表示清除成功
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


/* 上层设置队列请求，控制队列，激活/暂停/删除 */
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


/* 底层处理队列请求 */
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
				//队列中有item项，并且当前item项还没开始运行
				if(sPrnQ.prnQBuf[sPrnQ.qOutIdx].stat == ITEM_READY)
				{
					SET_QSTAT_STATUS(QSTAT_PAUSE);
					sPrnQ.qCtrlReq = QREQ_NONE;
				}
				
				//如果已经运行了就等下个item项激活前再进来停止	
			}
			else
			{//队列中没有item项
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
			//如果队列中有item项，而且余下的item项还没激活运行，可以全部删除
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
				isPrnqEmpty = TRUE;//正常情况是全部item项运行完，才置TRUE的，现在全部删除了马上置TRUE
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			else if(sPrnQ.qCnt)
			{
				//如果已经运行了就等下个item项激活前再进来调用删除
			}
			else
			{
				//由上面的else if(sPrnQ.qCnt)执行下来，说明刚刚只有一项,正在运行当中，运行完就执行这里
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			break;

		case QREQ_PNL_CLS:
			if(sPrnQ.pnlItem.type != NO_MSG)
			{
				if(sPrnQ.pnlItem.stat == ITEM_READY)//有面板item项，还没激活才能删除
				{
					release_prnq_item(&sPrnQ.pnlItem,ITEM_CANCEL);
					sPrnQ.pnlItem.type = NO_MSG;
					sPrnQ.qCtrlReq = QREQ_NONE;
					isPrnqEmpty = TRUE;
				}
			}
			else
			{	//没有面板item项了，清除qCtrlReq删除标志
				sPrnQ.qCtrlReq = QREQ_NONE;
			}
			break;
		default:
			break;
	}
}


/* 
 * 功能：处理队列中的item项 ,处理过程中，一直改变item项的stat运行结果
 * 输入：当前item项 
 * 返回：item项stat的填充状态
*/
static ITEM_STAT prnq_dispatch(QITEM *item)
{
	u32 itemTag_ctrl;
	MSG_INFO *pInfo;
	MSG_STAT r = NO_RUN;
	ITEM_STAT ret;

	if(itemEnable(item))
	{
		/* 准备要运行，设置 ACTING 标志 */
		item->stat = ITEM_ACTING;

		/* 延迟激活项 */
        if(item->dlyTime > 0)
        {
            vTaskDelay(item->dlyTime);
            item->dlyTime = 0;
        }		

		itemTag_ctrl = GET_QITEM_TAG(item) & ITEM_TAG_MASK;//获取上层设置的item项属性,传给bios_xxx_fun函数处理
		
		if(item->s_func)
		{
			CALL_QITEM_SFUNC(item);//执行start回调，可以再次修改item项参数
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
				CALL_QITEM_EFUNC(item);//执行end回调，取消上层该动作/模块的忙标志
			}

			item->stat = ITEM_CPLT;
			ret = item->stat;
		}
		else if(r == RUN_ERR)
		{
			/* 已激活item项运行，但是运行失败，重新运行当前item项 */
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

		/* 没能激活item项运行，继续等待激活 */
		item->stat = ITEM_READY;
		ret = item->stat;
	}

	return ret;
}


/*
 功能：优先处理sPrnQ.pnlItem的item项，		返回成功时，删除队列中的该item项(即sPrnQ.pnlItem.type = NO_MSG)，否则就重复执行该item项。
		再处理队列中的item项，返回成功时，删除队列中的该item项(即sPrnQ.qCnt--和sPrnQ.qOutIdx++)，否则就重复执行该item项。
 返回：TRUE表示item项运行完成。
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


		//只要其他任务调用完submit_item的释放事件后，就会马上(1个任务切换时间？)激活prnq_task任务执行
		//最好确保其他任务只有1个事件是激活prnq_task任务的，方便理解（添加PRNQ_REQ_EVENT时再想）
		if(event & PRNQ_SUBMIT_EVENT|PRNQ_REQ_EVENT)
		{
			while(1)
			{		
				if(sPrnQ.qCtrlReq != QREQ_NONE)
				{
					/* 通过上层设置sPrnQ.qCtrlReq，处理队列请求 */
					prnq_req_proc();
				}
		
				if(sPrnQ.qCnt || sPrnQ.pnlItem.type != NO_MSG)
				{
					/* 处理item项 */
				
					isPrnqEmpty = FALSE;

					if(!act_prnq_item())
					{
						vTaskDelay(5);
					}
				}
				else
				{
					/* 队列item项全部处理完成 */
				
					isPrnqEmpty = TRUE;
					break;
				}
			}
		}


	}
}




