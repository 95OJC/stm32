#include "common.h"

/*-----------------------------------------------------------------
����������ֲ�ײ�ע�����
1���鿴��·ͼ��IO���Ƿ��б仯����led��keyֻ��Ҫ�ײ��޸�gpio.h�ĺ꼴�ɣ���uart��Ҫ�޸ġ�����
2���ϲ㲻���޸ģ���Ϊ�ϲ�ֻ��Ҫ֪��led1��2��key1��2֮�ഫ��ȥ���ɣ�ͬʱ�ײ㷵��led״̬������(�����Ҫ�����ϲ㴫���ײ��led_num���ײ����num���ó�ʼ��led�����Ϳ���״̬�����ϲ�)��


//��������DevMain����Ϊ�˲�����ϲ�ͷ�ļ�����ʹ���ϲ��DevMain����(ʵ���ϲ�͵ײ�ĳ���ֲ�)��
//�ײ���øú�����ʼ���ϲ�Ӧ�ã�ֻ�н�ȥ�ú�����������ϲ�������ˡ�
//���ϲ���Ҫ���õײ㺯����������biosApi.h�����ײ㺯������Ϊ�ϲ�����˸�ͷ�ļ�������ϲ������(biosApi.h����Ҫ��common.h���������Ǳ�lovgdev.h����)��
------------------------------------------------------------------*/

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

static uint8_t *compiler_data = __DATE__;
static uint8_t *compiler_time = __TIME__;

FATFS fs; /* FatFs �ļ�ϵͳ���� */
FIL fnew; /* �ļ����� */
FRESULT res_flash; /* �ļ��������,���������ļ��Ĵ�����19�ִ�����ff.h�ж��� */
UINT fnum; /* �ļ��ɹ���д���� */
char fpath[100]; /* ���浱ǰɨ��·�� */
char readbuffer[512]; 

#define	SFLASH_WR_BUFF	256
u8 sflash_Wrandom[SFLASH_WR_BUFF]; 
u8 sflash_Rrandom[SFLASH_WR_BUFF]; 


