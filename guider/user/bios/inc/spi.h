#ifndef	__spi_h
#define __spi_h

#define LSB_FIRST   0
#define MSB_FIRST   1

#define CKPL_L      0
#define CKPL_H      1

#define CKPH_F      0
#define CKPH_S      1

#define DATAS_8     7  

#define NO_HW_CS    0x00
#define SPI_CS0     0x01
#define SPI_CS1     0x02
#define SPI_CS2     0x04
#define SPI_CS3     0x08

typedef struct _SPI_CFG_INFO{
    u32 baudrRate;
    u8  LSB;
    u8  ckpl;
    u8  ckph;
    u8  dataWidth;
    u8  cs;
    u8  flag;
}SPI_CFG_INFO;

DEV_STATUS spi_init(SPI_CFG_INFO *pInfo,u8 spinum);
DEV_STATUS spi_cs_connect(u8 cs,u8 spinum);
DEV_STATUS spi_cs_disconnect(u8 cs,u8 spinum);
DEV_STATUS spi_write(u8 *txbuf, u32 size, u8 spinum);
DEV_STATUS spi_write_2(const char*txBuf, u32 size, u8 spinum);
DEV_STATUS spi_read(u8 *rxbuf, u32 size, u8 spinum);


#endif	//__spi_h


