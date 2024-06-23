#include "common.h"

#define RDSR_WIP					0x0001//BUSY��־
#define RDSR_WEL					0x0002//дʹ�ܱ�־

#define W25X_FLASH_ID 				0XEF4017 	/* ID */
#define FLASH_SIZE 					0x800000 	/* �ܵ����� (8MByte) */
#define FLASH_BLOCK_COUNT			128			/*BLOCK����*/
#define FLASH_SECTOR_COUNT 			16 			/*SECTOR����*/
#define FLASH_PAGE_SIZE 			256    		/*Page�Ĵ�С*/
#define FLASH_DUMMY					0xAA

/* W25Q64�ͺţ�����IDΪEF��flash�ͺ�IDΪ0x4017*/
/* W25Q128�ͺţ�����IDΪEF��flash�ͺ�IDΪ0x4018*/

/*FLASH �������� */
#define W25X_WriteEnable 			0x06//д����
#define W25X_WriteDisable 			0x04//д��ֹ
#define W25X_ReadStatusReg 			0x05//��״̬�Ĵ���
#define W25X_ReadData 				0x03//��
#define W25X_FastReadData 			0x0B//���ٶ�
#define W25X_PageProgram 			0x02//ҳд
#define W25X_SectorErase 			0x20//sector����
#define W25X_BlockErase 			0xD8//block����
#define W25X_ChipErase 				0xC7//flash����
#define W25X_DeviceID 				0xAB//flash�ͺ�ID(��8λ) 
#define W25X_ManufactDeviceID 		0x90//����ID + flash�ͺ�ID(��8λ) 
#define W25X_JedecDeviceID 			0x9F//����ID + flash�ͺ�ID(��8λ)  + flash�ͺ�ID(��8λ)  

typedef struct _FLASH_ADDR{
	u32 dwStartAddr;//Sector start address
	u32	dwEndAddr;//Sector end address
}FLASH_ADDR;


#define SPI_FLASH_NUM   1//controller
#define SPI_FLASH_CS 	0//cs

#define	SPI_FLASH_INIT(p)				spi_init(p,SPI_FLASH_NUM)	
#define SPI_FLASH_CONNECT()     		spi_cs_connect(SPI_FLASH_CS,SPI_FLASH_NUM)
#define SPI_FLASH_DISCONNECT()  		spi_cs_disconnect(SPI_FLASH_CS,SPI_FLASH_NUM)
#define	SPI_FLASH_WRITE(txbuf,size) 	spi_write(txbuf,size,SPI_FLASH_NUM)
#define	SPI_FLASH_READ(rxbuf,size) 		spi_read(rxbuf,size,SPI_FLASH_NUM)

/*
	д������Ҫ�ȴ�flash���С�
	����flash��Ҫ�ȴ�flash���С�
	
	�����ݲ���Ҫ�ȴ�flash���С�
*/

//bolck��СΪ64K����128�����8M��flash
static const FLASH_ADDR g_spi_flash_block_tbl[FLASH_BLOCK_COUNT]={
{0x00000,0x0ffff},//block 0
{0x10000,0x1ffff},//block 1
{0x20000,0x2ffff},//block 2
{0x30000,0x3ffff},//block 3
{0x40000,0x4ffff},//block 4
{0x50000,0x5ffff},//block 5
{0x60000,0x6ffff},//block 6
{0x70000,0x7ffff},//block 7
{0x80000,0x8ffff},//block 8
{0x90000,0x9ffff},//block 9
//......		  //block 127
};


#define FLASH_BLOCK_1_BASE (0x10000)
//sector��СΪ4K,��16����ɿ��С
//ÿ��ֻ��256�ֽڵ�ҳд�룬�����Ҫ��Ӧ��ַ�ֶ��ҳд�롣
static const FLASH_ADDR g_spi_flash_sector_tbl[FLASH_SECTOR_COUNT]={
{FLASH_BLOCK_1_BASE+0x0000,FLASH_BLOCK_1_BASE+0x0fff},//sector 0
{FLASH_BLOCK_1_BASE+0x1000,FLASH_BLOCK_1_BASE+0x1fff},//sector 1
{FLASH_BLOCK_1_BASE+0x2000,FLASH_BLOCK_1_BASE+0x2fff},//sector 2
{FLASH_BLOCK_1_BASE+0x3000,FLASH_BLOCK_1_BASE+0x3fff},//sector 3
{FLASH_BLOCK_1_BASE+0x4000,FLASH_BLOCK_1_BASE+0x4fff},//sector 4
{FLASH_BLOCK_1_BASE+0x5000,FLASH_BLOCK_1_BASE+0x5fff},//sector 5
{FLASH_BLOCK_1_BASE+0x6000,FLASH_BLOCK_1_BASE+0x6fff},//sector 6
{FLASH_BLOCK_1_BASE+0x7000,FLASH_BLOCK_1_BASE+0x7fff},//sector 7
{FLASH_BLOCK_1_BASE+0x8000,FLASH_BLOCK_1_BASE+0x8fff},//sector 8
{FLASH_BLOCK_1_BASE+0x9000,FLASH_BLOCK_1_BASE+0x9fff},//sector 9
{FLASH_BLOCK_1_BASE+0xa000,FLASH_BLOCK_1_BASE+0xafff},//sector 10
{FLASH_BLOCK_1_BASE+0xb000,FLASH_BLOCK_1_BASE+0xbfff},//sector 11
{FLASH_BLOCK_1_BASE+0xc000,FLASH_BLOCK_1_BASE+0xcfff},//sector 12
{FLASH_BLOCK_1_BASE+0xd000,FLASH_BLOCK_1_BASE+0xdfff},//sector 13
{FLASH_BLOCK_1_BASE+0xe000,FLASH_BLOCK_1_BASE+0xefff},//sector 14
{FLASH_BLOCK_1_BASE+0xf000,FLASH_BLOCK_1_BASE+0xffff},//sector 15
};


