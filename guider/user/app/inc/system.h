#ifndef __system_h
#define	__system_h


/*RX_BUF*/
#define	SYS_RXBUF_LEN			(100)
#define	RX_NEAR_EMPTY_SIZE		(0)
#define	RX_NEAR_FULL_SIZE		(SYS_RXBUF_LEN - 10)

/*TX_BUF*/
#define	SYS_TXBUF_LEN			(10)
#define	TX_NEAR_EMPTY_SIZE		(0)
#define	TX_NEAR_FULL_SIZE		(SYS_TXBUF_LEN - 5)



typedef struct _SYS_POOL{
	u8 sysExtRxBuf[SYS_RXBUF_LEN];				//数据接收缓冲buf
	u8 sysExtTxBuf[SYS_TXBUF_LEN];				//数据发送缓存cache
	
}SYS_POOL;


#endif//__system_h



