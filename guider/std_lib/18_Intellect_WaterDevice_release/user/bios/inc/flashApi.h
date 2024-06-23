#ifndef __flashApi_h
#define	__flashApi_h


DEV_STATUS spi_flash_init(void);

DEV_STATUS spi_flash_read_jid(u8 *jid);
DEV_STATUS spi_flash_read_mid(u8 *mid);
DEV_STATUS spi_flash_chip_erase(void);
BOOL spi_flash_block_erase_no(u32 no);
BOOL spi_flash_sector_erase_no(u32 no);

DEV_STATUS spi_flash_read(u32 addr, u8 *rxBuf, u32 size);
DEV_STATUS spi_flash_read_fast(u32 addr, u8 *rxBuf, u32 size);


#endif	//__flashApi_h


