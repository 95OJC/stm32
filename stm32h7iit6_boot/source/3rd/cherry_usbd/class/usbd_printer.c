/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_printer.h"

struct printer_cfg_priv {
    uint8_t *device_id;
    uint8_t port_status;
} usbd_printer_cfg;

#if 1
/* USB device Printer Port Status.

  Summary:
    Printer status required by class-specific request.

  Description:
    This structure defines the Printer status.

  Remarks:
    None.
 */
typedef struct 
{
    /* 1 = No Error, 0 = Error */
    uint8_t errorStatus;
    
    /* 1 = Selected, 0 = Not Selected */
    uint8_t selectStatus;
    
    /* 1 = Paper Empty, 0 = Paper Not Empty */
    uint8_t paperEmptyStatus;

} USB_DEVICE_PRINTER_PORT_STATUS;


typedef enum
{
    /* class-specific request returns a device ID string that is compatible with
     * IEEE 1284 */
    USB_PRINTER_GET_DEVICE_ID   = 0x0,
    /* class-specific request returns the printer?s current status */
    USB_PRINTER_GET_PORT_STATUS = 0x1,
    /* class-specific request flushes all buffers and resets the Bulk endpoints 
     * to their default states */
    USB_PRINTER_SOFT_RESET      = 0x2
} USB_PRINTER_COMMAND;

#if 0
#define USB_DEVICE_PRINTER_DEVICE_ID_STRING_LENGTH    0x35

/* Device ID string including length in the first two bytes */
#define USB_DEVICE_PRINTER_DEVICE_ID_STRING  {0,USB_DEVICE_PRINTER_DEVICE_ID_STRING_LENGTH,'M','F','G',':','D','A','S','C','O','M',';','M','D','L',':','D','L','-','9','6','1','Z',';','C','L','S',':','P','R','I','N','T','E','R',';','D','E','S',':','D','A','S','C','O','M','D','L','-','9','x',';'}
#else
#define USB_DEVICE_PRINTER_DEVICE_ID_STRING_LENGTH    44
#define USB_DEVICE_PRINTER_DEVICE_ID_STRING  {0,0x2C,0x4D,0x46,0x47,0x3A,0x47,0x57,0x49,0x3B,0x4D,0x44,0x4C,0x3A,0x50,0x52,0x37,0x30,0x20,0x70,0x6C,0x75,0x73,0x20,0x50,0x72,0x69,0x6E,0x74,0x65,0x72,0x3B,0x43,0x4C,0x53,0x3A,0x50,0x52,0x49,0x4E,0x54,0x45,0x52,0x3B}
#endif
USB_DEVICE_PRINTER_PORT_STATUS portStatus;

uint8_t DeviceID_String[USB_DEVICE_PRINTER_DEVICE_ID_STRING_LENGTH]=USB_DEVICE_PRINTER_DEVICE_ID_STRING;
#endif

static int printer_class_interface_request_handler(uint8_t busid, struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
	uint8_t prntrStatus;
	
    USB_LOG_DBG("Printer Class request: "
                "bRequest 0x%02x\r\n",
                setup->bRequest);

    switch (setup->bRequest) {
        case PRINTER_REQUEST_GET_DEVICE_ID:
				*data = DeviceID_String;
				*len = USB_DEVICE_PRINTER_DEVICE_ID_STRING_LENGTH;
            break;
        case PRINTER_REQUEST_GET_PORT_SATTUS:								
				// Note Some USB printers may not always be able to determine this information. 
				// In this case, they should return benign status of ¡°Paper Not Empty,¡± ¡°Selected,¡± and ¡°No Error.¡±
												
				portStatus.paperEmptyStatus=0;// 1 = Paper Empty, 0 = Paper Not Empty
				portStatus.selectStatus=1;    // 1 = Selected, 0 = Not Selected																			 
				portStatus.errorStatus=1;     // 1 = No Error, 0 = Error
												
				prntrStatus = (uint8_t)(((portStatus.errorStatus << 3) & 0x08) | 
               ((portStatus.selectStatus << 4) & 0x10) |
               ((portStatus.paperEmptyStatus << 5) & 0x20));

			   *data = &prntrStatus;
			   *len = 1;
            break;
        case PRINTER_REQUEST_SOFT_RESET:
				*data = NULL;
				*len = 0;
            break;
        default:
            USB_LOG_WRN("Unhandled Printer Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

static void printer_notify_handler(uint8_t busid,uint8_t event, void *arg)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;

        default:
            break;
    }
}

struct usbd_interface *usbd_printer_alloc_intf(uint8_t busid)
{
    struct usbd_interface *intf = usb_malloc(sizeof(struct usbd_interface));
    if (intf == NULL) {
        USB_LOG_ERR("no mem to alloc intf\r\n");
        return NULL;
    }

    intf->class_interface_handler = printer_class_interface_request_handler;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = printer_notify_handler;

    return intf;
}