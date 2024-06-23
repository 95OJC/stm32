#include "common.h"


/*-----------------------------------------------------------------
该文件主要功能是验证iic。

硬件iic主发送器的步骤：
1、产生iic起始信号。
1.1、检测EV5事件(SB)并清标志(无需ACK)。
2、发送eeprom的地址和写方向。
2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)。
3、发送要写入的eeprom存储数据的地址(这时候TXE位置0，表示数据寄存器非空)。
3.1、数据被发送出去后，等待ACK和产生EV8事件(TxE置1)。
4、发送一字节要写入的数据(这时候TXE位置0，表示数据寄存器非空)。
4.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)。
5、循环4和4.1。
6、发送停止信号(主机主动停止发送数据，若从机返回NACK则主机被动停止发送数据)。
7、假如使能了 I2C 中断，以上所有事件产生时，都会产生 I2C 中断信号，进入同一个中断服务函数，到 I2C 中断服务程序后，再通过检查寄存器位来判断是哪一个事件。


要点：
1、每次发送地址和数据后，等待接收ACK或NACK过程是硬件负责的。
2、若有从机应答(ACK)，硬件自动把相应的事件产生(标志置1)。


与uart发送1个字节函数的区别：
1、串口发送只有1条TX线(硬件自动生成UART协议)，只需向8位的DR寄存器写1个字节即可。
2、iic发送需2条线(同时需要按照IIC协议)，首先发送起始信号(等待ACK)，然后发送地
   址(等待ACK)，再然后往8位的DR寄存器发送数据(等待ACK)......最后发送停止信号(不需等待ACK)。
3、因此相比uart，iic需要有起始信号、哪个IIC从设备的地址信号、停止信号，而且每个信号都需要等待ACK(除停止信号以外)。
4、若换了从设备，需要修改设备地址和读写地址。


***停止信号前面的每一步都以SCL被拉低为结束(下一步先动SDA，因为SCL是低电平不影响)，最后停止信号将会把SCL和SDA都拉高。
***每一步都是需要控制SCL一个脉冲，及时把SDA切换为输入和输出模式。
------------------------------------------------------------------*/

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
static __IO u32 iicTimeout = I2CT_FLAG_TIMEOUT;

//iic读写方向
#define IIC_DEVICE_ADDRESS_WRITE	0 
#define	IIC_DEVICE_ADDRESS_READ		1


static void iic_timeout_userCallBack(u8 errorCode)
{
	printf("iic_timeout! errorCode = %d",errorCode);
	while(1);
}

static void iic_gpio_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	IIC_RCC_GPIO_CLOCK(IIC_RCC_GPIO_PORT,ENABLE);

	//I2C_SCL信号线
	gpio_initStruct.GPIO_Pin = IIC_SCL_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//开漏输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(IIC_GPIO_PORT,&gpio_initStruct);
	
	//I2C_SDA数据线
	gpio_initStruct.GPIO_Pin = IIC_SDA_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//开漏输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(IIC_GPIO_PORT,&gpio_initStruct);
	
}

