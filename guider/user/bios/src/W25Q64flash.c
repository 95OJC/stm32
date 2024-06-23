#include "common.h"

/*
	1、写数据需要等待flash空闲。
	擦除flash需要等待flash空闲。
	
	2、读数据不需要等待flash空闲。

	3、写入地址并不要求按256字节对其，只要确认目标存储单元时擦除状态即可。
	若对“地址 x”执行页写入指令后，发送了200个字节数据后终止通讯，下一次再
	执行页写入指令，从“地址 (x+200)”开始写入 200 个字节也是，没有问题的 (小于 256 均可)
	只是在实际应用中由于基本擦除单元是 4KB，一般都以扇区为单位
	进行读写，想深入了解，可学习我们的“FLASH 文件系统”相关的例子。
*/


/*SPI_FLASH 常用命令 */
/* W25Q64型号：厂商ID为EF，flash型号ID为0x4017*/
/* W25Q128型号：厂商ID为EF，flash型号ID为0x4018*/
#define W25X_ReadStatusReg 			0x05//读状态寄存器
#define W25X_WriteEnable 			0x06//写允许
#define W25X_WriteDisable 			0x04//写禁止
#define W25X_ReadData 				0x03//读
#define W25X_FastReadData 			0x0B//快速读
#define W25X_PageProgram 			0x02//页写入
#define W25X_SectorErase 			0x20//sector扇区擦除4KB，最小单位的擦除
#define W25X_BlockErase 			0xD8//block块擦除64KB，1个块有16个扇区
#define W25X_ChipErase 				0xC7//flash整个芯片擦除8M，8M有128个块
#define W25X_DeviceID 				0xAB//返回1字节：flash型号ID(低8位) 
#define W25X_ManufactDeviceID 		0x90//返回2字节：厂商ID + flash型号ID(低8位) 
#define W25X_JedecDeviceID 			0x9F//返回3字节：厂商ID + flash型号ID(高8位)  + flash型号ID(低8位)  

/*
flash read status register：8bit
	s7:SRP
	s6:(R)
	s5:TB
	s4:BP2
	s3:BP1
	s2:BP0
	s1:WEL，置1表示写使能
	s0:BUSY，置1表示flash芯片处于忙状态，可能正在对内部的存储矩阵进行“擦除”或“数据写入”的操作
*/
#define RDSR_WIP					0x01//BUSY标志
#define RDSR_WEL					0x02//写使能标志


#define SPI_FLASH_NUM   1//controller
#define SPI_FLASH_CS 	0//cs
#define	SPI_FLASH_INIT(p)				spi_init(p,SPI_FLASH_NUM)	
#define SPI_FLASH_CONNECT()     		spi_cs_connect(SPI_FLASH_CS,SPI_FLASH_NUM)
#define SPI_FLASH_DISCONNECT()  		spi_cs_disconnect(SPI_FLASH_CS,SPI_FLASH_NUM)
#define	SPI_FLASH_WRITE(txbuf,size) 	spi_write(txbuf,size,SPI_FLASH_NUM)
#define	SPI_FLASH_WRITE_2(txbuf,size) 	spi_write_2(txbuf,size,SPI_FLASH_NUM)
#define	SPI_FLASH_READ(rxbuf,size) 		spi_read(rxbuf,size,SPI_FLASH_NUM)


#define FLASH_BLOCK_0_BASE 	(0x00000000)
#define FLASH_BLOCK_1_BASE 	(0x00010000)
#define FLASH_BLOCK_2_BASE 	(0x00020000)
#define FLASH_BLOCK_3_BASE 	(0x00030000)
#define FLASH_BLOCK_4_BASE 	(0x00040000)
#define FLASH_BLOCK_5_BASE 	(0x00050000)
#define FLASH_BLOCK_6_BASE 	(0x00060000)
#define FLASH_BLOCK_7_BASE 	(0x00070000)
#define FLASH_BLOCK_8_BASE 	(0x00080000)
#define FLASH_BLOCK_9_BASE 	(0x00090000)
//..... FLASH_BLOCK_128_BASE (0x007f0000)

typedef struct _FLASH_ADDR{
	u32 dwStartAddr;//Sector start address
	u32	dwEndAddr;//Sector end address
}FLASH_ADDR;


