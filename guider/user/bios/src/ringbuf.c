#include "common.h"
#include "ringbuf.h"

/*-----------------------------------------------------------------
ʵ��FIFO�Ļ���ѭ��ʹ�á�

//���ֽ�
1��ÿ��data_q_in������������ǰ����is_data_q_full�������ƣ����in��out֮��ʣ��һ���ֽڵĿռ�ʱ��ʾFIFO������data_q_in�����᷵��FALSE��
    a��Ϊ��ֹ����FALSE����Ҫ��data_q_in����������data_q_near_full�����ж�FIFO�Ƿ�׼������
    b������ô����ն�����ݣ��ж�����������ҪԶԶ����2���ֽڣ��������Ӳ����������������   	  ���������ص�������Ϊdata_q_in�����᷵��FALSE��û�м���FIFO�����¶�����
   
2��ÿ��data_q_out����ȡFIFO����ǰ����(q->pIn == q->pOut)�ж϶����Ƿ�Ϊ�գ��վͷ���FALSE��
    a��ÿ��ȡFIFOһ���ֽ����ݺ󣬶���data_q_out����������data_q_near_empty������������Ӳ���ء�
   ����ô��ڷ��ͽ�����Ӳ���أ��жϿյ������ɵ���0��
	   
   //�������in���ݵ��ٶȱȴ���out���ݵ��ٶȿ죬�ǲ�����ڶ���Ϊ�յ������ֻ��������ڶ������������
   //�������in���ݵ��ٶȱȴ���out���ݵ��ٶ������ǲ�����ڶ���Ϊ���������ֻ��������ڶ��пյ������
   //������in���ݵ��ٶȱȴ���out���ݵ��ٶȿ죬����ô��ڷ��ͽ�����Ӳ���أ��жϿյ������ɵ���0��  
   //������in���ݵ��ٶȱȴ���out���ݵ��ٶ���������ô��ڷ��ͽ�����Ӳ���أ��жϿյ��������ԶԶ����0���������Է�ֹ����Ϊ�յ�������ֵȴ���һ�������ݲ��γɿ�ִ������(���ʱ���൱�ڵȴ�)��


//���ֽ�
1��ÿ��data_q_in_more������������ǰ����get_data_q_in_block_free_size�������ƣ���������is_data_q_full�������ƣ������򷵻�n=0����������㲢���ؿɼ�����������n��Ȼ��ֻ����len�����ݣ����պ������سɹ������len��
    a��Ϊ��ֹ����n=0����n<len����Ҫ��data_q_in_more����������data_q_near_full�����ж�FIFO�Ƿ�׼������
    b�������USB�ն�����ݣ��ж���������������Ҫ����2����������
   ���ǵ�USB������ҪUSB�ӻ�ack���ܷ���һ���������񴮿����������ذɣ��������Ļ����ж���������Ϊ0�����У���Ϊ�ж�full��Ҫ��һ���ֽ����ݣ��������༸���ж�full���ף�
   ��ʵ��USBÿ��out���жϣ��ʼʱ�ж��Ƿ��������ж�������out�˵�İ�(�����Զ�����ACK��������һ����)������ж����ˣ������out����Ͳ����ټ����հ��ˡ�

2��ÿ��data_q_out_more����ȥ��FIFO����ǰ����get_data_q_out_block_free_size�������ƣ���������is_data_q_empty�������ƣ���(q->pIn == q->pOut)Ϊ�շ���n=0����������㲢���ؿ�ȡ�����������n��Ȼ��ֻȡ��len�����ݣ�
   ���պ������سɹ�ȡ����len��
	a��ÿ��ȡ��FIFO��һ�����ݺ󣬶���data_q_out_more����������data_q_near_empty������������ء�
	b�����ÿ��ȡ��ֵΪһ��len���ȣ���ôdata_q_out_more�������ܴ��ڷ���С��len���������ֹ�������data_q_near_empty������������ص��жϿյ���������Ϊ1����len��
   ���ǣ���ʵ����read����ÿ��ȡ��ֵΪһ��len=1����˿��԰�������data_q_out�������жϿյ������ɵ���0��



//�ܽ᣺
1��ringbuf.c�ļ��ĺ�������:�Դ������ΪFIFO_CTRL���ͽṹ��������in��out��peek����������near_full��near_empty�жϣ�ǰ����Ҫִ��data_q_init����ָ��FIFO_CTRL���ͽṹ�������Ϊ����
2������FIFO_CTRL_INFO�ṹ����ringbuf.c��uart.c�ȵײ�ͨѶ�ӿ��ļ�����Ҫ�õ��������Ҫ��ringbufApi.h�ļ����塣
3������data_q_in��������data_q_in_more�����������ݣ��ж�����������Ϊ0(Զ����2�ֽڻ�Զ���ڵ���)������ᶪ����
4������data_q_out��������data_q_out_more����ȡ�����ݣ��жϿ���������Ϊ0��
5��data_q_in������data_q_out�����Ӻ�ȡ����ʧ��ʱ����FALSE��
6��data_q_in_more������data_q_out_more�����Ӻ�ȡ����ʧ��ʱ����n=0���߼Ӻ�ȡ���ݲ���ʱ����n<len��ȡ����n<lenʱ��Ӱ��ʹ�ðѣ�



//DMA-ringbuf-list
��������˵������о��DMA��������ĺô������๤���޸ĵ�ʵuart1.c�ļ����ӣ���
1���жϺ�������Աȣ�
   ��ÿ��DMA����жϲ�����in+��DMA����ڵ��е���һ���׵�ַ(��һ��DMA���ݴ���׵�ַ)�����ж�near_full����,ע�����ز���������Ч����˻Ḳ��out�����ǲ��֣������Ҫ��2��block��Ҫ���������ˣ�
   ��near_empty������2��block��ã���û���1��block����������ԭ��
   ��ÿ�δ����жϽ������ݵ�data_q_in�����������ж�FIFO��full����in++����ָ����star�����ж�near_full���ء�
   ���߲��DMA����ֻ��Ҫ�ı�inָ��Ϊ��һ���׵�ַ���Դ����Σ�����data_q_in��������Ҫ��ָ����star��DMA���Դ�����star���������near_full�㹻���ж�FIFO��full����û�У���Ϊ�����ز���full��

2��read��������Աȣ�
   ��ÿ��DMA����ж���ı���inָ����൱���������ˣ���DMAһֱ�ڶ�����CPU��������ôread������Ҫinָ��ָ��ǰ׼��������dma��ַ���ǵù��жϲ���(��ֹ�������),Ȼ��data_q_out�������ж�near_empty���ء�
   (��Ϊ����Ϊ����︳ֵ�������⣬������ΪDMAʱ�̽������ݵ���inָ��ʱ�̱仯�����������жϽ��յģ�read�������������inָ��Ҳʱ�̱仯������inָ��û�б���ֵ����˲��õ��Ļ�����⣩
   ��ÿ�δ���read����������DMAʱ��ֱ��data_q_out�������ж�near_empty���ؼ��ɡ�
   ���߲����������DMA��������ʱ��read������Ҫ�ڹ��жϺ�inָ��ָ��ǰ׼��������dma��ַ�ٿ��жϣ�������DMA��������Ĵ���read����ֱ��data_q_out�������ж�near_empty���ؼ��ɡ�
   
   
------------------------------------------------------------------*/


