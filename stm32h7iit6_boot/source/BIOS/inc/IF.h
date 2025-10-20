/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#ifndef _IF_H
#define _IF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

#define IF_OPEN_BIT     0x01
#if 000
typedef struct{
    BOOL valid;/*< if port valid */
    BOOL (*open)(BUFINFO *pInfo, void *pCtrl);/*< open port */
    BOOL (*close)(void);             /*< close port */
    BOOL (*put)(u8 d);
    BOOL (*get)(u8 *d);
    u32  (*read)(u8 *buf,u32 len);   /*< read data from open port */
    u32  (*write)(u8 *buf,u32 len);  /*< write data to open port */
    u32  (*peek)(u8 *buf,u32 len);   /*< peek data from open port */
    BOOL (*peek_sync)(u8 d);         /*< peek sync data from open port */
    u32  (*io_ctrl)(u32 type, void *ctrl);/*< ioctrl */
    u32  tag;/*< OPEN,CLOSE*/
}IF_DRIVER;
#else
	typedef struct{
		BOOL valid;/*< if port valid */
		BOOL (*open)(BUFINFO *pInfo, void *pCtrl);/*< open port */
		BOOL (*close)(void);			 /*< close port */
		BOOL (*put)(u8 d);
		BOOL (*get)(u8 *d);
		u32  (*read)(u8 *buf,u32 len);	 /*< read data from open port */
		u32  (*write)(u8 *buf,u32 len);  /*< write data to open port */
		u32  (*peek)(u8 *buf,u32 len);	 /*< peek data from open port */
		BOOL (*peek_sync)(u8 d);		 /*< peek sync data from open port */
		u32  (*io_ctrl)(IOCTRL_TYPE type, void *ctrl);/*< ioctrl */
		u32  tag;/*< OPEN,CLOSE*/
	}IF_DRIVER;

#endif
#ifdef __cplusplus
}
#endif 

#endif