//bolck大小为64K，共128个组成8M的flash
static const FLASH_ADDR g_spi_flash_block_tbl[FLASH_BLOCK_COUNT]={
{FLASH_BLOCK_0_BASE,0x0000ffff},//block 0
{FLASH_BLOCK_1_BASE,0x0001ffff},//block 1
{FLASH_BLOCK_2_BASE,0x0002ffff},//block 2
{FLASH_BLOCK_3_BASE,0x0003ffff},//block 3
{FLASH_BLOCK_4_BASE,0x0004ffff},//block 4
{FLASH_BLOCK_5_BASE,0x0005ffff},//block 5
{FLASH_BLOCK_6_BASE,0x0006ffff},//block 6
{FLASH_BLOCK_7_BASE,0x0007ffff},//block 7
{FLASH_BLOCK_8_BASE,0x0008ffff},//block 8
{FLASH_BLOCK_9_BASE,0x0009ffff},//block 9
//......{0x007f0000,0x007fffff},	//block 127
};

//取某一个块其中的所有扇区单独作数组
//sector大小为4K,共16个组成一块大小，每次只能256字节的页写入，因此需要对应地址分多次页写入。
static const FLASH_ADDR g_spi_flash_sector_tbl[FLASH_SECTOR_COUNT]={
{FLASH_BLOCK_0_BASE+0x0000,FLASH_BLOCK_0_BASE+0x0fff},//sector 0
{FLASH_BLOCK_0_BASE+0x1000,FLASH_BLOCK_0_BASE+0x1fff},//sector 1
{FLASH_BLOCK_0_BASE+0x2000,FLASH_BLOCK_0_BASE+0x2fff},//sector 2
{FLASH_BLOCK_0_BASE+0x3000,FLASH_BLOCK_0_BASE+0x3fff},//sector 3
{FLASH_BLOCK_0_BASE+0x4000,FLASH_BLOCK_0_BASE+0x4fff},//sector 4
{FLASH_BLOCK_0_BASE+0x5000,FLASH_BLOCK_0_BASE+0x5fff},//sector 5
{FLASH_BLOCK_0_BASE+0x6000,FLASH_BLOCK_0_BASE+0x6fff},//sector 6
{FLASH_BLOCK_0_BASE+0x7000,FLASH_BLOCK_0_BASE+0x7fff},//sector 7
{FLASH_BLOCK_0_BASE+0x8000,FLASH_BLOCK_0_BASE+0x8fff},//sector 8
{FLASH_BLOCK_0_BASE+0x9000,FLASH_BLOCK_0_BASE+0x9fff},//sector 9
{FLASH_BLOCK_0_BASE+0xa000,FLASH_BLOCK_0_BASE+0xafff},//sector 10
{FLASH_BLOCK_0_BASE+0xb000,FLASH_BLOCK_0_BASE+0xbfff},//sector 11
{FLASH_BLOCK_0_BASE+0xc000,FLASH_BLOCK_0_BASE+0xcfff},//sector 12
{FLASH_BLOCK_0_BASE+0xd000,FLASH_BLOCK_0_BASE+0xdfff},//sector 13
{FLASH_BLOCK_0_BASE+0xe000,FLASH_BLOCK_0_BASE+0xefff},//sector 14
{FLASH_BLOCK_0_BASE+0xf000,FLASH_BLOCK_0_BASE+0xffff},//sector 15
};

//读flash的状态寄存器，把传入的指针解引用赋值为状态寄存器8位的值
static BOOL ReadStatusRreg(u8 *rdsr)
{
    BOOL ret = TRUE;
    u8 cmd;

    cmd = W25X_ReadStatusReg;

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd,1);
    ret &= SPI_FLASH_READ(rdsr,1);
    SPI_FLASH_DISCONNECT();

    return ret;

}

//宏RDSR_WIP，置1表示写使能
//每次写数据后就需要调用，然后循环等待该状态清忙！
static BOOL is_spi_flash_busy(void)
{
	BOOL ret;
    u8 rdsr = 0;

    ReadStatusRreg(&rdsr);
	if(rdsr & RDSR_WIP)
	{	ret = TRUE;}
	else
	{	ret = FALSE;}

    return ret;
}