/**
@brief ��ȡ������������
@param q :���е�ַ
@return �����е�������
*/
u32 get_data_q_size(FIFO_CTRL *q)
{
	s32 cnt;

	cnt = (s32)(q->pIn - q->pOut);

	if(cnt < 0)
	{
		cnt += q->Size;
	}

	return (u32)cnt;
}


/**
@brief ��ȡ����ʣ���������
@param q : ���е�ַ
@return ���п�������
*/
u32 get_data_q_free_size(FIFO_CTRL *q)
{
	//��Ϊsize�Ѿ������ֵ����Ҫ-1
	return (q->Size - get_data_q_size(q) - 1);

}


/**
@brief �ж϶����Ƿ���,�жϵ������Ƕ����е��������Ƕ��г���-1
@param q :���е�ַ
@return ����������TRUE
*/
BOOL is_data_q_full(FIFO_CTRL *q)
{
	//���Ƚ�������ǰ��û��ִ������������in++������ȷ��in���ڵ�ǰ����λ��
	u8 *end;
	
	end = q->pIn + 1;//����Ϊ���ֵָ��
	
	if(end >= q->pEnd)
	{
		end = q->pStart;//in+1���ʱ������ָ����ʼλ�ã����λ���
	}
	
	//in++����outʱ������û�ж���λ�ô����ݣ�����2����ⷽ��
	//1��in++�Ѿ��������������ָ����ʼλ�ã���outû�ı䣬����in�Ѿ�ָ���end
	//2��in++λ�ã���in��һλ���Ѿ���out��λ���ˣ�����in���ݸ���out����
	//��ʵ�����Դ�in��ǰλ�����ݣ���Ϊ��������in+1ǰ�Ѿ�ִ��һ�����溯����in++������൱��in��ǰ����λ��+2
	if(end == q->pOut)
	{
		return TRUE;
	}
	
	return 	FALSE;
}


