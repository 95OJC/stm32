/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "common.h"//"W25Q64flash.h"

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define	SPI_FLASH	3

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	//int result;
	u8 jid[3];
	u16 sflash_id;

	switch (pdrv) {
	case SPI_FLASH:
		spi_flash_read_jid(jid);
		sflash_id = (u16)(jid[1] << 8) | (u16)jid[2]; 
		if(sflash_id == W25X_FLASH_ID)
		{
			stat &= ~STA_NOINIT;
		}
		else
		{
			stat = STA_NOINIT;
		}
		return stat;
		
	case ATA :
		//result = ATA_disk_status();

		// translate the reslut code here

		return stat;

	case MMC :
		//result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
	//int result;
#if 1
	switch (pdrv) {
	case SPI_FLASH:
		spi_flash_init();
		delay_timer_ms(100);
		stat = disk_status(SPI_FLASH);
		return stat;
	
	case ATA :
		//result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
#else
	spi_flash_init();
	delay_timer_ms(100);
	stat &= ~STA_NOINIT;
	return stat;
#endif
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_PARERR;
	//int result;
	DEV_STATUS sflash_ret = DEV_OK;

	switch (pdrv) {
	case SPI_FLASH:
		/*
			需要注意：
			1、sector指的是0-2047，即使那个扇区，从0开始计算起，W25Q64一个扇区为4096字节，因此输入地址为sector*4096
			2、count是disk_ioctl函数的GET_BLOCK_SIZE(单次读写操作擦除扇区个数)，填1就是每次只读count(=1)*4096字节
		*/
		sflash_ret &= spi_flash_read(sector*4096,buff,count*4096);
		if(sflash_ret == DEV_OK)
		{
			res = RES_OK;
		}
		else
		{
			printf("FATFS:spi_flash_read fail!\r\n");
			res = RES_PARERR;
		}
		return res;
	
	case ATA :
		// translate the arguments here

		//result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_PARERR;
	//int result;
	DEV_STATUS sflash_ret = DEV_OK;

	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}
	
	switch (pdrv) {
	case SPI_FLASH:
		/*
			需要注意：
			1、sector指的是0-2047，即使那个扇区，从0开始计算起，W25Q64一个扇区为4096字节，因此输入地址为sector*4096
			2、count是disk_ioctl函数的GET_BLOCK_SIZE(单次读写操作擦除扇区个数)，填1就是每次只写count(=1)*4096字节
		*/
		sflash_ret &= spi_flash_sector_erase(sector*4096);
		sflash_ret &= spi_flash_write_page_random(sector*4096,buff,count*4096);
		if(sflash_ret == DEV_OK)
		{
			res =  RES_OK;
		}
		else
		{
			res =  RES_PARERR;
			printf("FATFS:spi_flash_erase/_program fail!\r\n");
		}
		return res;
	
	case ATA :
		// translate the arguments here

		//result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_PARERR;
	//int result;

	switch (pdrv) {
	case SPI_FLASH:
		switch (cmd) {
        /* 扇区数量:128块*16扇区=2048(8M) */
        case GET_SECTOR_COUNT:
          *(DWORD * )buff = 2048;		
        break;
        /* 扇区大小  */
        case GET_SECTOR_SIZE :
          *(WORD * )buff = 4096;
        break;
        /* 单次读写操作擦除扇区个数 */
        case GET_BLOCK_SIZE :
          *(DWORD * )buff = 1;
        break;        
      	}
      	res = RES_OK;
		return res;	
	case ATA :

		// Process of the command for the ATA drive

		return res;

	case MMC :

		// Process of the command for the MMC/SD card

		return res;

	case USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}
#endif

__weak DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}


