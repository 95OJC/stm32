#ifndef __W25Q64flash_h
#define	__W25Q64flash_h

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


#endif	//__W25Q64flash_h

