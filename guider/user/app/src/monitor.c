#include "main.h"


#define DEBUG 1

#if (DEBUG && APP_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif


MECH_STATUS gMechStat;


static void readUsartSet(UARTINFO* usartInfo) 
{
	usartInfo->Ctrl.Baud = 115200;
	usartInfo->Ctrl.Datas = USART_WordLength_8b;
	usartInfo->Ctrl.Stops = USART_StopBits_1;
	usartInfo->Ctrl.Prty = USART_Parity_No;
	usartInfo->Ctrl.Flw = USART_HardwareFlowControl_None;
	usartInfo->Ctrl.Mode = USART_Mode_Rx | USART_Mode_Tx;
}


void initRecvPort(void)
{
	BUFINFO stdBufInf;
	UARTINFO usartInfo;

	MEM_SET(&stdBufInf,0,sizeof(stdBufInf));
	MEM_SET(&usartInfo,0,sizeof(usartInfo));

	stdBufInf.pRxBuf = gSysPool.sysExtRxBuf;
	stdBufInf.pTxBuf = gSysPool.sysExtTxBuf;
	stdBufInf.RxSize = SYS_RXBUF_LEN;
	stdBufInf.TxSize = SYS_TXBUF_LEN;
	stdBufInf.RxNearEmptySize = RX_NEAR_EMPTY_SIZE;
	stdBufInf.RxNearFullSize =  RX_NEAR_FULL_SIZE;
	stdBufInf.TxNearEmptySize = TX_NEAR_EMPTY_SIZE;
	stdBufInf.TxNearFullSize = TX_NEAR_FULL_SIZE;

	readUsartSet(&usartInfo);
	INIT_RECV_PORT(UART_PORT,&stdBufInf,&usartInfo);
	SET_RECV_PORT(UART_PORT);


}


//at41功能以下：
//添加根据EDS项，然后把电机和出针最大限制值传给底层限制，根据EDS项赋值给底层gDevInfo，最后检测纸道是否有纸而报错
//DevInit()，提交开机复位item项给底层，*如果之前报错？？？
//注册上层的图像填充缓冲buf给prnq.c
//*while循环检测开机功能的按键，所有按键都没有按下才能退出while循环，然后若有超过1秒的按键输入，则把对应的开机功能函数注册到gSysInfo.pwonFun，给emul任务执行该回调函数
//initRecvPort()，根据EDS项的通讯口配置，给底层设置通讯接口结构体，然后打开if_open和设置if_set，然后其他任务就可以用if_read和if_write控制数据接收和发送了
//设置beep报警的标志SET_BEEP_OVER()
//获取错误num；上层gSysInfo.errNO = 底层(gDevInfo.errNO)，最后打印头报错优先级高，会覆盖其他错
//提交字车定位的item项给底层，一般时居中的字车动作，*如果此时已经报错了？？？
//....
//有错时，monitorState的errProg(GET_SYS_ERROR())会一直发beep的任务
//...
void monitorTask(void *p)
{

	//DevInit();
	//initRecvPort();//先屏蔽，因为与debug共用一个串口

	
	while(1)
	{
		DBG_TRACE("monitorTask\r\n");
		vTaskDelay(1000);
	}
}




