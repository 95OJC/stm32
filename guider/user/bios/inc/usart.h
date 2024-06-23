#ifndef __uart_h
#define	__uart_h

BOOL debug_usart_init(BUFINFO *pBuf,void *pCtrl);
void shell_usart_sendByte(u8 ch);
void shell_usart_sendString(u8 *str);
u8 shell_usart_getByte(void);
BOOL shell_usart_asyn_getByte(u8 *byte);


void usart_driver_register(void);


#endif//__uart_h