#if 1
static BOOL spi_flash_read_status_reg(u8 *rdsr)
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

static BOOL spi_flash_is_busy(void)
{
	BOOL ret;
    u8 rdsr = 0;

    spi_flash_read_status_reg(&rdsr);
	if(rdsr & RDSR_WIP)
	{	ret = TRUE;}
	else
	{	ret = FALSE;}

    return ret;
}

static BOOL spi_flash_is_write_enable(void)
{
	BOOL ret;
    u8 rdsr = 0;

	spi_flash_read_status_reg(&rdsr);
	if(rdsr & RDSR_WEL)
	{	ret = TRUE;}
	else
	{	ret = FALSE;}
	
	return ret;
}

static BOOL spi_flash_write_enable(void)
{
	BOOL ret = TRUE;
	u8 cmd = W25X_WriteEnable;

	SPI_FLASH_CONNECT();
	ret = SPI_FLASH_WRITE(&cmd,1);
	SPI_FLASH_DISCONNECT();

	while(!spi_flash_is_write_enable());
	
	return ret;
}

static BOOL spi_flash_write_disable(void)
{
    BOOL ret = TRUE;
    u8 cmd = W25X_WriteDisable;

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd,1);
    SPI_FLASH_DISCONNECT();

    while (spi_flash_is_write_enable());

    return ret;
}

#endif

DEV_STATUS spi_flash_read(u32 addr, u8 *rxBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_ReadData;
	cmd[1] = (u8)(addr >> 16);//23~16λ
	cmd[2] = (u8)(addr >> 8);//15~8λ
	cmd[3] = (u8)addr;//7~0λ

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

static BOOL spi_flash_program_data_chk(u32 addr, u8 *des, u32 size)
{
    u32 i;
    BOOL ret = TRUE;
    u8 Buf[FLASH_PAGE_SIZE];

	if(size > FLASH_PAGE_SIZE)
		return FALSE;

	ret &= spi_flash_read(addr,&Buf[0],size);

	if(ret)
	{
		for(i=0;i<FLASH_PAGE_SIZE;i++)
		{
			if(Buf[i] != des[i])
			{
				return FALSE;
			}
		}
	}

	return ret;
}

static BOOL spi_flash_program(u32 addr, u8 *txBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_PageProgram;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	spi_flash_write_enable();//д����

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_WRITE(txBuf,size);
	SPI_FLASH_CONNECT();

	while(spi_flash_is_busy());//�ȴ�flash����
	
	ret &= spi_flash_program_data_chk(addr,txBuf,size);

	while(spi_flash_is_busy());//�ȴ�flash����
	
	return ret;
}

static BOOL spi_flash_page_program(u32 adr, u8 *pWritData, u32 size)
{
	return TRUE;
}

#if 1
static BOOL spi_flash_sector_erase(u32 addr)
{
	BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_SectorErase;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	spi_flash_write_enable();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(spi_flash_is_busy());//�ȴ�flash����
	
	return ret;
}

//����0~15��num��������
BOOL spi_flash_sector_erase_no(u32 no)
{
	u32 addr;
	BOOL ret = TRUE;

	if(no >= FLASH_SECTOR_COUNT)
		return FALSE;

	addr = g_spi_flash_sector_tbl[no].dwStartAddr;

	ret &= spi_flash_sector_erase(addr);

	return ret;
}


static BOOL spi_flash_block_erase(u32 addr)
{
	BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_BlockErase;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	spi_flash_write_enable();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(spi_flash_is_busy());//�ȴ�flash����
	
	return ret;
}

BOOL spi_flash_block_erase_no(u32 no)
{
	u32 addr;
	BOOL ret = TRUE;

	if(no >= FLASH_BLOCK_COUNT)
		return FALSE;

	addr = g_spi_flash_block_tbl[no].dwStartAddr;

	ret &= spi_flash_block_erase(addr);

	return ret;
}

DEV_STATUS spi_flash_chip_erase(void)
{
	BOOL ret = TRUE;
    u8 cmd[1];

	cmd[0] = W25X_ChipErase;

	spi_flash_write_enable();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(spi_flash_is_busy());//�ȴ�flash����
	
	return ret ? DEV_OK : DEV_ERR;
}

#endif


#if 1

//����0xEF,0x17
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

//����0xEF,0x40,0x17
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


#endif

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


