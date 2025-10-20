#include "common.h"
#include "uart.h"

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

UARTINFO gUart0_info;
FIFO_CTRL_INFO gUart0_ctrl;
UART_HandleTypeDef UART0_Handle;

#define	UART0_FIFO_DEEP	16
u8 uart0_rx_buf[UART0_FIFO_DEEP];

static void uart0_flow_ctrl(u8 mode, BOOL f)
{
	switch (mode)
		{
			case UART_XON_XOFF:
				if(f)
				{
					IF_CLS_RX_BUSY(gUart0_ctrl.rx_tag);
					UART0_Handle.Instance->TDR = 0x11;
				}
				else
				{
					IF_SET_RX_BUSY(gUart0_ctrl.rx_tag);
					UART0_Handle.Instance->TDR = 0x13;
				}

			case UART_DTR_DSR:
			case UART_RTS_CTS:
				if(f)
				{
					IF_CLS_RX_BUSY(gUart0_ctrl.rx_tag);
					//uart0_rts_on();
					//uart0_dtr_on();
				}
				else
				{
					IF_SET_RX_BUSY(gUart0_ctrl.rx_tag);
					//uart0_rts_off();
					//uart0_dtr_off();
				}

			case UART_FLOW_NONE:
				if(f)
				{
					IF_CLS_RX_BUSY(gUart0_ctrl.rx_tag);
				}
				else
				{
					IF_SET_RX_BUSY(gUart0_ctrl.rx_tag);//接收中断里执行后，read函数才能执行清流控函数
				}
				
			default:		
				break;				
		}
}

