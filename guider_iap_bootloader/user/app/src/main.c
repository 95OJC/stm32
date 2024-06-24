//�ײ��õ���C��
#include <string.h>//�ڴ濽�������õ�
#include <stdio.h>//�ض������õ�
#include <stdlib.h>//malloc()��free()���õ�
#include <stdarg.h>//debug��va_start���õ�

//�ײ��STM32�⺯��(Ӳ�������/�ײ㣬��ӳ��Ĵ������Ĵ���λ�궨��)
//u8��u6��u32����������
//�����ˡ�stdint.h�����ͷ�ļ�������uint16_t֮��
#include "stm32f10x.h"

#ifndef BOOL
#define BOOL        u8
#endif

#ifndef	FALSE
#define	FALSE	0;
#endif

#ifndef	TRUE
#define	TRUE	1;
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef	null
#define null	0
#endif

#define MEM_SET(addr,val,l)			memset((addr),(val),(l))

typedef	void(*pFunction)(void);
#define	ApplicationAddress	0x08008000
#define	iap_buf_size		(10*1024)
u8 iap_buf[iap_buf_size];				//�����д��Ĵ�������
__IO u32 iap_buf_idx = 0;				//iap_buf�����±�ƫ�Ƽ�¼
__IO BOOL rev_completed_flag = FALSE;	//��������������ɱ�־

/***********************************timer****************************************/
//timer num
#define	TIMER_BUSx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define TIMER_CLOCK					RCC_APB1Periph_TIM7//RCC_APB1Periph_TIM6
#define	TIMER_NUM					TIM7//TIM6
//timer�ж�ʱ������
#define	TIMER_PERIOD_COUNT			(72 - 1)//����
#define	TIMER_PRESCALER				(1000 - 1)//��Ƶ

void delay_timer_ms(u32 ms)
{
	if(ms > 500)
	{
		ms = 500;
	}
	
	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	TIMER_BUSx_CLOCK(TIMER_CLOCK, ENABLE);
	
	basicTimer_initStruct.TIM_Period = ((72-1)*ms);//��װ��ֵ
	basicTimer_initStruct.TIM_Prescaler = (1000-1);//Ԥ��Ƶ������ʱ��ʱ��
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//��timer��־λ
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//ʹ�ܼ�����
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//�ȴ���־λ
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//�رն�ʱ��
	TIM_Cmd(TIMER_NUM,DISABLE);
}




/**********************************gpio******************************************/
//gpio_out
#define	LED_BUSx_CLOCK(x,s)			RCC_APB2PeriphClockCmd(x,s)		
#define LED_CLOCK					RCC_APB2Periph_GPIOB
#define	LED_GPIO					GPIOB
#define	LED_PIN						GPIO_Pin_0//GPIO_Pin_5

void led_on(void)
{
	
    GPIO_ResetBits(LED_GPIO,LED_PIN);
}

void led_off(void)
{
    GPIO_SetBits(LED_GPIO,LED_PIN);
}

void led_toggle(void)
{
	LED_GPIO->ODR ^= LED_PIN;
}

void led_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	LED_BUSx_CLOCK(LED_CLOCK,ENABLE);
	
    gpio_initStruct.GPIO_Pin = LED_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO,&gpio_initStruct);

	led_off();
}

//gpio_in
#define	KEY_BUSx_CLOCK(x,s)			RCC_APB2PeriphClockCmd(x,s)		
#define KEY_CLOCK					RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOA
#define	KEY_GPIO					GPIOC//GPIOA
#define	KEY_PIN						GPIO_Pin_13//GPIO_Pin_0
//exti and afio
#define	KEY_AFIO_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	KEY_AFIO_CLOCK				RCC_APB2Periph_AFIO
#define	KEY_AFIO_GPIO				GPIO_PortSourceGPIOC//GPIO_PortSourceGPIOA
#define	KEY_AFIO_PIN				GPIO_PinSource13//GPIO_PinSource0
#define	KEY_EXTI_LINE				EXTI_Line13//EXTI_Line0
//nvic
#define	KEY_NVIC_IRQN				EXTI15_10_IRQn//EXTI0_IRQn
#define	KEY_NVIC_PRIO				0x01
#define	KEY_IRQHandler_FUNC			EXTI15_10_IRQHandler//EXTI0_IRQHandler

void key_init(void)
{
    GPIO_InitTypeDef gpio_initStruct;

	KEY_BUSx_CLOCK(KEY_CLOCK,ENABLE);

    gpio_initStruct.GPIO_Pin = KEY_PIN;
    gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_GPIO,&gpio_initStruct);
}

