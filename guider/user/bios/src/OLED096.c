#include "common.h"
#include "OLED_font.h"

/*
    OLED096-IIC控制：128*64像素
*/

#ifdef USE_OLED_LIB

#define OLED_MODE 	0
#define SIZE 		8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  

#define	RCC_APB2Periph_OLED	RCC_APB2Periph_GPIOB
#define	OLED_SCL_GPIO_Port	GPIOB
#define	OLED_SCL_Pin		GPIO_Pin_6

#define	OLED_SDA_GPIO_Port	GPIOB
#define	OLED_SDA_Pin		GPIO_Pin_7	
				   

#define OLED_SCLK_Clr() GPIO_ResetBits(OLED_SCL_GPIO_Port, OLED_SCL_Pin)
#define OLED_SCLK_Set() GPIO_SetBits(OLED_SCL_GPIO_Port, OLED_SCL_Pin)//SCL


#define OLED_SDIN_Clr() GPIO_ResetBits(OLED_SDA_GPIO_Port, OLED_SDA_Pin)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(OLED_SDA_GPIO_Port, OLED_SDA_Pin)
 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
	IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
	Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
	Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
	IIC_Stop();
}
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		Write_IIC_Data(dat);
	}
	else 
	{
		Write_IIC_Command(dat);
	}
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(fill_Data,1);
		}
	}
}


/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~7		 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>Max_Column-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
	}
	else 
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		OLED_WR_Byte(F6x8[c][i],OLED_DATA);	
	}
}
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}
			else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){x=0;y+=2;}
		j++;
	}
}
//显示汉字
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
		adder+=1;
	}	
	OLED_Set_Pos(x,y+1);	
	for(t=0;t<16;t++)
	{	
		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
		adder+=1;
	}					
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
} 

//初始化SSD1306					    
void OLED_Init(void)
{ 	
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_OLED, ENABLE);	 
	GPIO_InitStructure.GPIO_Pin = OLED_SCL_Pin | OLED_SDA_Pin;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//速度50MHz
 	GPIO_Init(OLED_SCL_GPIO_Port, &GPIO_InitStructure);	  
 	GPIO_SetBits(OLED_SCL_GPIO_Port, OLED_SCL_Pin | OLED_SDA_Pin);	
	
	vTaskDelay(800);
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_Clear();
}

#else
#define PAGE_ALL    8
#define COLUMN_ALL  128

#define	OLED_GPIO_RCC(r,s)				RCC_APB2PeriphClockCmd(r,s)			
#define	OLED_GPIO_CLOCK					RCC_APB2Periph_GPIOB
#define OLED_GPIO_PORT					GPIOB
#define OLED_SCL_PIN					GPIO_Pin_6
#define	OLED_SDA_PIN					GPIO_Pin_7

#define	OLED_RCC(r,s)					RCC_APB1PeriphClockCmd(r,s)
#define	OLED_CLOCK						RCC_APB1Periph_I2C1
#define	OLED_IIC_NUM					I2C1

#if 1
#define OLED_SCL_H()					GPIO_SetBits(OLED_GPIO_PORT, OLED_SCL_PIN)
#define OLED_SCL_L()					GPIO_ResetBits(OLED_GPIO_PORT, OLED_SCL_PIN)
#define OLED_SDA_H()					GPIO_SetBits(OLED_GPIO_PORT, OLED_SDA_PIN)
#define OLED_SDA_L()					GPIO_ResetBits(OLED_GPIO_PORT, OLED_SDA_PIN)
#define OLED_SDA_READ()					GPIO_ReadInputDataBit(OLED_GPIO_PORT, OLED_SDA_PIN)
#endif//soft iic macro

/*等待超时时间*/
#define FLAG_TIMEOUT         ((uint32_t)0x1000)
static __IO u32 Timeout = FLAG_TIMEOUT;

//iic读写方向
#define DEVICE_ADDRESS_WRITE	0 
#define	DEVICE_ADDRESS_READ		1

void OLED096_write_command(uc8 command);

