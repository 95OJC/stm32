#include "common.h"
#include "ringbuf.h"

/*-----------------------------------------------------------------
实现FIFO的环形循环使用。

//单字节
1、每次data_q_in函数加入数据前，由is_data_q_full函数限制，如果in和out之间剩余一个字节的空间时表示FIFO满，即data_q_in函数会返回FALSE。
    a：为防止返回FALSE，需要在data_q_in函数下面用data_q_near_full函数判断FIFO是否准备满。
    b：如果用串口收多个数据，判断满的条件需要远远大于2个字节，否则软或硬流控来不及，导致   	  来不及流控的数据因为data_q_in函数会返回FALSE而没有加入FIFO，导致丢数。
   
2、每次data_q_out函数取FIFO数据前，由(q->pIn == q->pOut)判断队列是否为空，空就返回FALSE。
    a：每次取FIFO一个字节数据后，都在data_q_out函数下面用data_q_near_empty函数检测解除软或硬流控。
   如果用串口发送解除软或硬流控，判断空的条件可等于0。
	   
   //如果发送in数据的速度比处理out数据的速度快，是不会存在队列为空的情况，只会基本处于队列满的情况。
   //如果发送in数据的速度比处理out数据的速度慢，是不会存在队列为满的情况，只会基本处于队列空的情况。
   //当发送in数据的速度比处理out数据的速度快，如果用串口发送解除软或硬流控，判断空的条件可等于0。  
   //当发送in数据的速度比处理out数据的速度慢，如果用串口发送解除软或硬流控，判断空的条件最好远远大于0，这样可以防止队列为空的情况，又等待收一定的数据才形成可执行命令(这段时间相当于等待)。


//多字节
1、每次data_q_in_more函数加入数据前，由get_data_q_in_block_free_size函数限制，里面先由is_data_q_full函数限制，若满则返回n=0，不满则计算并返回可加入的最大数据n，然后只加入len的数据，最终函数返回成功加入的len。
    a：为防止返回n=0或者n<len，需要在data_q_in_more函数下面用data_q_near_full函数判断FIFO是否准备满。
    b：如果用USB收多包数据，判断满的条件至少需要大于2个包或以上
   考虑到USB主机需要USB从机ack才能发下一包，不会像串口来不及流控吧？这样理解的话，判断满条件可为0，不行，因为判断full需要留一个字节数据，还是留多几包判断full稳妥！
   得实的USB每次out包中断，最开始时判断是否满，而判断完再收out端点的包(收完自动返回ACK？触发下一个包)，如果判断满了，收完该out包后就不会再继续收包了。

2、每次data_q_out_more函数去除FIFO数据前，由get_data_q_out_block_free_size函数限制，里面先由is_data_q_empty函数限制，若(q->pIn == q->pOut)为空返回n=0，不空则计算并返回可取出的最大数据n，然后只取出len的数据，
   最终函数返回成功取出的len。
	a：每次取出FIFO的一包数据后，都在data_q_out_more函数下面用data_q_near_empty函数检测解除流控。
	b：如果每次取的值为一包len长度，那么data_q_out_more函数可能存在返回小于len的情况，防止该情况，data_q_near_empty函数检测解除流控的判断空的条件至少为1个包len。
   但是，得实这里read函数每次取的值为一包len=1，因此可以按照上面data_q_out函数的判断空的条件可等于0。



//总结：
1、ringbuf.c文件的函数作用:对传入参数为FIFO_CTRL类型结构体对象进行in、out、peek操作，还有near_full和near_empty判断，前提需要执行data_q_init，把指定FIFO_CTRL类型结构体变量作为对象。
2、由于FIFO_CTRL_INFO结构体在ringbuf.c和uart.c等底层通讯接口文件都需要用到，因此需要在ringbufApi.h文件定义。
3、不管data_q_in函数还是data_q_in_more函数加入数据，判断满条件不能为0(远大于2字节或远大于单包)，否则会丢数。
4、不管data_q_out函数还是data_q_out_more函数取出数据，判断空条件都可为0。
5、data_q_in函数和data_q_out函数加和取数据失败时返回FALSE。
6、data_q_in_more函数和data_q_out_more函数加和取数据失败时返回n=0或者加和取数据不够时返回n<len，取数据n<len时不影响使用把？



//DMA-ringbuf-list
若串口来说，二代芯的DMA环形链表的好处（以余工的修改得实uart1.c文件例子）：
1、中断函数处理对比：
   ①每次DMA完成中断产生，in+至DMA链表节点中的下一块首地址(下一个DMA数据存该首地址)，再判断near_full流控,注意流控不会马上生效，因此会覆盖out数据那部分，因此需要留2个block就要控制流控了，
   而near_empty流控留2个block最好，还没理解1个block不会出问题的原因。
   ②每次串口中断接收数据的data_q_in函数里面先判断FIFO的full，再in++和重指环形star，再判断near_full流控。
   两者差别：DMA链表只需要改变in指针为下一块首地址（自带环形），而data_q_in函数还需要重指环形star，DMA则自带环形star，另外如果near_full足够大，判断FIFO的full可以没有，因为有流控不会full。

2、read函数处理对比：
   ①每次DMA完成中断里改变了in指针后相当于有数据了，而DMA一直在独立于CPU工作，那么read函数需要in指针指向当前准备存数据dma地址，记得关中断操作(防止汇编问题),然后data_q_out函数和判断near_empty流控。
   (因为编译为汇编里赋值会有问题，另外因为DMA时刻接收数据导致in指针时刻变化，而串口是中断接收的，read函数里面过程中in指针也时刻变化，但是in指针没有被赋值，因此不用担心汇编问题）
   ②每次串口read函数，不用DMA时，直接data_q_out函数和判断near_empty流控即可。
   两者差别：由于利用DMA环形链表时，read函数需要在关中断后in指针指向当前准备存数据dma地址再开中断，而不用DMA环形链表的串口read函数直接data_q_out函数和判断near_empty流控即可。
   
   
------------------------------------------------------------------*/


