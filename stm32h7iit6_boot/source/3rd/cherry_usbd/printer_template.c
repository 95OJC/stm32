#include "usbd_core.h"
#include "usbd_printer.h"
#include "common.h"

#define PRN_IN_EP  0x81
#define PRN_OUT_EP 0x02
#define PRN_USBD_VID           0x206D//0xFFFF
#define PRN_USBD_PID           0x0203//0xFFFF

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t prn_read_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t prn_write_buffer[2048];
volatile uint32_t prn_rec_cnt = 0;

#ifdef CONFIG_USB_HS
#define PRN_MAX_MPS 512
#else
#define PRN_MAX_MPS 64
#endif

/*!< global descriptor */
static const uint8_t printer_descriptor[] = {
/********************************************** Device Descriptor */
	0x12,
	0x01,
	LBYTE(0x200),
	HBYTE(0x200),
	0x00,
	0x00,
	0x00,
	0x40,
	LBYTE(PRN_USBD_VID),
	HBYTE(PRN_USBD_VID),
	LBYTE(PRN_USBD_PID),
	HBYTE(PRN_USBD_PID),
	LBYTE(0x0101),
	HBYTE(0x0101),
	0x01,
	0x02,
	0x03,
	0x01,
/********************************************** Config Descriptor */
	9,							// 配置描述符的字节数大小
	0x02,						// 配置描述符类型编号
	LBYTE(9+9+7+7),				// 此配置返回的所有数据大小
	HBYTE((9+9+7+7)),			// 此配置返回的所有数据大小
	1,							// 此配置所支持的接口数量
	1,							// Set_Configuration命令所需要的参数值
	0,							// 描述该配置的字符串的索引值
	0x80 | 0x40,				// 供电模式的选择
	50,							// 设备从总线提取的最大电流

	9,							// 接口描述符的字节数大小
	0x04,						// 接口描述符的类型编号
	0,							// 该接口的编号
	0,							// 备用的接口描述符编号
	2,							// 该接口使用的端点数,不包括端点0
	0x07,						// 接口类型
	0x01,						// 接口子类型
	0x02,						// 接口遵循的协议
	0,							// 描述该接口的字符串索引值
   
	7,							// 端点描述符字节数大小
	0x05,						// 端点描述符类型编号
	PRN_OUT_EP,					// 端点地址及输入输出属性
	0x02,						// 端点的传输类型属性
	LBYTE(PRN_MAX_MPS),			// 端点收、发的最大包大小
	HBYTE(PRN_MAX_MPS),			// 端点收、发的最大包大小
	0,							// 主机查询端点的时间间隔

	7,
	0x05,
	PRN_IN_EP,
	0x02,
	LBYTE(PRN_MAX_MPS),
	HBYTE(PRN_MAX_MPS),
	0,
/********************************************** Class printer Descriptor */

/********************************************** Language ID String Descriptor */
    4,		//长度
    0x03,					//类型
    LBYTE(0x0409),	//语言ID
    HBYTE(0x0409),	//语言ID
/********************************************** String 1 Descriptor */
/* Cherry USB Team */
    0x1C,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x47, 0x00,                                 /*!< 'G' wcChar0 */
    0x57, 0x00,                                 /*!< 'W' wcChar1 */
    0x49, 0x00,                                 /*!< 'I' wcChar2 */
    0x20, 0x00,                                 /*!< ' ' wcChar3 */
    0x43, 0x00,                                 /*!< 'C' wcChar4 */
    0x6F, 0x00,                                 /*!< 'o' wcChar5 */
    0x2E, 0x00,                                 /*!< '.' wcChar6 */
    0x2C, 0x00,                                 /*!< ',' wcChar7 */
    0x20, 0x00,                                 /*!< ' ' wcChar8 */
    0x4C, 0x00,                                 /*!< 'L' wcChar9 */
    0x74, 0x00,                                 /*!< 't' wcChar10 */
    0x64, 0x00,                                 /*!< 'd' wcChar11 */
    0x2E, 0x00,                                 /*!< '.' wcChar12 */
/********************************************** String 2 Descriptor */
/* Demo Product */
    0x24,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x50, 0x00,                               	/*!< 'P' wcChar0 */
    0x52, 0x00,                                	/*!< 'R' wcChar0 */
    0x37, 0x00,                                	/*!< '7' wcChar0 */
    0x30, 0x00,                                	/*!< '0' wcChar0 */
    0x20, 0x00,                                	/*!< ' ' wcChar0 */
    0x70, 0x00,                                	/*!< 'p' wcChar0 */
    0x6c, 0x00,                                	/*!< 'l' wcChar0 */
    0x75, 0x00,                                	/*!< 'u' wcChar0 */
    0x73, 0x00,                                	/*!< 's' wcChar0 */
    0x20, 0x00,                                 /*!< ' ' wcChar0 */
    0x50, 0x00,                                	/*!< 'P' wcChar0 */
    0x72, 0x00,                              	/*!< 'r' wcChar0 */
    0x69, 0x00,                              	/*!< 'i' wcChar0 */
    0x6e, 0x00,                             	/*!< 'n' wcChar0 */
    0x74, 0x00,                              	/*!< 't' wcChar0 */
    0x65, 0x00,                              	/*!< 'e' wcChar0 */
    0x72, 0x00, 								/*!< 'r' wcChar0 */ 
/********************************************** String 3 Descriptor */
    0x1c,                                       /*!< bLength */
    0x03,                                       /*!< bDescriptorType */
    0x32, 0x00,                                 /*!< '2' wcChar0 */
    0x35, 0x00,                                	/*!< '5' wcChar0 */
    0x50, 0x00,                               	/*!< 'P' wcChar0 */
    0x30, 0x00,                                	/*!< '0' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x32, 0x00,                                 /*!< '2' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x31, 0x00,                                 /*!< '1' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x30, 0x00,                                 /*!< '0' wcChar0 */
    0x32, 0x00,                             	/*!< '2' wcChar0 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00	
};

void usb_dc_low_level_init(void)
{
	__HAL_RCC_USB1_CLK_ENABLE();
	uart_printf("USB Interrupt Demo, Bulk Transfer, Max Packet is 512\n");
	NVIC_EnableIRQ(USB_INT_IRQn); 

}

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            /* setup first out ep read transfer */
            usbd_ep_start_read(busid, PRN_OUT_EP, prn_read_buffer, PRN_MAX_MPS);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

extern FIFO_CTRL_INFO gUsbPrnCtrl;
extern void PrintBulkOut(void);
extern void UsbCenBusy(u8 ep,BOOL blBusy);
void usbd_printer_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    //USB_LOG_RAW("actual out len:%d\r\n", nbytes);
	prn_rec_cnt = nbytes;

	PrintBulkOut();

	if (is_data_q_near_full(&gUsbPrnCtrl.rx))  
    {
    	UsbCenBusy(0,TRUE);
    }
	else
	{
    	usbd_ep_start_read(busid, PRN_OUT_EP, prn_read_buffer, PRN_MAX_MPS);
	}
	//最后OUT端点产生ACK
}

