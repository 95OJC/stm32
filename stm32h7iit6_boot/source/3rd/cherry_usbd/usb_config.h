#ifndef __USB_CONFIG_H__
#define __USB_CONFIG_H__


//#include "rtthread.h"


#define CONFIG_USB_HS
#if 0 
#define USB_NOCACHE_RAM_SECTION

#define CONFIG_USB_PRINTF						rt_kprintf

#define usb_malloc								rt_malloc

#define usb_free								rt_free
#else
	/* attribute data into no cache ram */
	#define USB_NOCACHE_RAM_SECTION __attribute__((section(".noncacheable")))

	#define CONFIG_USB_PRINTF(...) printf(__VA_ARGS__)

	#define usb_malloc(size) malloc(size)
	#define usb_free(ptr)    free(ptr)

	#ifndef CONFIG_USB_DBG_LEVEL
	#define CONFIG_USB_DBG_LEVEL USB_DBG_WARNING//USB_DBG_LOG//USB_DBG_INFO
	#endif

	/* Enable print with color */
	#define CONFIG_USB_PRINTF_COLOR_ENABLE

	/* data align size when use dma */
	#ifndef CONFIG_USB_ALIGN_SIZE
	#define CONFIG_USB_ALIGN_SIZE 4
	#endif

#endif

/*     hub class 		*/

#define CONFIG_USBHOST_DEV_NAMELEN				8

#define CONFIG_USBHOST_MAX_ENDPOINTS			16

#define CONFIG_USBHOST_MAX_EHPORTS				4
#define CONFIG_USBHOST_MAX_RHPORTS				4

#define CONFIG_USBHOST_MAX_INTF_ALTSETTINGS		4
#define CONFIG_USBHOST_MAX_INTERFACES			4

//#define CONFIG_USB_ALIGN_SIZE					8

#define CONFIG_USBHOST_MAX_BUS					2

#define CONFIG_USBHOST_MAX_EXTHUBS				2

#define CONFIG_USBHOST_REQUEST_BUFFER_LEN		512

#define CONFIG_USBHOST_PSC_STACKSIZE			1024

#define CONFIG_USBHOST_PSC_PRIO					5

#define CONFIG_USBHOST_CONTROL_TRANSFER_TIMEOUT	1000

//#define CONFIG_USB_DBG_LEVEL					(USB_DBG_LOG-1)


#define CONFIG_USBHOST_PIPE_NUM					8



/*     mass storage class		*/
#define CONFIG_USBHOST_MAX_MSC_CLASS			2
#define CONFIG_USBHOST_MSC_TIMEOUT				10000


/*     hid class		*/
#define CONFIG_USBHOST_MAX_HID_CLASS			2



/* ================= USB Device Stack Configuration ================ */


#ifndef CONFIG_USBDEV_MAX_BUS
#define CONFIG_USBDEV_MAX_BUS 1 // for now, bus num must be 1 except hpm ip
#endif

#ifndef CONFIG_USBDEV_EP_NUM
#define CONFIG_USBDEV_EP_NUM 8
#endif


/* Ep0 in and out transfer buffer */
#ifndef CONFIG_USBDEV_REQUEST_BUFFER_LEN
#define CONFIG_USBDEV_REQUEST_BUFFER_LEN 512
#endif

#ifndef CONFIG_USBDEV_MSC_MAX_LUN
#define CONFIG_USBDEV_MSC_MAX_LUN 1
#endif

#ifndef CONFIG_USBDEV_MSC_MAX_BUFSIZE
#define CONFIG_USBDEV_MSC_MAX_BUFSIZE 512
#endif


#ifndef CONFIG_USBDEV_MSC_MANUFACTURER_STRING
#define CONFIG_USBDEV_MSC_MANUFACTURER_STRING ""
#endif

#ifndef CONFIG_USBDEV_MSC_PRODUCT_STRING
#define CONFIG_USBDEV_MSC_PRODUCT_STRING ""
#endif

#ifndef CONFIG_USBDEV_MSC_VERSION_STRING
#define CONFIG_USBDEV_MSC_VERSION_STRING "0.01"
#endif

//#define CONFIG_USBDEV_ADVANCE_DESC



#endif	/* __USB_CONFIG_H__ */


