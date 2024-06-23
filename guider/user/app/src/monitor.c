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


//at41�������£�
//��Ӹ���EDS�Ȼ��ѵ���ͳ����������ֵ�����ײ����ƣ�����EDS�ֵ���ײ�gDevInfo�������ֽ���Ƿ���ֽ������
//DevInit()���ύ������λitem����ײ㣬*���֮ǰ��������
//ע���ϲ��ͼ����仺��buf��prnq.c
//*whileѭ����⿪�����ܵİ��������а�����û�а��²����˳�whileѭ����Ȼ�����г���1��İ������룬��Ѷ�Ӧ�Ŀ������ܺ���ע�ᵽgSysInfo.pwonFun����emul����ִ�иûص�����
//initRecvPort()������EDS���ͨѶ�����ã����ײ�����ͨѶ�ӿڽṹ�壬Ȼ���if_open������if_set��Ȼ����������Ϳ�����if_read��if_write�������ݽ��պͷ�����
//����beep�����ı�־SET_BEEP_OVER()
//��ȡ����num���ϲ�gSysInfo.errNO = �ײ�(gDevInfo.errNO)������ӡͷ�������ȼ��ߣ��Ḳ��������
//�ύ�ֳ���λ��item����ײ㣬һ��ʱ���е��ֳ�������*�����ʱ�Ѿ������ˣ�����
//....
//�д�ʱ��monitorState��errProg(GET_SYS_ERROR())��һֱ��beep������
//...
void monitorTask(void *p)
{

	//DevInit();
	//initRecvPort();//�����Σ���Ϊ��debug����һ������

	
	while(1)
	{
		DBG_TRACE("monitorTask\r\n");
		vTaskDelay(1000);
	}
}




