/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef debug

    #define UART0_DBG 0
    #define UART1_DBG 1
    #define USB_DBG   2
    #define DEBUG_PORT UART0_DBG
    
    void debug_init(u8 port,BUFINFO *pBuf,void *pInfo);
    void debug_open(void);
    u8 get_char(void);
    void put_char(u8 d);
    void debug_out(c8 *fmt,...);

    #define PRINTF  debug_out
    #define BIOS_DBG_EN    1
    #define APP_DBG_EN     1
	
	#define DBG_OPEN 	debug_open
#else

    #define PRINTF
	#define DBG_OPEN()	  ((void)0)
	#define BIOS_DBG_EN    0
	#define APP_DBG_EN     0
	#define get_char       0
	#define put_char      ((void)0)

#endif


#endif


