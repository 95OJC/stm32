#ifndef	__uartApi_h
#define	__uartApi_h


//void usart_sendByte(u8 ch);
//u8 usart_getByte(void)
//void usart_sendString(u8 *str);
BOOL shell_port_init(BUFINFO *pBuf,void *pCtrl);
void shell_usart_sendByte(u8 ch);
void shell_usart_sendString(u8 *str);
u8 shell_usart_getByte(void);
BOOL shell_usart_asyn_getByte(u8 *byte);


void usart_driver_register(void);


#endif//__uartApi_h
