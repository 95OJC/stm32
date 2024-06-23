#include "common.h"


/*-----------------------------------------------------------------
��֤usart���ܡ�

���裺
1�������жϿ�����nvic��
2����ʼ��gpio����gpio���õ�usart��,����usart������
3��ʹ��usart�����жϡ�
4����д���ڷ���һ���ַ����ַ���������
5����usart�����жϷ�����ʵ�����ݽ��պͷ��ͻ��ԡ�

ע��:ORE(���ش���)
1���������RX�жϽ������ݻ���RX�ж����ȼ����ͣ����ܻ�������жϡ�(���ȫ���жϱ�����̫��Ӧ��Ҳ�������)
2�����յ��������ȸ�����λ�Ĵ�����Ȼ��RDR�Ĵ�����������DR�Ĵ���������Ա��DR�Ĵ���ȡ���ݡ�
   ��DR�Ĵ����н�������ʱ��RXNE��1(����RX�жϽ�������ʱ�жϸ�λ��DR������)��
   ��û��ȡDR�Ĵ�������ʱ��RXNEһֱΪ1��Ȼ����յ����ݸ�����λ�Ĵ�����Ȼ��RDR�Ĵ����󣬾ͻ����ORE��
   ����OREʱ�������������ݵĻ���RDR�Ĵ�����ֵ���ᶪʧ��������λ�Ĵ��������ݻᱻ���ǡ����������EIEλ��
   ORE��־��λ�ᵼ�²����жϵġ�
------------------------------------------------------------------*/

static UARTINFO sUsartInfo;//�����ϲ㴫�����������ײ�
static FIFO_CTRL_INFO sUsartCtrl;//����ָ���ϲ㶨������飬Ȼ��ָ�������д����,���ļ�������ringbuf.c�ĺ�������Ҫ��������������룬ÿ�μ���Ͷ�������Ҫ���ؼ��


/* rtsģ��IO�ڣ����ڳ�ʼ��������������(��Ϊ���Ǵ��ڵ�Ӳ��ר��RTS���������ģ��IO��)������PC���ڹ�������Ӳ���� */
static void usart_rts_io_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;
    MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	USART_RTS_BUSx_CLOCK(USART_RTS_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = USART_RTS_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_RTS_GPIO,&gpio_initStruct);

	//rts_on״̬
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
	GPIO_InitTypeDef gpio_initStruct;
    MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	USART_TX_BUSx_CLOCK(USART_TX_CLOCK,ENABLE);
	USART_RX_BUSx_CLOCK(USART_RX_CLOCK,ENABLE);

	USART_BUSx_CLOCK(USART_CLOCK,ENABLE);

	gpio_initStruct.GPIO_Pin = USART_TX_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_TX_GPIO,&gpio_initStruct);

	gpio_initStruct.GPIO_Pin = USART_RX_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������(�޸������빦�ܿ���)
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_RX_GPIO,&gpio_initStruct);

	#if 1//Ӳ����init
	usart_rts_io_init();
	#endif
}

static void usart_config_init(UARTINFO *usartInfo)
{
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

	//usart���ȼ�����
	NVIC_ClearPendingIRQ(USART_NVIC_IRQN);
	NVIC_SetPriority(USART_NVIC_IRQN,USART_NVIC_PRIO);
	NVIC_EnableIRQ(USART_NVIC_IRQN);

	//usart��ʼ��
	usart_gpio_init();
	usart_config_init(usartInfo);

	//ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART_NUM, USART_IT_RXNE, ENABLE);

	//ʹ�ܴ���
	USART_Cmd(USART_NUM, ENABLE);

    //�˿ڵĻ������
    IF_SET_ACT(sUsartInfo.State);
    IF_SET_CONNECT(sUsartInfo.State);

	//�˿ڵ��͡�����״̬
    IF_SET_TX_EMPTY(sUsartCtrl.tx_tag);
	IF_CLS_RX_BUSY(sUsartCtrl.rx_tag);

	return TRUE;
}

//����һ���ַ������确1��֮�࣬����ַ�����----��Ӧ���at41��put_uart0����
void usart_sendByte(u8 ch)
{
	//��DR���ݼĴ���д��ֵ��TXEλ����TCλ(���ȶ�SR��TCλ)
	USART_SendData(USART_NUM,ch);
	//�ȴ�TXE��1�����ɷ�����һ���ֽ�
	//������ȴ�TC��1����ʱ���ϻ��һЩ
	while (USART_GetFlagStatus(USART_NUM, USART_FLAG_TXE) == RESET);
}

//����һ���ַ���������"123"�����ַ�����
void usart_sendString(u8 *str)
{
	while(*str)
	{
		usart_sendByte(*str++);
	}

	//�ȴ�TC��1����ζ����ɴ˴δ���
	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART_NUM,USART_FLAG_TC)==RESET);

}

//����һ��ʮ�����Ƶ�ֵ��0x00~0xFF----��Ӧ���at41��get_uart0����
u8 usart_getByte(void)
{
	while(USART_GetFlagStatus(USART_NUM,USART_FLAG_RXNE) == RESET);
	//��DR���ݼĴ�����RXNEλ
	return (u8)USART_ReceiveData(USART_NUM);
}