//宏RDSR_WEL，置1表示flash芯片处于忙状态，可能正在对内部的存储矩阵进行“擦除”或“数据写入”的操作
//每次发送写允许或写禁止的命令就需要调用，然后循环等待该状态完成！
static BOOL is_spi_flash_write_enable(void)
{
	BOOL ret;
    u8 rdsr = 0;

	ReadStatusRreg(&rdsr);
	if(rdsr & RDSR_WEL)
	{	ret = TRUE;}
	else
	{	ret = FALSE;}
	
	return ret;
}

static BOOL write_enable_spi_flash(void)
{
	BOOL ret = TRUE;
	u8 cmd = W25X_WriteEnable;

	SPI_FLASH_CONNECT();
	ret = SPI_FLASH_WRITE(&cmd,1);
	SPI_FLASH_DISCONNECT();

	while(!is_spi_flash_write_enable());//等待flash写使能状态完成
	
	return ret;
}

#if 1
/*
	在扇区擦除指令前，还需要先发送“写使能”指令，发送扇区擦除指令后，
	通过读取寄存器状态等待扇区擦除操作完毕。
*/
DEV_STATUS spi_flash_sector_erase(u32 addr)
{
	BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_SectorErase;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//写允许

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//等待flash空闲（等待擦除完毕）
	
	return ret ? DEV_OK : DEV_ERR;
}

DEV_STATUS spi_flash_block_erase(u32 addr)
{
	BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_BlockErase;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//写允许

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//等待flash空闲
	
	return ret ? DEV_OK : DEV_ERR;
}

//对于0~15的num扇区擦除
//注意发送擦除地址时高位在前，调用扇区擦除指令时注意输入的地址要对齐到4KB
DEV_STATUS spi_flash_sector_erase_no(u32 no)
{
	u32 addr;
	BOOL ret = TRUE;

	if(no >= FLASH_SECTOR_COUNT)
		return DEV_ERR;

	addr = g_spi_flash_sector_tbl[no].dwStartAddr;

	ret &= spi_flash_sector_erase(addr);

	return ret ? DEV_OK : DEV_ERR;
}

DEV_STATUS spi_flash_block_erase_no(u32 no)
{
	u32 addr;
	BOOL ret = TRUE;

	if(no >= FLASH_BLOCK_COUNT)
		return DEV_ERR;

	addr = g_spi_flash_block_tbl[no].dwStartAddr;

	ret &= spi_flash_block_erase(addr);

	return ret ? DEV_OK : DEV_ERR;
}

DEV_STATUS spi_flash_chip_erase(void)
{
	BOOL ret = TRUE;
    u8 cmd[1];

	cmd[0] = W25X_ChipErase;

	write_enable_spi_flash();//写允许

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//等待flash空闲
	
	return ret ? DEV_OK : DEV_ERR;
}

/*
	FLASH 芯片的数据读取要简单得多，使用读取指令“Read Data”即可。发送了指令
编码及要读的起始地址后，FLASH 芯片就会按地址递增的方式返回存储矩阵的内容，
读取的数据量没有限制，只要没有停止通讯，FLASH 芯片就会一直返回数据。
*/
DEV_STATUS spi_flash_read(u32 addr, u8 *rxBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_ReadData;
	cmd[1] = (u8)(addr >> 16);//23~16位
	cmd[2] = (u8)(addr >> 8);//15~8位
	cmd[3] = (u8)addr;//7~0位

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_READ(rxBuf,size);
	SPI_FLASH_DISCONNECT();

	return ret ? DEV_OK : DEV_ERR;
}

DEV_STATUS spi_flash_read_fast(u32 addr, u8 *rxBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[5];

	cmd[0] = W25X_FastReadData;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;
	cmd[4] = FLASH_DUMMY;

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_READ(rxBuf,size);
	SPI_FLASH_DISCONNECT();

	return ret ? DEV_OK : DEV_ERR;
}