static void uart0_enable_port(void)
{
    uart0_flow_ctrl(gUart0_info.Ctrl.Flw, TRUE);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	u16 i = 0;

	if(huart->Instance == UART0_Handle.Instance)
	{
		if(huart->ErrorCode & HAL_UART_ERROR_RTO)
		{
			//DBG_TRACE("toc\r\n");
			for(i=0;i < (huart->RxXferSize - huart->RxXferCount); i++)
			{
				data_q_in(&gUart0_ctrl.rx,uart0_rx_buf[i]);
			}
			if(is_data_q_near_full(&gUart0_ctrl.rx))
			{
				uart0_flow_ctrl(gUart0_info.Ctrl.Flw, FALSE);
			}
			HAL_UART_Receive_IT(huart,uart0_rx_buf,sizeof(uart0_rx_buf));//开启接收中断,赋值huart->RxISR = UART_RxISR_8BIT_FIFOEN;			
		}	
		
		if(huart->ErrorCode & HAL_UART_ERROR_PE)
		{
			DBG_TRACE("HAL_UART_ERROR_PE\r\n");
		}
		
		if(huart->ErrorCode & HAL_UART_ERROR_NE)
		{
			DBG_TRACE("HAL_UART_ERROR_NE\r\n");
		}

		if(huart->ErrorCode & HAL_UART_ERROR_FE)
		{
			DBG_TRACE("HAL_UART_ERROR_FE\r\n");
		}

		if(huart->ErrorCode & HAL_UART_ERROR_ORE)
		{
			DBG_TRACE("HAL_UART_ERROR_ORE\r\n");
		}
	}
	else
	{
		DBG_TRACE("HAL_UART_ErrorCallback: what to do?\r\n");
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	u16 i = 0;
	
	//DBG_TRACE("rc\r\n");
	if(huart->Instance == UART0_Handle.Instance)
	{
		for(i=0;i < huart->RxXferSize; i++)
		{
			data_q_in(&gUart0_ctrl.rx,uart0_rx_buf[i]);
		}
		if(is_data_q_near_full(&gUart0_ctrl.rx))
		{
			uart0_flow_ctrl(gUart0_info.Ctrl.Flw, FALSE);
		}
		HAL_UART_Receive_IT(huart,uart0_rx_buf,sizeof(uart0_rx_buf));//开启接收中断,赋值huart->RxISR = UART_RxISR_8BIT_FIFOEN;
	}
	
}

void UART_Idle_Handler(UART_HandleTypeDef *huart)
{
	u16 i = 0;
	
	//DBG_TRACE("ic\r\n");
	for(i=0;i < (huart->RxXferSize - huart->RxXferCount); i++)
	{
		data_q_in(&gUart0_ctrl.rx,uart0_rx_buf[i]);
	}
	if(is_data_q_near_full(&gUart0_ctrl.rx))
	{
		uart0_flow_ctrl(gUart0_info.Ctrl.Flw, FALSE);
	}

	HAL_UART_Receive_IT(huart,uart0_rx_buf,sizeof(uart0_rx_buf));//开启接收中断,赋值huart->RxISR = UART_RxISR_8BIT_FIFOEN;
}

void UART_0_IRQHandler(void)
{	
	if(__HAL_UART_GET_FLAG(&UART0_Handle, UART_FLAG_IDLE) != RESET)
	{
		// 清除空闲中断标志
		__HAL_UART_CLEAR_IDLEFLAG(&UART0_Handle);
	}
	HAL_UART_IRQHandler(&UART0_Handle);
}

static void uart0_driver_init(UART_HandleTypeDef *huart, UARTINFO *uInfo)
{
	GPIO_InitTypeDef gpio_uart = {0};

    UART_0_CLK_EN();
    UART_0_GPIO_EN();

    gpio_uart.Pin = UART_0_TX_PIN|UART_0_RX_PIN;
    gpio_uart.Mode = GPIO_MODE_AF_PP;
    gpio_uart.Pull = GPIO_NOPULL;
    gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_uart.Alternate = UART_0_AF;
    HAL_GPIO_Init(UART_0_RX_TX_GPIO, &gpio_uart);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(UART0_INT_IRQn, UART0_INT_PRI, 0);
    HAL_NVIC_EnableIRQ(UART0_INT_IRQn);	
	
    huart->Instance        = UART_0_NUM;  
    huart->Init.BaudRate   = uInfo->Ctrl.Baud; 
    huart->Init.WordLength = uInfo->Ctrl.Datas;
    huart->Init.StopBits   = uInfo->Ctrl.Stops;
    huart->Init.Parity     = uInfo->Ctrl.Prty;
    huart->Init.Mode       = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	HAL_UART_Init(huart);

	//硬件fifo关键配置
	//HAL_UARTEx_SetTxFifoThreshold(&huart, UART_TXFIFO_THRESHOLD_1_2);
	HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_8_8);
	HAL_UARTEx_EnableFifoMode(huart);

	// 设置超时值
	// 超时时间(秒) = (RTO * 32) / 波特率
	// 例如：在115200波特率下，RTO=0x40 ≈ 17.8ms
	//HAL_UART_ReceiverTimeout_Config(huart, 0x40);
	//HAL_UART_EnableReceiverTimeout(huart);

	//使能空闲中断
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

	HAL_UART_Receive_IT(huart,uart0_rx_buf,sizeof(uart0_rx_buf));//开启接收中断,赋值huart->RxISR = UART_RxISR_8BIT_FIFOEN;
}

static BOOL uart0_init(BUFINFO *pInfo, UARTINFO *uInfo)
{
	MEM_SET(&gUart0_info, 0, sizeof(UARTINFO));
	MEM_SET(&gUart0_ctrl, 0, sizeof(FIFO_CTRL_INFO));
	
	if(pInfo)
	{
		data_q_init(&gUart0_ctrl.rx, pInfo->pRxBuf, pInfo->RxSize, pInfo->RxNearEmptySize, pInfo->RxNearFullSize);
		data_q_init(&gUart0_ctrl.tx, pInfo->pTxBuf, pInfo->TxSize, pInfo->TxNearEmptySize, pInfo->TxNearFullSize);
		MEM_CPY(&gUart0_info, uInfo, sizeof(UARTINFO));
	}else
	{
		return FALSE;
	}
		
	//端口激活
	IF_SET_ACT(gUart0_info.Stat);
	IF_SET_CONNECT(gUart0_info.Stat);
	IF_SET_RX_EMPTY(gUart0_ctrl.rx_tag);
	IF_SET_TX_EMPTY(gUart0_ctrl.tx_tag);

	uart0_driver_init(&UART0_Handle,uInfo);
	uart0_enable_port();	
	
	return TRUE;
}

