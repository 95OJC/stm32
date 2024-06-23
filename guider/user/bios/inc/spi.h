#ifndef	__spi_h
#define __spi_h

#define	SPI_RCC_GPIO_CLOCK(r,s)			RCC_APB2PeriphClockCmd(r,s)			
#define	SPI_RCC_CS_PORT					RCC_APB2Periph_GPIOC
#define	SPI_RCC_SCK_PORT				RCC_APB2Periph_GPIOA
#define	SPI_RCC_MISO_PORT				RCC_APB2Periph_GPIOA
#define	SPI_RCC_MOSI_PORT				RCC_APB2Periph_GPIOA
#define SPI_CS_PORT						GPIOC
#define SPI_SCK_PORT					GPIOA
#define SPI_MISO_PORT					GPIOA
#define SPI_MOSI_PORT					GPIOA
#define SPI_CS_PIN						GPIO_Pin_0
#define	SPI_SCK_PIN						GPIO_Pin_5
#define SPI_MISO_PIN					GPIO_Pin_6
#define	SPI_MOSI_PIN					GPIO_Pin_7

#define	SPI_RCC_CLOCK(r,s)				RCC_APB2PeriphClockCmd(r,s)
#define	SPI_RCC_PORT					RCC_APB2Periph_SPI1

#define	SPI_NUM							SPI1


#endif	//__spi_h


