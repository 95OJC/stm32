#include "common.h"


/*-----------------------------------------------------------------
���ļ���Ҫ��������֤iic��

Ӳ��iic���������Ĳ��裺
1������iic��ʼ�źš�
1.1�����EV5�¼�(SB)�����־(����ACK)��
2������eeprom�ĵ�ַ��д����
2.1�����յ�ACK(���յ��ӻ���NACK��ƥ����ADDR����1)��Ȼ�����EV6�¼�(ADDR��1)�Ͳ���EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)��
3������Ҫд���eeprom�洢���ݵĵ�ַ(��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�)��
3.1�����ݱ����ͳ�ȥ�󣬵ȴ�ACK�Ͳ���EV8�¼�(TxE��1)��
4������һ�ֽ�Ҫд�������(��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�)��
4.1�����ݱ����ͳ�ȥ�󣬵ȴ�ACK(������NACK�����������ڷ�������)�Ͳ���EV8�¼�(TxE��1����ʾ���ݼĴ���Ϊ��)��
5��ѭ��4��4.1��
6������ֹͣ�ź�(��������ֹͣ�������ݣ����ӻ�����NACK����������ֹͣ��������)��
7������ʹ���� I2C �жϣ����������¼�����ʱ��������� I2C �ж��źţ�����ͬһ���жϷ��������� I2C �жϷ���������ͨ�����Ĵ���λ���ж�����һ���¼���


Ҫ�㣺
1��ÿ�η��͵�ַ�����ݺ󣬵ȴ�����ACK��NACK������Ӳ������ġ�
2�����дӻ�Ӧ��(ACK)��Ӳ���Զ�����Ӧ���¼�����(��־��1)��


��uart����1���ֽں���������
1�����ڷ���ֻ��1��TX��(Ӳ���Զ�����UARTЭ��)��ֻ����8λ��DR�Ĵ���д1���ֽڼ��ɡ�
2��iic������2����(ͬʱ��Ҫ����IICЭ��)�����ȷ�����ʼ�ź�(�ȴ�ACK)��Ȼ���͵�
   ַ(�ȴ�ACK)����Ȼ����8λ��DR�Ĵ�����������(�ȴ�ACK)......�����ֹͣ�ź�(����ȴ�ACK)��
3��������uart��iic��Ҫ����ʼ�źš��ĸ�IIC���豸�ĵ�ַ�źš�ֹͣ�źţ�����ÿ���źŶ���Ҫ�ȴ�ACK(��ֹͣ�ź�����)��
4�������˴��豸����Ҫ�޸��豸��ַ�Ͷ�д��ַ��


***ֹͣ�ź�ǰ���ÿһ������SCL������Ϊ����(��һ���ȶ�SDA����ΪSCL�ǵ͵�ƽ��Ӱ��)�����ֹͣ�źŽ����SCL��SDA�����ߡ�
***ÿһ��������Ҫ����SCLһ�����壬��ʱ��SDA�л�Ϊ��������ģʽ��
------------------------------------------------------------------*/

