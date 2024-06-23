#ifndef __systick_rcc_h
#define	__systick_rcc_h

//进1次中断需要多久时间
#define SYSTICK_UNIT_1US	(SystemCoreClock/1000000)
#define SYSTICK_UNIT_1MS	(SystemCoreClock/1000)
#define SYSTICK_UNIT_10MS	(SystemCoreClock/100)

//进多少次中断
#define	SYSHANDLE_COUNT		1000


#endif//__systick_rcc_h