/**
@brief �ж������Ƿ�Ϊ��
@param q ���е�ַ
@return �������Ϊ�շ���TRUE
*/
BOOL is_data_q_empty(FIFO_CTRL *q)
{
    if (q->pIn == q->pOut)
    {
        return TRUE;
    }
    return FALSE;
}


/**
@brief �������������
@param q: ���е�ַ
@param d: ����
@return ����гɹ�����TRUE,���������,�����ʧ�ܷ���FALSE
*/
BOOL data_q_in(FIFO_CTRL *q,u8 d)
{
	BOOL ret;
	
	ret = FALSE;
		
	//�������Ƿ�����������������������
	if(!is_data_q_full(q))
	{
		*q->pIn++ = d;//��ȡinָ������ݣ�Ȼ��in��ַ+1�����(*q)++����ȡqָ������ݣ�Ȼ��qָ�������+1

		if(q->pIn >= q->pEnd)
		{
			q->pIn = q->pStart;//���λ���
		}
		ret = TRUE;
	}

	return ret;
}


static u32 get_data_q_in_block_free_size(FIFO_CTRL *q)
{
	u32	n = 0;
	u8 *pIn,*pOut;

	//ע��FIFO�л�ʣ��һ���ֽڵĿռ�ʱ��ʾFIFO��
	if(is_data_q_full(q))
	{
		return n;
	}
	
	pIn = q->pIn;
	pOut = q->pOut;

	/*
		�����ȫ������q->pIn == q->pOut;�ⷴ����ʾFIFO�յ�����������Ҫn-1
	*/
	
	if(pIn > pOut)
	{
		n = q->pEnd - pIn;	//ֻ��ȡin��end��free��С
		if(pOut == q->pStart)
		{
			//��ֹ q->pIn == q->pOut
			n -= 1;
		}
	}
	else if(pIn < pOut)
	{
		n = pOut - pIn;
			//��ֹ q->pIn == q->pOut
			n -= 1;
	}
	else
	{
		n = q->pEnd - pIn;
		if(pOut == q->pStart)
		{
			//��ֹ q->pIn == q->pOut
			n -= 1;
		}
	}

	return n;
}


u32 data_q_in_more(FIFO_CTRL *q, u8 *buf, u32 len)
{
	u32 i = 0, n = 0, ret = 0;

	while(len)
	{
		n = get_data_q_in_block_free_size(q);//��ȡ��ǰring��freebuf��С������ring����Ϊ��һ��whileִ�У��ϲ㣩

		if(n)
		{
			if(n >= len)
			{
				n = len;
			}
			MEM_CPY(q->pIn, &buf[i], n);

			q->pIn += n;

			if(q->pIn >= q->pEnd)
			{
				q->pIn = q->pStart;
			}

			i += n;		//����buf�Ĵ�in��ʼ�±�
			len -= n;	//��ǰʣ��in�����ݴ�С
			ret += n;	//����in�ɹ������ݴ�С
		}
		else
		{
			break;
		}
	}

	return ret;
}