/*�ȴ���ʱʱ��*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
static __IO u32 iicTimeout = I2CT_FLAG_TIMEOUT;

//iic��д����
#define IIC_DEVICE_ADDRESS_WRITE	0 
#define	IIC_DEVICE_ADDRESS_READ		1


static void iic_timeout_userCallBack(u8 errorCode)
{
	printf("iic_timeout! errorCode = %d",errorCode);
	while(1);
}

static void iic_gpio_init(void)
{
	//gpio��ʼ��
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	IIC_RCC_GPIO_CLOCK(IIC_RCC_GPIO_PORT,ENABLE);

	//I2C_SCL�ź���
	gpio_initStruct.GPIO_Pin = IIC_SCL_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//��©���
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(IIC_GPIO_PORT,&gpio_initStruct);
	
	//I2C_SDA������
	gpio_initStruct.GPIO_Pin = IIC_SDA_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//��©���
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(IIC_GPIO_PORT,&gpio_initStruct);
	
}

static void iic_config_init(void)
{
	I2C_InitTypeDef	iic_initStruct;
	MEM_SET(&iic_initStruct,0,sizeof(iic_initStruct));
	
	IIC_RCC_CLOCK(IIC_RCC_PORT,ENABLE);

	iic_initStruct.I2C_ClockSpeed = 400000;//400k����ģʽ
	iic_initStruct.I2C_Mode = I2C_Mode_I2C;//iicģʽ(����Ҫ�ڴ���������ģʽ)
	iic_initStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	iic_initStruct.I2C_OwnAddress1 = 0x0A;//��IIC������ַ��һ������(����Ҳ��Ҫ���Լ���iic��ַ)
	iic_initStruct.I2C_Ack = I2C_Ack_Enable;//�������ַ�����ݣ����Զ��ȴ�Ӧ���ź�(Ӳ��ʵ��)
	iic_initStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7λ��ַ

	I2C_Init(IIC_NUM,&iic_initStruct);
}

DEV_STATUS iic_init(void)
{
	iic_gpio_init();
	iic_config_init();

	return DEV_OK;
}


DEV_STATUS iic_ByteWrite(u8 device_addr, u32 writeAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;

	//1������iic��ʼ�ź�
	I2C_GenerateSTART(IIC_NUM,ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1�����EV5�¼�(SB)�����־(����ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2������eeprom�ĵ�ַ��д����
	I2C_Send7bitAddress(IIC_NUM,(device_addr|IIC_DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1�����յ�ACK(���յ��ӻ���NACK��ƥ����ADDR����1)��Ȼ�����EV6�¼�(ADDR��1)�Ͳ���EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(1);
	}

	//3������Ҫд���eeprom�ڲ��洢���ĵ�ַ
	I2C_SendData(IIC_NUM,(uint8_t)writeAddr);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//3.1�����ݱ����ͳ�ȥ�󣬵ȴ�ACK�Ͳ���EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(2);		
	}

	//4������һ�ֽ�Ҫд�������
	I2C_SendData(IIC_NUM,*pBuffer);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//4.1�����ݱ����ͳ�ȥ�󣬵ȴ�ACK(������NACK�����������ڷ�������)�Ͳ���EV8�¼�(TxE��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(3); 	
	}

	// ......�����ظ�4д�벻ͬ���ݣ����ǵ�ַ��Ҫ�ı䡣
	
	//����ֹͣ�ź�(��������ֹͣ�������ݣ����ӻ�����NACK����������ֹͣ��������)
	I2C_GenerateSTOP(IIC_NUM, ENABLE);

	return ret;
}


DEV_STATUS iic_ByteRead(u8 device_addr, u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;
	
	//1������iic��ʼ�ź�
	I2C_GenerateSTART(IIC_NUM,ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1�����EV5�¼�(SB)�����־(����ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2������eeprom�ĵ�ַ��д����
	I2C_Send7bitAddress(IIC_NUM,(device_addr|IIC_DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1�����յ�ACK(���յ��ӻ���NACK��ƥ����ADDR����1)��Ȼ�����EV6�¼�(ADDR��1)�Ͳ���EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(1);
	}

	//ͨ����������PEλ���EV6�¼�(ADDR��0),Ϊ�˵ڶ��εķ���eeprom�ĵ�ַ�Ͷ�������Ҫ�ж�ADDR��1
	I2C_Cmd(IIC_NUM, ENABLE);

	//3������Ҫд���eeprom�ڲ��洢���ĵ�ַ
	I2C_SendData(IIC_NUM,(uint8_t)readAddr);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//3.1�����ݱ����ͳ�ȥ�����յ�ACK���ͻ����EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(2);		
	}

	//1������iic��ʼ�ź�
	I2C_GenerateSTART(IIC_NUM, ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1�����EV5�¼�(SB)�����־(����ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2������eeprom�ĵ�ַ�Ͷ�����
	I2C_Send7bitAddress(IIC_NUM, (device_addr|IIC_DEVICE_ADDRESS_READ), I2C_Direction_Receiver);//��ʱ��TXEλ��0����ʾ���ݼĴ����ǿ�
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1�����յ�ACK(���յ��ӻ���NACK��ƥ����ADDR����1)��Ȼ�����EV6�¼�(ADDR��1)�Ͳ���EV8�¼�(TXEλ��1����ʾ���ݼĴ���Ϊ��)
	while(!I2C_CheckEvent(IIC_NUM, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	  	if((iicTimeout--) == 0) 
			iic_timeout_userCallBack(1);
	}

	//�������������һ���ֽ�����
	{
		//�ȴ�EV7�¼���SR1�Ĵ�����RXNE����1����ʾ�������ݼĴ����ǿ�
		iicTimeout = I2CT_FLAG_TIMEOUT;
		while(I2C_CheckEvent(IIC_NUM, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((iicTimeout--) == 0)
				iic_timeout_userCallBack(2);
		} 

		*pBuffer = I2C_ReceiveData(IIC_NUM);//��RXNE����ʾ�������ݼĴ���Ϊ��

		I2C_AcknowledgeConfig(IIC_NUM, DISABLE);//���ͷ�Ӧ���ź�
		I2C_GenerateSTOP(IIC_NUM, ENABLE);//����ֹͣ�ź�
	}

	return ret;
}


#if 1
//100kHZ��iic�ٶȣ�5us����
static void soft_delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

static void soft_iic_start(void)
{
	SOFT_IIC_SDA_H();
	SOFT_IIC_SCL_H();//��ʱ���ߺ�������ȫ�����ߣ����䴦�ڿ���״̬
	soft_delay(30);//at24c02����4.7us
	
	SOFT_IIC_SDA_L();//SDA������ͣ������½��أ�������ʼ�ź�
	soft_delay(30);//at24c02����4us
	
	SOFT_IIC_SCL_L();//SCL������ͣ�����ʱ����
	soft_delay(1);//at24c02����0.1us(TAA:ʱ�ӱ�͵����������Ч),���4.5us������Ҫ��SDA���ߣ��ٷ�����Ч����
}

static void soft_iic_stop(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/

	SOFT_IIC_SDA_L();
	SOFT_IIC_SCL_L();
	soft_delay(40);//at24c02Ϊ�˴�ack�Ĳ��Σ��ӳپ�һ��
	
	SOFT_IIC_SCL_H();//SCL�������
	soft_delay(1);
	
	SOFT_IIC_SDA_H();//SDA������ߣ����������أ�����ֹͣ�ź�
	soft_delay(30);//at24c02�ڿ�ʼһ���µĴ���ǰ���߱��ͷ�ʱ������4.7us
}

static void soft_iic_Ack(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/

	SOFT_IIC_SDA_L();//CPU����SDA=0��SDA�͵�ƽΪӦ���ź�
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	
	SOFT_IIC_SCL_H();//CPU����1��ʱ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(30);//��
}

static void soft_iic_NoAck(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/

	SOFT_IIC_SDA_H();//CPU����SDA=1��SDA�ߵ�ƽΪ��Ӧ���ź�	
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	
	SOFT_IIC_SCL_H();//CPU����1��ʱ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ��췢��Ӧ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(1);
}

static BOOL soft_iic_WaitAck(void)
{
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/
	BOOL ret;
	SOFT_IIC_SDA_H();//CPU�ͷ�SDA����,����at24c02��˵�Ѿ��õ�SDA��Ȩ��Ӧ���ˣ����Բ���һֱ��ʾΪ�͵�ƽ��
	soft_delay(1);//����Ҫ�ȴ�̫�ã���Ϊat24c02�Ѿ��õ�SDA��Ȩ��Ӧ���ˡ�
	//(������û���õ���Ȩ�����ã�)������������SDA��Ϊ����ģʽ���ټӸ�soft_delay();
	
	SOFT_IIC_SCL_H();//CPU����SCL=1, ��ʱ�����᷵��SDA�ߵ�ACKӦ��
	soft_delay(1);//�ڴ��ڼ䣬���Ծ����ȡӦ���źŻ��߷�Ӧ���ź�,��˵ȴ�ʱ����Ծ����١�
	if(SOFT_IIC_SDA_READ())
	{
		ret = FALSE;//�ߵ�ƽ��ʾ��Ӧ���ź�
	}
	else
	{
		ret = TRUE;//�͵�ƽ��ʾӦ���ź�
	}

	SOFT_IIC_SCL_L();//����SCL����
	soft_delay(5);//��ʱӦ�������at24c02�黹SDA��Ȩ��MCU������Ǹߵ�ƽ��Ҳ����Ϊ�͵�ƽ��
	//(������û���õ���Ȩ�����ã�)������������SDA��Ϊ���ģʽ���ټӸ�soft_delay();
	
	return ret;
}

static void soft_iic_SendByte(u8 txdata)
{
	u8 i;
	
	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/

	for(i=0;i<8;i++)
	{	
		//�л���Ч���ݵ�ƽ״̬
		if(txdata & 0x80)//�ȴ����λ����Ϊ��MSB��ʽ
		{
			SOFT_IIC_SDA_H();
		}
		else
		{
			SOFT_IIC_SDA_L();
		}
		soft_delay(30);//at24c02�ȴ���Ч���ݵ�ƽ�ȶ�����200ns����SCL�͵�ƽʱ������4.7us
		
		SOFT_IIC_SCL_H();//SCL���ߣ���ȡ��Ч����
		soft_delay(30);//at24c02��SCL�ߵ�ƽʱ������4us
		SOFT_IIC_SCL_L();//SCL������ͣ���ȡ��Ч�������

		txdata <<= 1;//�Ƶ���һ��λ

		//soft_delay(1);//SCL���ͺ���ά����Ч���ݵ�ƽһ��ʱ��,at24c02û�и�ʱ��Ҫ��
	}

		soft_delay(1);//��һ������Ϊ�ȴ�Ӧ��at24c02��Ӧ�����ʱ��������Ҫ100ns
}

static u8 soft_iic_ReadByte(void)
{
	u8 i,redata;

	/*��������ǰSCL�Ѿ��ǵ͵�ƽ��*/

	//������������SDA��Ϊ����ģʽ���ټӸ�soft_delay();
	soft_delay(25);//at24c02��SCL�͵�ƽʱ������4.7us
	
	redata = 0;
	for(i=0;i<8;i++)
	{		
		redata <<= 1;
		SOFT_IIC_SCL_H();//SCL���ߣ���ȡ��Ч����
		soft_delay(1);//at24c02������Ծ��������
		if(SOFT_IIC_SDA_READ())//SDA����ƽ��Ҫbit0����Ϊ��MSB�����ʽ
		{
			redata++;//��ǰλΪ1��redata |= 1Ҳ����
		}
		soft_delay(30);//at24c02��SCL�ߵ�ƽʱ������4us
		SOFT_IIC_SCL_L();//SCL�������
		soft_delay(30);//at24c02��SCL�͵�ƽʱ������4.7us
	}

	//������������SDA��Ϊ���ģʽ���ټӸ�soft_delay();
	
	return redata;
}

