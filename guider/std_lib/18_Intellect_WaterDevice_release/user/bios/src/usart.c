#include "common.h"
#include "usart.h"

/*-----------------------------------------------------------------
验证usart功能。

步骤：
1、配置中断控制器nvic。
2、初始化gpio，并gpio复用到usart上,配置usart参数。
3、使能usart接收中断。
4、编写串口发送一个字符和字符串函数。
5、在usart接收中断服务函数实现数据接收和发送回显。

注意:ORE(过载错误)
1、如果不用RX中断接收数据或者RX中断优先级过低，可能会产生该中断。(如果全局中断被屏蔽太久应该也会出问题)
2、接收的数据首先给到移位寄存器，然后RDR寄存器，最后给到DR寄存器，程序员读DR寄存器取数据。
   当DR寄存器有接收数据时，RXNE置1(不用RX中断接收数据时判断该位读DR的数据)。
   当没有取DR寄存器数据时，RXNE一直为1，然后接收的数据给到移位寄存器，然后到RDR寄存器后，就会产生ORE。
   产生ORE时，继续接收数据的话，RDR寄存器的值不会丢失，但是移位寄存器的数据会被覆盖。如果设置了EIE位，
   ORE标志置位会导致产生中断的。
------------------------------------------------------------------*/

static UARTINFO sUsartInfo;//用于上层传串口配置来底层
static FIFO_CTRL_INFO sUsartCtrl;//用于指向上层定义的数组，然后指针操作读写数据,该文件的所有ringbuf.c的函数都需要用这个做参数传入，每次加入和读出后都需要流控检测

static uint8_t *compiler_data = __DATE__;
static uint8_t *compiler_time = __TIME__;

void usart_sendString(u8 *str);

/*rts模拟IO口，串口初始化配置填无流控(因为不是串口的硬件专门RTS，而是软件模拟IO的)，但是PC串口工具需填硬流控*/
static void usart_rts_io_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;
    MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	RCC_APB2PeriphClockCmd(USART_RTS_CLOCK_GPIOx,ENABLE);
	
    gpio_initStruct.GPIO_Pin = USART_RTS_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_RTS_GPIO,&gpio_initStruct);

	//rts_on状态
	GPIO_ResetBits(USART_RTS_GPIO,USART_RTS_PIN);

}

static void usart_rts_on(void)
{
	GPIO_ResetBits(USART_RTS_GPIO,USART_RTS_PIN);
}

static void usart_rts_off(void)
{
	GPIO_SetBits(USART_RTS_GPIO,USART_RTS_PIN);
}

static void usart_gpio_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef gpio_initStruct;
    MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	RCC_APB2PeriphClockCmd(USART_TX_CLOCK_GPIOx,ENABLE);
	RCC_APB2PeriphClockCmd(USART_RX_CLOCK_GPIOx,ENABLE);

	USART_APBx_CLOCK(USART_CLOCK,ENABLE);

	gpio_initStruct.GPIO_Pin = USART_TX_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_TX_GPIO,&gpio_initStruct);

	gpio_initStruct.GPIO_Pin = USART_RX_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入(无复用输入功能可用)
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_RX_GPIO,&gpio_initStruct);

	#if 1//硬流控init
	usart_rts_io_init();
	#endif
}

static void usart_config_init(UARTINFO *usartInfo)
{
	//usart初始化
	USART_InitTypeDef usart_initStruct;
	MEM_SET(&usart_initStruct,0,sizeof(usart_initStruct));
	
	usart_initStruct.USART_BaudRate = usartInfo->Ctrl.Baud;
	usart_initStruct.USART_WordLength = usartInfo->Ctrl.Datas;
	usart_initStruct.USART_StopBits = usartInfo->Ctrl.Stops;
	usart_initStruct.USART_Parity = usartInfo->Ctrl.Prty;
	usart_initStruct.USART_HardwareFlowControl = usartInfo->Ctrl.Flw;
	usart_initStruct.USART_Mode = usartInfo->Ctrl.Mode;
	USART_Init(USART_NUM, &usart_initStruct);
}