/**
@brief �������ݳ�����
@param q ���е�ַ
@param d �������ݻ���
@return ȡ�����ݷ���TRUE,���������û�����ݷ���FALSE
*/
BOOL data_q_out(FIFO_CTRL *q,u8 *d)
{
	if(q->pIn == q->pOut)//�ж϶����Ƿ�Ϊ��
	{
		return 	FALSE;
	}

	*d = *q->pOut++;

	if(q->pOut >= q->pEnd)
	{
		q->pOut = q->pStart;//���λ���
	}

	q->pPeek = q->pOut;//ͬ��peekָ��

	return TRUE;
	
}


static u32 get_data_q_out_block_free_size(FIFO_CTRL *q)
{
	u32	n = 0;
	u8 *pIn,*pOut;	

	if(is_data_q_empty(q))
	{
		return n;
	}

    pIn = q->pIn;
    pOut= q->pOut;

	if(pIn > pOut)
	{
		n = pIn - pOut;
	}
	else if(pIn < pOut)
	{
		n = q->pEnd - pOut;
	}
	else
	{
		n = 0;
	}

	return n;
}


u32 data_q_out_more(FIFO_CTRL *q,u8 *buf,u32 len)
{
	u32 i = 0, n = 0, ret = 0;	

	while(len)
	{
		n = get_data_q_out_block_free_size(q);

		if(n)
		{
			if(n >= len)
			{
				n = len;
			}

			if(buf)
			{
				MEM_CPY(&buf[i], q->pOut, n);
			}
			q->pOut += n;

			if(q->pOut >= q->pEnd)
			{
				q->pOut = q->pStart;
			}

			i += n;
			len -= n;
			ret += n;
			
		}
		else
		{
			break;
		}
	}
	
	return ret;

}


/**
@brief �������еĵ�������
       �Ӷ�����ȡ��һ�����ݣ����ǲ��������ݴӶ�����ɾ����
       ��data_q_peek_sync���������Ƿ�Ӷ�����ɾ��
       ���ݻ��ߵ���data_q_out������ȡ����
@param q ���е�ַ
@param d ���ݽ��ջ���
@return ���������������ȡ�����ݷ���TRUE
*/
BOOL data_q_peek(FIFO_CTRL *q,u8 *d)
{
	if(q->pIn == q->pPeek)//�������Ƿ�Ϊ�գ������peekָ���ǵ���out++���ָ��
	{
		return FALSE;
	}

	//ȡ�����ݣ������ݲ����Ӷ�����ɾ��
	*d = *q->pPeek++;

	if(q->pPeek >= q->pEnd)
	{
		q->pPeek = q->pStart;
	}

	return TRUE;
}



/**
@brief �������Ƿ���
@param q ���е�ַ
@return TRUE ���н���
*/
BOOL data_q_near_full(FIFO_CTRL *q)
{
	if(get_data_q_size(q) >= q->NearFullSize)
	{
		return TRUE;
	}

	return FALSE;
}


/**
@brief �������Ƿ񽫿�
@param q ���е�ַ
@return TRUE ���н���
*/
BOOL data_q_near_empty(FIFO_CTRL *q)
{
	if(get_data_q_size(q) <= q->NearEmptySize)
	{
		return TRUE;
	}

	return FALSE;
}


/**
@brief
@param q ���е�ַ
@return TRUE
*/
void data_q_init(FIFO_CTRL *q, u8 *buf, u32 size, u32 empty, u32 full)
{
	q->pStart = buf;
	q->Size = size;
	q->pEnd = buf + size;//���ֵָ��
	q->NearEmptySize = empty;
	q->NearFullSize = full;

	q->pIn = q->pOut = q->pStart;
	q->pPeek = q->pStart;
}


void data_q_reset(FIFO_CTRL *q)
{
	q->pIn = q->pOut = q->pStart;
	q->pPeek = q->pStart;
}


/**
@brief �������
@param q ���е�ַ
@return TRUE
*/
void data_q_cls(FIFO_CTRL *q)
{
	q->pOut = q->pIn;
	q->pPeek = q->pIn;
}

