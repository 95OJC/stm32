#ifndef __W25Q64flash_h
#define	__W25Q64flash_h

#define RDSR_WIP					0x0001//BUSY标志
#define RDSR_WEL					0x0002//写使能标志


#define W25X_FLASH_ID 				0XEF4017 	/* ID */
#define FLASH_SIZE 					0x800000 	/* 总的容量 (8MByte) */
#define FLASH_BLOCK_COUNT			128			/*BLOCK数量*/
#define FLASH_SECTOR_COUNT 			16 			/*SECTOR数量*/
#define FLASH_PAGE_SIZE 			256    		/*Page的大小*/
#define FLASH_DUMMY					0xAA


/* W25Q64型号：厂商ID为EF，flash型号ID为0x4017*/
/* W25Q128型号：厂商ID为EF，flash型号ID为0x4018*/

/*FLASH 常用命令 */
#define W25X_WriteEnable 			0x06//写允许
#define W25X_WriteDisable 			0x04//写禁止
#define W25X_ReadStatusReg 			0x05//读状态寄存器
#define W25X_ReadData 				0x03//读
#define W25X_FastReadData 			0x0B//快速读
#define W25X_PageProgram 			0x02//页写
#define W25X_SectorErase 			0x20//sector擦除
#define W25X_BlockErase 			0xD8//block擦除
#define W25X_ChipErase 				0xC7//flash擦除
#define W25X_DeviceID 				0xAB//flash型号ID(低8位) 
#define W25X_ManufactDeviceID 		0x90//厂商ID + flash型号ID(低8位) 
#define W25X_JedecDeviceID 			0x9F//厂商ID + flash型号ID(高8位)  + flash型号ID(低8位)  


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


#endif	//__W25Q64flash_h