void USBD_UMS_LL_PrepareReceive(uint8_t busid)
{
	usbd_ep_start_read(busid, PRN_OUT_EP, prn_read_buffer, PRN_MAX_MPS);
}

void USBD_UMS_LL_sendData(uint8_t busid)
{
	u16 i = 0;
	u8 d;

	if(get_data_q_size(&gUsbPrnCtrl.tx) == 0)
	{
		IF_SET_TX_EMPTY(gUsbPrnCtrl.tx_tag);
		return ;
	}
	
	while ((i < PRN_MAX_MPS) && (get_data_q_size(&gUsbPrnCtrl.tx)!= 0))
	{
		data_q_out(&gUsbPrnCtrl.tx,&d);//从缓冲区取数据发送
		prn_write_buffer[i] = d;
		i++;
	}	
	usbd_ep_start_write(busid,PRN_IN_EP,prn_write_buffer,i);//需要主机发送IN令牌包
	#if 0
	//如果发送数据的nbytes长度可以被PRN_MAX_MPS整除,则需要发送zlp包表示结束，因为主机不知道IN结束包
	//如果不可以被PRN_MAX_MPS整除，说明已经小于一包数据，肯定是最后一包了，不需要发送zlp包表示结束(确保每一包都是PRN_MAX_MPS字节发送)
	if ((i % PRN_MAX_MPS) == 0 && i) {
		/* send zlp */
		usbd_ep_start_write(busid, PRN_IN_EP, NULL, 0);
	} else {

	}
	#endif
}

void usbd_printer_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{	
	//已经发送了nbytes长度的数据
    //USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    USBD_UMS_LL_sendData(0);
#if 0
	//如果发送数据的nbytes长度可以被PRN_MAX_MPS整除,则需要发送zlp包表示结束，因为主机不知道IN结束包
	//如果不可以被PRN_MAX_MPS整除，说明已经小于一包数据，肯定是最后一包了，不需要发送zlp包表示结束(确保每一包都是PRN_MAX_MPS字节发送)
	if ((nbytes % PRN_MAX_MPS) == 0 && nbytes) {
        /* send zlp */
        usbd_ep_start_write(busid, PRN_IN_EP, NULL, 0);
    } else {

    }
#endif	
}

/*!< endpoint call back */
struct usbd_endpoint printer_out_ep = {
    .ep_addr = PRN_OUT_EP,
    .ep_cb = usbd_printer_bulk_out
};

struct usbd_endpoint printer_in_ep = {
    .ep_addr = PRN_IN_EP,
    .ep_cb = usbd_printer_bulk_in
};

void printer_init(uint8_t busid, uint32_t reg_base)
{
    usbd_desc_register(busid, printer_descriptor);
    usbd_add_interface(busid, usbd_printer_alloc_intf(busid));
    usbd_add_endpoint(busid, &printer_out_ep);
    usbd_add_endpoint(busid, &printer_in_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
}
