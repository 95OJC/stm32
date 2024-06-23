#include "common.h"

/*
	1��д������Ҫ�ȴ�flash���С�
	����flash��Ҫ�ȴ�flash���С�
	
	2�������ݲ���Ҫ�ȴ�flash���С�

	3��д���ַ����Ҫ��256�ֽڶ��䣬ֻҪȷ��Ŀ��洢��Ԫʱ����״̬���ɡ�
	���ԡ���ַ x��ִ��ҳд��ָ��󣬷�����200���ֽ����ݺ���ֹͨѶ����һ����
	ִ��ҳд��ָ��ӡ���ַ (x+200)����ʼд�� 200 ���ֽ�Ҳ�ǣ�û������� (С�� 256 ����)
	ֻ����ʵ��Ӧ�������ڻ���������Ԫ�� 4KB��һ�㶼������Ϊ��λ
	���ж�д���������˽⣬��ѧϰ���ǵġ�FLASH �ļ�ϵͳ����ص����ӡ�
*/


/*SPI_FLASH �������� */
/* W25Q64�ͺţ�����IDΪEF��flash�ͺ�IDΪ0x4017*/
/* W25Q128�ͺţ�����IDΪEF��flash�ͺ�IDΪ0x4018*/
#define W25X_ReadStatusReg 			0x05//��״̬�Ĵ���
#define W25X_WriteEnable 			0x06//д����
#define W25X_WriteDisable 			0x04//д��ֹ
#define W25X_ReadData 				0x03//��
#define W25X_FastReadData 			0x0B//���ٶ�
#define W25X_PageProgram 			0x02//ҳд��
#define W25X_SectorErase 			0x20//sector��������4KB����С��λ�Ĳ���
#define W25X_BlockErase 			0xD8//block�����64KB��1������16������
#define W25X_ChipErase 				0xC7//flash����оƬ����8M��8M��128����
#define W25X_DeviceID 				0xAB//����1�ֽڣ�flash�ͺ�ID(��8λ) 
#define W25X_ManufactDeviceID 		0x90//����2�ֽڣ�����ID + flash�ͺ�ID(��8λ) 
#define W25X_JedecDeviceID 			0x9F//����3�ֽڣ�����ID + flash�ͺ�ID(��8λ)  + flash�ͺ�ID(��8λ)  

/*
flash read status register��8bit
	s7:SRP
	s6:(R)
	s5:TB
	s4:BP2
	s3:BP1
	s2:BP0
	s1:WEL����1��ʾдʹ��
	s0:BUSY����1��ʾflashоƬ����æ״̬���������ڶ��ڲ��Ĵ洢������С�������������д�롱�Ĳ���
*/
#define RDSR_WIP					0x01//BUSY��־
#define RDSR_WEL					0x02//дʹ�ܱ�־


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


//bolck��СΪ64K����128�����8M��flash
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

//ȡĳһ�������е�������������������
//sector��СΪ4K,��16�����һ���С��ÿ��ֻ��256�ֽڵ�ҳд�룬�����Ҫ��Ӧ��ַ�ֶ��ҳд�롣
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

//��flash��״̬�Ĵ������Ѵ����ָ������ø�ֵΪ״̬�Ĵ���8λ��ֵ
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

//��RDSR_WIP����1��ʾдʹ��
//ÿ��д���ݺ����Ҫ���ã�Ȼ��ѭ���ȴ���״̬��æ��
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

//��RDSR_WEL����1��ʾflashоƬ����æ״̬���������ڶ��ڲ��Ĵ洢������С�������������д�롱�Ĳ���
//ÿ�η���д�����д��ֹ���������Ҫ���ã�Ȼ��ѭ���ȴ���״̬��ɣ�
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

	while(!is_spi_flash_write_enable());//�ȴ�flashдʹ��״̬���
	
	return ret;
}

#if 1
/*
	����������ָ��ǰ������Ҫ�ȷ��͡�дʹ�ܡ�ָ�������������ָ���
	ͨ����ȡ�Ĵ���״̬�ȴ���������������ϡ�
*/
DEV_STATUS spi_flash_sector_erase(u32 addr)
{
	BOOL ret = TRUE;
    u8 cmd[4];

	cmd[0] = W25X_SectorErase;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//�ȴ�flash���У��ȴ�������ϣ�
	
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

	write_enable_spi_flash();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//�ȴ�flash����
	
	return ret ? DEV_OK : DEV_ERR;
}

//����0~15��num��������
//ע�ⷢ�Ͳ�����ַʱ��λ��ǰ��������������ָ��ʱע������ĵ�ַҪ���뵽4KB
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

	write_enable_spi_flash();//д����

    SPI_FLASH_CONNECT();
    ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
    SPI_FLASH_DISCONNECT();	

	while(is_spi_flash_busy());//�ȴ�flash����
	
	return ret ? DEV_OK : DEV_ERR;
}

/*
	FLASH оƬ�����ݶ�ȡҪ�򵥵ö࣬ʹ�ö�ȡָ�Read Data�����ɡ�������ָ��
���뼰Ҫ������ʼ��ַ��FLASH оƬ�ͻᰴ��ַ�����ķ�ʽ���ش洢��������ݣ�
��ȡ��������û�����ƣ�ֻҪû��ֹͣͨѶ��FLASH оƬ�ͻ�һֱ�������ݡ�
*/
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


//д������ǰ���Ȳ���������
//�����͵����ݳ��� 256 ������Ḳ��ǰ�淢�͵����ݡ�
BOOL spi_flash_write_page(u32 addr, u8 *txBuf, u32 size)
{
    BOOL ret = TRUE;
    u8 cmd[4];

	if(size > FLASH_PAGE_SIZE)//���ƴ���size����256�ֽ�
	{
	   size = FLASH_PAGE_SIZE;
	   printf("SPI_FLASH_PageWrite too large!\r\n"); 
	} 

	cmd[0] = W25X_PageProgram;
	cmd[1] = (u8)(addr >> 16);
	cmd[2] = (u8)(addr >> 8);
	cmd[3] = (u8)addr;

	write_enable_spi_flash();//д����

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_WRITE(txBuf,size);
	SPI_FLASH_DISCONNECT();

	while(is_spi_flash_busy());//�ȴ�flash����
	
	ret &= spi_flash_program_data_chk(addr,txBuf,size);

	while(is_spi_flash_busy());//�ȴ�flash����
	
	return ret;
}


//д������ǰ���Ȳ���������
//�����͵����ݳ��� 256 ������Ḳ��ǰ�淢�͵����ݡ�
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

	write_enable_spi_flash();//д����

	SPI_FLASH_CONNECT();
	ret &= SPI_FLASH_WRITE(&cmd[0],sizeof(cmd));
	ret &= SPI_FLASH_WRITE_2(txBuf,size);
	SPI_FLASH_DISCONNECT();

	while(is_spi_flash_busy());//�ȴ�flash����
	
	return ret;
}

/*
	����Ҫд�벻���������ݣ�ֱ�ӵ��á�ҳд�롱�����������ر𷽱㣬��������
	�����Ļ����ϱ�д�ˡ�����������д�롱�ĺ�����
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