uc8 OLED_init_cmd[] = {
    0xAE,//关闭显示

    0xD5,//设置时钟分频因子，震荡频率
    0x80,//[3:0],分频因子;[7:4],震荡频率

	0xA8,//设置驱动电路
	0x3F,//默认0x3F(1/64)

    0xD3,//设置显示偏移
    0x00,//默认为0

	0x40,//设置显示开始行[5:0],行数.

    0x8D,//电荷泵设置
    0x14,//bit，开启/关闭
	
    0x20,//设置内存地址模式
    0x02,//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;

    0xA1,//段重定义设置,bit0:0,0->0; 1,0->127;
    
	0xC8,//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数

	0xDA,//设置COM硬件引脚配置
	0x12,//[5:4]配置

	0x81,//对比度设置
    0xEF,//1~255;默认0X7F (亮度设置,越大越亮)

    0xD9,//设置预充电周期
    0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;

    0xDB,//设置VCOMH 电压倍率
    0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)

    0xA6,//设置显示方式;bit0:0,正常显示;1,反相显示;

    0xAF,//开启显示 
};
	

static void timeout_userCallBack(u8 errorCode)
{
	printf("timeout! errorCode = %d",errorCode);
	while(1);
}

static void OLED096_IO_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef initStruct;
    MEM_SET(&initStruct,0,sizeof(GPIO_InitTypeDef));

    OLED_GPIO_RCC(OLED_GPIO_CLOCK,ENABLE);

    //I2C_SCL信号线
    initStruct.GPIO_Pin = OLED_SCL_PIN;
    initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//开漏输出
    initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(OLED_GPIO_PORT,&initStruct);
	
    //I2C_SDA数据线
    initStruct.GPIO_Pin = OLED_SDA_PIN;
    initStruct.GPIO_Mode = GPIO_Mode_AF_OD;	//开漏输出
    initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(OLED_GPIO_PORT,&initStruct);    
	
}


static void OLED096_config_init(void)
{
	I2C_InitTypeDef	iic_initStruct;
	MEM_SET(&iic_initStruct,0,sizeof(iic_initStruct));
	
	OLED_RCC(OLED_CLOCK,ENABLE);

	iic_initStruct.I2C_ClockSpeed = 100000;//100k模式
	iic_initStruct.I2C_Mode = I2C_Mode_I2C;//iic模式(不需要在此区分主从模式)
	iic_initStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	iic_initStruct.I2C_OwnAddress1 = 0x0A;//与IIC器件地址不一样即可(主机也需要有自己的iic地址)
	iic_initStruct.I2C_Ack = I2C_Ack_Enable;//发送完地址或数据，会自动等待应答信号(硬件实现)
	iic_initStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7位地址

	I2C_Init(OLED_IIC_NUM,&iic_initStruct);

}

static void OLED096_command_init(void)
{
    u8 i;
    for(i=0;i<ARRAY_SIZE(OLED_init_cmd);i++)
    {
        OLED096_write_command(OLED_init_cmd[i]);
    }
}

void OLED096_init(void)
{
    OLED096_IO_init();
    OLED096_config_init();

	vTaskDelay(10);
	OLED096_command_init();
}

static void OLED096_write_command(uc8 command)
{
    //1、产生iic起始信号
    I2C_GenerateSTART(OLED_IIC_NUM,ENABLE);
    //1.1、检测EV5事件(SB)并清标志(无需ACK)
    Timeout = FLAG_TIMEOUT;
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((Timeout--) == 0)
            timeout_userCallBack(0);
    }

    //2、发送oled096的地址和写方向
    I2C_Send7bitAddress(OLED_IIC_NUM,(0x78|DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((Timeout--) == 0)
            timeout_userCallBack(1);
    }

	//3、发送一字节0x00，后面发送字节代表command
    I2C_SendData(OLED_IIC_NUM,0x00);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //3.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((Timeout--) == 0)
             timeout_userCallBack(2); 	
    }

	//4、发送一字节要写入的数据
    I2C_SendData(OLED_IIC_NUM,command);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //4.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((Timeout--) == 0)
             timeout_userCallBack(3); 	
    }

    //发送停止信号(主机主动停止发送数据，若从机返回NACK则主机被动停止发送数据)
    I2C_GenerateSTOP(OLED_IIC_NUM, ENABLE);

}

