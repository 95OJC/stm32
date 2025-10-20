/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#ifndef _RINGBUFAPI_H
#define _RINGBUFAPI_H

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _FIFO_CTRL
{
    u8 *pIn; 
    u8 *pOut;
    u8 *pPeek;
    u8 *pStart;
    u8 *pEnd;
    u32 NearFullSize;
    u32 NearEmptySize;
    u32 Size;
}FIFO_CTRL;
    
typedef struct _FIFO_CTRL_INFO{
    FIFO_CTRL rx;
    u32 rx_tag;
    FIFO_CTRL tx;
    u32 tx_tag;
}FIFO_CTRL_INFO;//记录ringbuf指针和空满等不固定状态

u32  get_data_q_size(FIFO_CTRL *q);
u32 get_data_q_free_size(FIFO_CTRL *q);

BOOL is_data_q_full(FIFO_CTRL *q);
BOOL is_data_q_empty(FIFO_CTRL *q);

BOOL data_q_in(FIFO_CTRL *q,u8 d);
u32 data_q_in_more(FIFO_CTRL *q, u8 *buf, u32 len);

BOOL data_q_out(FIFO_CTRL *q,u8 *d);
u32 data_q_out_more(FIFO_CTRL *q, u8 *buf, u32 len);

BOOL data_q_peek(FIFO_CTRL *q,u8 *d);
BOOL data_q_peek_sync(FIFO_CTRL *q,BOOL m);

BOOL is_data_q_near_full(FIFO_CTRL *q);
BOOL is_data_q_near_empty(FIFO_CTRL *q);

BOOL data_q_cls(FIFO_CTRL *q);
void data_q_init(FIFO_CTRL *q,u8 *buf, u32 size,u32 empty,u32 full);
void data_q_reset(FIFO_CTRL *q);


#ifdef __cplusplus
}
#endif 

#endif