static BOOL open_uart0(BUFINFO *pInfo, void *uInfo)
{
	return uart0_init(pInfo,(UARTINFO*)uInfo);
}

static BOOL close_uart0(void)
{
	BOOL ret = FALSE;
	
	data_q_reset(&gUart0_ctrl.rx);
	data_q_reset(&gUart0_ctrl.tx);

	gUart0_info.Stat = 0;
	gUart0_ctrl.rx_tag = 0;
	gUart0_ctrl.tx_tag = 0;
	
	ret = TRUE;
	return ret;
}

static u32 read_uart0(u8 *buf, u32 len)
{
	u32 i = 0;

    i = data_q_out_more(&gUart0_ctrl.rx,buf,len);

    if (is_data_q_near_empty(&gUart0_ctrl.rx) && IF_GET_RX_BUSY(gUart0_ctrl.rx_tag))
    {
        uart0_flow_ctrl(gUart0_info.Ctrl.Flw, TRUE);
    }
		
	return i;
}

static u32 write_uart0(u8 *buf, u32 len)
{
    u32	i=0;
    u8  d=0;
	u32 j=0;

    j = i = data_q_in_more(&gUart0_ctrl.tx, buf,len);

	if (i)
	{
		do
		{
			if (IF_GET_TX_EMPTY(gUart0_ctrl.tx_tag))
	        {
	            if (data_q_out(&gUart0_ctrl.tx,&d))
	            {					
					IF_CLS_TX_EMPTY(gUart0_ctrl.tx_tag);	
	                UART0_Handle.Instance->TDR = d;
					while(!(UART0_Handle.Instance->ISR & USART_ISR_TC))
					{};
					IF_SET_TX_EMPTY(gUart0_ctrl.tx_tag);
	            }
	        }
			
			j--;
		}while(j);
			
	}
    return i;

}

static BOOL put_uart0(u8 d)
{
    UART0_Handle.Instance->TDR = d;
	while(!(UART0_Handle.Instance->ISR & USART_ISR_TC))
	{};	
	
	return	TRUE;
}

static BOOL get_uart0(u8 *d)
{
	read_uart0(d,1);
	return TRUE;
}	
static u32 peek_uart0(u8 *buf, u32 len)
{
	u32 i = 0;

	return i;
}

static BOOL peek_sync_uart0(u8 m)
{
	
	return TRUE;
}

static u32 io_ctrl_uart0(IOCTRL_TYPE type, void *p)
{
	u32 ret = 0;
	
	return ret;
}

BOOL uart0_driver_regsit(void)
{
	BOOL ret;
	
	ret = if_regsit_driver(
					UART_PORT,
					open_uart0,
					close_uart0,
					put_uart0,
					get_uart0,
					read_uart0,
					write_uart0,
					peek_uart0,
					peek_sync_uart0,
					io_ctrl_uart0
				);
	return ret;
}

#ifdef debug
#if DEBUG_PORT == UART0_DBG

BOOL debug_port_init(BUFINFO *pBuf,void *pCtrl)
{
    return open_uart0(pBuf,pCtrl);
}

u32 debug_get_data(u8 *d)
{
    return read_uart0(d,1);
}

u32 debug_send_data(u8 d)
{
    put_uart0(d);
    return 1;
}
#endif
#endif