static void soft_iic_gpio_init(void)
{
	//gpio��ʼ��
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	SOFT_IIC_RCC_GPIO_CLOCK(SOFT_IIC_RCC_GPIO_PORT,ENABLE);

	//I2C_SCL�ź���
	gpio_initStruct.GPIO_Pin = SOFT_IIC_SCL_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SOFT_IIC_GPIO_PORT,&gpio_initStruct);
	
	//I2C_SDA������
	gpio_initStruct.GPIO_Pin = SOFT_IIC_SDA_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//��©���
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SOFT_IIC_GPIO_PORT,&gpio_initStruct);

	/* ��λI2C�����ϵ������豸������ģʽ */
	SOFT_IIC_SCL_H();
	SOFT_IIC_SDA_H();

}

DEV_STATUS soft_iic_CheckDevice(u8 device_addr)
{
	soft_iic_gpio_init();

	soft_iic_start();

	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_WRITE);

	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(0);
		
		/* ʧ�ܺ��мǷ���I2C����ֹͣ�ź� */
		soft_iic_stop();
		return DEV_ERR;
	}

	soft_iic_stop();

	return DEV_OK;
}

DEV_STATUS soft_iic_write(u8 device_addr, u32 writeAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;
	
	/* ��1��������I2C���������ź� */
	soft_iic_start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_WRITE);

	/* ��3�����ȴ�ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(0);
	}
	
	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	soft_iic_SendByte((u8)writeAddr);

	/* ��5�����ȴ�ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(1);
	}

	/* ��6������ʼд������ */
	soft_iic_SendByte(*pBuffer);

	/* ��7�����ȴ�ACK*/
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(2);
	}

	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	soft_iic_stop();

	return ret;
}

DEV_STATUS soft_iic_read(u8 device_addr, u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;

	/* ��1��������I2C���������ź� */
	soft_iic_start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_WRITE);

	/* ��3�����ȴ�ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(0);
	}

	/* ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ */
	soft_iic_SendByte((u8)readAddr);

	/* ��5�����ȴ�ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(1);
	}

	/* ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ���� */
	soft_iic_start();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_READ);

	/* ��8�����ȴ�ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(2);
	}
	
	/* ��9����ѭ����ȡ���� */
	//�������������һ���ֽ�����
	{
		*pBuffer = soft_iic_ReadByte();

		soft_iic_NoAck();
	}

	/* ����ִ�гɹ�������I2C����ֹͣ�ź� */
	soft_iic_stop();
	
	return ret;
}

#endif