/*�ض���c�⺯��printf�����ڣ��ض������ϲ�͵ײ��ʹ��printf����*/
int fputc(int ch, FILE *F)
{
	//����һ���ֽڵ�����
	USART_SendData(USART_NUM, (u8)ch);

	//�ȴ��������
	while(USART_GetFlagStatus(USART_NUM, USART_FLAG_TXE) == RESET); 

	return ch;
}

void USART_IRQHandler_FUNC(void)
{
	u8 ucTemp;
	
	if(USART_GetITStatus(USART_NUM,USART_IT_RXNE)!=RESET)
	{
		//USART_ReceiveData������ȡDR�Ĵ������ݺ�,Ӳ���Զ���RXNE��־λ(�������ﲻ��Ҫ���жϱ�־λ),Ȼ��ſɼ���������һ���ֽ�
		ucTemp = USART_ReceiveData(USART_NUM);

#if 1
		data_q_in(&sUsartCtrl.rx,ucTemp);
#else
		data_q_in_more(&sUsartCtrl.rx,&ucTemp,1);
#endif
		if(data_q_near_full(&sUsartCtrl.rx))
		{
			#if 0	//�������أ��ᶪ������ʱ��֪��ԭ��,Ӳ����ϵ���ǼĴ������ã�
			usart_sendByte(0x13);
			#else	//��Ӳ����
			usart_rts_off();
			#endif
		}
	}

}


#if 1	//����if�ļ��Ĺ̶���ʽ
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

	//ʧ�ܴ��ڽ����ж�
	USART_ITConfig(USART_NUM, USART_IT_RXNE, DISABLE);

	//ʧ�ܴ���
	USART_Cmd(USART_NUM, DISABLE);

	return TRUE;
}

/*���سɹ��ӻ�����������ݴ�Сlen���п���С��len*/
static u32 read_usart(u8 *buf,u32 len)
{
	u32 i;
	
	i = data_q_out_more(&sUsartCtrl.rx,buf,len);
		
	if(data_q_near_empty(&sUsartCtrl.rx))
	{
		#if 0	//�������أ��ᶪ������ʱ��֪��ԭ��,Ӳ����ϵ���ǼĴ������ã�
		usart_sendByte(0x11);
		#else	//��Ӳ����
		usart_rts_on();
		#endif
	}

	return	i;
}

/*���سɹ����뻺������ݴ�Сlen���п���С��len */
//���at41���ﲻ��ѭ������i�����ݵģ�����ֻ����1�����ݣ�����ϲ���Ҫһֱ���ͣ�ֱ��len--Ϊ0
static u32 write_usart(u8 *buf,u32 len)
{
	u32 i,j;
	u8 d;
	
	i = data_q_in_more(&sUsartCtrl.tx,buf,len);//ÿ�η��͵�len���ܳ���TxSize,�����ϲ���ѭ�����ͳ���TxSize��len��
	j = i;

	if(IF_GET_TX_EMPTY(sUsartCtrl.tx_tag))
	{
		IF_CLS_TX_EMPTY(sUsartCtrl.tx_tag);

		while(j)
		{
			if(data_q_out(&sUsartCtrl.tx,&d))
			{
				usart_sendByte(d);
			}
			
			j--;
		}
		
		IF_SET_TX_EMPTY(sUsartCtrl.tx_tag);
	}
	return i;
}

/*���سɹ��鿴�����ݵĴ�Сlen�����ܻ�С��len*/
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

/*���ƻ���usart1�Ļ���ͻ���״̬*/
static BOOL io_ctrl_usart(IOCTRL_TYPE type,void *p)
{
	BOOL ret = FALSE;
	u32 n,d;
	switch (type)
		{
			case IOCTRL_RX_DATA_SIZE:
				n = get_data_q_size(&sUsartCtrl.rx);
				REG32(p) = n;//���ϲ㷵�ض������������
				ret = TRUE;
				break;

			case IOCTRL_TX_DATA_SIZE:
				n = get_data_q_size(&sUsartCtrl.tx);
				REG32(p) = n;//���ϲ㷵�ض������������
				ret = TRUE;
				break;

			case IOCTRL_RESET_RX_BUF:
				data_q_reset(&sUsartCtrl.rx);
				IF_CLS_RX_BUSY(sUsartCtrl.rx_tag);
				ret = TRUE;
				break;

			case IOCTRL_RESET_TX_BUF:
				data_q_reset(&sUsartCtrl.tx);
				IF_SET_TX_EMPTY(sUsartCtrl.tx_tag);
				ret = TRUE;
				break;

			case IOCTRL_RESET_RT_BUF:
				data_q_reset(&sUsartCtrl.rx);
				IF_CLS_RX_BUSY(sUsartCtrl.rx_tag);
				data_q_reset(&sUsartCtrl.tx);
				IF_SET_TX_EMPTY(sUsartCtrl.tx_tag);
				ret = TRUE;
				break;

			case IOCTRL_GET_PORT_STAT:
				d = (sUsartInfo.State|sUsartCtrl.rx_tag|sUsartCtrl.tx_tag);
				REG32(p) = d;
				ret = TRUE;

				break;
			default:
				break;
		}
	
	return ret;
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

BOOL debug_usart_init(BUFINFO *pBuf,void *pCtrl)
{
	return open_usart(pBuf,pCtrl);
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
		vTaskDelay(5);
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







