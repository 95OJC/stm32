/******************************************************************************
* Copyright(C) 2017-2018 DASCOM Corporation All Rights Reserved
******************************************************************************/

#include "common.h"
#include "IF.h"

#define DEBUG 1

#if (DEBUG && BIOS_DBG_EN)
    #define DBG_TRACE(...) PRINTF(__VA_ARGS__)
#else
    #define DBG_TRACE(...)
#endif

#define MAX_DRIVER_NUM 10
#define VAR_CHECK   TRUE

static IF_DRIVER driver_tbl[MAX_DRIVER_NUM];
static IF_DRIVER *pCurrentDriver;

void if_init(void)
{
    u8 i;
    
    pCurrentDriver = NULL;

    for(i=0; i<MAX_DRIVER_NUM; i++)
    {
        driver_tbl[i].valid = FALSE;
        driver_tbl[i].tag = 0;
    }

}

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
            )
{
    IF_DRIVER *p;
    
    p = &driver_tbl[n];
	if(p->valid != TRUE)
	{
		p->valid = TRUE;
		p->tag = 0;
		p->open = open;
		p->close = close;
		p->put = put;
		p->get = get;
		p->read = read;
		p->write = write;
		p->peek = peek;
		p->peek_sync = peek_sync;
		p->io_ctrl = io_ctrl;
		return TRUE;
		
	}else
	{
		return FALSE;
	}
}

BOOL get_if_io_driver(PORT_NUM n, IO_FUNCTION *io_func)
{
    BOOL ret = TRUE;
    IF_DRIVER *pDriver;
    
    if((n >= MAX_DRIVER_NUM) || (!driver_tbl[n].valid))
    {
        return FALSE;
    }

    pDriver = &driver_tbl[n];
    io_func->open = pDriver->open;
    io_func->close = pDriver->close;
    io_func->put = pDriver->put;
    io_func->get = pDriver->get;
    io_func->read =  pDriver->read;
    io_func->write = pDriver->write;
    io_func->peek = pDriver->peek;
    io_func->peek_sync = pDriver->peek_sync;
    io_func->io_ctrl = pDriver->io_ctrl;

    return ret;
}

PORT_NUM if_find_data_port(void)
{
    PORT_NUM port = NULL_PORT;
    u8 i;
    u32 n;
    
    for(i=0; i<MAX_DRIVER_NUM; i++)
    {
        if(!driver_tbl[i].valid || !(driver_tbl[i].tag & IF_OPEN_BIT))
        {
            continue;
        }

        driver_tbl[i].io_ctrl(IOCTRL_RX_DATA_SIZE,&n);
        
        if(n > 0)
        {
            port = (PORT_NUM)i;
            break;
        }
    }
    
    return port;
}

BOOL if_set_cur_port(PORT_NUM port)
{
    if(port >= MAX_DRIVER_NUM)
    {
        return FALSE;
    }

    if(!(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return FALSE;
    }
    
    pCurrentDriver = &driver_tbl[port];
    
    return TRUE;
}

BOOL if_open(PORT_NUM n,BUFINFO *pInfo, void *pCtrl)
{
    BOOL ret = FALSE;
    IF_DRIVER *pDriver;

    if(n >= MAX_DRIVER_NUM)
    {
        return FALSE;
    }
    
    pDriver = &driver_tbl[n];

    if(!pDriver->valid)
    {
        return FALSE;
    }
    if(pDriver->open(pInfo,pCtrl))
    {
        pDriver->tag |= IF_OPEN_BIT;
        ret = TRUE;
    }
   
    return ret;
}

BOOL if_close_by_port(PORT_NUM port)
{
    BOOL ret = FALSE;

#if VAR_CHECK == TRUE

    if(port >= MAX_DRIVER_NUM)
    {
        return ret;
    }

    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    }

#endif

    if(driver_tbl[port].close())
    {
        driver_tbl[port].tag &= ~IF_OPEN_BIT;
        //driver_tbl[port].valid = FALSE;
        ret = TRUE;
    }

    return ret;

}
u32 if_read_data_by_port(u8 *buf, u32 len,PORT_NUM port)
{
    u32 ret = 0;
    
#if VAR_CHECK == TRUE

    if(port >= MAX_DRIVER_NUM)
    {
        return ret;
    }
    
    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    }
    
#endif

    ret = driver_tbl[port].read(buf,len);
    
    return ret;
}

u32 if_write_data_by_port(u8 *buf, u32 len,PORT_NUM port)
{
    u32 ret = 0;

#if VAR_CHECK == TRUE

    if(port >= MAX_DRIVER_NUM)
    {
        return ret;
    }
    
    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    }  

#endif
    ret = driver_tbl[port].write(buf,len);

    return ret;
}

u32 if_peek_data_by_port(u8 * buf, u32 len,PORT_NUM port)
{
    u32 ret = 0;
    
#if VAR_CHECK == TRUE

    if(port >= MAX_DRIVER_NUM)
    {
        return ret;
    }
    
    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    }

#endif

    ret = driver_tbl[port].peek(buf,len);

    return ret;
}

BOOL if_peek_sync_by_port(u8 m,PORT_NUM port)
{
    BOOL ret = FALSE;

#if VAR_CHECK == TRUE

    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    } 

#endif

    ret = driver_tbl[port].peek_sync(m);

    return ret;
}

u32 if_io_ctrl_by_port(IOCTRL_TYPE type,void *ctrl,PORT_NUM port)
{
    u32 ret = 0;

#if VAR_CHECK == TRUE

    if(!driver_tbl[port].valid || !(driver_tbl[port].tag & IF_OPEN_BIT))
    {
        return ret;
    }

#endif

    ret = driver_tbl[port].io_ctrl(type,ctrl);

    return ret;    
}

BOOL if_close(void)
{
    BOOL ret = FALSE;
    
    if(pCurrentDriver)
    {
        if(pCurrentDriver->close())
        {
            pCurrentDriver->tag &= ~IF_OPEN_BIT;
            //pCurrentDriver->valid = FALSE;
            ret = TRUE;
        }
    }
    
    return ret;
}

u32 if_read_data(u8 *buf, u32 len)
{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->read(buf,len);
    }
    
    return ret;
}

u32 if_write_data(u8 *buf, u32 len)
{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->write(buf,len);
    }

    return ret;
}

u32 if_peek_data(u8 *buf, u32 len)
{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->peek(buf,len);
    }
    
    return ret;
}

BOOL if_peek_sync(u8 m)
{
    BOOL ret = FALSE;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->peek_sync(m);
    }

    return ret;
}

u32 if_io_ctrl(u32 type,void *ctrl)

{
    u32 ret = 0;
    
    if(pCurrentDriver)
    {
        ret = pCurrentDriver->io_ctrl(type,ctrl);
    }

    return ret;    
}



