#ifndef __iic_h_
#define	__iic_h_


DEV_STATUS iic_init(void);
DEV_STATUS iic_ByteWrite(u8 device_addr, u32 writeAddr, u8 *pBuffer);
DEV_STATUS iic_ByteRead(u8 device_addr, u32 readAddr, u8 *pBuffer);

#if 1
DEV_STATUS soft_iic_CheckDevice(u8 device_addr);
DEV_STATUS soft_iic_write(u8 device_addr, u32 writeAddr, u8 *pBuffer);
DEV_STATUS soft_iic_read(u8 device_addr, u32 readAddr, u8 *pBuffer);
#endif


#endif	//__iic_h


