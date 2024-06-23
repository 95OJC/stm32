#include "common.h"

/*-----------------------------------------------------------------
换新主板移植底层注意事项：
1、查看电路图的IO口是否有变化，如led、key只需要底层修改gpio.h的宏即可，如uart需要修改。。。
2、上层不用修改，因为上层只需要知道led1、2和key1、2之类传下去即可，同时底层返回led状态上来。(因此需要做到上层传到底层的led_num，底层根据num配置初始化led驱动和开关状态返回上层)。


//这里声明DevMain函数为了不添加上层头文件，而使用上层的DevMain函数(实现上层和底层的程序分层)。
//底层调用该函数初始化上层应用，只有进去该函数里面就是上层的世界了。
//而上层想要调用底层函数，必须在biosApi.h声明底层函数，因为上层包含了该头文件，因此上层可以用(biosApi.h不需要被common.h包含，而是被lovgdev.h包含)。
------------------------------------------------------------------*/

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

static uint8_t *compiler_data = __DATE__;
static uint8_t *compiler_time = __TIME__;

FATFS fs; /* FatFs 文件系统对象 */
FIL fnew; /* 文件对象 */
FRESULT res_flash; /* 文件操作结果,管理运行文件的错误，有19种错误，在ff.h有定义 */
UINT fnum; /* 文件成功读写数量 */
char fpath[100]; /* 保存当前扫描路径 */
char readbuffer[512]; 

#define	SFLASH_WR_BUFF	256
u8 sflash_Wrandom[SFLASH_WR_BUFF]; 
u8 sflash_Rrandom[SFLASH_WR_BUFF]; 


/*
	使用文件读写操作之前都必须使用 f_open 函数打开文件，开始的时候读写指针是在文件起始位
	置的，马上写入数据的话会覆盖原来文件内容。这里，我们使用 f_lseek 函数定位到文件末尾位
	置。f_lseek 函数有两个形参，第一个参数为文件对象指针，第二个参数为需要定位的字节数，
	这个字节数是相对文件起始位置的，比如设置为 0，则将文件读写指针定位到文件起
	始位置。
*/
static FRESULT miscellaneous(void)
{
	DIR dir;
	FATFS *pfs;
	DWORD fre_clust, fre_sect, tot_sect;

	printf("\n*************** 设备信息获取 ***************\r\n");
	/* 获取设备信息和空簇大小 */
	res_flash = f_getfree("3:", &fre_clust, &pfs);
	/* 计算得到总的扇区个数和空扇区个数 */
	tot_sect = (pfs->n_fatent - 2) * pfs->csize;
	fre_sect = fre_clust * pfs->csize;
	/* 打印信息 (4096 字节/扇区) */
	printf("》设备总空间：%10lu KB。\n》可用空间： %10lu KB。\n",tot_sect *4, fre_sect *4);

	printf("\n******** 文件定位和格式化写入功能测试 ********\r\n");
	res_flash = f_open(&fnew, "3:FatFs 功能测试文件.txt", FA_CREATE_ALWAYS|FA_WRITE|FA_READ );
	res_flash = f_write(&fnew,"欢迎使用野火 STM32 开发板",50,&fnum);
	if ( res_flash == FR_OK )
	{
		/* 文件定位，定位到文件的末尾 */
		res_flash = f_lseek(&fnew,f_size(&fnew)-1);
		if (res_flash == FR_OK)
		{
			/* 格式化写入，参数格式类似 printf 函数 */
			f_printf (&fnew,"\n在原来文件新添加一行内容\n");
			f_printf (&fnew,"》设备总空间：%10lu KB。\n》可用空间;%10lu KB。\n",tot_sect *4, fre_sect *4);

			/* 文件定位到文件起始位置 */
			res_flash = f_lseek(&fnew,0);
			/* 读取文件所有内容到缓存区 */
			res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
			if (res_flash == FR_OK)
			{
				printf("》文件内容：\n%s\n",readbuffer);
			}
		}
		f_close(&fnew);
		
		printf("\n********** 目录创建和重命名功能测试 **********\r\n");
		/* 尝试打开目录 */
		res_flash=f_opendir(&dir,"3:TestDir");
		if (res_flash!=FR_OK)
		{
			/* 打开目录失败，就创建目录 */
			res_flash=f_mkdir("3:TestDir");
		}
		else
		{
			/* 如果目录已经存在，关闭它 */
			res_flash=f_closedir(&dir);
			/* 删除文件 */
			f_unlink("3:TestDir/testdir.txt");
		}

		if(res_flash==FR_OK)
		{
			/* 重命名并移动文件 */
			res_flash=f_rename("3:FatFs 功能测试文件.txt","3:TestDir/testdir.txt");
			if (res_flash==FR_OK)
			{
				printf("》重命名并移动文件操作成功\n");
			}
			else
			{
				printf("》重命名并移动文件操作失败：%d\n",res_flash);
			}

		}
	}
	else
	{
		printf("!! 打开文件失败：%d\n",res_flash);
		printf("!! 或许需要再次运行“FatFs 移植与读写测试”工程\n");
	}

	return res_flash;
}


