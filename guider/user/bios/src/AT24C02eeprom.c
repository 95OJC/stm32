#include "common.h"


/** EEPROM 器件地址 */
#define EEPROM_ID       0xA0

DEV_STATUS eeprom_init(void)
{
	DEV_STATUS ret;
	//可添加iic的配置，然后传入iic_init()
	ret = iic_init();

	return ret;
}

DEV_STATUS eeprom_write(u32 writeAddr, u8 *pBuffer)
{
	DEV_STATUS ret;
	
	ret = iic_ByteWrite(EEPROM_ID,writeAddr,pBuffer);
	led_init();
	led_on();
	
	return ret;
}

DEV_STATUS eeprom_read(u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret;

	ret = iic_ByteRead(EEPROM_ID,readAddr,pBuffer);
	if(*pBuffer == 8)
	{
		led_off();
	}
	
	return ret;
}

#if 1
DEV_STATUS AT24C02eeprom_init(void)
{
	DEV_STATUS ret = DEV_OK;
	
	ret = soft_iic_CheckDevice(EEPROM_ID);

	return ret;
}

DEV_STATUS AT24C02eeprom_write(u32 writeAddr, u8 *pBuffer)
{
	DEV_STATUS ret;
	
	ret = soft_iic_write(EEPROM_ID,writeAddr,pBuffer);
	
	return ret;
}

DEV_STATUS AT24C02eeprom_read(u32 readAddr, u8 *pBuffer)
{
	DEV_STATUS ret;

	ret = soft_iic_read(EEPROM_ID,readAddr,pBuffer);

	return ret;
}

#endif