static void OLED096_write_data(uc8 data)
{
    //1、产生iic起始信号
    I2C_GenerateSTART(OLED_IIC_NUM,ENABLE);
    //1.1、检测EV5事件(SB)并清标志(无需ACK)
    Timeout = FLAG_TIMEOUT;
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((Timeout--) == 0)
            timeout_userCallBack(4);
    }

    //2、发送oled096的地址和写方向
    I2C_Send7bitAddress(OLED_IIC_NUM,(0x78|DEVICE_ADDRESS_WRITE),I2C_Direction_Transmitter);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //2.1、先收到ACK(若收到从机的NACK或不匹配则ADDR不置1)，然后产生EV6事件(ADDR置1)和产生EV8事件(TXE位置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((Timeout--) == 0)
            timeout_userCallBack(5);
    }

	//3、发送一字节0x40，后面发送字节代表data
    I2C_SendData(OLED_IIC_NUM,0x40);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //3.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((Timeout--) == 0)
             timeout_userCallBack(6); 	
    }

	//4、发送一字节要写入的数据
    I2C_SendData(OLED_IIC_NUM,data);//这时候TXE位置0，表示数据寄存器非空
    Timeout = FLAG_TIMEOUT;
    //4.1、数据被发送出去后，等待ACK(若返回NACK，则主机不在发送数据)和产生EV8事件(TxE置1，表示数据寄存器为空)
    while(!I2C_CheckEvent(OLED_IIC_NUM,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((Timeout--) == 0)
             timeout_userCallBack(7); 	
    }

    //发送停止信号(主机主动停止发送数据，若从机返回NACK则主机被动停止发送数据)
    I2C_GenerateSTOP(OLED_IIC_NUM, ENABLE);

}

void OLED096_full_test(void)
{
    u8 page_num,column;
	u8 column_set = 0;

	for(page_num=0;page_num<PAGE_ALL;page_num++)
    {
        OLED096_write_command(0xb0+page_num);

		OLED096_write_command(0x00|UINT8_BREAK_LO(column_set));//列地址低位
        OLED096_write_command(0x10|UINT8_BREAK_HI(column_set));//列地址高位

		for(column=0;column<COLUMN_ALL;column++)
        {
            OLED096_write_data(0xff);//因为上面的当前page的column高低位设置后，这里每次自动增加列地址cloumn
        }

	}

}

void OLED096_clear_test(void)
{
    u8 page_num,column;
	u8 column_set = 0;

	for(page_num=0;page_num<PAGE_ALL;page_num++)
    {
        OLED096_write_command(0xb0+page_num);

		OLED096_write_command(0x00|UINT8_BREAK_LO(column_set));//列地址低位
        OLED096_write_command(0x10|UINT8_BREAK_HI(column_set));//列地址高位

		for(column=0;column<COLUMN_ALL;column++)
        {
            OLED096_write_data(0x00);//因为上面的当前page的column高低位设置后，这里每次自动增加列地址cloumn
        }

	}

}

#if 1
static void OLED096_SoftIIC_write_command(uc8 command);
static void OLED096_SoftIIC_IO_init(void)
{
	//gpio初始化
	GPIO_InitTypeDef initStruct;
    MEM_SET(&initStruct,0,sizeof(GPIO_InitTypeDef));

    OLED_GPIO_RCC(OLED_GPIO_CLOCK,ENABLE);

    //I2C_SCL信号线
    initStruct.GPIO_Pin = OLED_SCL_PIN;
    initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
    initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(OLED_GPIO_PORT,&initStruct);
	
    //I2C_SDA数据线
    initStruct.GPIO_Pin = OLED_SDA_PIN;
    initStruct.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
    initStruct.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(OLED_GPIO_PORT,&initStruct);    

	/* 复位I2C总线上的所有设备到待机模式 */
	OLED_SCL_H();
	OLED_SDA_H();
}

static void OLED096_SoftIIC_command_init(void)
{
    u8 i;
    for(i=0;i<ARRAY_SIZE(OLED_init_cmd);i++)
    {
        OLED096_SoftIIC_write_command(OLED_init_cmd[i]);
    }

}

