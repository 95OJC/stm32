/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#ifndef _IFAPI_H
#define _IFAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

typedef enum{
    IOCTRL_RX_DATA_SIZE = 0,
    IOCTRL_TX_DATA_SIZE,
    IOCTRL_RESET_RX_BUF,
    IOCTRL_RESET_TX_BUF,
    IOCTRL_RESET_RT_BUF,
    IOCTRL_SET_PORT_STAT,
    IOCTRL_GET_PORT_STAT,
    IOCTRL_GET_CTS_IO,
}IOCTRL_TYPE;

typedef struct{
    BOOL (*open)(BUFINFO *pInfo, void *pCtrl);/*< open port */
    BOOL (*close)(void);            /*< close port */
    BOOL (*put)(u8 d);              /*< put one byte data to port with sync */
    BOOL (*get)(u8 *d);             /*< get one byte data with polling */
    u32 (*read)(u8 *buf,u32 len);   /*< read data from open port */
    u32 (*write)(u8 *buf,u32 len);  /*< write data to open port */
    u32 (*peek)(u8 *buf,u32 len);   /*< peek data from open port */
    BOOL (*peek_sync)(u8 d);        /*< peek sync data from open port */
    u32 (*io_ctrl)(IOCTRL_TYPE type, void *ctrl);/*< ioctrl */
}IO_FUNCTION;


typedef enum{
    USBD_PORT = 0,
    HID_PORT = 1,
    LPT_PORT = 2,
    UART_PORT = 3,
    UART1_PORT = 4,
    BLUETOOTH_PORT = 5,
    WIFI_PORT = 6,
    EHTER_PORT = 7,
    SHARE_PORT = 0xFE,
    NULL_PORT = 0xFF
}PORT_NUM;

void if_init(void);
PORT_NUM if_find_data_port(void);
BOOL if_set_cur_port(PORT_NUM port);
BOOL if_open(PORT_NUM n,BUFINFO *pInfo, void *pCtrl);
BOOL if_close_by_port(PORT_NUM port);
BOOL if_close(void);
u32 if_read_data(u8 *buf, u32 len);
u32 if_write_data(u8 *buf, u32 len);
u32 if_peek_data(u8 * buf, u32 len);
BOOL if_peek_sync(u8 m);
u32 if_io_ctrl(u32 type,void *ctrl);
u32 if_read_data_by_port(u8 *buf, u32 len,PORT_NUM port);
u32 if_write_data_by_port(u8 *buf, u32 len,PORT_NUM port);
u32 if_peek_data_by_port(u8 * buf, u32 len,PORT_NUM port);
BOOL if_peek_sync_by_port(u8 m,PORT_NUM port);
u32 if_io_ctrl_by_port(IOCTRL_TYPE type,void *ctrl,PORT_NUM port);
BOOL get_if_io_driver(PORT_NUM n, IO_FUNCTION *io_func);

BOOL if_regsit_driver(
                PORT_NUM n,
                BOOL (*open)(BUFINFO *pInfo, void *pCtrl),
                BOOL (*close)(void),
                BOOL (*put)(u8 d),
                BOOL (*get)(u8 *d),
                u32 (*read)(u8 *buf,u32 len),
                u32 (*write)(u8 *buf,u32 len),
                u32 (*peek)(u8 *buf,u32 len),
                BOOL (*peek_sync)(u8 d),
                u32 (*io_ctrl)(IOCTRL_TYPE type, void *ctrl)
            );



#ifdef __cplusplus
}
#endif 

#endif