/**
@brief 获取队列中数据量
@param q :队列地址
@return 队列中的数据量
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
@brief 获取队列剩余空闲容量
@param q : 队列地址
@return 队列空闲容量
*/
u32 get_data_q_free_size(FIFO_CTRL *q)
{
	//因为size已经是溢出值，需要-1
	return (q->Size - get_data_q_size(q) - 1);

}


/**
@brief 判断队列是否满,判断的依据是队列中的数据量是队列长度-1
@param q :队列地址
@return 队列满返回TRUE
*/
BOOL is_data_q_full(FIFO_CTRL *q)
{
	//首先进来函数前，没有执行其他函数的in++操作，确保in处于当前数据位置
	u8 *end;
	
	end = q->pIn + 1;//可能为溢出值指针
	
	if(end >= q->pEnd)
	{
		end = q->pStart;//in+1溢出时，重新指向起始位置，环形缓存
	}
	
	//in++等于out时，缓存没有多余位置存数据，以下2个理解方向：
	//1、in++已经处于溢出，重新指向起始位置，而out没改变，但是in已经指向过end
	//2、in++位置，即in下一位置已经是out的位置了，不能in数据覆盖out数据
	//其实还可以存in当前位置数据？因为这里上面in+1前已经执行一次外面函数的in++，因此相当于in当前数据位置+2
	if(end == q->pOut)
	{
		return TRUE;
	}
	
	return 	FALSE;
}