static void iic_config_init(void)
{
	I2C_InitTypeDef	iic_initStruct;
	MEM_SET(&iic_initStruct,0,sizeof(iic_initStruct));
	
	IIC_RCC_CLOCK(IIC_RCC_PORT,ENABLE);

	iic_initStruct.I2C_ClockSpeed = 400000;//400k快速模式
	iic_initStruct.I2C_Mode = I2C_Mode_I2C;//iic模式(不需要在此区分主从模式)
	iic_initStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	iic_initStruct.I2C_OwnAddress1 = 0x0A;//与IIC器件地址不一样即可(主机也需要有自己的iic地址)
	iic_initStruct.I2C_Ack = I2C_Ack_Enable;//发送完地址或数据，会自动等待应答信号(硬件实现)
	iic_initStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7位地址

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

	//1、产生iic起始信号
	I2C_GenerateSTART(IIC_NUM,ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1、检测EV5事件(SB)并清标志(无需ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2、发送eeprom的地址和写方向
	I2C_Send7bitAddress(IIC_NUM,(device_addr|IIC_DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(1);
	}

	//3、发送要写入的eeprom内部存储器的地址
	I2C_SendData(IIC_NUM,(uint8_t)writeAddr);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//3.1、数据被发送出去后，等待ACK和产生EV8事件(TXE位置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(2);		
	}

	//4、发送一字节要写入的数据
	I2C_SendData(IIC_NUM,*pBuffer);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//4.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(3); 	
	}

	// ......可以重复4写入不同数据，但是地址需要改变。
	
	//发送停止信号(主机主动停止发送数据，若从机返回NACK则主机被动停止发送数据)
	I2C_GenerateSTOP(IIC_NUM, ENABLE);

	return ret;
}


DEV_STATUS iic_ByteRead(u8 device_addr, u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;
	
	//1、产生iic起始信号
	I2C_GenerateSTART(IIC_NUM,ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1、检测EV5事件(SB)并清标志(无需ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2、发送eeprom的地址和写方向
	I2C_Send7bitAddress(IIC_NUM,(device_addr|IIC_DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(1);
	}

	//通过重新设置PE位清除EV6事件(ADDR置0),为了第二次的发送eeprom的地址和读方向需要判断ADDR置1
	I2C_Cmd(IIC_NUM, ENABLE);

	//3、发送要写入的eeprom内部存储器的地址
	I2C_SendData(IIC_NUM,(uint8_t)readAddr);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//3.1、数据被发送出去后，若收到ACK，就会产生EV8事件(TXE位置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(2);		
	}

	//1、产生iic起始信号
	I2C_GenerateSTART(IIC_NUM, ENABLE);
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//1.1、检测EV5事件(SB)并清标志(无需ACK)
	while(!I2C_CheckEvent(IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((iicTimeout--) == 0)
			iic_timeout_userCallBack(0);
	}

	//2、发送eeprom的地址和读方向
	I2C_Send7bitAddress(IIC_NUM, (device_addr|IIC_DEVICE_ADDRESS_READ), I2C_Direction_Receiver);//这时候TXE位置0，表示数据寄存器非空
	iicTimeout = I2CT_FLAG_TIMEOUT;
	//2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)
	while(!I2C_CheckEvent(IIC_NUM, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	  	if((iicTimeout--) == 0) 
			iic_timeout_userCallBack(1);
	}

	//该括号里面接收一个字节数据
	{
		//等待EV7事件，SR1寄存器的RXNE被置1，表示接收数据寄存器非空
		iicTimeout = I2CT_FLAG_TIMEOUT;
		while(I2C_CheckEvent(IIC_NUM, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((iicTimeout--) == 0)
				iic_timeout_userCallBack(2);
		} 

		*pBuffer = I2C_ReceiveData(IIC_NUM);//清RXNE，表示接收数据寄存器为空

		I2C_AcknowledgeConfig(IIC_NUM, DISABLE);//发送非应答信号
		I2C_GenerateSTOP(IIC_NUM, ENABLE);//发送停止信号
	}

	return ret;
}


#if 1
//100kHZ的iic速度，5us左右
static void soft_delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}

static void soft_iic_start(void)
{
	SOFT_IIC_SDA_H();
	SOFT_IIC_SCL_H();//将时钟线和数据线全部拉高，让其处于空闲状态
	soft_delay(30);//at24c02至少4.7us
	
	SOFT_IIC_SDA_L();//SDA最后拉低，产生下降沿，产生起始信号
	soft_delay(30);//at24c02至少4us
	
	SOFT_IIC_SCL_L();//SCL最后拉低，拉低时钟线
	soft_delay(1);//at24c02至少0.1us(TAA:时钟变低到数据输出有效),最多4.5us，就需要把SDA拉高，再发送有效数据
}

static void soft_iic_stop(void)
{
	/*进入这里前SCL已经是低电平了*/

	SOFT_IIC_SDA_L();
	SOFT_IIC_SCL_L();
	soft_delay(40);//at24c02为了错开ack的波形，延迟久一点
	
	SOFT_IIC_SCL_H();//SCL最后拉高
	soft_delay(1);
	
	SOFT_IIC_SDA_H();//SDA最后拉高，产生上升沿，产生停止信号
	soft_delay(30);//at24c02在开始一个新的传输前总线被释放时间至少4.7us
}

static void soft_iic_Ack(void)
{
	/*进入这里前SCL已经是低电平了*/

	SOFT_IIC_SDA_L();//CPU驱动SDA=0，SDA低电平为应答信号
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	
	SOFT_IIC_SCL_H();//CPU产生1个时钟
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(30);//？
}

static void soft_iic_NoAck(void)
{
	/*进入这里前SCL已经是低电平了*/

	SOFT_IIC_SDA_H();//CPU驱动SDA=1，SDA高电平为非应答信号	
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	
	SOFT_IIC_SCL_H();//CPU产生1个时钟
	soft_delay(1);//在此期间，可以尽快发出应答信号或者非应答信号,因此等待时间可以尽量少。
	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(1);
}

static BOOL soft_iic_WaitAck(void)
{
	/*进入这里前SCL已经是低电平了*/
	BOOL ret;
	SOFT_IIC_SDA_H();//CPU释放SDA总线,对于at24c02来说已经拿到SDA线权而应答了，所以波形一直显示为低电平。
	soft_delay(1);//不需要等待太久，因为at24c02已经拿到SDA线权而应答了。
	//(若上面没有拿到线权才配置？)这里可添加配置SDA线为输入模式，再加个soft_delay();
	
	SOFT_IIC_SCL_H();//CPU驱动SCL=1, 此时器件会返回SDA线的ACK应答
	soft_delay(1);//在此期间，可以尽快读取应答信号或者非应答信号,因此等待时间可以尽量少。
	if(SOFT_IIC_SDA_READ())
	{
		ret = FALSE;//高电平表示非应答信号
	}
	else
	{
		ret = TRUE;//低电平表示应答信号
	}

	SOFT_IIC_SCL_L();//最后把SCL拉低
	soft_delay(5);//此时应答结束，at24c02归还SDA线权给MCU，因此是高电平，也可能为低电平。
	//(若上面没有拿到线权才配置？)这里可添加配置SDA线为输出模式，再加个soft_delay();
	
	return ret;
}

static void soft_iic_SendByte(u8 txdata)
{
	u8 i;
	
	/*进入这里前SCL已经是低电平了*/

	for(i=0;i<8;i++)
	{	
		//切换有效数据电平状态
		if(txdata & 0x80)//先传最高位，因为是MSB格式
		{
			SOFT_IIC_SDA_H();
		}
		else
		{
			SOFT_IIC_SDA_L();
		}
		soft_delay(30);//at24c02等待有效数据电平稳定至少200ns，而SCL低电平时间至少4.7us
		
		SOFT_IIC_SCL_H();//SCL拉高，获取有效数据
		soft_delay(30);//at24c02的SCL高电平时间至少4us
		SOFT_IIC_SCL_L();//SCL最后拉低，获取有效数据完毕

		txdata <<= 1;//移到下一低位

		//soft_delay(1);//SCL拉低后，需维持有效数据电平一定时间,at24c02没有该时间要求
	}

		soft_delay(1);//下一个函数为等待应答，at24c02的应答输出时间至少需要100ns
}

static u8 soft_iic_ReadByte(void)
{
	u8 i,redata;

	/*进入这里前SCL已经是低电平了*/

	//这里可添加配置SDA线为输入模式，再加个soft_delay();
	soft_delay(25);//at24c02的SCL低电平时间至少4.7us
	
	redata = 0;
	for(i=0;i<8;i++)
	{		
		redata <<= 1;
		SOFT_IIC_SCL_H();//SCL拉高，读取有效数据
		soft_delay(1);//at24c02这里可以尽快读数据
		if(SOFT_IIC_SDA_READ())//SDA最后电平需要bit0，因为是MSB传输格式
		{
			redata++;//当前位为1，redata |= 1也可以
		}
		soft_delay(30);//at24c02的SCL高电平时间至少4us
		SOFT_IIC_SCL_L();//SCL最后拉低
		soft_delay(30);//at24c02的SCL低电平时间至少4.7us
	}

	//这里可添加配置SDA线为输出模式，再加个soft_delay();
	
	return redata;
}

static void soft_iic_gpio_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef gpio_initStruct;
	MEM_SET(&gpio_initStruct,0,sizeof(GPIO_InitTypeDef));

	SOFT_IIC_RCC_GPIO_CLOCK(SOFT_IIC_RCC_GPIO_PORT,ENABLE);

	//I2C_SCL信号线
	gpio_initStruct.GPIO_Pin = SOFT_IIC_SCL_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SOFT_IIC_GPIO_PORT,&gpio_initStruct);
	
	//I2C_SDA数据线
	gpio_initStruct.GPIO_Pin = SOFT_IIC_SDA_PIN;
	gpio_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	gpio_initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(SOFT_IIC_GPIO_PORT,&gpio_initStruct);

	/* 复位I2C总线上的所有设备到待机模式 */
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
		
		/* 失败后，切记发送I2C总线停止信号 */
		soft_iic_stop();
		return DEV_ERR;
	}

	soft_iic_stop();

	return DEV_OK;
}

DEV_STATUS soft_iic_write(u8 device_addr, u32 writeAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;
	
	/* 第1步：发起I2C总线启动信号 */
	soft_iic_start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_WRITE);

	/* 第3步：等待ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(0);
	}
	
	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	soft_iic_SendByte((u8)writeAddr);

	/* 第5步：等待ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(1);
	}

	/* 第6步：开始写入数据 */
	soft_iic_SendByte(*pBuffer);

	/* 第7步：等待ACK*/
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(2);
	}

	/* 命令执行成功，发送I2C总线停止信号 */
	soft_iic_stop();

	return ret;
}

DEV_STATUS soft_iic_read(u8 device_addr, u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret = DEV_OK;

	/* 第1步：发起I2C总线启动信号 */
	soft_iic_start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_WRITE);

	/* 第3步：等待ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(0);
	}

	/* 第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址 */
	soft_iic_SendByte((u8)readAddr);

	/* 第5步：等待ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(1);
	}

	/* 第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据 */
	soft_iic_start();

	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位 */
	soft_iic_SendByte(device_addr|IIC_DEVICE_ADDRESS_READ);

	/* 第8步：等待ACK */
	if(!soft_iic_WaitAck())
	{
		iic_timeout_userCallBack(2);
	}
	
	/* 第9步：循环读取数据 */
	//该括号里面接收一个字节数据
	{
		*pBuffer = soft_iic_ReadByte();

		soft_iic_NoAck();
	}

	/* 命令执行成功，发送I2C总线停止信号 */
	soft_iic_stop();
	
	return ret;
}

#endif



