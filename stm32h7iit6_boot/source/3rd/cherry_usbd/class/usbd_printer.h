/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_PRINTER_H
#define USBD_PRINTER_H

//#include "usb_printer.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define PRINTER_SUBCLASS 0x01U

#define PRINTER_REQUEST_GET_DEVICE_ID   0x00U
#define PRINTER_REQUEST_GET_PORT_SATTUS 0x01U
#define PRINTER_REQUEST_SOFT_RESET      0x02U

#define PRINTER_STATUS_NO_ERROR    0x00U
#define PRINTER_STATUS_SELECTED    0x08U
#define PRINTER_STATUS_PAPER_EMPTY 0x10U

/* Alloc printer interface driver */
struct usbd_interface *usbd_printer_alloc_intf(uint8_t busid);

#ifdef __cplusplus
}
#endif

#endif /* USBD_PRINTER_H */
