//底层用到的C库
#include <string.h>//内存拷贝函数用到
#include <stdio.h>//重定向函数用到
#include <stdlib.h>//malloc()、free()等用到
#include <stdarg.h>//debug的va_start等用到

//底层的STM32库函数(硬件抽象层/底层，含映射寄存器及寄存器位宏定义)
//u8、u6、u32定义在里面
//包含了“stdint.h”这个头文件，可用uint16_t之类
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
u8 iap_buf[iap_buf_size];				//假设待写入的代码数据
__IO u32 iap_buf_idx = 0;				//iap_buf数组下标偏移记录
__IO BOOL rev_completed_flag = FALSE;	//接收升级数据完成标志

/***********************************timer****************************************/
//timer num
#define	TIMER_BUSx_CLOCK(x,s)		RCC_APB1PeriphClockCmd(x,s)
#define TIMER_CLOCK					RCC_APB1Periph_TIM7//RCC_APB1Periph_TIM6
#define	TIMER_NUM					TIM7//TIM6
//timer中断时间配置
#define	TIMER_PERIOD_COUNT			(72 - 1)//次数
#define	TIMER_PRESCALER				(1000 - 1)//分频

void delay_timer_ms(u32 ms)
{
	if(ms > 500)
	{
		ms = 500;
	}
	
	TIM_TimeBaseInitTypeDef basicTimer_initStruct;
	
	TIMER_BUSx_CLOCK(TIMER_CLOCK, ENABLE);
	
	basicTimer_initStruct.TIM_Period = ((72-1)*ms);//重装载值
	basicTimer_initStruct.TIM_Prescaler = (1000-1);//预分频器：定时器时钟
	TIM_TimeBaseInit(TIMER_NUM,&basicTimer_initStruct);
	//清timer标志位
	TIM_ClearFlag(TIMER_NUM,TIM_FLAG_Update);
	//使能计数器
	TIM_Cmd(TIMER_NUM,ENABLE);	
	//等待标志位
	while(!TIM_GetFlagStatus(TIMER_NUM,TIM_FLAG_Update));
	//关闭定时器
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
	NVIC_SetPriority(KEY_NVIC_IRQN,KEY_NVIC_PRIO);//因为分组4没子优先级，若有则取bit几的抢占和子优先级算
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
		delay_timer_ms(10);	//软件消抖
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
//流控模拟io
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
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_TX_GPIO,&gpio_initStruct);

	gpio_initStruct.GPIO_Pin = USART_RX_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入(无复用输入功能可用)
    gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(USART_RX_GPIO,&gpio_initStruct);

	#if 0//硬流控init
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
	//usart优先级配置
	NVIC_ClearPendingIRQ(USART_NVIC_IRQN);
	NVIC_SetPriority(USART_NVIC_IRQN,USART_NVIC_PRIO);
	NVIC_EnableIRQ(USART_NVIC_IRQN);

	//usart初始化
	usart_gpio_init();
	usart_config_init();

	//使能串口接收中断
	USART_ITConfig(USART_NUM, USART_IT_RXNE, ENABLE);

	//使能串口空闲中断
	USART_ITConfig(USART_NUM, USART_IT_IDLE, ENABLE);

	//使能串口
	USART_Cmd(USART_NUM, ENABLE);	
}

//发送一个字符，例如‘1’之类，填非字符乱码----对应针打at41的put_uart0函数
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

//返回一个十六进制的值，0x00~0xFF----对应针打at41的get_uart0函数
u8 usart_getByte(void)
{
	while(USART_GetFlagStatus(USART_NUM,USART_FLAG_RXNE) == RESET);
	//读DR数据寄存器清RXNE位
	return (u8)USART_ReceiveData(USART_NUM);
}

/*重定向c库函数printf到串口，重定向后可上层和底层可使用printf函数*/
int fputc(int ch, FILE *F)
{
	//发送一个字节到串口
	USART_SendData(USART_NUM, (u8)ch);

	//等待发送完毕
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
		//软件序列清除该位(先读USART_SR，然后读USART_DR)
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级
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
	u16 iap_temp;//临时数据
	u32 flash_addr = ApplicationAddress;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级

	//1、初始化时钟为72M,LED,按键,串口
	SystemInit();
	led_init();
	key_init();
	usart_init();
	usart_sendString("boot:usart_init ok!\r\n");
	
	//2、检测按键是否按下，进入升级模式
	if(test_key1Ctrl())
	{
		usart_sendString("进入升级模式，擦除flash中...\r\n");
		//3、擦除用户程序(0x08008000~0x0807FFFF地址空间的flash)
		FLASH_Unlock();//解锁
		FLASH_SetLatency(FLASH_Latency_2);//因为系统时钟为72M，所以设置2个时钟周期
		for( i=0 ; i<240 ; i++ )//擦除flash
		{
			if(FLASH_ErasePage(flash_addr + i*2048) != FLASH_COMPLETE)
			{
				usart_sendString("flash erase fail！\r\n");
				while(1);
			}
		}
		usart_sendString("擦除flash成功，等待接收升级数据，灯开始闪烁\r\n");
		
		//4、检测串口数据接口接收完毕
		while(!rev_completed_flag)
		{
			led_toggle();
			delay_timer_ms(500);
		}
		
		led_on();
		usart_sendString("接收升级文件完成，灯常亮，正在升级中...\r\n");
		rev_completed_flag = FALSE;
		
		//5、从串口读取新的用户代码数据，把代码写入用户程序空间0x08008000~0x0807FFFF
		for( i=0 ; i<(iap_buf_size/2) ; i++ )//写入flash
		{
			iap_temp = (iap_buf[2*i+1] << 8) | (iap_buf[2*i]);//2个u8整合1个u16
			if(FLASH_ProgramHalfWord(flash_addr, iap_temp) != FLASH_COMPLETE)
			{
				usart_sendString("flash write fail!\r\n");
				while(1);
			}
			flash_addr += 2;//每次地址+2，因为每次写入是2个字节
		}

		//6、用户程序更新完毕，等待重新上电或复位
		FLASH_Lock();//上锁
		usart_sendString("写入升级数据到flash成功，执行复位\r\n");
		NVIC_SystemReset();
		while(1);//升级成功后不会执行到这里

	}
	else
	{
		#if 0
		led_on();
		usart_sendString("跳转到用户程序运行\r\n");
		while(1);
		#else
		//7、跳转到用户程序(强制将PC指针跳转到0x08008000+4处)
		usart_sendString("跳转到用户程序运行...\r\n");
		usart_sendString("\r\n");
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);//如果不加该行代码，会有时导致app程序一直重启，是因为app找不到中断向量表？
		if( ( (*(__IO u32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000 )//如果用户空间的首地址存在指向ram空间的堆栈地址，那么认为存在app程序，可进行跳转
		{
			JumpAddress = *(__IO u32*)(ApplicationAddress + 4);//获取用户程序PC指针指向的地址->Reset_Handler函数的入口地址
			Jump_to_Application = (pFunction)JumpAddress;//令函数指针指向复位函数入口地址
			__set_MSP(*(__IO u32*)ApplicationAddress);//堆栈的初始化，重新设定栈顶地址为用户程序的栈顶地址
			Jump_to_Application();//跳转到新的复位函数
		}
		while(1);//跳转成功后不会执行到这里
		#endif
	}
	
#endif

	//return 0;
}
