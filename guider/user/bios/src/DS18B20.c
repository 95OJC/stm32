#include "common.h"

/*
  DS18B20�¶ȴ�����-�����߿��ƣ�
  
  1����λ���У�(��λ�ʹ�������)
    �����������߲�����480us(����һ����λ����)��Ȼ���ͷ�����,�������״̬���ȴ�5~60us��DS18B20���������ߣ�
    Ȼ�󷢳�һ����60~240us�͵�ƽ�źŹ��ɵĴ������塣

  2��дʱ��
    ����дʱ��������ٳ��� 60us����������д����֮������ 1us �Ļָ�ʱ�䡣
    �����߿������������ߴ��߼��ߵ�ƽ�����͵�ƽ��ʱ��дʱ��ʼ����дʱ��ʼ���15us֮�ڽ�����д��λ�͵�
    �������ϡ���15~60us֮��������߽��в��ã�����Ǹߵ�ƽ��д1���͵�ƽд0��

  3����ʱ��
	���ж�ʱ��������� 60us,�������������ڼ����� 1us �Ļָ�ʱ�䡣
    �����߿������������ߴӸߵ�ƽ�����͵�ƽʱ����ʱ��ʼ�������߱������ٱ��� 1us,Ȼ�����߱��ͷš��ȴ������ݣ���
    ����������ʱ��ŵ�15us�����
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

	//�������������ߵ�ƽ
}

static BOOL DS18B20_ResetInit(void)
{
	u8 ack;
	
	DS18B20_PIN_H();//�����ø�
	delay_timer_us(50);
	
	DS18B20_PIN_L();//���͸�λ����
	delay_timer_us(600);//��ʱ����480us
	DS18B20_PIN_H();//�ͷ�����
	delay_timer_us(50);//�ȴ�15~60us
	
	ack = DS18B20_PIN_READ();//��ȡ�������壬0Ϊ���ڣ�1Ϊ������
	delay_timer_us(240);//��ȡ15~60us�Ĵ�����������߱�����Ϊ�ߵ�ƽ
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
		DS18B20_PIN_L();//����дʱ���ڿ�ʼ��һ��дʱ��ǰ������1us���ϵĸߵ�ƽ�ָ��ڡ�
		delay_timer_us(5);//��������1us�ſ�ʼд����
		
		if(data & 0x01)
		{
			DS18B20_PIN_H();
		}
		else
		{
			DS18B20_PIN_L();
		}
		delay_timer_us(60);//��15~60us֮��������߽��в��ã�����Ǹߵ�ƽд1���͵�ƽд0
		
		DS18B20_PIN_H();
		delay_timer_us(5);//�ߵ�ƽ�ָ�������1us
		
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
		DS18B20_PIN_L();//������ʱ�������߱������ٱ��� 1us,Ȼ�����߱��ͷ�
		delay_timer_us(5);//��������1us�ſ�ʼ������
		
		data >>= 1;
		DS18B20_PIN_H();//���߱��ͷ�

		delay_timer_us(5);//�ȴ������ݣ��ѿ���������ʱ��ŵ�15us�����
		if(DS18B20_PIN_READ())
		{
          data |= 0x80;
		}

		//DS18B20_PIN_H();
		delay_timer_us(60);//�ߵ�ƽ�ָ�������1us
	}
	
	
	return data;
}

u16 DS18B20_ReadTemperature(void)
{
    u16 temp;
    u8 tempL,tempH;
	
    while(!DS18B20_ResetInit());//��ʼ��
    DS18B20_WriteByte(0xcc);//������64λ���кŵĲ�����������һ���ӻ������
    DS18B20_WriteByte(0x44);//�����¶�ת��
    delay_timer_ms(2);//��Ҫ�ȴ�ת��ʱ��

    while(!DS18B20_ResetInit());//��ʼ��
    DS18B20_WriteByte(0xcc);//������64λ���кŵĲ�����������һ���ӻ������
    DS18B20_WriteByte(0xbe);//���¶ȼĴ�����ͷ����ֵ�ֱ�Ϊ�¶ȵĵ�λ�͸�λ��

    tempL = DS18B20_ReadByte();
    tempH = DS18B20_ReadByte();

    if(tempH > 0x7f)//���λΪ1ʱ�¶��Ǹ�
    {
        tempL =~ tempL;
		tempH =~ tempH+1;
    }

	temp = tempL/16 + tempH*16;//��������
	
	return temp;
}