static DEV_STATUS spi_flash_program_data_chk(u32 addr, u8 *des, u32 size)
{
    u32 i;
    BOOL ret = TRUE;
    u8 Buf[FLASH_PAGE_SIZE];

	if(size > FLASH_PAGE_SIZE)
	{
		size = FLASH_PAGE_SIZE;
		printf("SPI_FLASH_PageWrite too large!\r\n"); 
	}
	ret &= spi_flash_read(addr,&Buf[0],size);

	if(ret)
	{
		for(i=0;i<size;i++)
		{
			if(Buf[i] != des[i])
			{
				printf("i = %d Buf[i] = %d des[i] = %d\r\n",i,Buf[i],des[i]);
				return DEV_ERR;
			}
		}
	}

	return ret ? DEV_OK : DEV_ERR;
}


//写入数据前，先擦除！！！
//若发送的数据超出 256 个，则会覆盖前面发送的数据。
BOOL spi_flash_write_page(u32 addr, u8 *txBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	if(size > FLASH_PAGE_SIZE)//限制传入size超过256字节
	{
	   size = FLASH_PAGE_SIZE;
	   printf("SPI_FLASH_PageWrite too large!\r\n"); 
	} 

	cmd[0] = W25X_PageProgram;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//写允许

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_WRITE(txBuf,size);
	SPI_FLASH_DISCONNECT();

	while(is_spi_flash_busy());//等待flash空闲
	
	ret &= spi_flash_program_data_chk(addr,txBuf,size);

	while(is_spi_flash_busy());//等待flash空闲
	
	return ret;
}


//写入数据前，先擦除！！！
//若发送的数据超出 256 个，则会覆盖前面发送的数据。
BOOL spi_flash_write_page2(u32 addr, const char *txBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	if(size > FLASH_PAGE_SIZE)
	{
	   size = FLASH_PAGE_SIZE;
	   printf("SPI_FLASH_PageWrite too large!\r\n"); 
	} 

	cmd[0] = W25X_PageProgram;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//写允许

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_WRITE_2(txBuf,size);
	SPI_FLASH_DISCONNECT();

	while(is_spi_flash_busy());//等待flash空闲
	
	return ret;
}

/*
	常常要写入不定量的数据，直接调用“页写入”函数并不是特别方便，所以我们
	在它的基础上编写了“不定量数据写入”的函数。
*/
BOOL spi_flash_write_page_random(u32 addr, const char *txBuf, u32 size)
{
	BOOL ret = TRUE;
	u32 Page_cnt;
	u32 page_remainder;
	u32 addr_add = addr;
	Page_cnt = size / FLASH_PAGE_SIZE;
	page_remainder = size % FLASH_PAGE_SIZE;

	if(Page_cnt)
	{
		while(Page_cnt--)
		{
			spi_flash_write_page2(addr_add, txBuf, FLASH_PAGE_SIZE);
			addr_add += FLASH_PAGE_SIZE;
			txBuf += FLASH_PAGE_SIZE;
		}

		if(page_remainder)
		{
			spi_flash_write_page2(addr_add, txBuf, page_remainder);
		}
	}
	else
	{
		if(page_remainder)
		{
			spi_flash_write_page2(addr_add, txBuf, page_remainder);
		}

	}
	
	return ret;
}

#endif

DEV_STATUS spi_flash_read_did(u8 *did)
{
	DEV_STATUS ret = DEV_OK;
	u8 cmd[4];

	cmd[0] = W25X_DeviceID;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_READ(did,1);
	SPI_FLASH_DISCONNECT();

	return ret;
}

DEV_STATUS spi_flash_read_mid(u8 *mid)
{
	DEV_STATUS ret = DEV_OK;
	u8 cmd[4];

	cmd[0] = W25X_ManufactDeviceID;
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_READ(mid,2);
	SPI_FLASH_DISCONNECT();

	return ret;
}

DEV_STATUS spi_flash_read_jid(u8 *jid)
{
	DEV_STATUS ret = DEV_OK;
	u8 cmd[1];

	cmd[0] = W25X_JedecDeviceID;

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_READ(jid,3);
	SPI_FLASH_DISCONNECT();

	return ret;
}

DEV_STATUS spi_flash_init(void)
{
	SPI_CFG_INFO pInfo;
	
	pInfo.baudrRate = 12000000;
    pInfo.LSB = MSB_FIRST;
    pInfo.ckpl = CKPL_H;
    pInfo.ckph = CKPH_F;
    pInfo.dataWidth = DATAS_8;
    pInfo.cs = SPI_CS0;
    pInfo.flag = 0;

	return SPI_FLASH_INIT(&pInfo);
}


