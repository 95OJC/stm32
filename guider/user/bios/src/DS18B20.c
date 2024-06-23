#include "common.h"

/*
  DS18B20温度传感器-单总线控制：
  
  1、复位序列：(复位和存在脉冲)
    主机拉低总线并保持480us(发出一个复位脉冲)，然后释放总线,进入接收状态，等待5~60us后DS18B20把总线拉高，
    然后发出一个由60~240us低电平信号构成的存在脉冲。

  2、写时序：
    所有写时序必须最少持续 60us，包括两个写周期之间至少 1us 的恢复时间。
    当总线控制器把数据线从逻辑高电平拉到低电平的时候，写时序开始。在写时序开始后的15us之内将所需写的位送到
    数据线上。在15~60us之间对数据线进行采用，如果是高电平就写1，低电平写0。

  3、读时序：
	所有读时序必须最少 60us,包括两个读周期间至少 1us 的恢复时间。
    当总线控制器把数据线从高电平拉到低电平时，读时序开始，数据线必须至少保持 1us,然后总线被释放。等待读数据，把
    控制器采样时间放到15us的最后。
*/


#define DS18B20_RCC(r,s)		RCC_APB2PeriphClockCmd(r,s)
#define DS18B20_CLOCK			RCC_APB2Periph_GPIOA
#define	DS18B20_GPIO			GPIOA
#define	DS18B20_PIN				GPIO_Pin_4

#define	DS18B20_PIN_H()			GPIO_SetBits(DS18B20_GPIO, DS18B20_PIN)
#define	DS18B20_PIN_L()			GPIO_ResetBits(DS18B20_GPIO, DS18B20_PIN)

#define	DS18B20_PIN_READ()		GPIO_ReadInputDataBit(DS18B20_GPIO, DS18B20_PIN)


void DS18B20_init(void)
{
	GPIO_InitTypeDef structInit;
	
	DS18B20_RCC(DS18B20_CLOCK,ENABLE);
	structInit.GPIO_Pin = DS18B20_PIN;
	structInit.GPIO_Mode = GPIO_Mode_Out_OD;
	structInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS18B20_GPIO,&structInit);

	//被上拉电阻拉高电平
}

static BOOL DS18B20_ResetInit(void)
{
	u8 ack;
	
	DS18B20_PIN_H();//总线置高
	delay_timer_us(50);
	
	DS18B20_PIN_L();//发送复位脉冲
	delay_timer_us(600);//延时大于480us
	DS18B20_PIN_H();//释放总线
	delay_timer_us(50);//等待15~60us
	
	ack = DS18B20_PIN_READ();//读取存在脉冲，0为存在，1为不存在
	delay_timer_us(240);//读取15~60us的存在脉冲后，总线被上拉为高电平
	if(!ack)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


static BOOL DS18B20_WriteByte(u8 data)
{
	u8 i;

	for(i=0;i<8;i++)
	{
		DS18B20_PIN_L();//产生写时序，在开始另一个写时序前必须有1us以上的高电平恢复期。
		delay_timer_us(5);//保持至少1us才开始写数据
		
		if(data & 0x01)
		{
			DS18B20_PIN_H();
		}
		else
		{
			DS18B20_PIN_L();
		}
		delay_timer_us(60);//在15~60us之间对数据线进行采用，如果是高电平写1，低电平写0
		
		DS18B20_PIN_H();
		delay_timer_us(5);//高电平恢复期至少1us
		
		data >>= 1;
	}
	
	return TRUE;
}

static u8 DS18B20_ReadByte(void)
{
    u8 data = 0;
    u8 i;
	
	for(i=0;i<8;i++)
	{
		DS18B20_PIN_H();
		delay_timer_us(5);
		DS18B20_PIN_L();//产生读时序，数据线必须至少保持 1us,然后总线被释放
		delay_timer_us(5);//保持至少1us才开始读数据
		
		data >>= 1;
		DS18B20_PIN_H();//总线被释放

		delay_timer_us(5);//等待读数据，把控制器采样时间放到15us的最后
		if(DS18B20_PIN_READ())
		{
          data |= 0x80;
		}

		//DS18B20_PIN_H();
		delay_timer_us(60);//高电平恢复期至少1us
	}
	
	
	return data;
}

u16 DS18B20_ReadTemperature(void)
{
    u16 temp;
    u8 tempL,tempH;
	
    while(!DS18B20_ResetInit());//初始化
    DS18B20_WriteByte(0xcc);//跳过读64位序列号的操作，仅用于一个从机情况下
    DS18B20_WriteByte(0x44);//启动温度转换
    delay_timer_ms(2);//需要等待转换时间

    while(!DS18B20_ResetInit());//初始化
    DS18B20_WriteByte(0xcc);//跳过读64位序列号的操作，仅用于一个从机情况下
    DS18B20_WriteByte(0xbe);//读温度寄存器（头两个值分别为温度的低位和高位）

    tempL = DS18B20_ReadByte();
    tempH = DS18B20_ReadByte();

    if(tempH > 0x7f)//最高位为1时温度是负
    {
        tempL =~ tempL;
		tempH =~ tempH+1;
    }

	temp = tempL/16 + tempH*16;//整数部分
	
	return temp;
}