static BOOL usart_init(BUFINFO *pInfo,UARTINFO *usartInfo)
{
	if(pInfo)
	{
		data_q_init(&sUsartCtrl.rx,pInfo->pRxBuf,pInfo->RxSize,pInfo->RxNearEmptySize,pInfo->RxNearFullSize);
		data_q_init(&sUsartCtrl.tx,pInfo->pTxBuf,pInfo->TxSize,pInfo->TxNearEmptySize,pInfo->TxNearFullSize);
		MEM_CPY(&sUsartInfo, usartInfo, sizeof(UARTINFO));
	}
	else
	{
		return FALSE;
	}

	//usart优先级配置
	NVIC_ClearPendingIRQ(USART_NVIC_IRQN);
	NVIC_SetPriority(USART_NVIC_IRQN,USART_NVIC_PRIO);
	NVIC_EnableIRQ(USART_NVIC_IRQN);

	//usart初始化
	usart_gpio_init();
	usart_config_init(usartInfo);

	//使能串口接收中断
	USART_ITConfig(USART_NUM, USART_IT_RXNE, ENABLE);

	//使能串口
	USART_Cmd(USART_NUM, ENABLE);

	//usart_sendString("usart init success!\r\n");
	printf("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	printf("file,fun,line = %s,%s,%d\r\n",__FILE__,__FUNCTION__,__LINE__);

	return TRUE;
}

//发送一个字符，例如‘1’之类，填非字符乱码
void usart_sendByte(u8 ch)
{
	//向DR数据寄存器写入值清TXE位和清TC位(需先读SR的TC位)
	USART_SendData(USART_NUM,ch);
	//等待TXE置1，方可发送下一个字节
	//若这里等待TC置1，则时间上会久一些
	while (USART_GetFlagStatus(USART_NUM, USART_FLAG_TXE) == RESET);
}

//发送一个字符串，例如"123"，填字符报错
void usart_sendString(u8 *str)
{
	while(*str)
	{
		usart_sendByte(*str++);
	}

	//等待TC置1，意味着完成此次传输
	/* 等待发送完成 */
	while (USART_GetFlagStatus(USART_NUM,USART_FLAG_TC)==RESET);

}

u8 usart_getByte(void)
{
	while(USART_GetFlagStatus(USART_NUM,USART_FLAG_RXNE) == RESET);
	//读DR数据寄存器清RXNE位
	return (u8)USART_ReceiveData(USART_NUM);
}

void USART1_IRQHandler(void)
{
	u8 ucTemp;
	
	if(USART_GetITStatus(USART_NUM,USART_IT_RXNE)!=RESET)
	{
		//USART_ReceiveData函数读取DR寄存器数据后,硬件自动清RXNE标志位(所以这里不需要清中断标志位),然后才可继续接收下一个字节
		ucTemp = USART_ReceiveData(USART_NUM);

#if 1
		data_q_in(&sUsartCtrl.rx,ucTemp);
#else
		data_q_in_more(&sUsartCtrl.rx,&ucTemp,1);
#endif
		if(data_q_near_full(&sUsartCtrl.rx))
		{
			#if 0	//置软流控，会丢数，暂时不知道原因,硬件关系还是寄存器配置？
			usart_sendByte(0x13);
			#else	//置硬流控
			usart_rts_off();
			#endif
		}
	}

}

//void Delay_ojc(__IO uint32_t nCount);
void USART2_IRQHandler(void)
{
	u8 ucTemp;
	//led_toggle();
	if(USART_GetITStatus(USART_NUM,USART_IT_RXNE)!=RESET)
	{
		//USART_ReceiveData函数读取DR寄存器数据后,硬件自动清RXNE标志位(所以这里不需要清中断标志位),然后才可继续接收下一个字节
		ucTemp = USART_ReceiveData(USART_NUM);

#if 1
		data_q_in(&sUsartCtrl.rx,ucTemp);
#else
		data_q_in_more(&sUsartCtrl.rx,&ucTemp,1);
#endif
		if(data_q_near_full(&sUsartCtrl.rx))
		{
			#if 0	//置软流控，会丢数，暂时不知道原因,硬件关系还是寄存器配置？
			usart_sendByte(0x13);
			#else	//置硬流控
			usart_rts_off();
			#endif
		}
	}
	//led_toggle();
}

void USART3_IRQHandler(void)
{
	u8 ucTemp;
	
	if(USART_GetITStatus(USART_NUM,USART_IT_RXNE)!=RESET)
	{
		//USART_ReceiveData函数读取DR寄存器数据后,硬件自动清RXNE标志位(所以这里不需要清中断标志位),然后才可继续接收下一个字节
		ucTemp = USART_ReceiveData(USART_NUM);

#if 1
		data_q_in(&sUsartCtrl.rx,ucTemp);
#else
		data_q_in_more(&sUsartCtrl.rx,&ucTemp,1);
#endif
		if(data_q_near_full(&sUsartCtrl.rx))
		{
			#if 0	//置软流控，会丢数，暂时不知道原因,硬件关系还是寄存器配置？
			usart_sendByte(0x13);
			#else	//置硬流控
			usart_rts_off();
			#endif
		}
	}

}

#if 1	//调用if文件的固定格式
static BOOL open_usart(BUFINFO *pInfo, void *uInfo)
{
	return usart_init(pInfo,(UARTINFO *)uInfo);
}

static BOOL close_usart(void)
{
	data_q_reset(&sUsartCtrl.rx);
    data_q_reset(&sUsartCtrl.tx);

    sUsartInfo.State = 0;
    sUsartCtrl.tx_tag = 0;
    sUsartCtrl.rx_tag = 0;

	//失能串口接收中断
	USART_ITConfig(USART_NUM, USART_IT_RXNE, DISABLE);

	//失能串口
	USART_Cmd(USART_NUM, DISABLE);

	return TRUE;
}

/*返回成功从缓冲读出的数据大小len，有可能小于len*/
static u32 read_usart(u8 *buf,u32 len)
{
	u32 i;
	
	i = data_q_out_more(&sUsartCtrl.rx,buf,len);
		
	if(data_q_near_empty(&sUsartCtrl.rx))
	{
		#if 0	//清软流控，会丢数，暂时不知道原因,硬件关系还是寄存器配置？
		usart_sendByte(0x11);
		#else	//清硬流控
		usart_rts_on();
		#endif
	}

	return	i;
}

/*返回成功加入缓存的数据大小len，有可能小于len*/
static u32 write_usart(u8 *buf,u32 len)
{
	u32 i,j;
	u8 d;
	
	i = data_q_in_more(&sUsartCtrl.tx,buf,len);//每次发送的len不能超过TxSize,除非上层有循环发送超过TxSize的len。
	j = i;
	
	while(j)
	{
		if(data_q_out(&sUsartCtrl.tx,&d))
		{
			usart_sendByte(d);
		}
		
		j--;
	}

	return i;
}

/*返回成功查看的数据的大小len，可能会小于len*/
static u32 peek_usart(u8 *buf, u32 len)
{
	u32 i = 0;
	
	while(i < len)
	{
		if(data_q_peek(&sUsartCtrl.rx,buf))
		{
			buf++;
			i++;
		}
		else
		{
			break;
		}
	}

	return i;
}

/*控制或监控usart1的缓冲和缓存状态*/
static BOOL io_ctrl_usart(IOCTRL_TYPE type,void *p)
{
	REG32(p) = 0;
	
	return TRUE;
}

void usart_driver_register(void)
{
	if_register_driver(
		UART_PORT, 
		open_usart, 
		close_usart, 
		read_usart, 
		write_usart, 
		peek_usart, 
		io_ctrl_usart
		);
}

#endif

/*重定向c库函数printf到串口，重定向后可上层和底层可使用printf函数*/
int fputc(int ch, FILE *F)
{
	//发送一个字节到串口
	USART_SendData(USART_NUM, (u8)ch);

	//等待发送完毕
	while(USART_GetFlagStatus(USART_NUM, USART_FLAG_TXE) == RESET); 

	return ch;
}

void shell_usart_sendByte(u8 ch)
{
	write_usart(&ch,1);
}

void shell_usart_sendString(u8 *str)
{
	while(*str)
	{
		shell_usart_sendByte(*str++);
	}
}

u8 shell_usart_getByte(void)
{
	u8 d;
	
	while(!read_usart(&d,1))
	{
		//定时器delay
		//delay_timer_ms(5);
	}

	return d;
}

BOOL shell_usart_asyn_getByte(u8 *byte)
{
	u8 ret = FALSE;
	u8 d;

	if(read_usart(&d,1))
	{
		*byte = d; 
		ret = TRUE;
	}

	return ret;
}


