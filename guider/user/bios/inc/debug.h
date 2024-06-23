#ifndef __debug_h
#define	__debug_h

void debug_open(void);
void debug_out(char *fmt,...);



#define DBG_OPEN() 	debug_open()
#define PRINTF  	debug_out

#define BIOS_DBG_EN    1
#define APP_DBG_EN     1




#endif//__debug_h