void key_exti_init(void)
{
	EXTI_InitTypeDef exti_initStruct;
	MEM_SET(&exti_initStruct,0,sizeof(EXTI_InitTypeDef));

	NVIC_ClearPendingIRQ(KEY_NVIC_IRQN);
	NVIC_SetPriority(KEY_NVIC_IRQN,KEY_NVIC_PRIO);//��Ϊ����4û�����ȼ���������ȡbit������ռ�������ȼ���
	NVIC_EnableIRQ(KEY_NVIC_IRQN);

	KEY_AFIO_BUSx_CLOCK(KEY_AFIO_CLOCK,ENABLE);
	GPIO_EXTILineConfig(KEY_AFIO_GPIO,KEY_AFIO_PIN);
	
	exti_initStruct.EXTI_Line = KEY_EXTI_LINE;
	exti_initStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_initStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_initStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_initStruct);
}

BOOL test_key1Ctrl(void)
{
	if(GPIO_ReadInputDataBit(KEY_GPIO,KEY_PIN) != 0)
	{
		delay_timer_ms(10);	//�������
		if(GPIO_ReadInputDataBit(KEY_GPIO,KEY_PIN) != 0)
		{
			//led_on();
			return TRUE;
		}
		else
		{
			//led_off();
			return FALSE;
		}
	}
	else
	{
		//led_off();
		return FALSE;
	}
}

void KEY_IRQHandler_FUNC(void)
{
	if(EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
	{
		led_toggle();
		EXTI_ClearITPendingBit(KEY_EXTI_LINE);
	}
}




/***********************************usart****************************************/
//����ģ��io
#define	USART_RTS_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RTS_CLOCK				RCC_APB2Periph_GPIOC//RCC_APB2Periph_GPIOB
#define USART_RTS_GPIO				GPIOC//GPIOB
#define USART_RTS_PIN				GPIO_Pin_7//GPIO_Pin_6

#define	USART_NUM					USART1
#define	USART_BUSx_CLOCK(x,s)		RCC_APB2PeriphClockCmd(x,s)
#define	USART_CLOCK					RCC_APB2Periph_USART1

#define	USART_TX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_TX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_TX_GPIO				GPIOA
#define USART_TX_PIN				GPIO_Pin_9

#define	USART_RX_BUSx_CLOCK(x,s)	RCC_APB2PeriphClockCmd(x,s)
#define	USART_RX_CLOCK				RCC_APB2Periph_GPIOA
#define USART_RX_GPIO				GPIOA
#define USART_RX_PIN				GPIO_Pin_10
//nvic
#define	USART_NVIC_IRQN				USART1_IRQn
#define	USART_NVIC_PRIO				0x01
#define	USART_IRQHandler_FUNC		USART1_IRQHandler

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

	#if 0//Ӳ����init
	usart_rts_io_init();
	#endif
}

static void usart_config_init(void)
{
	USART_InitTypeDef usart_initStruct;
	MEM_SET(&usart_initStruct,0,sizeof(usart_initStruct));
	
	usart_initStruct.USART_BaudRate = 115200;
	usart_initStruct.USART_WordLength = USART_WordLength_8b;
	usart_initStruct.USART_StopBits = USART_StopBits_1;
	usart_initStruct.USART_Parity = USART_Parity_No;
	usart_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_initStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_NUM, &usart_initStruct);
}

void usart_init(void)
{
	//usart���ȼ�����
	NVIC_ClearPendingIRQ(USART_NVIC_IRQN);
	NVIC_SetPriority(USART_NVIC_IRQN,USART_NVIC_PRIO);
	NVIC_EnableIRQ(USART_NVIC_IRQN);

	//usart��ʼ��
	usart_gpio_init();
	usart_config_init();

	//ʹ�ܴ��ڽ����ж�
	USART_ITConfig(USART_NUM, USART_IT_RXNE, ENABLE);

	//ʹ�ܴ��ڿ����ж�
	USART_ITConfig(USART_NUM, USART_IT_IDLE, ENABLE);

	//ʹ�ܴ���
	USART_Cmd(USART_NUM, ENABLE);	
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
		ucTemp = USART_ReceiveData(USART_NUM);
		if(iap_buf_idx < iap_buf_size)
		{
			iap_buf[iap_buf_idx] = ucTemp;
			//printf("iap_buf[%d] = %d\r\n",iap_buf_idx,iap_buf[iap_buf_idx]);
			iap_buf_idx++;
		}
	}
	else if(USART_GetITStatus(USART_NUM,USART_IT_IDLE)!=RESET)
	{
		//������������λ(�ȶ�USART_SR��Ȼ���USART_DR)
		USART_NUM->SR;
		USART_NUM->DR;
		rev_completed_flag = TRUE;
		//printf("rev_completed_flag = TRUE\r\n");
		//printf("iap_buf[%d] = %d\r\n",iap_buf_idx-1,iap_buf[iap_buf_idx-1]);
	}

}






