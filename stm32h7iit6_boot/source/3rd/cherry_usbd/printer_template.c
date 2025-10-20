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
	9,							// �������������ֽ�����С
	0x02,						// �������������ͱ��
	LBYTE(9+9+7+7),				// �����÷��ص��������ݴ�С
	HBYTE((9+9+7+7)),			// �����÷��ص��������ݴ�С
	1,							// ��������֧�ֵĽӿ�����
	1,							// Set_Configuration��������Ҫ�Ĳ���ֵ
	0,							// ���������õ��ַ���������ֵ
	0x80 | 0x40,				// ����ģʽ��ѡ��
	50,							// �豸��������ȡ��������

	9,							// �ӿ����������ֽ�����С
	0x04,						// �ӿ������������ͱ��
	0,							// �ýӿڵı��
	0,							// ���õĽӿ����������
	2,							// �ýӿ�ʹ�õĶ˵���,�������˵�0
	0x07,						// �ӿ�����
	0x01,						// �ӿ�������
	0x02,						// �ӿ���ѭ��Э��
	0,							// �����ýӿڵ��ַ�������ֵ
   
	7,							// �˵��������ֽ�����С
	0x05,						// �˵����������ͱ��
	PRN_OUT_EP,					// �˵��ַ�������������
	0x02,						// �˵�Ĵ�����������
	LBYTE(PRN_MAX_MPS),			// �˵��ա�����������С
	HBYTE(PRN_MAX_MPS),			// �˵��ա�����������С
	0,							// ������ѯ�˵��ʱ����

	7,
	0x05,
	PRN_IN_EP,
	0x02,
	LBYTE(PRN_MAX_MPS),
	HBYTE(PRN_MAX_MPS),
	0,
/********************************************** Class printer Descriptor */

/********************************************** Language ID String Descriptor */
    4,		//����
    0x03,					//����
    LBYTE(0x0409),	//����ID
    HBYTE(0x0409),	//����ID
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
	//���OUT�˵����ACK
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
		data_q_out(&gUsbPrnCtrl.tx,&d);//�ӻ�����ȡ���ݷ���
		prn_write_buffer[i] = d;
		i++;
	}	
	usbd_ep_start_write(busid,PRN_IN_EP,prn_write_buffer,i);//��Ҫ��������IN���ư�
	#if 0
	//����������ݵ�nbytes���ȿ��Ա�PRN_MAX_MPS����,����Ҫ����zlp����ʾ��������Ϊ������֪��IN������
	//��������Ա�PRN_MAX_MPS������˵���Ѿ�С��һ�����ݣ��϶������һ���ˣ�����Ҫ����zlp����ʾ����(ȷ��ÿһ������PRN_MAX_MPS�ֽڷ���)
	if ((i % PRN_MAX_MPS) == 0 && i) {
		/* send zlp */
		usbd_ep_start_write(busid, PRN_IN_EP, NULL, 0);
	} else {

	}
	#endif
}

void usbd_printer_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{	
	//�Ѿ�������nbytes���ȵ�����
    //USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    USBD_UMS_LL_sendData(0);
#if 0
	//����������ݵ�nbytes���ȿ��Ա�PRN_MAX_MPS����,����Ҫ����zlp����ʾ��������Ϊ������֪��IN������
	//��������Ա�PRN_MAX_MPS������˵���Ѿ�С��һ�����ݣ��϶������һ���ˣ�����Ҫ����zlp����ʾ����(ȷ��ÿһ������PRN_MAX_MPS�ֽڷ���)
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