void OLED096_SoftIIC_init(void)
{
    OLED096_SoftIIC_IO_init();
	OLED096_SoftIIC_command_init();
}

static void OLED096_SoftIIC_star(void)
{
    OLED_SDA_H();
	delay_timer_us(1);
    OLED_SCL_H();
	delay_timer_us(1);
	OLED_SDA_L();
	delay_timer_us(5);//至少0.6us再拉低SCL线
	OLED_SCL_L();
}

static void OLED096_SoftIIC_write(u8 data)
{
    u8 i;

    for(i=0;i<8;i++)
    {
		if(data & 0x80)
		{
			OLED_SDA_H();
		}
		else
		{
			OLED_SDA_L();
		}
		delay_timer_us(1+4);//至少100ns才拉高SCL线 + 100k速率的周期10us,5us的低电平
		OLED_SCL_H();
		delay_timer_us(5);//100k速率的周期为10us，5us的高电平
		OLED_SCL_L();
		//delay_timer_us(1);//SCL拉低后的SDA线保持时间至少0us
		
		data <<= 1;
	}



}

static BOOL OLED096_SoftIIC_WaitAck(void)
{
	BOOL retAck;
	
	OLED_SDA_H();//释放SDA线，等待从机ack
	delay_timer_us(5);
	OLED_SCL_H();
	delay_timer_us(5);
	if(!OLED_SDA_READ())//读取从机应答
	{
		retAck = TRUE;
	}
	else
	{
		retAck = FALSE;
	}
	OLED_SCL_L();

	return retAck;
}

static void OLED096_SoftIIC_stop(void)
{
	OLED_SDA_L();
	OLED_SCL_L();
	delay_timer_us(5);
	OLED_SCL_H();
	delay_timer_us(5);
	OLED_SDA_H();
}

static void OLED096_SoftIIC_write_command(uc8 command)
{
	OLED096_SoftIIC_star();
	
	OLED096_SoftIIC_write(0x78|DEVICE_ADDRESS_WRITE);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(0);
	}
	
	OLED096_SoftIIC_write(0x00);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(1);
	}
	
	OLED096_SoftIIC_write(command);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(2);
	}

	OLED096_SoftIIC_stop();
}

static void OLED096_SoftIIC_write_data(uc8 data)
{
	OLED096_SoftIIC_star();
	
	OLED096_SoftIIC_write(0x78|DEVICE_ADDRESS_WRITE);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(3);
	}
	
	OLED096_SoftIIC_write(0x40);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(4);
	}
	
	OLED096_SoftIIC_write(data);
	if(!OLED096_SoftIIC_WaitAck())
	{
		timeout_userCallBack(5);
	}

	OLED096_SoftIIC_stop();
}

void OLED096_SoftIIC_full_test(void)
{
    u8 page_num,column;
	u8 column_set = 0;

	for(page_num=0;page_num<PAGE_ALL;page_num++)
    {
        OLED096_SoftIIC_write_command(0xb0+page_num);

		OLED096_SoftIIC_write_command(0x00|UINT8_BREAK_LO(column_set));//列地址低位
        OLED096_SoftIIC_write_command(0x10|UINT8_BREAK_HI(column_set));//列地址高位

		for(column=0;column<COLUMN_ALL;column++)
        {
            OLED096_SoftIIC_write_data(0xff);//因为上面的当前page的column高低位设置后，这里每次自动增加列地址cloumn
        }

	}

}

void OLED096_SoftIIC_clear_test(void)
{
    u8 page_num,column;
	u8 column_set = 0;

	for(page_num=0;page_num<PAGE_ALL;page_num++)
    {
        OLED096_SoftIIC_write_command(0xb0+page_num);

		OLED096_SoftIIC_write_command(0x00|UINT8_BREAK_LO(column_set));//列地址低位
        OLED096_SoftIIC_write_command(0x10|UINT8_BREAK_HI(column_set));//列地址高位

		for(column=0;column<COLUMN_ALL;column++)
        {
            OLED096_SoftIIC_write_data(0x00);//因为上面的当前page的column高低位设置后，这里每次自动增加列地址cloumn
        }

	}

}

#endif//soft iic

#endif
