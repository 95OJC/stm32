#ifndef __AT24C02eeprom_h
#define	__AT24C02eeprom_h


DEV_STATUS eeprom_init(void);
DEV_STATUS eeprom_write(u32 writeAddr, u8 *pBuffer);
DEV_STATUS eeprom_read(u32 readAddr, u8 *pBuffer);

DEV_STATUS AT24C02eeprom_init(void);
DEV_STATUS AT24C02eeprom_write(u32 writeAddr, u8 *pBuffer);
DEV_STATUS AT24C02eeprom_read(u32 readAddr, u8 *pBuffer);


#endif//__AT24C02eeprom_h