/*
	ʹ���ļ���д����֮ǰ������ʹ�� f_open �������ļ�����ʼ��ʱ���дָ�������ļ���ʼλ
	�õģ�����д�����ݵĻ��Ḳ��ԭ���ļ����ݡ��������ʹ�� f_lseek ������λ���ļ�ĩβλ
	�á�f_lseek �����������βΣ���һ������Ϊ�ļ�����ָ�룬�ڶ�������Ϊ��Ҫ��λ���ֽ�����
	����ֽ���������ļ���ʼλ�õģ���������Ϊ 0�����ļ���дָ�붨λ���ļ���
	ʼλ�á�
*/
static FRESULT miscellaneous(void)
{
	DIR dir;
	FATFS *pfs;
	DWORD fre_clust, fre_sect, tot_sect;

	printf("\n*************** �豸��Ϣ��ȡ ***************\r\n");
	/* ��ȡ�豸��Ϣ�Ϳմش�С */
	res_flash = f_getfree("3:", &fre_clust, &pfs);
	/* ����õ��ܵ����������Ϳ��������� */
	tot_sect = (pfs->n_fatent - 2) * pfs->csize;
	fre_sect = fre_clust * pfs->csize;
	/* ��ӡ��Ϣ (4096 �ֽ�/����) */
	printf("���豸�ܿռ䣺%10lu KB��\n�����ÿռ䣺 %10lu KB��\n",tot_sect *4, fre_sect *4);

	printf("\n******** �ļ���λ�͸�ʽ��д�빦�ܲ��� ********\r\n");
	res_flash = f_open(&fnew, "3:FatFs ���ܲ����ļ�.txt", FA_CREATE_ALWAYS|FA_WRITE|FA_READ );
	res_flash = f_write(&fnew,"��ӭʹ��Ұ�� STM32 ������",50,&fnum);
	if ( res_flash == FR_OK )
	{
		/* �ļ���λ����λ���ļ���ĩβ */
		res_flash = f_lseek(&fnew,f_size(&fnew)-1);
		if (res_flash == FR_OK)
		{
			/* ��ʽ��д�룬������ʽ���� printf ���� */
			f_printf (&fnew,"\n��ԭ���ļ������һ������\n");
			f_printf (&fnew,"���豸�ܿռ䣺%10lu KB��\n�����ÿռ�;%10lu KB��\n",tot_sect *4, fre_sect *4);

			/* �ļ���λ���ļ���ʼλ�� */
			res_flash = f_lseek(&fnew,0);
			/* ��ȡ�ļ��������ݵ������� */
			res_flash = f_read(&fnew,readbuffer,f_size(&fnew),&fnum);
			if (res_flash == FR_OK)
			{
				printf("���ļ����ݣ�\n%s\n",readbuffer);
			}
		}
		f_close(&fnew);
		
		printf("\n********** Ŀ¼���������������ܲ��� **********\r\n");
		/* ���Դ�Ŀ¼ */
		res_flash=f_opendir(&dir,"3:TestDir");
		if (res_flash!=FR_OK)
		{
			/* ��Ŀ¼ʧ�ܣ��ʹ���Ŀ¼ */
			res_flash=f_mkdir("3:TestDir");
		}
		else
		{
			/* ���Ŀ¼�Ѿ����ڣ��ر��� */
			res_flash=f_closedir(&dir);
			/* ɾ���ļ� */
			f_unlink("3:TestDir/testdir.txt");
		}

		if(res_flash==FR_OK)
		{
			/* ���������ƶ��ļ� */
			res_flash=f_rename("3:FatFs ���ܲ����ļ�.txt","3:TestDir/testdir.txt");
			if (res_flash==FR_OK)
			{
				printf("�����������ƶ��ļ������ɹ�\n");
			}
			else
			{
				printf("�����������ƶ��ļ�����ʧ�ܣ�%d\n",res_flash);
			}

		}
	}
	else
	{
		printf("!! ���ļ�ʧ�ܣ�%d\n",res_flash);
		printf("!! ������Ҫ�ٴ����С�FatFs ��ֲ���д���ԡ�����\n");
	}

	return res_flash;
}