int main(void)
{
#if	0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�
	led_init();
	key_init();
	usart_init();
	usart_sendString("usart_init ok!\r\n");
	#if 0
	key_exti_init();
	while(1);
	#else
	while(1)
	{
		test_key1Ctrl();
	}
	#endif
#else
	u32	JumpAddress;
	pFunction	Jump_to_Application;
	
	u32 i;
	u16 iap_temp;//��ʱ����
	u32 flash_addr = ApplicationAddress;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�

	//1����ʼ��ʱ��Ϊ72M,LED,����,����
	SystemInit();
	led_init();
	key_init();
	usart_init();
	usart_sendString("boot:usart_init ok!\r\n");
	
	//2����ⰴ���Ƿ��£���������ģʽ
	if(test_key1Ctrl())
	{
		usart_sendString("��������ģʽ������flash��...\r\n");
		//3�������û�����(0x08008000~0x0807FFFF��ַ�ռ��flash)
		FLASH_Unlock();//����
		FLASH_SetLatency(FLASH_Latency_2);//��Ϊϵͳʱ��Ϊ72M����������2��ʱ������
		for( i=0 ; i<240 ; i++ )//����flash
		{
			if(FLASH_ErasePage(flash_addr + i*2048) != FLASH_COMPLETE)
			{
				usart_sendString("flash erase fail��\r\n");
				while(1);
			}
		}
		usart_sendString("����flash�ɹ����ȴ������������ݣ��ƿ�ʼ��˸\r\n");
		
		//4����⴮�����ݽӿڽ������
		while(!rev_completed_flag)
		{
			led_toggle();
			delay_timer_ms(500);
		}
		
		led_on();
		usart_sendString("���������ļ���ɣ��Ƴ���������������...\r\n");
		rev_completed_flag = FALSE;
		
		//5���Ӵ��ڶ�ȡ�µ��û��������ݣ��Ѵ���д���û�����ռ�0x08008000~0x0807FFFF
		for( i=0 ; i<(iap_buf_size/2) ; i++ )//д��flash
		{
			iap_temp = (iap_buf[2*i+1] << 8) | (iap_buf[2*i]);//2��u8����1��u16
			if(FLASH_ProgramHalfWord(flash_addr, iap_temp) != FLASH_COMPLETE)
			{
				usart_sendString("flash write fail!\r\n");
				while(1);
			}
			flash_addr += 2;//ÿ�ε�ַ+2����Ϊÿ��д����2���ֽ�
		}

		//6���û����������ϣ��ȴ������ϵ��λ
		FLASH_Lock();//����
		usart_sendString("д���������ݵ�flash�ɹ���ִ�и�λ\r\n");
		NVIC_SystemReset();
		while(1);//�����ɹ��󲻻�ִ�е�����

	}
	else
	{
		#if 0
		led_on();
		usart_sendString("��ת���û���������\r\n");
		while(1);
		#else
		//7����ת���û�����(ǿ�ƽ�PCָ����ת��0x08008000+4��)
		usart_sendString("��ת���û���������...\r\n");
		usart_sendString("\r\n");
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);//������Ӹ��д��룬����ʱ����app����һֱ����������Ϊapp�Ҳ����ж�������
		if( ( (*(__IO u32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000 )//����û��ռ���׵�ַ����ָ��ram�ռ�Ķ�ջ��ַ����ô��Ϊ����app���򣬿ɽ�����ת
		{
			JumpAddress = *(__IO u32*)(ApplicationAddress + 4);//��ȡ�û�����PCָ��ָ��ĵ�ַ->Reset_Handler��������ڵ�ַ
			Jump_to_Application = (pFunction)JumpAddress;//���ָ��ָ��λ������ڵ�ַ
			__set_MSP(*(__IO u32*)ApplicationAddress);//��ջ�ĳ�ʼ���������趨ջ����ַΪ�û������ջ����ַ
			Jump_to_Application();//��ת���µĸ�λ����
		}
		while(1);//��ת�ɹ��󲻻�ִ�е�����
		#endif
	}
	
#endif

	//return 0;
}