/*
	f_stat 函数用于获取文件的属性，有两个形参，第一个参数为文件路径，第二个参数为返回指向
	文件信息结构体变量的指针。文件信息结构体变量包含文件的大小、最后修改时间和日期、文件
	属性、短文件名以及长文件名等信息。
*/
static FRESULT file_check(void)
{
	static FILINFO fno;
	/* 获取文件信息，必须确保文件存在 */
	res_flash=f_stat("3:TestDir/testdir.txt",&fno);
	if (res_flash==FR_OK)
	{
		printf("\n*************** “testdir.txt”文件信息：***************\n");
		printf("》文件大小: %ld(字节)\n", fno.fsize);
		printf("》时间戳: %u/%02u/%02u, %02u:%02u\n",
		(fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
		fno.ftime >> 11, fno.ftime >> 5 & 63);

		printf("》属性: %c%c%c%c%c\n\n",
			(fno.fattrib & AM_DIR) ? 'D' : '-', // 是一个目录
			(fno.fattrib & AM_RDO) ? 'R' : '-', // 只读文件
			(fno.fattrib & AM_HID) ? 'H' : '-', // 隐藏文件
			(fno.fattrib & AM_SYS) ? 'S' : '-', // 系统文件
			(fno.fattrib & AM_ARC) ? 'A' : '-'); // 档案文件
	}

	return res_flash;
}


/*
	scan_files 函数用来扫描指定路径下的文件。比如我们设计一个 mp3 播放器，我们需要提取 mp3
	格式文件，诸如 *.txt、.c 文件我们统统不可要的，这时我们就必须扫描路径下所有文件并把.mp3
	或 *.MP3 格式文件提取出来。这里我们不提取特定格式文件，而是把所有文件名称都通过串口
	打印出来。
*/
static FRESULT scan_files (char* path)
{
	FRESULT res; //部分在递归过程被修改的变量，不用全局变量
	FILINFO fno;
	DIR dir;
	int i;
	char *fn; // 文件名
	
	printf("\n***************** 文件扫描测试 ****************\r\n");

#if _USE_LFN
	/* 长文件名支持 */
	/* 简体中文需要 2 个字节保存一个“字”*/
	static char lfn[_MAX_LFN*2 + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

	//打开目录
	//这里的path参数是外面传进来的“3:”，f_opendir函数可以不需要传路径
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		i = strlen(path);
		for (;;)
		{
			//读取目录下的内容，再读会自动读下一个文件
			res = f_readdir(&dir, &fno);
			//为空时表示所有项目读取完毕，跳出
			if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;		
#else
			fn = fno.fname;
#endif
			//点表示当前目录，跳过
			if (*fn == '.') continue;
			//目录，递归读取
			if (fno.fattrib & AM_DIR) 
			{
				//合成完整目录
				sprintf(&path[i], "/%s", fn);
				//递归遍历
				res = scan_files(path);
				path[i] = 0;
				//打开失败，跳出循环
				if (res != FR_OK)	break;
			}
			else
			{
				printf("%s/%s\r\n", path, fn); //输出文件名
				/* 可以在这里提取特定格式的文件路径 */
			}
		}
		
	}

	return res;
}

void DevMain(void);
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//配置NVIC优先级分组为4（即7:4bit全部为抢占优先级），可以设置0~15优先级

	DBG_OPEN();
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

#if 0
	DevMain();//初始化硬件和创建多任务启动
#endif

#if 0
	pwm_test();
#endif

#if 0
	//SleepMode_test();
	//StopMode_test();
	//PVD_config_init();//配置PVD，当电压过低时，会进入中断服务函数，亮红灯
#endif

#if 0
	//iwdg_test();
	//wwdg_test();
#endif

#if 0
	//dma_m2m_test();
	//dma_m2m_interrupt_test();
	//dma_m2p_test(M2P_USART);
	//dma_m2p_interrupt_test(M2P_USART);
	//dma_p2m_test(P2M_USART);
	//dma_p2m_interrupt_test(P2M_USART);
#endif

#if 0
		extern __IO u16 AD_Value;
		float AD_getV;
		ad_init();
		while(1)
		{
			AD_getV = (float)(AD_Value*3.3)/4095;
			printf("AD_Value = %d\r\n",AD_Value);
			printf("ADC_get_V = %f\r\n",AD_getV);
			printf("\r\n");
			delay_timer_ms(500);
		}	
	
#endif

#if 0
	da_init();//打开串口debug后不能正常输出电压，暂没查原因
#endif

#if 0//test iic eeprom
	u8 test_TxBuf[10];
	MEM_SET(test_TxBuf, 1, sizeof(test_TxBuf));
	u8 test_RxBuf[10];
	MEM_SET(test_RxBuf, 0, sizeof(test_RxBuf));
	#if 1
	eeprom_init();
	eeprom_write(0,test_TxBuf);
	Delay_ojc(0x4fffff);
	eeprom_read(0,test_RxBuf);
	printf("test_RxBuf[0] = %d\r\n",test_RxBuf[0]);
	#else
	AT24C02eeprom_init();
	AT24C02eeprom_write(0xaa,test_TxBuf);
	Delay_ojc(0x4fffff);
	AT24C02eeprom_read(0xaa,test_RxBuf);
	printf("test_RxBuf[0] = %d\r\n",test_RxBuf[0]);
	#endif
#endif

#if 0//test spi flash

	DEV_STATUS ret = DEV_OK;
	u8 jid[3],mid[2],did[1];
	
	spi_flash_init();
	
	//ret = spi_flash_read(0x101010,rxbuf,sizeof(rxbuf));
	ret &= spi_flash_read_jid(jid);
	ret &= spi_flash_read_mid(mid);
	ret &= spi_flash_read_did(did);

	if(ret == DEV_OK)
	{
		printf("jid = 0x%x,0x%x,0x%x \r\n",jid[0],jid[1],jid[2]);
		printf("mid = 0x%x,0x%x\r\n",mid[0],mid[1]);
		printf("did = 0x%x\r\n",did[0]);
	}

	u16 k = 0;
	u8 w_data[10] = {1,2,3,4,5,6,7,8,9,10};
	u8 r_data[10] = {0};

	spi_flash_init();
	
	ret &= spi_flash_sector_erase_no(0);
	ret &= spi_flash_write_page(0x00000000,w_data,sizeof(w_data));
	ret &= spi_flash_read(0x00000000,r_data,sizeof(w_data));
	
	if(ret == DEV_OK)
	{
		printf("sflash W/R OK!\r\n");
		while(k != sizeof(w_data))
		{
			printf(" %d --- %d\r\n",k,r_data[k]);
			k++;
		}
	}

#endif

#if 0//测试spi flash的多次页写入
	DEV_STATUS ret = DEV_OK;
	u32 sflash_w = 0;
	u16 sflash_r = 0;

	spi_flash_init();
		
	ret &= spi_flash_sector_erase_no(0);
	while(sflash_w != sizeof(sflash_Wrandom))
	{
		sflash_Wrandom[sflash_w] = sflash_w;
		printf("W: %d --- %d\r\n",sflash_w,sflash_Wrandom[sflash_w]);
		sflash_w++;
	}
	ret &= spi_flash_write_page_random(0x00000000,sflash_Wrandom,sizeof(sflash_Wrandom));
	ret &= spi_flash_read(0x00000000,sflash_Rrandom,sizeof(sflash_Rrandom));

	if(ret == DEV_OK)
	{
		printf("\r\n sflash W/R random OK! \r\n");
		while(sflash_r != sizeof(sflash_Rrandom))
		{
			printf("R: %d --- %d\r\n",sflash_r,sflash_Rrandom[sflash_r]);
			sflash_r++;
		}
	}

#endif

#if 1//测试sflash的fatfs文件系统的挂载、格式化、读写的基本功能
	BYTE buffer[100]= {0};	/* 读缓冲区 */
	BYTE textFileBuffer[] = "新建文件系统测试文件\r\n";

	printf("\r\n****** 这是一个 SPI FLASH 文件系统实验 ******\r\n");
	//在外部 SPI Flash 挂载文件系统，文件系统挂载时会对 SPI 设备初始化
	/*
		初始化函数调用流程如下：
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/	
	res_flash = f_mount(&fs,"3:",1);


	/*----------------------- 格式化测试 -----------------*/
	/* 如果没有文件系统就格式化创建文件系统 */
	if (res_flash == FR_NO_FILESYSTEM)
	{
		printf("》FLASH 还没有文件系统，即将进行格式化...\r\n");
		res_flash=f_mkfs("3:",0,0);//格式化

		if (res_flash == FR_OK)
		{
			printf("》FLASH 已成功格式化文件系统。\r\n");
			/* 格式化成功后需要先取消挂载原来设备，再重新挂载设备 */
			res_flash = f_mount(NULL,"3:",1);
			/* 重新挂载 */
			res_flash = f_mount(&fs,"3:",1);
		}
		else
		{
			printf("《《格式化失败。(%d)》》\r\n",res_flash);
			while (1);
		}
	}
	else if(res_flash != FR_OK)
	{
		printf("！！外部 Flash 挂载文件系统失败。(%d)\r\n",res_flash);
		printf("！！可能原因：SPI Flash 初始化不成功。\r\n");
		while (1);
	}
	else
	{
		printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}


	/*----------------------- 文件系统测试：写测试 -------------------*/
	/* 打开文件，每次都以新的形式打开，属性为可写 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");
	res_flash = f_open(&fnew, "3:FatFs 读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("》打开/创建 FatFs 读写测试文件.txt 文件成功，向文件写入数据。\r\n");
		/* 将指定存储区内容写入到文件内 */
		res_flash=f_write(&fnew,textFileBuffer,sizeof(textFileBuffer),&fnum);

		if (res_flash==FR_OK)
		{
			printf("》文件写入成功，写入字节数据：%d\n",fnum);
			printf("》向文件写入的数据为：\r\n%s\r\n",textFileBuffer);
		}
		else
		{
			printf("！！文件写入失败：(%d)\n",res_flash);
		}

		/* 不再读写，关闭文件 */
		f_close(&fnew);
	}
	else
	{
		printf("！！打开/创建文件失败。(%d)\r\n",res_flash);
	}


	/*------------------- 文件系统测试：读测试 --------------------------*/
	printf("\r\n****** 即将进行文件读取测试... ******\r\n");
	res_flash = f_open(&fnew, "3:FatFs 读写测试文件.txt",FA_OPEN_EXISTING | FA_READ);
	if (res_flash == FR_OK)
	{
		printf("》打开文件成功。\r\n");
		res_flash = f_read(&fnew, buffer, sizeof(buffer), &fnum);
		if (res_flash==FR_OK)
		{
			printf("》文件读取成功, 读到字节数据：%d\r\n",fnum);
			printf("》读取得的文件数据为：\r\n%s \r\n", buffer);
		}
		else
		{
			printf("！！文件读取失败：(%d)\n",res_flash);
		}
	}
	else
	{
		printf("！！打开文件失败。(%d)\r\n",res_flash);
	}
	
	/* 不再读写，关闭文件 */
	f_close(&fnew);

	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(NULL,"3:",1);

	/* 操作完成，停机 */
	
#endif

#if 0//测试sflash的FatFs多项功能
	printf("\r\n****** 这是一个 SPI FLASH 文件系统实验 ******\r\n");

	//在外部 SPI Flash 挂载文件系统，文件系统挂载时会对 SPI 设备初始化
	/*
		初始化函数调用流程如下：
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/	
	res_flash = f_mount(&fs,"3:",1);
	if(res_flash != FR_OK)
	{
		printf("！！外部 Flash 挂载文件系统失败。(%d)\r\n",res_flash);
		printf("！！可能原因：SPI Flash 初始化不成功。\r\n");
		while (1);
	}
	else
	{
		printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}

	//FatFs多项功能测试
	res_flash = miscellaneous();//f_printf函数感觉没有起效果，以后待查！
	

	//文件信息获取
	res_flash = file_check();

	//路径扫描
	strcpy(fpath,"3:");
	scan_files(fpath);

#endif


	while(1);
}