/*
	f_stat �������ڻ�ȡ�ļ������ԣ��������βΣ���һ������Ϊ�ļ�·�����ڶ�������Ϊ����ָ��
	�ļ���Ϣ�ṹ�������ָ�롣�ļ���Ϣ�ṹ����������ļ��Ĵ�С������޸�ʱ������ڡ��ļ�
	���ԡ����ļ����Լ����ļ�������Ϣ��
*/
static FRESULT file_check(void)
{
	static FILINFO fno;
	/* ��ȡ�ļ���Ϣ������ȷ���ļ����� */
	res_flash=f_stat("3:TestDir/testdir.txt",&fno);
	if (res_flash==FR_OK)
	{
		printf("\n*************** ��testdir.txt���ļ���Ϣ��***************\n");
		printf("���ļ���С: %ld(�ֽ�)\n", fno.fsize);
		printf("��ʱ���: %u/%02u/%02u, %02u:%02u\n",
		(fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
		fno.ftime >> 11, fno.ftime >> 5 & 63);

		printf("������: %c%c%c%c%c\n\n",
			(fno.fattrib & AM_DIR) ? 'D' : '-', // ��һ��Ŀ¼
			(fno.fattrib & AM_RDO) ? 'R' : '-', // ֻ���ļ�
			(fno.fattrib & AM_HID) ? 'H' : '-', // �����ļ�
			(fno.fattrib & AM_SYS) ? 'S' : '-', // ϵͳ�ļ�
			(fno.fattrib & AM_ARC) ? 'A' : '-'); // �����ļ�
	}

	return res_flash;
}


/*
	scan_files ��������ɨ��ָ��·���µ��ļ��������������һ�� mp3 ��������������Ҫ��ȡ mp3
	��ʽ�ļ������� *.txt��.c �ļ�����ͳͳ����Ҫ�ģ���ʱ���Ǿͱ���ɨ��·���������ļ�����.mp3
	�� *.MP3 ��ʽ�ļ���ȡ�������������ǲ���ȡ�ض���ʽ�ļ������ǰ������ļ����ƶ�ͨ������
	��ӡ������
*/
static FRESULT scan_files (char* path)
{
	FRESULT res; //�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���
	FILINFO fno;
	DIR dir;
	int i;
	char *fn; // �ļ���
	
	printf("\n***************** �ļ�ɨ����� ****************\r\n");

#if _USE_LFN
	/* ���ļ���֧�� */
	/* ����������Ҫ 2 ���ֽڱ���һ�����֡�*/
	static char lfn[_MAX_LFN*2 + 1];
	fno.lfname = lfn;
	fno.lfsize = sizeof(lfn);
#endif

	//��Ŀ¼
	//�����path���������洫�����ġ�3:����f_opendir�������Բ���Ҫ��·��
	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		i = strlen(path);
		for (;;)
		{
			//��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
			res = f_readdir(&dir, &fno);
			//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
			if (res != FR_OK || fno.fname[0] == 0) break;
#if _USE_LFN
			fn = *fno.lfname ? fno.lfname : fno.fname;		
#else
			fn = fno.fname;
#endif
			//���ʾ��ǰĿ¼������
			if (*fn == '.') continue;
			//Ŀ¼���ݹ��ȡ
			if (fno.fattrib & AM_DIR) 
			{
				//�ϳ�����Ŀ¼
				sprintf(&path[i], "/%s", fn);
				//�ݹ����
				res = scan_files(path);
				path[i] = 0;
				//��ʧ�ܣ�����ѭ��
				if (res != FR_OK)	break;
			}
			else
			{
				printf("%s/%s\r\n", path, fn); //����ļ���
				/* ������������ȡ�ض���ʽ���ļ�·�� */
			}
		}
		
	}

	return res;
}

void DevMain(void);
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����NVIC���ȼ�����Ϊ4����7:4bitȫ��Ϊ��ռ���ȼ�������������0~15���ȼ�

	DBG_OPEN();
	DBG_TRACE("compiler time = %s,%s\r\n",compiler_data,compiler_time);
	DBG_TRACE("file name = %s , fun name = %s , line number= %d\r\n",__FILE__,__FUNCTION__,__LINE__);

#if 0
	DevMain();//��ʼ��Ӳ���ʹ�������������
#endif

#if 0
	pwm_test();
#endif

#if 0
	//SleepMode_test();
	//StopMode_test();
	//PVD_config_init();//����PVD������ѹ����ʱ��������жϷ������������
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
	da_init();//�򿪴���debug�������������ѹ����û��ԭ��
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

#if 0//����spi flash�Ķ��ҳд��
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