/**
@brief 判定队列是否为空
@param q 队列地址
@return 如果队列为空返回TRUE
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
@brief 单个数据入队列
@param q: 队列地址
@param d: 数据
@return 入队列成功返回TRUE,如果队列满,入队列失败返回FALSE
*/
BOOL data_q_in(FIFO_CTRL *q,u8 d)
{
	BOOL ret;
	
	ret = FALSE;
		
	//检测队列是否满，如果不满则数据入队列
	if(!is_data_q_full(q))
	{
		*q->pIn++ = d;//先取in指向的数据，然后in地址+1；如果(*q)++则先取q指向的数据，然后q指向的数据+1

		if(q->pIn >= q->pEnd)
		{
			q->pIn = q->pStart;//环形缓存
		}
		ret = TRUE;
	}

	return ret;
}


static u32 get_data_q_in_block_free_size(FIFO_CTRL *q)
{
	u32	n = 0;
	u8 *pIn,*pOut;

	//注意FIFO中还剩余一个字节的空间时表示FIFO满
	if(is_data_q_full(q))
	{
		return n;
	}
	
	pIn = q->pIn;
	pOut = q->pOut;

	/*
		如果完全填满则q->pIn == q->pOut;这反而表示FIFO空的情况，因此需要n-1
	*/
	
	if(pIn > pOut)
	{
		n = q->pEnd - pIn;	//只能取in到end的free大小
		if(pOut == q->pStart)
		{
			//防止 q->pIn == q->pOut
			n -= 1;
		}
	}
	else if(pIn < pOut)
	{
		n = pOut - pIn;
			//防止 q->pIn == q->pOut
			n -= 1;
	}
	else
	{
		n = q->pEnd - pIn;
		if(pOut == q->pStart)
		{
			//防止 q->pIn == q->pOut
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
		n = get_data_q_in_block_free_size(q);//获取当前ring的freebuf大小，超出ring部分为下一个while执行（上层）

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

			i += n;		//传入buf的待in起始下标
			len -= n;	//当前剩余in的数据大小
			ret += n;	//返回in成功的数据大小
		}
		else
		{
			break;
		}
	}

	return ret;
}

/**
@brief 单个数据出队列
@param q 队列地址
@param d 接收数据缓冲
@return 取出数据返回TRUE,如果队列中没有数据返回FALSE
*/
BOOL data_q_out(FIFO_CTRL *q,u8 *d)
{
	if(q->pIn == q->pOut)//判断队列是否为空
	{
		return 	FALSE;
	}

	*d = *q->pOut++;

	if(q->pOut >= q->pEnd)
	{
		q->pOut = q->pStart;//环形缓存
	}

	q->pPeek = q->pOut;//同步peek指针

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
@brief 检查队列中的单个数据
       从队列中取出一个数据，但是并不将数据从队列中删除，
       由data_q_peek_sync函数决定是否从队列中删除
       数据或者调用data_q_out将数据取出。
@param q 队列地址
@param d 数据接收缓冲
@return 如果队列中有数据取出数据返回TRUE
*/
BOOL data_q_peek(FIFO_CTRL *q,u8 *d)
{
	if(q->pIn == q->pPeek)//检查队列是否为空，这里的peek指针是等于out++后的指针
	{
		return FALSE;
	}

	//取出数据，但数据并不从队列中删除
	*d = *q->pPeek++;

	if(q->pPeek >= q->pEnd)
	{
		q->pPeek = q->pStart;
	}

	return TRUE;
}



/**
@brief 检查队列是否将满
@param q 队列地址
@return TRUE 队列将满
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
@brief 检查队列是否将空
@param q 队列地址
@return TRUE 队列将空
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
@param q 队列地址
@return TRUE
*/
void data_q_init(FIFO_CTRL *q, u8 *buf, u32 size, u32 empty, u32 full)
{
	q->pStart = buf;
	q->Size = size;
	q->pEnd = buf + size;//溢出值指针
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
@brief 清除队列
@param q 队列地址
@return TRUE
*/
void data_q_cls(FIFO_CTRL *q)
{
	q->pOut = q->pIn;
	q->pPeek = q->pIn;
}

