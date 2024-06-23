#ifndef __W25Q64flash_h
#define	__W25Q64flash_h

//spi_flash参数值的宏定义
#define W25X_FLASH_ID 				0X4017 		/* ID */
#define FLASH_SIZE 					0x00800000 	/* 总的容量 (8MByte) */
#define FLASH_BLOCK_COUNT			128			/*BLOCK数量*/
#define FLASH_SECTOR_COUNT 			16 			/*SECTOR数量*/
#define FLASH_PAGE_SIZE 			256    		/*一次写的Page页大小*/
#define FLASH_DUMMY					0xAA

DEV_STATUS spi_flash_init(void);

DEV_STATUS spi_flash_read_did(u8 *did);
DEV_STATUS spi_flash_read_mid(u8 *mid);
DEV_STATUS spi_flash_read_jid(u8 *jid);

DEV_STATUS spi_flash_sector_erase(u32 addr);
DEV_STATUS spi_flash_block_erase(u32 addr);
DEV_STATUS spi_flash_chip_erase(void);
BOOL spi_flash_block_erase_no(u32 no);
BOOL spi_flash_sector_erase_no(u32 no);

DEV_STATUS spi_flash_read(u32 addr, u8 *rxBuf, u32 size);
DEV_STATUS spi_flash_read_fast(u32 addr, u8 *rxBuf, u32 size);
BOOL spi_flash_write_page(u32 addr, u8 *txBuf, u32 size);
BOOL spi_flash_write_page2(u32 addr, const char *txBuf, u32 size);
BOOL spi_flash_write_page_random(u32 addr, const char *txBuf, u32 size);



#endif	//__W25Q64flash_h