#if 1//����sflash��fatfs�ļ�ϵͳ�Ĺ��ء���ʽ������д�Ļ�������
	BYTE buffer[100]= {0};	/* �������� */
	BYTE textFileBuffer[] = "�½��ļ�ϵͳ�����ļ�\r\n";

	printf("\r\n****** ����һ�� SPI FLASH �ļ�ϵͳʵ�� ******\r\n");
	//���ⲿ SPI Flash �����ļ�ϵͳ���ļ�ϵͳ����ʱ��� SPI �豸��ʼ��
	/*
		��ʼ�����������������£�
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/	
	res_flash = f_mount(&fs,"3:",1);


	/*----------------------- ��ʽ������ -----------------*/
	/* ���û���ļ�ϵͳ�͸�ʽ�������ļ�ϵͳ */
	if (res_flash == FR_NO_FILESYSTEM)
	{
		printf("��FLASH ��û���ļ�ϵͳ���������и�ʽ��...\r\n");
		res_flash=f_mkfs("3:",0,0);//��ʽ��

		if (res_flash == FR_OK)
		{
			printf("��FLASH �ѳɹ���ʽ���ļ�ϵͳ��\r\n");
			/* ��ʽ���ɹ�����Ҫ��ȡ������ԭ���豸�������¹����豸 */
			res_flash = f_mount(NULL,"3:",1);
			/* ���¹��� */
			res_flash = f_mount(&fs,"3:",1);
		}
		else
		{
			printf("������ʽ��ʧ�ܡ�(%d)����\r\n",res_flash);
			while (1);
		}
	}
	else if(res_flash != FR_OK)
	{
		printf("�����ⲿ Flash �����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
		printf("��������ԭ��SPI Flash ��ʼ�����ɹ���\r\n");
		while (1);
	}
	else
	{
		printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
	}


	/*----------------------- �ļ�ϵͳ���ԣ�д���� -------------------*/
	/* ���ļ���ÿ�ζ����µ���ʽ�򿪣�����Ϊ��д */
	printf("\r\n****** ���������ļ�д�����... ******\r\n");
	res_flash = f_open(&fnew, "3:FatFs ��д�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		printf("����/���� FatFs ��д�����ļ�.txt �ļ��ɹ������ļ�д�����ݡ�\r\n");
		/* ��ָ���洢������д�뵽�ļ��� */
		res_flash=f_write(&fnew,textFileBuffer,sizeof(textFileBuffer),&fnum);

		if (res_flash==FR_OK)
		{
			printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n",fnum);
			printf("�����ļ�д�������Ϊ��\r\n%s\r\n",textFileBuffer);
		}
		else
		{
			printf("�����ļ�д��ʧ�ܣ�(%d)\n",res_flash);
		}

		/* ���ٶ�д���ر��ļ� */
		f_close(&fnew);
	}
	else
	{
		printf("������/�����ļ�ʧ�ܡ�(%d)\r\n",res_flash);
	}


	/*------------------- �ļ�ϵͳ���ԣ������� --------------------------*/
	printf("\r\n****** ���������ļ���ȡ����... ******\r\n");
	res_flash = f_open(&fnew, "3:FatFs ��д�����ļ�.txt",FA_OPEN_EXISTING | FA_READ);
	if (res_flash == FR_OK)
	{
		printf("�����ļ��ɹ���\r\n");
		res_flash = f_read(&fnew, buffer, sizeof(buffer), &fnum);
		if (res_flash==FR_OK)
		{
			printf("���ļ���ȡ�ɹ�, �����ֽ����ݣ�%d\r\n",fnum);
			printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", buffer);
		}
		else
		{
			printf("�����ļ���ȡʧ�ܣ�(%d)\n",res_flash);
		}
	}
	else
	{
		printf("�������ļ�ʧ�ܡ�(%d)\r\n",res_flash);
	}
	
	/* ���ٶ�д���ر��ļ� */
	f_close(&fnew);

	/* ����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ */
	f_mount(NULL,"3:",1);

	/* ������ɣ�ͣ�� */
	
#endif

#if 0//����sflash��FatFs�����
	printf("\r\n****** ����һ�� SPI FLASH �ļ�ϵͳʵ�� ******\r\n");

	//���ⲿ SPI Flash �����ļ�ϵͳ���ļ�ϵͳ����ʱ��� SPI �豸��ʼ��
	/*
		��ʼ�����������������£�
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/	
	res_flash = f_mount(&fs,"3:",1);
	if(res_flash != FR_OK)
	{
		printf("�����ⲿ Flash �����ļ�ϵͳʧ�ܡ�(%d)\r\n",res_flash);
		printf("��������ԭ��SPI Flash ��ʼ�����ɹ���\r\n");
		while (1);
	}
	else
	{
		printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\r\n");
	}

	//FatFs����ܲ���
	res_flash = miscellaneous();//f_printf�����о�û����Ч�����Ժ���飡
	

	//�ļ���Ϣ��ȡ
	res_flash = file_check();

	//·��ɨ��
	strcpy(fpath,"3:");
	scan_files(fpath);

#endif


	while(1);
}




